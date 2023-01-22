/*$T EDImainframe_act.cpp GC 1.134 09/13/04 13:07:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIapp.h"
#include "EDImainframe.h"
#include "EDImainframe_act.h"
#include "EDIpaths.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/ENGvars.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGcheck.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGmerge.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/VERsion/VERsion_Number.h"
#include "EDItors/Sources/MENu/MENframe.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "EDItors/Sources/OUTput/OUTframe_act.h"
#include "EDIeditors_infos.h"
#include "EDIstrings.h"
#include "Res/Res.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "INOut/INOjoystick.h"
#include "AIinterp/Sources/AIdebug.h"
#include "ENGine/Sources/ENGvars.h"
#include "DIAlogs/DIAbase.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKstruct_reg.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "SouND/Sources/SND.h"
#include "DIAlogs/DIAname_dlg.h"
#include "Dialogs/DIAspeedact_dlg.h"
#include "TIMer/TIMdefs.h"

#ifdef JADEFUSION
extern int		SpecialFlags_b_ActiveTracer;
extern int		SpecialFlags_b_Init;
extern char		SpecialFlags_Text[5000][500];
extern int		SpecialFlags_b_Reset;
extern int		SpecialFlags_FlagToTrace;
extern void		SpecialFlags_Init(void);
extern int		*SpecialFlags_Array;
extern float	TIM_gf_SynchroFrequency;
extern void		BIG_MakeWolList(char *);
extern int		AI_EvalFunc_ARRBitTest_C(int *, int);
extern void		BIG_CleanByFat(void);
extern void		BIG_CleanLoaded(void);
extern int		AI_gi_TestContent;
extern ULONG	SPG2_PrimitivLimit;
extern void		BIG_GetAllIdKey(void);
extern BOOL		BIG_gb_CleanFatEmpty;
#else
extern "C" int		SpecialFlags_b_ActiveTracer;
extern "C" int		SpecialFlags_b_Init;
extern "C" char		SpecialFlags_Text[5000][500];
extern "C" int		SpecialFlags_b_Reset;
extern "C" int		SpecialFlags_FlagToTrace;
extern "C" void		SpecialFlags_Init(void);
extern "C" int		*SpecialFlags_Array;
extern "C" float	TIM_gf_SynchroFrequency;
extern "C" void		BIG_MakeWolList(char *);
extern "C" int		AI_EvalFunc_ARRBitTest_C(int *, int);
extern "C" void		BIG_CleanByFat(void);
extern "C" void		BIG_CleanLoaded(void);
extern "C" int		AI_gi_TestContent;
extern "C" ULONG	SPG2_PrimitivLimit;
extern "C" void		BIG_GetAllIdKey(void);
extern "C" BOOL		BIG_gb_CleanFatEmpty;
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
extern     char*	EDI_Feature_Text[EDI_MTL_MAX_FEATURES];
#endif
void EDI_cl_MainFrame::OnAction(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect				o_Rect;
	int					ix_Temp;
	EDI_cl_BaseFrame	*po_Ed;
	EDI_cl_BaseFrame	*po_Editor;
	BIG_INDEX			ul_Dir;
	ULONG				ul_NewAction;
	int					num;
	EDIA_cl_BaseDialog	o_Dlg(DIALOGS_IDD_ABOUT);
	char				*p;
	int					key;
	char				asz_Msg[512];
	char				asz_Msg1[512];
	EDIA_cl_SpeedAct	*o_SA;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p = NULL;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Normal action
	 -------------------------------------------------------------------------------------------------------------------
	 */

	switch(_ul_Action)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_SPEEDACT:
		o_SA = new EDIA_cl_SpeedAct;
		o_SA->DoModeless();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_EXIT:
		if(EDI_go_TheApp.mb_RunEngine)
		{
			ERR_X_ForceError("You must stop engine before", NULL);
			break;
		}

		OnExit();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_OPENPROJECT:
		if(EDI_go_TheApp.mb_RunEngine)
		{
			ERR_X_ForceError("You must stop engine before", NULL);
			break;
		}

		DestroyVssFile();
		DestroyRefFile();
		OnProjectOpen();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_NEWPROJECT:
		if(EDI_go_TheApp.mb_RunEngine)
		{
			ERR_X_ForceError("You must stop engine before", NULL);
			break;
		}

		OnProjectNew();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_CLOSEPROJECT:
		if(EDI_go_TheApp.mb_RunEngine)
		{
			ERR_X_ForceError("You must stop engine before", NULL);
			break;
		}

		OnProjectClose();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_CHECKVSSBASE:
		VssFileChanged(FALSE, TRUE);
		SWAP_BIG();
		AfxGetApp()->DoWaitCursor(1);
		BIG_b_CheckFile(FALSE);
		AfxGetApp()->DoWaitCursor(-1);
		SWAP_BIG();
		break;

	case EDI_ACTION_CHECKVSSBASEBF:
		REFFileChanged(FALSE, TRUE);
		SWAP_BIG();
		AfxGetApp()->DoWaitCursor(1);
		BIG_b_CheckFile(FALSE);
		AfxGetApp()->DoWaitCursor(-1);
		SWAP_BIG();
		break;

	case EDI_ACTION_CLEANVSSBASE:
		if(M_MF()->MessageBox("This will REGENERATE the file of vss rights.", "Are you sure ?", MB_YESNO) != IDYES)
			return;
		if(!b_LockBigFile()) return;
		VssFileChanged(FALSE, TRUE);
		COPY_BIG4();
		SWAP_BIG();
		L_strcpy(asz_Msg, BIG_gst.asz_Name);
		AfxGetApp()->DoWaitCursor(1);
		BIG_gb_CleanFatEmpty = TRUE;
		BIG_CleanByFat();
		BIG_gb_CleanFatEmpty = FALSE;
		AfxGetApp()->DoWaitCursor(-1);
		SWAP_BIG();
		L_strcpy(asz_Msg1, BIG_gst.asz_Name);
		RESTORE_BIG4();
		CopyFile(asz_Msg1, asz_Msg, FALSE);
		DeleteFile(asz_Msg1);
		UnLockBigFile();
		break;

	case EDI_ACTION_CHECKBIGFILE:
		AfxGetApp()->DoWaitCursor(1);
		BIG_b_CheckFile(FALSE);
		AfxGetApp()->DoWaitCursor(-1);
		break;

	case EDI_ACTION_CHECKBIGFILESTAT:
		AfxGetApp()->DoWaitCursor(1);
		BIG_b_CheckFile(TRUE);
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_CHECKBIGFILEC:
		AfxGetApp()->DoWaitCursor(1);
		BIG_gb_CanClean = TRUE;
		BIG_b_CheckFile(TRUE);
		BIG_gb_CanClean = FALSE;
		FatHasChanged();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_RESTOREDEL:
		AfxGetApp()->DoWaitCursor(1);
		BIG_RestoreDeleted();
		FatHasChanged();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_CLEANBIGFILE:
		AfxGetApp()->DoWaitCursor(1);
		BIG_Clean(FALSE);
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_GENMAPLIST:
		AfxGetApp()->DoWaitCursor(1);
		BIG_MakeWolList(NULL);
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_CLEANFINAL:
		AfxGetApp()->DoWaitCursor(1);
		BIG_Clean(TRUE);
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_CLEANFAT:
		AfxGetApp()->DoWaitCursor(1);
		BIG_CleanByFat();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_CLEANLOADED:
		AfxGetApp()->DoWaitCursor(1);
		BIG_CleanLoaded();
		AfxGetApp()->DoWaitCursor(-1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_GETLASTKEY:
		BIG_GetLastKey(&key);
		sprintf(asz_Msg, "Last Key is 0x%x (%d)", key, key);
		LINK_PrintStatusMsg(asz_Msg);
		break;

    case EDI_ACTION_GETLASTKEYUSER:
        {
        EDIA_cl_NameDialog o_Dialog("Enter a user ID (in decimal)");
        if(o_Dialog.DoModal() == IDOK)
        {
            ULONG ulUserID = atoi((char *) (LPCSTR) o_Dialog.mo_Name);

            if (ulUserID)
            {
                BIG_GetLastKeyForUser(&key,ulUserID);
	        	sprintf(asz_Msg, "Last Key for User 0x%x (%d) is 0x%x (%d)", ulUserID, ulUserID, key, key);
		        LINK_PrintStatusMsg(asz_Msg);
            }
        }
        }
		break;

    case EDI_ACTION_GETALLIDKEY:
		BIG_GetAllIdKey();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_CLOSEUNIVERSE:
		Universe_Close();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_SETUNIVERSEAIKEY:
		Universe_SetAIKey();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_REINITENGINE:
		Universe_ReinitEngine();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_ACTIVATEUNIV:
		mst_Ini.b_ActivateUniverse = mst_Ini.b_ActivateUniverse ? FALSE : TRUE;
		ENG_gb_ActivateUniverse = mst_Ini.b_ActivateUniverse;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_SPEEDDRAW:
		LINK_gi_SpeedDraw++;
		if(LINK_gi_SpeedDraw == 2)
		{
			SPG2_PrimitivLimit = 100000;
			LINK_gi_SpeedDraw = 0;
		}
		else
		{
			SPG2_PrimitivLimit = 0;
		}

		LINK_Refresh();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_TOGGLEMENU:
		mst_Ini.b_LeftFrameVisible = mst_Ini.b_LeftFrameVisible ? FALSE : TRUE;
		if(mst_Ini.b_LeftFrameVisible)
		{
			mo_BigSplitter.SetColumnInfo(0, mst_Ini.i_LeftFrameSize, 0);
		}
		else
		{
			mo_BigSplitter.GetColumnInfo(0, mst_Ini.i_LeftFrameSize, ix_Temp);
			mo_BigSplitter.SetColumnInfo(0, -1, -1);
		}

		mo_BigSplitter.RecalcLayout();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_CHECKMEMORY:
		MEM_CheckAllocatedBlocks(1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_CHECKMEMONTRAME:
		ENG_gb_TestMemory = !ENG_gb_TestMemory;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_DUMPMEMORY:
		MEM_CheckAllocatedBlocks(3);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_TOGGLEENGINE:
		AI_gi_OneStepMode = 0;
		EDI_go_TheApp.mb_RunEngine = !EDI_go_TheApp.mb_RunEngine;
		if(EDI_go_TheApp.mb_RunEngine == FALSE) ENG_gb_ForceEndEngine = TRUE;
		break;

	case EDI_ACTION_TOGGLEENGINES:
		Universe_ReinitEngine();
		AI_gi_OneStepMode = 0;
		EDI_go_TheApp.mb_RunEngine = TRUE;
		ENG_bg_FirstFrameSpeedRun = TRUE;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_ENGINESTEP:
		AI_gi_OneStepMode = 0;
		ENG_gb_OneStepEngine = TRUE;
		EDI_go_TheApp.mb_RunEngine = TRUE;
		break;

	case EDI_ACTION_LIMITFPS:
		ENG_gb_LimitFPS = !ENG_gb_LimitFPS;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_TOGGLEORIENT:
		mst_Desktop.b_HrzOrient = mst_Desktop.b_HrzOrient ? FALSE : TRUE;
		LockDisplay(&mo_BigSplitter);
		mo_Splitter.ToggleOrientation();
		mo_Splitter1.ToggleOrientation();
		mo_Splitter2.ToggleOrientation();
		mo_BigSplitter.RecalcLayout();
		UnlockDisplay(&mo_BigSplitter);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_DESKEQUALIZE:
		OnDesktopEqualize();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_DESKFILL:
		OnDesktopFill();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_DESKMAX:
		po_Ed = EDI_go_TheApp.GetFocusedEditor();
		if(po_Ed && (po_Ed->mpo_MyView != mpo_MenuView))
		{
			LockDisplay(&mo_BigSplitter);
			SwapMaximised((EDI_cl_BaseView *) po_Ed->GetParent());
			UnlockDisplay(&mo_BigSplitter);
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_DESKFULLMAX:
		po_Ed = EDI_go_TheApp.GetFocusedEditor();
		if(po_Ed && (po_Ed->mpo_MyView != mpo_MenuView))
		{
			LockDisplay(&mo_BigSplitter);
			if(mpo_MaxView && !mst_Desktop.b_VeryMaximized) SwapMaximised((EDI_cl_BaseView *) po_Ed->GetParent());
			mst_Desktop.b_VeryMaximized = TRUE;
			SwapMaximised((EDI_cl_BaseView *) po_Ed->GetParent());
			UnlockDisplay(&mo_BigSplitter);
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_DESKFULLMAXRES:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			GDI_tdst_Resolution ast_Res[2] = { { 640, 480, 32, 60 }, { 800, 600, 32, 60 } };
			GDI_tdst_Resolution *_pst_CurRes;
			int					res;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			po_Ed = EDI_go_TheApp.GetFocusedEditor();

			if(po_Ed->mi_FullScreenResolution == -1)
			{
				po_Ed->mi_FullScreenResolution = GDI_i_Resolution_GetIndex(&ast_Res[0]);
			}
			else
			{
				_pst_CurRes = GDI_pst_Resolution_Get(po_Ed->mi_FullScreenResolution);
				for(res = 0; res < 2; res++)
				{
					if(_pst_CurRes->w == ast_Res[res].w) break;
				}

				if(res < 1)
					po_Ed->mi_FullScreenResolution = GDI_i_Resolution_GetIndex(&ast_Res[res + 1]);
				else
					po_Ed->mi_FullScreenResolution = -1;
			}
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_DESKSAVE:
		SaveWorkspace();
		FlashJade();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_DESKNEW:
		NewDesktop();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_SET0:
		ForceActionConfig(0);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_SET1:
		ForceActionConfig(1);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_LINKCONTROL:
		OnLinkControl();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_JOYSTICK:
		Joystick_Calibrate();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_JOYSTICKPC:
		win32INO_l_Joystick_Mode = INO_Joy_PCMode;
		break;

#ifdef JADEFUSION
	case EDI_ACTION_JOYSTICKXENON:
		win32INO_l_Joystick_Mode = INO_Joy_XenonMode;
		break;
#endif

	case EDI_ACTION_JOYSTICKPS2:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_NameDialogCombo o_Dialog("Select joy adapter");
			char					*JoyName[5] = 
									{ 
										"0 - USB Smart joy Deluxe", 
										"1 - Smart joypad",
										"2 - USB2 Smart joy ",
										"3 - Koo interactive",
										"4 - Superbox 3 Pro",
									};
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			o_Dialog.AddItem(JoyName[0], 0);
			o_Dialog.AddItem(JoyName[1], 1);
			o_Dialog.AddItem(JoyName[2], 2);
			o_Dialog.AddItem(JoyName[3], 3);
			o_Dialog.AddItem(JoyName[4], 4);

			o_Dialog.SetDefault(JoyName[win32INO_l_JoyUSBPS2]);

#if 0
			if(o_Dialog.DoModal() == IDOK) INO_SelectJoyUSBPS2(o_Dialog.mi_CurSelData);
#else
            if ( o_Dialog.DoModal() == IDOK )
            {
            }
#endif

			win32INO_l_Joystick_Mode = INO_Joy_Ps2Mode;
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_SETLANG:
		OnSetLanguage();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_MEMSTATS:
		MemoryStats();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_TEXTSTATS:
		TextureStats();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_REFSTATS:
		RefStats();
		break;

	case EDI_ACTION_UNREFSTATS:
		UnrefStats();
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_DESK1:
		_ul_Action = WM_USER + 200;
		break;
	case EDI_ACTION_DESK2:
		_ul_Action = WM_USER + 201;
		break;
	case EDI_ACTION_DESK3:
		_ul_Action = WM_USER + 202;
		break;
	case EDI_ACTION_DESK4:
		_ul_Action = WM_USER + 203;
		break;
	case EDI_ACTION_DESK5:
		_ul_Action = WM_USER + 204;
		break;
	case EDI_ACTION_DESK6:
		_ul_Action = WM_USER + 205;
		break;
	case EDI_ACTION_DESK7:
		_ul_Action = WM_USER + 206;
		break;
	case EDI_ACTION_DESK8:
		_ul_Action = WM_USER + 207;
		break;
	case EDI_ACTION_DESK9:
		_ul_Action = WM_USER + 208;
		break;
	case EDI_ACTION_DESK10:
		_ul_Action = WM_USER + 209;
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_SPYSFON:
		SpecialFlags_b_ActiveTracer = SpecialFlags_b_ActiveTracer ? 0 : 1;
		break;

	case EDI_ACTION_SPYSFRESET:
		SpecialFlags_b_Reset = 1;
		break;

	case EDI_ACTION_SPYCONTENTSF:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_NameDialog	cl_GetKey("Enter SpecialFlag to Trace (0 for none)", 5);
			char				sz_Key[10];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			sprintf(sz_Key, "%u", SpecialFlags_FlagToTrace);
			cl_GetKey.mo_Name = sz_Key;
			if(cl_GetKey.DoModal() == IDOK)
			{
				sscanf(cl_GetKey.mo_Name, "%u", &AI_gi_TestContent);
			}
			break;
		}

	case EDI_ACTION_SPYSFTRACE:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_NameDialog	cl_GetKey("Enter SpecialFlag to Trace (0 for none)", 5);
			char				asz_Log[500];
			char				sz_Key[10];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			sprintf(sz_Key, "%u", SpecialFlags_FlagToTrace);
			cl_GetKey.mo_Name = sz_Key;

			if(cl_GetKey.DoModal() == IDOK)
			{
				sscanf(cl_GetKey.mo_Name, "%u", &SpecialFlags_FlagToTrace);
			}

			if(SpecialFlags_FlagToTrace)
			{
				if(SpecialFlags_FlagToTrace > 5000)
				{
					LINK_PrintStatusMsg("SpecialFlags limited to 5000 for now in Tracer. Update if needed");
				}
				else
				{
					/*~~*/
					int i;
					/*~~*/

					if(SpecialFlags_b_Init)
						SpecialFlags_Init();
					else
						LINK_PrintStatusMsg("--------------------------");
					for(i = SpecialFlags_FlagToTrace; i < SpecialFlags_FlagToTrace + 20; i++)
					{
						sprintf(asz_Log, "SpecialFlag %u <%s>", i, SpecialFlags_Text[i]);
						LINK_PrintStatusMsg(asz_Log);
						if(i == SpecialFlags_FlagToTrace) LINK_PrintStatusMsg("--------------------------");
					}
				}
			}
		}
		break;

	case EDI_ACTION_SPYSFSTATE:
		{
			if(!SpecialFlags_Array)
			{
				M_MF()->MessageBox("SpecialFlags Array not defined. Run the Engine", "Kesta foutu nondediou?", MB_OK);
			}
			else
			{
				/*~~~~~~~~~~~~~~~~~*/
				int		i;
				char	asz_Log[500];
				/*~~~~~~~~~~~~~~~~~*/

				if(SpecialFlags_b_Init) SpecialFlags_Init();
				for(i = 0; i < 96 * 32; i++)
				{
					if(AI_EvalFunc_ARRBitTest_C(SpecialFlags_Array, i))
					{
						sprintf(asz_Log, "SpecialFlag_set(%i) //%s", i, SpecialFlags_Text[i]);
						LINK_PrintStatusMsg(asz_Log);
					}
				}
			}
		}
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case EDI_ACTION_HELPABOUT:
		o_Dlg.DoModal();
		break;

	case EDI_ACTION_HELPPATH:
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			EDIA_cl_NameDialog	o_HelpPath("Enter search path for help");
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			o_HelpPath.mo_Name = M_MF()->mst_ExternIni.asz_PathHelp;
			if(o_HelpPath.DoModal() == IDOK)
			{
				L_strcpy(M_MF()->mst_ExternIni.asz_PathHelp, (char *) (LPCSTR) o_HelpPath.mo_Name);
			}
		}
		break;

	case EDI_ACTION_HELP:
		EDI_go_TheApp.SearchHelp("JADE");
		break;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Dynamic menu action state
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Action >= WM_USER)
	{
		ul_NewAction = _ul_Action - WM_USER;

		/* Show/Hide editors */
		if(ul_NewAction < 200)
		{
			po_Editor = po_EditorFromUserID(_ul_Action);
			if(po_Editor)
			{
				po_Editor->mst_BaseIni.b_IsVisible = po_Editor->mst_BaseIni.b_IsVisible ? FALSE : TRUE;
				po_Editor->mpo_MyView->ActivateDisactivateEditors();
			}
		}

		/* Desktop */
		else
		{
			ul_NewAction -= 200;
			num = 0;

			ul_Dir = BIG_ul_SearchDir(EDI_Csz_Ini_Desktop);
			if(ul_Dir == BIG_C_InvalidIndex) return;
			ul_Dir = BIG_SubDir(ul_Dir);
			while(ul_Dir != BIG_C_InvalidIndex)
			{
				if((UINT) num == ul_NewAction)
				{
					ChangeDesktop(BIG_NameDir(ul_Dir));
					return;
				}

				num++;
				ul_Dir = BIG_NextDir(ul_Dir);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::b_OnActionValidate(ULONG _ul_Action, BOOL _b_Disp)
{
	if(BIG_Handle() == NULL) return FALSE;
	switch(_ul_Action)
	{
	case EDI_ACTION_RESTOREDEL:
		if(BIG_gst.st_ToSave.ul_FirstFreeFile == BIG_C_InvalidIndex) return FALSE;
		break;

	case EDI_ACTION_DESK1:
	case EDI_ACTION_DESK2:
	case EDI_ACTION_DESK3:
	case EDI_ACTION_DESK4:
	case EDI_ACTION_DESK5:
	case EDI_ACTION_DESK6:
	case EDI_ACTION_DESK7:
	case EDI_ACTION_DESK8:
	case EDI_ACTION_DESK9:
	case EDI_ACTION_DESK10:
		if(_b_Disp) return FALSE;
		break;

	case EDI_ACTION_REINITENGINE:
		if(!WOR_ul_Universe_GetNbWorlds()) return FALSE;
		break;

	case EDI_ACTION_REFSTATS:
	case EDI_ACTION_UNREFSTATS:
		if(!LOA_gi_InitRef) return FALSE;
		return TRUE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_Action *EDI_cl_MainFrame::po_ActionGetSetDynKey(EDI_cl_ActionList *_po_List, EDI_cl_Action *_po_Act)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_Act;
	EDI_cl_Action	*po_NewAct;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Active desktop */
	po_NewAct = NULL;
	if(_po_Act->mul_Action >= WM_USER + 200)
	{
		ul_Act = _po_Act->mul_Action - WM_USER - 200;
		switch(ul_Act)
		{
		case 0:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK1); break;
		case 1:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK2); break;
		case 2:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK3); break;
		case 3:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK4); break;
		case 4:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK5); break;
#ifdef JADEFUSION //POPOWARNING ?? ?? ??
		case 5:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK6); break;
		case 6:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK7); break;
		case 7:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK8); break;
		case 8:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK9); break;
		case 9:	    po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK10); break;
#else
		case 6:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK6); break;
		case 7:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK7); break;
		case 8:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK8); break;
		case 9:		po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK9); break;
		case 10:	po_NewAct = _po_List->po_GetActionById(EDI_ACTION_DESK10); break;
#endif
		}
	}

	return po_NewAct;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnActionUI(ULONG _ul_Action, CString &_o_Ref, CString &o_Ret)
{
	/*~~~~~~~~~~~~~~~~~*/
	char	sz_Value[10];
	/*~~~~~~~~~~~~~~~~~*/

	o_Ret = _o_Ref;
	switch(_ul_Action)
	{
	case EDI_ACTION_JOYSTICKPS2:	sprintf(sz_Value, " (%d)", win32INO_l_JoyUSBPS2); o_Ret += sz_Value; break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EDI_cl_MainFrame::ui_OnActionState(ULONG _ul_Action)
{
	/*~~~~~~~~~~~~~*/
	UINT	ui_State;
	/*~~~~~~~~~~~~~*/

	ui_State = (UINT) - 1;

	switch(_ul_Action)
	{
	case EDI_ACTION_JOYSTICKPC:
		ui_State = DFCS_BUTTONRADIO;
		if(win32INO_l_Joystick_Mode == INO_Joy_PCMode) ui_State |= DFCS_CHECKED;
		break;

#ifdef JADEFUSION
	case EDI_ACTION_JOYSTICKXENON:
		ui_State = DFCS_BUTTONRADIO;
		if(win32INO_l_Joystick_Mode == INO_Joy_XenonMode) ui_State |= DFCS_CHECKED;
		break;
#endif

	case EDI_ACTION_JOYSTICKPS2:
		ui_State = DFCS_BUTTONRADIO;
		if(win32INO_l_Joystick_Mode == INO_Joy_Ps2Mode) ui_State |= DFCS_CHECKED;
		break;

	case EDI_ACTION_SET0:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_ActionSet == 0) ui_State |= DFCS_CHECKED;
		break;

	case EDI_ACTION_SET1:
		ui_State = DFCS_BUTTONRADIO;
		if(mst_Ini.i_ActionSet == 1) ui_State |= DFCS_CHECKED;
		break;

	case EDI_ACTION_TOGGLEENGINE:
		ui_State = DFCS_BUTTONCHECK;
		if(EDI_go_TheApp.mb_RunEngine) ui_State |= DFCS_CHECKED;
		break;

	case EDI_ACTION_LIMITFPS:
		ui_State = DFCS_BUTTONCHECK;
		if ( ENG_gb_LimitFPS ) ui_State |= DFCS_CHECKED;
		break;

	case EDI_ACTION_TOGGLEMENU:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.b_LeftFrameVisible) ui_State |= DFCS_CHECKED;
		break;

	case EDI_ACTION_TOGGLEORIENT:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Desktop.b_HrzOrient) ui_State |= DFCS_CHECKED;
		break;

	case EDI_ACTION_DESKMAX:
		ui_State = DFCS_BUTTONCHECK;
		if(mpo_MaxView) ui_State |= DFCS_CHECKED;
		break;

	case EDI_ACTION_DESKFULLMAX:
		ui_State = DFCS_BUTTONCHECK;
		if(mpo_MaxView && mst_Desktop.b_VeryMaximized) ui_State |= DFCS_CHECKED;
		break;

	case EDI_ACTION_ACTIVATEUNIV:
		ui_State = DFCS_BUTTONCHECK;
		if(mst_Ini.b_ActivateUniverse) ui_State |= DFCS_CHECKED;
		break;

	case EDI_ACTION_CHECKMEMONTRAME:
		ui_State = DFCS_BUTTONCHECK | ((ENG_gb_TestMemory) ? DFCS_CHECKED : 0);
		break;

	/*
	 * case EDI_ACTION_SPEEDDRAW: ui_State = DFCS_BUTTONCHECK; if(LINK_gb_SpeedDraw)
	 * ui_State |= DFCS_CHECKED; break;
	 */
	case EDI_ACTION_SPYSFON:
		ui_State = DFCS_BUTTONCHECK;
		if(SpecialFlags_b_ActiveTracer) ui_State |= DFCS_CHECKED;
		break;
	}

	return ui_State;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
UINT EDI_cl_MainFrame::ui_ActionFillDynamic(EDI_cl_ConfigList *_po_List, POSITION _pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EDI_cl_Action		*po_Action;
	EDI_cl_BaseView		*po_View;
	EDI_cl_BaseFrame	*po_Editor;
	int					i, j, num, numsep;
	BOOL				bFirst;
	BIG_INDEX			ul_Dir;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Action = _po_List->mo_List.GetAt(_pos);
	switch((int) po_Action->mul_Action)
	{

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Add list of all present editors
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EDI_SEPACTION_EDITORS:
		num = 0;
		numsep = 0;

		for(i = 0; i < EDI_C_MaxViews; i++)
		{
			po_View = po_GetViewWithNum(i);
			bFirst = TRUE;
			for(j = 0; j < M_CurNumEd(po_View->mi_NumPane); j++)
			{
				po_Editor = M_CurEd(po_View->mi_NumPane, j).po_Editor;
				if(!po_Editor) continue;
				if(!po_Editor->b_CanActivate()) continue;

				/* Insert a separator the first time we add an editor */
				if(i && bFirst)
				{
					bFirst = FALSE;
					po_Action = new EDI_cl_Action;
					po_Action->mo_DisplayName = "";
					po_Action->mo_Key = "";
					po_Action->muw_Key = 0;
					po_Action->mui_Resource = 0;
					po_Action->mui_State = 0;
					po_Action->mul_Action = 0;
					_po_List->mo_List.InsertAfter(_pos, po_Action);
					_po_List->mo_List.GetNext(_pos);
					numsep++;
				}

				/* Insert the editor */
				po_Action = new EDI_cl_Action;

				po_Action->mo_DisplayName = po_Editor->mst_Def.asz_Name;
				po_Action->mo_Key = "";
				po_Action->muw_Key = 0;
				po_Action->mui_Resource = 0;

				po_Action->mui_State = DFCS_BUTTONCHECK;
				if(po_Editor->mst_BaseIni.b_IsVisible) po_Action->mui_State |= DFCS_CHECKED;

				po_Action->mul_Action = WM_USER + num++;

				_po_List->mo_List.InsertAfter(_pos, po_Action);
				_po_List->mo_List.GetNext(_pos);
			}
		}

		return num + numsep;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    List of all desktops
	 -------------------------------------------------------------------------------------------------------------------
	 */

	case EDI_SEPACTION_DESKTOP:
		ul_Dir = BIG_ul_SearchDir(EDI_Csz_Ini_Desktop);
		if(ul_Dir == BIG_C_InvalidIndex) break;
		ul_Dir = BIG_SubDir(ul_Dir);
		num = 0;

		/* Separator */
		if(ul_Dir != BIG_C_InvalidIndex)
		{
			po_Action = new EDI_cl_Action;
			po_Action->mo_DisplayName = "User Desktops";
			po_Action->mo_Key = "";
			po_Action->muw_Key = 0;
			po_Action->mui_Resource = 0;
			po_Action->mui_State = 0;
			po_Action->mul_Action = 0;
			_po_List->mo_List.InsertAfter(_pos, po_Action);
			_po_List->mo_List.GetNext(_pos);
		}

		/* Add all subdirs (desktop names) */
		while(ul_Dir != BIG_C_InvalidIndex)
		{
			po_Action = new EDI_cl_Action;

			po_Action->mo_DisplayName = BIG_NameDir(ul_Dir);
			po_Action->mo_Key = "";
			po_Action->muw_Key = 0;
			po_Action->mui_Resource = 0;

			po_Action->mui_State = DFCS_BUTTONRADIO;
			if(!L_strcmpi(BIG_NameDir(ul_Dir), mst_Ini.asz_CurrentDeskName)) po_Action->mui_State |= DFCS_CHECKED;

			po_Action->mul_Action = WM_USER + 200 + num++;

			_po_List->mo_List.InsertAfter(_pos, po_Action);
			_po_List->mo_List.GetNext(_pos);

			ul_Dir = BIG_NextDir(ul_Dir);
		}

		return num + 1 /* + 1 cause of separator */ ;
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDI_cl_MainFrame::i_IsItVarAction(ULONG _ul_Action, EVAV_cl_View *_po_List)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVAV_cl_ViewItem	*po_Item;
#ifdef JADEFUSION
	int					i, i_FeatureMaxNum;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(_ul_Action)
	{
	case EDI_SEPACTION_OPTIONS:
		/* To avoid reset and refresh the list if not necessary */
		if(!_po_List) return 2;
		if(_po_List->mul_ID == EDI_SEPACTION_OPTIONS) return 2;
		_po_List->mul_ID = EDI_SEPACTION_OPTIONS;

		_po_List->ResetList();
		_po_List->AddItem("General", EVAV_EVVIT_Separator, NULL);
		_po_List->AddItem(EDI_STR_Csz_OptionSaveExit, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_SaveWhenExit);
		_po_List->AddItem(EDI_STR_Csz_AskExit, EVAV_EVVIT_Bool, &mst_Ini.mst_Options.b_AskExit);
		_po_List->AddItem("Synchronize Editors Datas", EVAV_EVVIT_Bool, &mst_ExternIni.b_SynchroEditorsData);
		_po_List->AddItem("Display", EVAV_EVVIT_Separator, NULL);
		_po_List->AddItem(EDI_STR_Csz_ColorRun, EVAV_EVVIT_Color, &mst_Ini.mst_Options.x_ColRunEngine);
		_po_List->AddItem("Height menus", EVAV_EVVIT_Int, &mst_Ini.ui_HeightMenu, 0, 14, 24);

#ifdef JADEFUSION
		_po_List->AddItem("Display Int As Hexa", EVAV_EVVIT_Bool, &mst_Ini.b_IntAsHex, 0, 0, 0);
		_po_List->AddItem("MTL Features", EVAV_EVVIT_Separator, NULL);
		
		//prevent crash by never adding more features than the mst_Ini Struct have
		i_FeatureMaxNum = EDI_MTL_FeaturesCount <= EDI_MTL_MAX_FEATURES ? EDI_MTL_FeaturesCount : EDI_MTL_MAX_FEATURES;
		
		for(i = 0; i < i_FeatureMaxNum; i++)
			po_Item = _po_List->AddItem(EDI_Feature_Text[i] ? EDI_Feature_Text[i] : "Unnamed Feature!!", EVAV_EVVIT_Bool, &ab_MtlFeatures[i]);
#else
		po_Item = _po_List->AddItem("Display Int As Hexa", EVAV_EVVIT_Bool, &mst_Ini.b_IntAsHex, 0, 0, 0);
#endif
		po_Item->mpfn_CB = LINK_CallBack_Refresh3DEngineAndPointers;
		return 1;
	}

	return 0;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EDI_cl_BaseFrame *EDI_cl_MainFrame::po_EditorFromUserID(UINT nID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j, num;
	EDI_cl_BaseFrame	*po_Editor;
	EDI_cl_BaseView		*po_View;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	nID -= WM_USER;
	num = 0;

	for(i = 0; i < EDI_C_MaxViews; i++)
	{
		po_View = po_GetViewWithNum(i);
		for(j = 0; j < M_CurNumEd(po_View->mi_NumPane); j++)
		{
			po_Editor = M_CurEd(po_View->mi_NumPane, j).po_Editor;
			if(!po_Editor) continue;
			if(!po_Editor->b_CanActivate()) continue;
			if((UINT) num == nID) return po_Editor;
			num++;
		}
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim: Force a set of action and associted keys. In: i Number of the set (0 for default, 1 for custom).
 =======================================================================================================================
 */
void EDI_cl_MainFrame::ForceActionConfig(int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos, pos1;
	EDI_cl_ActionList	*po_ActionC;
	EDI_cl_ConfigList	*po_List;
	EDI_cl_Action		*po_Action;
	EDI_cl_Action		st_DumAct;
	EMEN_cl_Frame		*po_Menu;
	int					i_ID;
	int					j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mst_Ini.i_ActionSet = i;

	/* Inform all lists */
	pos = mo_ActionList.GetHeadPosition();
	while(pos)
	{
		po_ActionC = mo_ActionList.GetNext(pos);
		if(i <= po_ActionC->mo_List.GetCount() - 1) po_ActionC->mi_CurrentList = i;

		/* Inform toolbar about each change */
		pos1 = po_ActionC->mo_List.FindIndex(po_ActionC->mi_CurrentList);
		if(!pos1) continue;

		po_List = po_ActionC->mo_List.GetAt(pos1);
		pos1 = po_List->mo_List.GetHeadPosition();
		while(pos1)
		{
			po_Action = po_List->mo_List.GetNext(pos1);
			if(po_ActionC->mpo_Editor)
				i_ID = po_ActionC->mpo_Editor->mst_Def.i_Type;
			else
				i_ID = -1;
		}
	}

	/* Refresh current list */
	for(j = 0; j < EDI_C_MaxDuplicate + 1; j++)
	{
		po_Menu = (EMEN_cl_Frame *) po_GetEditorByType(EDI_IDEDIT_MENU, j);
		if(!po_Menu) po_Menu = (EMEN_cl_Frame *) po_GetEditorByType(EDI_IDEDIT_MENU, 0);
		if(po_Menu && po_Menu->mpo_CurrentExpand) po_Menu->mpo_CurrentExpand->FillList();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnToolBarCommand(UINT nID)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern BOOL VSS_gb_OpenOneTime;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(nID)
	{
	case EDI_ID_SAVEWORKSPACE:
		OnAction(EDI_ACTION_DESKSAVE);
		break;
	case EDI_ID_DATACONTROL:
		if(!VSS_gb_OpenOneTime)
		{
			OnLinkControl();
		}
		else
		{
			mst_Ini.b_LinkControlON = mst_Ini.b_LinkControlON ? FALSE : TRUE;
		}
		break;
	case EDI_ID_FRAME1:
		if(MAI_gst_MainHandles.pst_DisplayData)
		{
			if(MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames == 1)
				MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 0;
			else
				MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 1;
			EDI_go_TheApp.OnIdle(0);
		}
		break;

	case EDI_ID_FRAME2:
		if(MAI_gst_MainHandles.pst_DisplayData)
		{
			if(MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames == 2)
				MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 0;
			else
				MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 2;
			EDI_go_TheApp.OnIdle(0);
		}
		break;


	case EDI_ID_FRAME3:
		if(MAI_gst_MainHandles.pst_DisplayData)
		{
			if(MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames == 3)
				MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 0;
			else
				MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames = 3;
			EDI_go_TheApp.OnIdle(0);
		}
		break;

	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::OnToolBarCommandUI(UINT nID, CCmdUI *pUI)
{
	switch(nID)
	{
	case EDI_ID_DATACONTROL:
		pUI->SetCheck(mst_Ini.b_LinkControlON ? 1 : 0);
		break;
	case EDI_ID_FRAME1:
		if(MAI_gst_MainHandles.pst_DisplayData)
		{
			pUI->SetCheck((MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames == 1)? 1 : 0);
		}
		break;

	case EDI_ID_FRAME2:
		if(MAI_gst_MainHandles.pst_DisplayData)
		{
			pUI->SetCheck((MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames == 2)? 1 : 0);
		}
		break;

	case EDI_ID_FRAME3:
		if(MAI_gst_MainHandles.pst_DisplayData)
		{
			pUI->SetCheck((MAI_gst_MainHandles.pst_DisplayData->uc_ForceNumFrames == 3)? 1 : 0);
		}
		break;

	}

	pUI->Enable(TRUE);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EDI_cl_MainFrame::InitPopupMenuAction(EDI_cl_BaseFrame *ped, EMEN_cl_SubMenu *_po_Menu)
{
	/*~~~~~~~~~~~~*/
	POSITION	pos;
	/*~~~~~~~~~~~~*/

	/* Delete menu list */
	mo_TrackPopupMenuList.RemoveAll();

	/* Delete custom actions */
	pos = mo_TrackPopupMenuActionList.GetHeadPosition();
	while(pos) delete mo_TrackPopupMenuActionList.GetNext(pos);
	mo_TrackPopupMenuActionList.RemoveAll();

	/* Create a popup menu */
	_po_Menu->CreatePopupMenu();
	if(ped)
		_po_Menu->mpo_MenuFrame = ped->mpo_MenuFrame;
	else
		_po_Menu->mpo_MenuFrame = mpo_MenuFrame;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EDI_cl_MainFrame::AddPopupMenuAction
(
	EDI_cl_BaseFrame	*ped,
	EMEN_cl_SubMenu		*_po_Menu,
	ULONG				_ul_Action,
	BOOL				_b_Custom,
	char				*_psz_Name,
	UINT				_ui_State
)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	EDI_cl_Action	*po_Act;
	ULONG			ul_Last;
	POSITION		pos;
	/*~~~~~~~~~~~~~~~~~~~~*/

	/* To avoid following separators */
	if(mo_TrackPopupMenuList.GetCount())
	{
		pos = mo_TrackPopupMenuList.GetTailPosition();
		ul_Last = mo_TrackPopupMenuList.GetAt(pos);
		if(ul_Last == 0xFFFFFFFF)
		{
			pos = mo_TrackPopupMenuActionList.GetTailPosition();
			po_Act = mo_TrackPopupMenuActionList.GetAt(pos);
			ul_Last = po_Act->mul_Action;
		}

		if(!ul_Last && !_ul_Action && (!_psz_Name || !*_psz_Name)) return FALSE;
	}
	else
	{
		if(!_ul_Action && !_psz_Name) return FALSE;
	}

	/* A custom action */
	if(_b_Custom)
	{
		/* Create a new dummy action */
		po_Act = new EDI_cl_Action;
		po_Act->mo_DisplayName = _psz_Name;
		po_Act->mul_Action = _ul_Action;
		po_Act->mo_Key = "";
		po_Act->mui_Resource = 0;
		po_Act->mui_State = _ui_State;
		mo_TrackPopupMenuActionList.AddTail(po_Act);
		mo_TrackPopupMenuList.AddTail(0xFFFFFFFF);
		return TRUE;
	}

	/* A normal registered action */
	mo_TrackPopupMenuList.AddTail(_ul_Action);
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EDI_cl_MainFrame::TrackPopupMenuAction(EDI_cl_BaseFrame *ped, CPoint pt, EMEN_cl_SubMenu *_po_Menu)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_Sel;
	POSITION		pos, pos1, pos2;
	ULONG			ul_Action, ul_Custom;
	CWnd			*po_Wnd;
	EDI_cl_Action	*po_Action;
	MSG				msg;
	CString			o_String;
	UINT			flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Clean menu action list */
	pos = mo_TrackPopupMenuList.GetHeadPosition();
	while(pos)
	{
		pos1 = pos;
		ul_Action = mo_TrackPopupMenuList.GetNext(pos);
		while(ul_Action <= 0)
		{
			if(pos1 == mo_TrackPopupMenuList.GetHeadPosition())
			{
				mo_TrackPopupMenuList.RemoveHead();
				pos = mo_TrackPopupMenuList.GetHeadPosition();
				break;
			}

			if(!pos)
			{
				mo_TrackPopupMenuList.RemoveTail();
				break;
			}

			pos1 = pos;
			ul_Action = mo_TrackPopupMenuList.GetNext(pos1);
			if(ul_Action <= 0) mo_TrackPopupMenuList.RemoveAt(pos);
			pos = pos1;
		}
	}

	/* Remove separator at the end of the menu */
	pos = mo_TrackPopupMenuList.GetTailPosition();
	if(!pos) return -1;
	ul_Action = mo_TrackPopupMenuList.GetAt(pos);
	if(!ul_Action)
		mo_TrackPopupMenuList.RemoveAt(pos);
	else if(ul_Action == 0xFFFFFFFF)
	{
		pos2 = mo_TrackPopupMenuActionList.GetTailPosition();
		po_Action = mo_TrackPopupMenuActionList.GetAt(pos2);
		if(!po_Action->mul_Action)
		{
			mo_TrackPopupMenuList.RemoveAt(pos);
			mo_TrackPopupMenuActionList.RemoveAt(pos2);
		}
	}

	/* No menu */
	if(!mo_TrackPopupMenuList.GetCount()) return 0;

	/* Construct menu */
	pos = mo_TrackPopupMenuList.GetHeadPosition();
	i_Sel = 1;
	ul_Custom = 0;
	while(pos)
	{
		ul_Action = mo_TrackPopupMenuList.GetNext(pos);

		/* Custom action ? */
		if(ul_Action == 0xFFFFFFFF)
		{
			pos2 = mo_TrackPopupMenuActionList.FindIndex(ul_Custom);
			po_Action = mo_TrackPopupMenuActionList.GetAt(pos2);
			ul_Custom++;
		}

		/* Else normal registered action */
		else
		{
			if(ped)
			{
				po_Action = ped->mpo_Actions->po_GetActionById(ul_Action);
				po_Action->mb_Disabled = FALSE;
				if(!ped->b_OnActionValidate(ul_Action))
				{
					po_Action->mo_RealDisplayName = po_Action->mo_DisplayName;
					po_Action->mb_Disabled = TRUE;
					po_Action->mui_State = -1;
				}
				else
				{
					po_Action->mui_State = ped->ui_OnActionState(po_Action->mul_Action);
					ped->OnActionUI(po_Action->mul_Action, po_Action->mo_DisplayName, o_String);
					po_Action->mo_RealDisplayName = o_String;
				}
			}
			else
			{
				po_Action = mpo_Actions->po_GetActionById(ul_Action);
				po_Action->mb_Disabled = FALSE;
				if(!b_OnActionValidate(ul_Action))
				{
					po_Action->mb_Disabled = TRUE;
					po_Action->mui_State = -1;
				}
				else
				{
					po_Action->mui_State = ui_OnActionState(po_Action->mul_Action);
				}
			}
		}

		flags = MF_OWNERDRAW | MF_BYPOSITION;
		if(po_Action->mb_Disabled) flags = MF_OWNERDRAW | MF_BYPOSITION | MF_DISABLED;
		_po_Menu->InsertMenu(i_Sel, flags, i_Sel++, (char *) po_Action);
	}

	/* Track */
	EDI_gb_LockKeyboard = TRUE;
	po_Wnd = GetFocus();
	i_Sel = _po_Menu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, pt.x, pt.y, this);
	if(i_Sel)
	{
		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	EDI_gb_LockKeyboard = FALSE;

	if((short) i_Sel > 0)
	{
		/* Return ID of selected action */
		pos = mo_TrackPopupMenuList.FindIndex(i_Sel - 1);
		ul_Action = mo_TrackPopupMenuList.GetAt(pos);

		/* It's not a custom action. Call on action */
		if(ul_Action != 0xFFFFFFFF)
		{
			if(ped)
				ped->OnAction(ul_Action);
			else
				OnAction(ul_Action);
		}

		/* Custom action. Search the real action id */
		else
		{
			pos = mo_TrackPopupMenuList.GetHeadPosition();
			ul_Custom = 0;
			while(pos)
			{
				ul_Action = mo_TrackPopupMenuList.GetNext(pos);
				if(ul_Action == 0xFFFFFFFF)
				{
					if((int) ul_Custom == i_Sel - 1)
					{
						pos = mo_TrackPopupMenuActionList.FindIndex(ul_Custom);
						po_Action = mo_TrackPopupMenuActionList.GetAt(pos);
						ul_Action = po_Action->mul_Action;
						break;
					}

					ul_Custom++;
				}
			}
		}
	}

	if(po_Wnd && IsWindow(po_Wnd->m_hWnd)) po_Wnd->SetFocus();
	_po_Menu->DestroyMenu();

	/* Delete custom actions */
	pos = mo_TrackPopupMenuActionList.GetHeadPosition();
	while(pos) delete mo_TrackPopupMenuActionList.GetNext(pos);
	mo_TrackPopupMenuActionList.RemoveAll();

	return ul_Action;
}
#endif
