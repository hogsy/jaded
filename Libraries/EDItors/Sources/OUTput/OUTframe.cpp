/*$T OUTframe.cpp GC! 1.100 08/23/01 14:50:34 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "EDIapp.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#define ACTION_GLOBAL
#include "OUTframe.h"
#include "OUTstrings.h"
#include "OUTframe_act.h"
#include "VAVview/VAVlist.h"
#include "F3Dframe/F3Dframe.h"
#include "F3Dframe/F3Dview.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKstruct_reg.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLmain.h"
#include "GraphicDK/Sources/GEOmetric/GEOzone.h"
#include "OUTmsg.h"
#include "EDIpaths.h"
#include "EDIapp.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDItors/Sources/MENu/MENsubmenu.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "ENGine/Sources/ENGvars.h"
#include "SELection/SELection.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "EDIsplitter.h"
#include "DIAlogs/DIAtoolbox_dlg.h"
#include "DIAlogs/DIAtoolbox_groview.h"
#include "DIAlogs/DIACOLOR_dlg.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"
#include "DIAlogs/DIA_SKN_dlg.h"
#include "DIAlogs/DIAradiosity_dlg.h"
#include "DIAlogs/DIAname_dlg.h"
#include "DIAlogs/DIAuvmapper_dlg.h"
#include "DIAlogs/DIAselection_dlg.h"
#ifdef JADEFUSION
#include "DIAlogs/DIAlightrej_dlg.h"
#endif
#include "DIAlogs/DIAgamematerial_dlg.h"
#include "DIAlogs/DIAgamematerial_inside.h"
#include "DIAlogs/DIAanim_dlg.h"
#include "DIAlogs/DIAsecto_dlg.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "AIinterp/Sources/AIBench.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGcheck.h"
#include "BIGfiles/BIGmdfy_dir.h"
#ifdef JADEFUSION
#include "Light/LIGHTrejection.h"
#endif
#include "EDIapp_options.h"
#include "EDImsg.h"
#include "GEOmetric/GEO_STRIP.h"
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEOstaticLOD.h"

#include "DIAlogs/DIAmorphing3Dview.h"

#ifdef JADEFUSION
extern void BIG_CleanBinFiles(ULONG);

extern char			*F3D_STR_Csz_Zone_AI[16];
extern BOOL		ENG_gb_ActiveSectorization;
extern void		TEXT_FreeAll(void);
extern BAS_tdst_barray WOR_ListAllKeys;
extern BOOL		EDI_gb_ComputeMap;
extern BOOL		EDI_gb_NoVerbose;
extern ULONG	BIG_gul_CanAskKey;
extern int		EDI_gi_GenSpe;
#else
extern "C" void BIG_CleanBinFiles(ULONG);

extern char			*F3D_STR_Csz_Zone_AI[16];
extern "C" BOOL		ENG_gb_ActiveSectorization;
extern "C" void		TEXT_FreeAll(void);
extern "C" BAS_tdst_barray WOR_ListAllKeys;
extern "C" BOOL		EDI_gb_ComputeMap;
extern "C" BOOL		EDI_gb_NoVerbose;
extern "C" ULONG	BIG_gul_CanAskKey;
extern "C" int		EDI_gi_GenSpe;
#endif

BOOL				EDI_gb_SlashLLoaded = FALSE;
extern BOOL			EDI_gb_SlashLoadAndQuit;
extern EDIA_cl_SectoDialog	*gspo_SectoDialog;
extern OBJ_tdst_GameObject *OUT_gpst_GORef;

#ifdef JADEFUSION
extern BOOL         EDI_gb_SlashLXMP;
extern BOOL         EDI_gb_SlashLXPK;
extern BOOL         EDI_gb_SlashXPK;
extern BOOL         EDI_gb_SlashXTX;
extern void Xe_GenerateAllDds();
extern void LOG_Use(char *bigfile);
#else
extern "C"	void LOG_Use(char *);
#endif
/*$4
 ***********************************************************************************************************************
    GLOBAL VARS
 ***********************************************************************************************************************
 */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    MESSAGE MAP.
 -----------------------------------------------------------------------------------------------------------------------
 */

IMPLEMENT_DYNCREATE(EOUT_cl_Frame, EDI_cl_BaseFrame)
BEGIN_MESSAGE_MAP(EOUT_cl_Frame, EDI_cl_BaseFrame)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_KEYUP()
	ON_WM_RBUTTONDOWN()
    ON_WM_MOVING()
END_MESSAGE_MAP()

/*$4
 ***********************************************************************************************************************
    CONSTRUCT
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EOUT_cl_Frame::EOUT_cl_Frame(void)
{
	mul_CurrentWorld = BIG_C_InvalidIndex;
	mul_LoadedWorld = BIG_C_InvalidIndex;
	mpo_EngineFrame = new F3D_cl_Frame;
	mb_CanDestroy = TRUE;
	mb_LockUpdate = FALSE;
	mb_HideUnselected = TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EOUT_cl_Frame::~EOUT_cl_Frame(void)
{
}

/*
 =======================================================================================================================
    Aim:    Call to create the editor.
 =======================================================================================================================
 */
int EOUT_cl_Frame::OnCreate(LPCREATESTRUCT lpcs)
{
	if(EDI_cl_BaseFrame::OnCreate(lpcs) == -1) return -1;

	/* Create output frame. */
	mpo_EngineFrame->MyCreate(this, this);

	/* Register display view */
	LINK_RegisterPointer
	(
		DP()->mst_WinHandles.pst_DisplayData,
		LINK_C_StructDisplayData,
		mst_Def.asz_Name,
		EDI_Csz_Path_DisplayData
	);
	LINK_UpdatePointers();

	/* Force recalc for toolbar */
	RecalcLayout();
	return 0;
}

/*
 =======================================================================================================================
    Aim:    pretranslate message
 =======================================================================================================================
 */
 #if 0
BOOL EOUT_cl_Frame::PreTranslateMessage( MSG *pMsg )
{
	if(pMsg->message == WM_KEYDOWN)
	{
		// touche raccourci quand en sub object mode
		if(DP()->Selection_b_IsInSubObjectMode() && DP()->mpo_ToolBoxDialog )
		{
			if ((GetAsyncKeyState(VK_CONTROL) >= 0) && (GetAsyncKeyState(VK_SHIFT) >= 0))
			{
				// raccourci 'B' : swap between sub object mode : vertex, edge, face
				if (pMsg->wParam == 'B') 
				{
					switch( DDD()->pst_EditOptions->ul_Flags & GRO_Cul_EOF_Mode )
					{
					case GRO_Cul_EOF_Vertex:((EDIA_cl_ToolBox_GROView *) DP()->mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_EdgeMode(); break;
					case GRO_Cul_EOF_Edge:	((EDIA_cl_ToolBox_GROView *) DP()->mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_FaceMode(); break;
					case GRO_Cul_EOF_Face:	((EDIA_cl_ToolBox_GROView *) DP()->mpo_ToolBoxDialog->mpo_ToolView[0])->OnRadio_VertexMode(); break;
					}
					LINK_Refresh();
					return 1;
				}
				
				// raccourci 'C' : swap between edge tool cut / turn
				/*
				if ( (pMsg->wParam == 'C') && ((DDD()->pst_EditOptions->ul_Flags & GRO_Cul_EOF_Mode) == GRO_Cul_EOF_Edge) )
				{
					if(DDD()->pst_EditOptions->i_EdgeTool == GRO_i_EOT_EdgeTurn)
						((EDIA_cl_ToolBox_GROView *) DP()->mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_CutEdge();
					else
						((EDIA_cl_ToolBox_GROView *) DP()->mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_TurnEdge();
					return 1;
				}
				*/
				
				// raccourci 'S' : smooth sel on / off
				/*
				if ( (pMsg->wParam == 'S') && ((DDD()->pst_EditOptions->ul_Flags & GRO_Cul_EOF_Mode) == GRO_Cul_EOF_Vertex) )
				{
					((EDIA_cl_ToolBox_GROView *) DP()->mpo_ToolBoxDialog->mpo_ToolView[0])->OnCheck_SmoothSel();
					return 1;
				}
				*/
			}
		}
	}
	return EDI_cl_BaseFrame::PreTranslateMessage(pMsg);
}
#endif

/*
 =======================================================================================================================
    Aim:    Call when editor is destroyed
 =======================================================================================================================
 */
void EOUT_cl_Frame::OnDestroy(void)
{
	LINK_DelRegisterPointer(DP()->mst_WinHandles.pst_DisplayData);
	if(DW()) WOR_Universe_DeleteWorld(DW(), 1);
	LINK_UpdatePointers();
}
#ifdef JADEFUSION
extern void SND_StopAll(void);
extern BOOL SND_gb_EdiPause;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::CloseWorld(void)
{
	/*~~~~~~*/
	int i_Res;
	/*~~~~~~*/

	if(DW())
	{
		if(DP()->mb_AnimOn)
			DP()->Selection_Anim();

		/*
		 * if((DP()->mo_UndoManager.mi_CurrentIndex !=
		 * DP()->mo_UndoManager.mi_MarkedIndex) || (DW()->c_HaveToBeSaved))
		 */
		if((DP()->b_RelevantChange()) || (DW()->c_HaveToBeSaved))
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	sz_Text[256];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(sz_Text, "Some changes weren't saved in World [%s], save now ?", DW()->sz_Name);
			i_Res = M_MF()->MessageBox(sz_Text, "Warning", MB_YESNOCANCEL | MB_ICONQUESTION);
			if(i_Res == IDYES)
			{
				DP()->SaveWorld();
				DP()->mo_UndoManager.Clean();
			}

			if(i_Res == IDCANCEL) return;
		}

		TEXT_FreeAll();
		mb_CanDestroy = FALSE;
		mul_CurrentWorld = BIG_C_InvalidIndex;
		mul_LoadedWorld = BIG_C_InvalidIndex;
		M_MF()->SendMessageToEditors(EOUT_MESSAGE_DESTROYWORLD, (ULONG) DW(), 0);

        // Set world pointers to NULL before deleting world to avoid bug related to delete order
		if(mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog) mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog->mpst_World = NULL;
		if(gspo_SectoDialog) gspo_SectoDialog->mpst_World = NULL;

		WOR_Universe_DeleteWorld(DW(), 1);
		WOR_Universe_Close(TRUE);
		DW() = NULL;
		RefreshMenu();
		OnSetMode();
		LINK_Refresh();
		mb_CanDestroy = TRUE;
		TAB_PFtable_RemoveHoles(&WOR_gst_Universe.st_WorldsTable);

		/* Update selection dialog */
		if(mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog)
			mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog->ClearList();

		/* Update GameMaterial dialog */
		if(mpo_EngineFrame->mpo_DisplayView->mpo_GameMaterialDialog)
		{
			mpo_EngineFrame->mpo_DisplayView->mpo_GameMaterialDialog->mpo_View->mul_Index = BIG_C_InvalidIndex;
			mpo_EngineFrame->mpo_DisplayView->mpo_GameMaterialDialog->mpo_View->UpdateList();
		}

		/* Update secto dialog */
		if(gspo_SectoDialog)
			gspo_SectoDialog->FillListSect();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::ChangeWorld(BIG_INDEX _ul_Index, BOOL _b_CanSend)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_NewWorld;
	char			asz_Name[BIG_C_MaxLenPath];
	ULONG			ul_IndexW, ul_SaveIndex;
	BOOL			b_WorldList;
	BOOL			b_Merge;
	LARGE_INTEGER	Start, End;
	LARGE_INTEGER	TicksPerSecond;
	INT64			Ticks;
	int				i_sec, i_min;
	char			sz_Msg[256];
	BOOL            b_LoadSuccessful = TRUE;


	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Merge = FALSE;
	if(_ul_Index == BIG_C_InvalidIndex) return;
	ul_SaveIndex = _ul_Index;
	mul_LoadedWorld = _ul_Index;

	QueryPerformanceFrequency( &TicksPerSecond );
	QueryPerformanceCounter( &Start );

#	ifdef JADEFUSION
	// Set world key that we are loading for light rejection manager
	//LRL_SetWorldKey(_ul_Index);
	// First time init of light rejection list for this map (integrated from POP5)
	LRL_Init( _ul_Index );

#		ifdef ACTIVE_EDITORS
	//Disable Highlight Mode
	DDD()->ul_HighlightMode = 0;

	//reset Gao Properties clipboard
	DDD()->b_PasteGaoProperties = FALSE;
	F3D_ResetGaoClipboard();
#		endif// ACTIVE_EDITORS
#	endif    // JADEFUSION

	BAS_binit(&WOR_ListAllKeys, 100);

	/* Is it a world list ? */
	b_WorldList = FALSE;
	if(BIG_b_IsFileExtension(_ul_Index, EDI_Csz_ExtWorldList)) b_WorldList = TRUE;

	AfxGetApp()->DoWaitCursor(1);

	/* Info in log */
	sprintf(asz_Name, "%s %s", EOUT_STR_Csz_LoadWorld, BIG_NameFile(_ul_Index));
	LINK_PrintStatusMsg(asz_Name);
    strcat(asz_Name,"\n");
    ERR_LogPrint(asz_Name);

	//Disable ODE Edition Mode
	DDD()->uc_EditODE = 0;

_Try_
	/* A merge or a replace */
	if(GetAsyncKeyState(VK_CONTROL) < 0)
	{
		b_Merge = TRUE;
		if(b_WorldList)
			pst_NewWorld = WOR_pst_Universe_MergeWorldList(DW(), BIG_FileKey(_ul_Index), 1);
		else
			pst_NewWorld = WOR_pst_Universe_MergeWorld(DW(), BIG_FileKey(_ul_Index), 1, TRUE);
	}
	else if(b_WorldList)
		pst_NewWorld = WOR_pst_Universe_AddWorldList(NULL, BIG_FileKey(_ul_Index), 1);
	else
	{
		WOR_pst_Universe_AddWorldList(NULL, -1, 1);
		pst_NewWorld = WOR_pst_Universe_AddWorld(BIG_FileKey(_ul_Index), 1, TRUE);
	}

	if(pst_NewWorld)
	{
		_ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_NewWorld);
		ERR_X_Assert(_ul_Index != BIG_C_InvalidIndex);

		/* Force world to be activate */
		ul_IndexW = TAB_ul_PFtable_GetElemIndexWithPointer(&WOR_gst_Universe.st_WorldsTable, pst_NewWorld);
		ERR_X_Assert(ul_IndexW != TAB_Cul_BadIndex);
		WOR_Universe_SetWorldActive(ul_IndexW);

		/* Add world to history */
		M_MF()->AddHistoryFile(this, BIG_FileKey(ul_SaveIndex));
		if(pst_NewWorld == MAI_gst_MainHandles.pst_World)
			mst_Ini.b_EngineDisplay = TRUE;
		else
			mst_Ini.b_EngineDisplay = FALSE;

		/* Attach the new one */
		mul_CurrentWorld = _ul_Index;
		if(_b_CanSend) M_MF()->SendMessageToLinks(this, EOUT_MESSAGE_CREATEWORLD, mul_CurrentWorld, 0);

		DW() = pst_NewWorld;
		if(pst_NewWorld == MAI_gst_MainHandles.pst_World)
		{
			mst_Ini.b_EngineDisplay = TRUE;
			DW() = NULL;
		}
		else
		{
			mst_Ini.b_EngineDisplay = FALSE;
		}

		/* Update selection dialog */
		if(mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog)
		{
			mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog->mpst_World = pst_NewWorld;
			mpo_EngineFrame->mpo_DisplayView->mpo_SelectionDialog->UpdateList();
		}

		/* Update secto dialog */
		if(gspo_SectoDialog)
		{
			gspo_SectoDialog->mpst_World = pst_NewWorld;
			gspo_SectoDialog->FillListSect();
		}
#ifdef JADEFUSION
		/* Update Light Rejection Dialog */
		if(mpo_EngineFrame->mpo_DisplayView->mpo_LightRejectDialog)
		{
			mpo_EngineFrame->mpo_DisplayView->mpo_LightRejectDialog->mpo_View = mpo_EngineFrame->mpo_DisplayView;
			mpo_EngineFrame->mpo_DisplayView->mpo_LightRejectDialog->Refresh();
		}
#endif
		/*
		 * If we have merged a world, we dont want to reset the current Mode whatever it
		 * was. We only want to reset the Zone Edition mode if we really change to another
		 * world.
		 */
		if(!b_Merge)
		{
			/* Reset of the Zone flags and quit the zone edition mode if needed. */
			mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_ShowZDM;
			mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_ShowZDE;
			mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData->ul_DisplayFlags &= ~GDI_Cul_DF_ShowCOB;
		}

		/* Load postit */
		mpo_EngineFrame->mpo_DisplayView->LoadPostIt();

#if defined(_XENON_RENDER)
        // SC: TODO: Re-enable
        //mpo_EngineFrame->mpo_DisplayView->CheckXMPConsistency();
#endif

		/* Refresh */
		RefreshMenu();
		OnSetMode();
		LINK_Refresh();
		M_MF()->DataHasChanged();
	}

    BAS_bfree(&WOR_ListAllKeys);

_Catch_
    b_LoadSuccessful = FALSE;
_End_
	AfxGetApp()->DoWaitCursor(-1);

    if (b_LoadSuccessful)
        ERR_LogPrint("\nWorld loaded successfully\n");
    else
        ERR_LogPrint("\nWorld not loaded\n");

	// display timing results
	QueryPerformanceCounter( &End );

	Ticks = End.QuadPart - Start.QuadPart;
	
	i_sec = (int)(Ticks / (DOUBLE)TicksPerSecond.QuadPart);
	i_min = i_sec / 60;
	
	sprintf(sz_Msg, "--- Loading Time : %02dm%02ds ---", i_min, i_sec%60);
	LINK_PrintStatusMsg(sz_Msg);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EOUT_cl_Frame::b_AcceptToCloseProject(void)
{
	/*~~~~~~*/
	int i_Res;
	/*~~~~~~*/

	if(DW())
	{
		/*
		 * if((DP()->mo_UndoManager.mi_CurrentIndex !=
		 * DP()->mo_UndoManager.mi_MarkedIndex) || (DW()->c_HaveToBeSaved))
		 */
		if((DP()->b_RelevantChange()) || (DW()->c_HaveToBeSaved))
		{
			/*~~~~~~~~~~~~~~~~~*/
			char	sz_Text[256];
			/*~~~~~~~~~~~~~~~~~*/

			sprintf(sz_Text, "Some changes wasn't saved in World [%s], save now ?", DW()->sz_Name);
			i_Res = M_MF()->MessageBox(sz_Text, "Warnainges", MB_YESNOCANCEL | MB_ICONQUESTION);
			if(i_Res == IDYES) DP()->SaveWorld();
			if(i_Res == IDCANCEL) return FALSE;
		}
	}

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Call to create the editor.
 =======================================================================================================================
 */
void EOUT_cl_Frame::OnSize(UINT n, int cx, int cy)
{
	EDI_cl_BaseFrame::OnSize(n, cx, cy);
	mpo_EngineFrame->UpdatePosSize();
}

/*$4
 ***********************************************************************************************************************
    INTERFACE
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::OneTrameEnding(void)
{
	/* Speed run : Lock to engine cam, hide all editor objects */
	if(ENG_bg_FirstFrameSpeedRun)
	{
		OnAction(EOUT_ACTION_DISPLAYHIDEALL);
		if ( !( DDD()->ul_DrawMask & GDI_Cul_DM_Fogged ) )
		{
			OnAction( EOUT_ACTION_DISPLAYNOFOG );
		}
		DDD()->uc_EngineCamera = 0;
		OnAction(EOUT_ACTION_ENGINECAM);

		// if no world is loaded, enable the 'active engine display' ~hogsy
		if ( !mst_Ini.b_EngineDisplay && DW() == nullptr )
		{
			OnAction( EOUT_ACTION_ENGINEDISPLAY );
		}
	}

	if(!mb_IsActivate) return;
	if(M_MF()->mpo_MaxView && M_MF()->mpo_MaxView != mpo_MyView && !mpo_MyView->mb_Floating) return;

	if(mpo_EngineFrame->mpo_DisplayView->mb_AutoZoomExtendOnRotate)
		DP()->ZoomExtendSelected(&DDD()->st_Camera, DW());

	/* Box ? */
	if(mpo_EngineFrame->mpo_DisplayView->mpo_ToolBoxDialog && mpo_EngineFrame->mpo_DisplayView->mb_ToolBoxOn)
		mpo_EngineFrame->mpo_DisplayView->mpo_ToolBoxDialog->OneTrameEnding();

	/* Box ? */
	if(mpo_EngineFrame->mpo_DisplayView->mpo_AnimDialog && mpo_EngineFrame->mpo_DisplayView->mb_AnimOn)
		mpo_EngineFrame->mpo_DisplayView->mpo_AnimDialog->OneTrameEnding();

	/*
	 * Force right world for engine display, cause we don't know if the first
	 * OnSetMode has been called before engine world creation
	 */
	if((DP()->mst_WinHandles.pst_DisplayData->pst_World != MAI_gst_MainHandles.pst_World) && mst_Ini.b_EngineDisplay)
	{
		OnSetMode();
		LINK_gx_PointersJustUpdated.SetAt(MAI_gst_MainHandles.pst_World, 0);
		LINK_UpdatePointers();
	}

	PRO_StopTrameRaster(&ENG_gpst_RasterEng_Editors);
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_Display);
	mpo_EngineFrame->mpo_DisplayView->Refresh();
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_Display);
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_Editors);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::OnActivate( void )
{
	EDI_cl_BaseFrame::OnActivate();

	if ( mpo_EngineFrame == nullptr )
	{
		return;
	}

	F3D_cl_View *view = mpo_EngineFrame->mpo_DisplayView;
	if ( view == nullptr )
	{
		return;
	}

	if ( view->mpo_AnimDialog && view->mb_AnimOn )
	{
		view->mpo_AnimDialog->ShowWindow( SW_SHOW );
	}
}

void EOUT_cl_Frame::OnDisactivate()
{
	EDI_cl_BaseFrame::OnDisactivate();

	if ( mpo_EngineFrame == nullptr )
	{
		return;
	}

	F3D_cl_View *view = mpo_EngineFrame->mpo_DisplayView;
	if ( view == nullptr )
	{
		return;
	}

	if ( view->mpo_AnimDialog )
	{
		view->mpo_AnimDialog->ShowWindow( SW_HIDE );
	}
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::OnRButtonDown(UINT nFlags, CPoint point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CRect							o_Rect;
	EMEN_cl_SubMenu					o_Menu(FALSE);
	OBJ_tdst_GameObject				*pst_GO;
	GEO_tdst_GraphicZone			*pst_GZ;
	CPoint							pt;
	SOFT_tdst_PickingBuffer_Pixel	*pst_Pixel;
	BOOL							b_Treat, b_Multi;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
	/* In 3D View ? */
	ClientToScreen(&point);
	mpo_EngineFrame->mpo_DisplayView->GetWindowRect(&o_Rect);

	if(!o_Rect.PtInRect(point)) return;

	pt = point;
	mpo_EngineFrame->mpo_DisplayView->ScreenToClient(&pt);
	DP()->Pick_l_UnderPoint(&pt, SOFT_Cuc_PBQF_All, 0);

	/* First, we look if there already something selected. */
	pst_Pixel = DP()->Pick_pst_GetFirst(SOFT_Cuc_PBQF_All, 1);
	if(!pst_Pixel)
	{
		b_Treat = TRUE;
		pst_Pixel = DP()->Pick_pst_GetFirst(SOFT_Cuc_PBQF_All, 0);
		if(!pst_Pixel) return;
	}
	else
		b_Treat = FALSE;

	

	if(pst_Pixel->ul_ValueExt & SOFT_Cuc_PBQF_Zone)
	{
		/*~~~~~~~~~~~~*/
		LONG	l_Param;
		UCHAR	uc_Type;
		/*~~~~~~~~~~~~*/

		InitPopupMenuAction(&o_Menu);

		/* Gets the GraphicZone from the SoftPickingBuffer. */
		pst_GZ = mpo_EngineFrame->mpo_DisplayView->mst_WinHandles.pst_DisplayData->st_DisplayedZones.dpst_GraphicZone + (ULONG) pst_Pixel->ul_Value;

		/* Gets the other info stored in the SoftPickingBuffer. */
		l_Param = (LONG) (pst_GZ->pst_GO);
		uc_Type = pst_GZ->uc_Type;

		switch(uc_Type)
		{
		case GEO_Cul_GraphicZDx:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				COL_tdst_ZDx		*pst_ZDx;
				COL_tdst_Instance	*pst_Instance;
				UCHAR				uc_ENG_Index;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_ZDx = (COL_tdst_ZDx *) pst_GZ->pv_Data;
				if(!pst_ZDx || !pst_ZDx->pst_GO) return;
				pst_Instance = ((COL_tdst_Base *) ((OBJ_tdst_GameObject *) pst_ZDx->pst_GO)->pst_Extended->pst_Col)->pst_Instance;
				uc_ENG_Index = COL_uc_Instance_GetEngineIndexWithZone(pst_Instance, pst_ZDx);

				/* Fills the Selection. */
				if(DP()->Selection_b_Treat(pst_ZDx, l_Param, SEL_C_SIF_ZDx, 0))
				{
					LINK_Refresh();
					LINK_UpdatePointers();
					RefreshMenu();
				}

				M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, pst_ZDx->pst_GO->sz_Name);
				if(pst_ZDx->uc_AI_Index < 16)
				{
					M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, F3D_STR_Csz_Zone_AI[pst_ZDx->uc_AI_Index]);
				}
				else
					M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "Invalid Index");
				M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_RENAME);
				if(COL_b_Instance_IsActive(pst_Instance, uc_ENG_Index))
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_DESACTIVATE);
				else
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_ACTIVATE);
				if(COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_Specific))
				{
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_SHARED);
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_CHECKBV);
				}
				else
				{
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_SPECIFIC);
					M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "");
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_DELETE);
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_SETINDEX);
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_CHECKBV);
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_EDITCOLSET);
				}
			}
			break;

		case GEO_Cul_GraphicCob:
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				COL_tdst_ColMap *pst_ColMap;
				COL_tdst_Cob	*pst_Cob;
				ULONG			i, ul_Cob;
				char			asz_Line[100];
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_Cob = (COL_tdst_Cob *) pst_GZ->pv_Data;
				if(pst_GZ->pst_GO && pst_GZ->pst_GO->pst_Extended && pst_GZ->pst_GO->pst_Extended->pst_Col)
				{
					pst_ColMap = ((COL_tdst_Base *) pst_GZ->pst_GO->pst_Extended->pst_Col)->pst_ColMap;
					if(pst_ColMap)
					{
						for(i = 0; i < pst_ColMap->uc_NbOfCob; i++)
						{
							if(pst_ColMap->dpst_Cob[i] == pst_Cob)
							{
								ul_Cob = i;
							}
						}
					}

					if(pst_ColMap->dpst_Cob && pst_ColMap->dpst_Cob[ul_Cob] && pst_ColMap->dpst_Cob[ul_Cob]->pst_GO)
					{
						sprintf(asz_Line, "* %s *", pst_ColMap->dpst_Cob[ul_Cob]->pst_GO->sz_Name);
						M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, asz_Line);
					}

					sprintf(asz_Line, "Cob n° %u", ul_Cob);
					M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, asz_Line);

					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_RENAME);

					if(COL_b_ColMap_IsActive(pst_ColMap, (UCHAR) ul_Cob))
						M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_DESACTIVATE);
					else
						M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_ACTIVATE);

					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_WIRE);
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_UPDATEOK3);
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_CREATEGRO);
					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_DUPLICATECOB);

					if
					(
						pst_ColMap->dpst_Cob
					&&	pst_ColMap->dpst_Cob[ul_Cob]
					&&	pst_ColMap->dpst_Cob[ul_Cob]->pst_GO
					&&	!(pst_ColMap->dpst_Cob[ul_Cob]->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
					) M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_COMPUTEBV);
					else
						M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_CHECKBV);

					M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_SELECTOBJECT);

				}

				/* Fills the Selection. */
				if(DP()->Selection_b_Treat(pst_Cob, l_Param, SEL_C_SIF_Cob, 0))
				{
					LINK_Refresh();
					LINK_UpdatePointers();
					RefreshMenu();
				}
			}
			break;
		}

		TrackPopupMenuAction(point, &o_Menu);
	}
	else if(pst_Pixel->ul_ValueExt & SOFT_Cuc_PBQF_GameObject)
	{
		if(DP()->mpst_BonePickObj)
			pst_GO = DP()->mpst_BonePickObj;
		else
			pst_GO = (OBJ_tdst_GameObject *) pst_Pixel->ul_Value;
		if
		(
			b_Treat
		&&	DP()->Selection_b_Treat
				(
					pst_GO,
					pst_Pixel->ul_ValueExt,
					SEL_C_SIF_Object,
					(nFlags & MK_CONTROL) ? TRUE : FALSE,
					TRUE
				)
		)
		{
			LINK_Refresh();
			LINK_UpdatePointers();
			RefreshMenu();
		}

		b_Multi = SEL_l_CountItem(DW()->pst_Selection, SEL_C_SIF_Object) > 1;
		OnGaoSelect(pst_GO, b_Multi, point);
	}
}

void EOUT_cl_Frame::OnMoving( UINT, LPRECT )
{
	mpo_EngineFrame->UpdatePosSize();
}

void EOUT_cl_Frame::OnGaoSelect(OBJ_tdst_GameObject*pst_GO, BOOL b_Multi, CPoint point)
{
    BOOL b_SndCapacity;
    OBJ_tdst_GameObject				*pst_GOFather;
	EMEN_cl_SubMenu					o_Menu(FALSE);


	/* Gizmo on an animation */
	if(ANI_b_IsGizmoAnim(pst_GO, &pst_GOFather))
	{
		DP()->PopupAnimation(pst_GOFather, pst_GO);
		return;
	}

	InitPopupMenuAction(&o_Menu);

    /* snd test */
    b_SndCapacity = FALSE;
    if(pst_GO->pst_Extended)
    {
        if(pst_GO->pst_Extended->pst_Sound)
            b_SndCapacity = TRUE;
    }
    

	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, b_Multi ? "Multi selection" : pst_GO->sz_Name);
	M_MF()->AddPopupMenuAction(NULL, &o_Menu, EOUT_ACTION_COPYGAOREF);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_SETASINITIALPOSITION);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_RESTOREINITIALPOSITION);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ORIENTTOCHILD);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ORIENTPIVOTTOCHILD);
    if (!b_Multi)
    {
        M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ISLIGHTEDBY );
        M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_GOINFORMATION );
    }
    M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "Prefab");
    M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_PREFABADDUNSORTED);
    M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_PREFABADD);
    M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_PREFABMERGE);
    M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_PREFABSELOBJ);
    M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_PREFABUPDATE);
    if (pst_GO->ul_PrefabKey != BIG_C_InvalidIndex || pst_GO->ul_PrefabObjKey != BIG_C_InvalidIndex)
        M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_PREFABCUTLINK);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "");
	if(!b_Multi) M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_RENAME);
	if(!b_Multi) M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_CAMASOBJ);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_HIDE);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_DISPLAY);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "Col / GameMat");
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_CREATECOLSET);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_CREATECOB);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_ZONE_CREATECOLMAP);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_UPDATECOLMAP);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_UPDATECOLMAPKEEPIDMAT);
    M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_DESTROYGAMEMAT);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_DISPLAYSKELOBBOX);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_LOGUNCOLLIDABLE);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "Events");
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_CREATEEVENTS);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_DELETEEVENTS);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_CREATEANIMATION);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "Network");
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_SELECTNET);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_SELECTLINKS);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_SETROOT);
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_CREATEFROMSELECTION);
	M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "Mad");
	M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_EXPORTQUICK);
    M_MF()->AddPopupMenuAction(this, &o_Menu, 0, TRUE, "Sound");
	if(b_SndCapacity && !b_Multi) 
		M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_SNDSETSOLO);
	if(b_SndCapacity && !b_Multi) 
		M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_SNDSETMUTE);
    M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_SNDSOLOOFF);
    M_MF()->AddPopupMenuAction(this, &o_Menu, EOUT_ACTION_SNDMUTEOFF);
    

	TrackPopupMenuAction(point, &o_Menu);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::OnSetMode(void)
{
	if(mst_Ini.b_EngineDisplay)
	{
		if(DW() == NULL || DW() != MAI_gst_MainHandles.pst_World)
		{
			/* Attach world to display data : create texture */
			GDI_l_AttachWorld(DP()->mst_WinHandles.pst_DisplayData, MAI_gst_MainHandles.pst_World);

			DW() = MAI_gst_MainHandles.pst_World;

			mul_CurrentWorld = BIG_C_InvalidIndex;
			mul_LoadedWorld = BIG_C_InvalidIndex;
			if(MAI_gst_MainHandles.pst_World)
			{
				mul_CurrentWorld = LOA_ul_SearchKeyWithAddress((ULONG) MAI_gst_MainHandles.pst_World);
				ERR_X_Assert(mul_CurrentWorld != BIG_C_InvalidIndex);
				mul_CurrentWorld = BIG_ul_SearchKeyToFat(mul_CurrentWorld);
				ERR_X_Assert(mul_CurrentWorld != BIG_C_InvalidIndex);
			}
		}
	}
	else
	{
		GDI_l_AttachWorld(DP()->mst_WinHandles.pst_DisplayData, DP()->mst_WinHandles.pst_World);
	}

	if(DW())
	{
		/* Copy matrix saved in world into camera matrix */
		MATH_CopyMatrix(&DP()->mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix, &DW()->st_CameraPosSave);
	}

	DP()->mo_UndoManager.Clean();
	DP()->Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::SetWidth(int cx)
{
	/*~~~~~~~~~~~~~*/
	CRect	o_Rect;
	CRect	o_Rect1;
	CWnd	*po_View;
	/*~~~~~~~~~~~~~*/

	if(cx < 10) cx = 10;

	po_View = GetParent();
	GetWindowRect(&o_Rect);
	po_View->ScreenToClient(&o_Rect);
	po_View->GetClientRect(&o_Rect1);
	cx += o_Rect1.Width() - o_Rect.Width();

	/* Upper left */
	if(po_View == M_MF()->mo_Splitter1.GetPane(0, 0))
	{
		if(M_MF()->mo_Splitter1.GetRowCount() == 2)
		{
			M_MF()->mo_Splitter.SetColumnInfo(0, cx, 0);
			M_MF()->mo_Splitter.SetColumnInfo(1, 0, 0);
		}
		else
		{
			M_MF()->mo_Splitter1.SetColumnInfo(0, cx, 0);
			M_MF()->mo_Splitter1.SetColumnInfo(1, 0, 0);
		}
	}

	/* Upper right */
	else if
		(
			((M_MF()->mo_Splitter1.GetRowCount() == 2) && (po_View == M_MF()->mo_Splitter1.GetPane(1, 0)))
		||	((M_MF()->mo_Splitter1.GetRowCount() == 1) && (po_View == M_MF()->mo_Splitter1.GetPane(0, 1)))
		)
	{
		if(M_MF()->mo_Splitter1.GetRowCount() == 2)
		{
			M_MF()->mo_Splitter.SetColumnInfo(0, cx, 0);
			M_MF()->mo_Splitter.SetColumnInfo(1, 0, 0);
		}
		else
		{
			M_MF()->mo_Splitter1.GetClientRect(&o_Rect);
			M_MF()->mo_Splitter1.SetColumnInfo(0, o_Rect.Width() - cx - SPLIT_WIDTH - 4, 0);
			M_MF()->mo_Splitter1.SetColumnInfo(1, 0, 0);
		}
	}

	/* Bottom left */
	else if(po_View == M_MF()->mo_Splitter2.GetPane(0, 0))
	{
		if(M_MF()->mo_Splitter2.GetRowCount() == 2)
		{
			M_MF()->mo_Splitter.GetClientRect(&o_Rect);
			M_MF()->mo_Splitter.SetColumnInfo(0, o_Rect.Width() - cx - SPLIT_WIDTH - 4, 0);
			M_MF()->mo_Splitter.SetColumnInfo(1, 0, 0);
		}
		else
		{
			M_MF()->mo_Splitter2.SetColumnInfo(0, cx, 0);
			M_MF()->mo_Splitter2.SetColumnInfo(1, 0, 0);
		}
	}

	/* Bottom right */
	else if
		(
			((M_MF()->mo_Splitter2.GetRowCount() == 2) && (po_View == M_MF()->mo_Splitter2.GetPane(1, 0)))
		||	((M_MF()->mo_Splitter2.GetRowCount() == 1) && (po_View == M_MF()->mo_Splitter2.GetPane(0, 1)))
		)
	{
		if(M_MF()->mo_Splitter2.GetRowCount() == 2)
		{
			M_MF()->mo_Splitter.GetClientRect(&o_Rect);
			M_MF()->mo_Splitter.SetColumnInfo(0, o_Rect.Width() - cx - SPLIT_WIDTH - 4, 0);
			M_MF()->mo_Splitter.SetColumnInfo(1, 0, 0);
		}
		else
		{
			M_MF()->mo_Splitter2.GetClientRect(&o_Rect);
			M_MF()->mo_Splitter2.SetColumnInfo(0, o_Rect.Width() - cx - SPLIT_WIDTH - 4, 0);
			M_MF()->mo_Splitter2.SetColumnInfo(1, 0, 0);
		}
	}

	M_MF()->mo_Splitter.RecalcLayout();
	M_MF()->mo_Splitter1.RecalcLayout();
	M_MF()->mo_Splitter2.RecalcLayout();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::SetHeight(int cy)
{
	/*~~~~~~~~~~~~~*/
	CRect	o_Rect;
	CRect	o_Rect1;
	CWnd	*po_View;
	/*~~~~~~~~~~~~~*/

	if(cy < 10) cy = 10;

	po_View = GetParent();
	GetWindowRect(&o_Rect);
	po_View->ScreenToClient(&o_Rect);
	po_View->GetClientRect(&o_Rect1);
	cy += o_Rect1.Height() - o_Rect.Height();

	/* Upper left */
	if(po_View == M_MF()->mo_Splitter1.GetPane(0, 0))
	{
		if(M_MF()->mo_Splitter1.GetRowCount() == 2)
		{
			M_MF()->mo_Splitter1.SetRowInfo(0, cy, 0);
			M_MF()->mo_Splitter1.SetRowInfo(1, 0, 0);
		}
		else
		{
			M_MF()->mo_Splitter.SetRowInfo(0, cy, 0);
			M_MF()->mo_Splitter.SetRowInfo(1, 0, 0);
		}
	}

	/* Upper right */
	else if
		(
			((M_MF()->mo_Splitter1.GetRowCount() == 2) && (po_View == M_MF()->mo_Splitter1.GetPane(1, 0)))
		||	((M_MF()->mo_Splitter1.GetRowCount() == 1) && (po_View == M_MF()->mo_Splitter1.GetPane(0, 1)))
		)
	{
		if(M_MF()->mo_Splitter1.GetRowCount() == 2)
		{
			M_MF()->mo_Splitter1.GetClientRect(&o_Rect);
			M_MF()->mo_Splitter1.SetRowInfo(0, o_Rect.Height() - cy - SPLIT_WIDTH - 4, 0);
			M_MF()->mo_Splitter1.SetRowInfo(1, 0, 0);
		}
		else
		{
			M_MF()->mo_Splitter.SetRowInfo(0, cy, 0);
			M_MF()->mo_Splitter.SetRowInfo(1, 0, 0);
		}
	}

	/* Bottom left */
	else if(po_View == M_MF()->mo_Splitter2.GetPane(0, 0))
	{
		if(M_MF()->mo_Splitter2.GetRowCount() == 2)
		{
			M_MF()->mo_Splitter2.SetRowInfo(0, cy, 0);
			M_MF()->mo_Splitter2.SetRowInfo(1, 0, 0);
		}
		else
		{
			M_MF()->mo_Splitter.GetClientRect(&o_Rect);
			M_MF()->mo_Splitter.SetRowInfo(0, o_Rect.Height() - cy - SPLIT_WIDTH - 4, 0);
			M_MF()->mo_Splitter.SetRowInfo(1, 0, 0);
		}
	}

	/* Bottom right */
	else if
		(
			((M_MF()->mo_Splitter2.GetRowCount() == 2) && (po_View == M_MF()->mo_Splitter2.GetPane(1, 0)))
		||	((M_MF()->mo_Splitter2.GetRowCount() == 1) && (po_View == M_MF()->mo_Splitter2.GetPane(0, 1)))
		)
	{
		if(M_MF()->mo_Splitter2.GetRowCount() == 2)
		{
			M_MF()->mo_Splitter2.GetClientRect(&o_Rect);
			M_MF()->mo_Splitter2.SetRowInfo(0, o_Rect.Height() - cy - SPLIT_WIDTH - 4, 0);
			M_MF()->mo_Splitter2.SetRowInfo(1, 0, 0);
		}
		else
		{
			M_MF()->mo_Splitter.GetClientRect(&o_Rect);
			M_MF()->mo_Splitter.SetRowInfo(0, o_Rect.Height() - cy - SPLIT_WIDTH - 4, 0);
			M_MF()->mo_Splitter.SetRowInfo(1, 0, 0);
		}
	}

	M_MF()->mo_Splitter.RecalcLayout();
	M_MF()->mo_Splitter1.RecalcLayout();
	M_MF()->mo_Splitter2.RecalcLayout();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
extern void LOA_AddRefInLoadedList(void);
extern BOOL LOA_gb_SpeedMode;
extern void WOR_Check_InfoPhoto( WOR_tdst_World *, int );
extern void AI2C_GenDllSourceFile(void);
extern void GEO_DoOptimzeCacheForPS2(void);
extern int WOR_gi_CurrentConsole ;
extern void AI2C_SaveCurrentModelsList(void);
extern void INO_SaveLanguageFile(void);
extern void ESON_SaveSoundReport(void);
extern ULONG BIG_gul_GlobalKey;
#else
extern "C" void LOA_AddRefInLoadedList(void);
extern "C" BOOL LOA_gb_SpeedMode;
extern "C" void WOR_Check_InfoPhoto( WOR_tdst_World *, int );
extern "C" void AI2C_GenDllSourceFile(void);
extern void GEO_DoOptimzeCacheForPS2(void);
extern "C" int WOR_gi_CurrentConsole ;
extern "C" void AI2C_SaveCurrentModelsList(void);
extern "C" void INO_SaveLanguageFile(void);
extern "C" void ESON_SaveSoundReport(void);
extern "C" ULONG BIG_gul_GlobalKey;
#endif

extern BOOL EDI_gb_SaveSoundReport;
extern char *EDI_gpz_SlashP;
extern char *EDI_gpz_SlashPG;

void EOUT_cl_Frame::OnRealIdle(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_File;
	FILE		*file;
	char		asz_Path[BIG_C_MaxLenPath];
	extern BOOL EDI_gb_CanBin;
	extern BOOL EDI_gb_CleanBinFiles;
    int			i;
	char		*pc;
	ULONG		len;
	char		c, *p;
	static int	log = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	// Generation d'une marque
	if(EDI_gpz_SlashP)
	{
		SAV_Begin("Root", " ");

		// Mark
		i = 0;
		while(*EDI_gpz_SlashP)
		{
			c = (char) (*EDI_gpz_SlashP++ + i++);
			SAV_Buffer(&c, 1);
		}

		c = 0;
		SAV_Buffer(&c, 1);
		c = 0;
		SAV_Buffer(&c, 1);
		p = (char *) malloc(50000);
		for(i = 0; i < 50000; i++) p[i] = (char) rand();
		SAV_Buffer(p, 50000);
		free(p);

		BIG_gb_CanAskKey = FALSE;
		BIG_gul_CanAskKey = 0xFFC0D666;
		if(BIG_ul_SearchKeyToFat(0xFFC0D666) != BIG_C_InvalidIndex) 
		{
			ERR_X_Error(0, "Clef deja allouée !", NULL);
	        ExitProcess(0);
		}

		SAV_ul_End();
        ExitProcess(0);
	}

	// Affichage d'une marque
	if(EDI_gpz_SlashPG)
	{
		ul_File = BIG_ul_SearchKeyToFat(0xFFC0D666);
		if(ul_File == BIG_C_InvalidIndex)
		{
			ERR_X_Error(0, "Clef non trouvée !", NULL);
	        ExitProcess(0);
		}

		pc = BIG_pc_ReadFileTmp(BIG_PosFile(ul_File), &len);
		file = fopen("Pach Log.txt", "wb");
		if(!file) ExitProcess(0);

		// Mark
		i = 0;
		while(*pc)
		{
			c = (char) ((*pc++) - i++);
			fputc(c, file);
		}

		// Reste
		fputc('\r', file);
		fputc('\n', file);
		pc++;
		while(*pc)
		{
			i = 0;
			while(*pc)
			{
				c = *pc - i++;
				pc++;
				fputc(c, file);
			}

			pc++;
			fputc('\r', file);
			fputc('\n', file);
		}

		fclose(file);
        ExitProcess(0);
	}

	if(!EDI_gb_CanBin) return;
	if(EDI_gb_SlashL || EDI_gb_CleanBinFiles)
	{
		/*~~~~~~~~~~~~~~~~~*/
		char		*pz_Temp;
		static int	irr = 0;
		/*~~~~~~~~~~~~~~~~~*/

		if(irr) return;
		irr = 1;
#ifdef JADEFUSION
        if (EDI_gb_SlashLKTM)
        {
            strcpy(asz_Path, EDI_Csz_Path_Objects);
        }
        else
#endif
		{
			strcpy(asz_Path, EDI_Csz_Path_Levels);
		}
		strcat(asz_Path, "/");
		strcat(asz_Path, EDI_gaz_SlashL);
		pz_Temp = L_strrchr(asz_Path, '/');
		if(pz_Temp)
		{
			*pz_Temp = 0;
			ul_File = BIG_ul_SearchFileExt(asz_Path, pz_Temp + 1);
			if(ul_File != BIG_C_InvalidIndex)
			{
                if(EDI_gb_CleanBinFiles)
                {
                    BIG_CleanBinFiles(BIG_FileKey(ul_File));
                    ExitProcess(0);
                }
                else
                {
				    LINK_PrintStatusMsg("Loading world...");
				    i_OnMessage(EDI_MESSAGE_SELFILE, 0, ul_File);
				    if(!EDI_gb_NoVerbose) ERR_TestWarning();
				    EDI_gb_SlashLLoaded = TRUE;
					
					if(EDI_gb_SaveSoundReport)
						ESON_SaveSoundReport();
					

					if(!EDI_gi_GenSpe)
				    {

						if(EDI_gb_ComputeMap)
					    {
#ifdef BENCH_IA_TEST
							AIBnch_Close();
#endif
                            AI2C_GenDllSourceFile();
							AI2C_SaveCurrentModelsList();

						    LINK_PrintStatusMsg("Computing strips...");
						    // ComputeLODStrips(TRUE); // Compute Strips + LOD
#if defined(_XENON_RENDER)
                            // No need to compute strips for Xenon...
                            if (0) // WOR_gi_CurrentConsole != 3)
#endif
                            {
                                ComputeStrips(TRUE); // Compute Strips
                            }

#if defined(_XENON_RENDER)
                            // Make sure the tangent space smoothing groups are applied
                            DP()->XeComputeTangentSpaceSmoothing(0xffffffff);
#endif

                            if(WOR_gi_CurrentConsole == 1)
                            {
                                LINK_PrintStatusMsg("Reparse GEO object for PS2...");
                                GEO_DoOptimzeCacheForPS2();
//                              LINK_PrintStatusMsg("Recompute OK3 for PS2...");
//								COL_RecomputeAllVisualOK3(DP()->mst_WinHandles.pst_World);
                            }

                            //WOR_Check_InfoPhoto( DP()->mst_WinHandles.pst_World , 1 );

                            LINK_PrintStatusMsg("Saving world...");
						    DP()->SaveWorld();


					    }
#ifdef JADEFUSION
                        // Check consistency on the Xenon processed meshes
                        if (EDI_gb_SlashLXMP)
                        {
                            DP()->CheckXMPConsistency();
                            DP()->SaveWorld();
                        }

                        if (EDI_gb_SlashLXPK)
                        {
#if defined(_XENON_RENDER)
                            // Make sure the tangent space smoothing groups are applied
                            DP()->XeComputeTangentSpaceSmoothing(0xffffffff);
#endif
                            DP()->SaveWorld();
                        }
#endif
                        if(LOA_gb_SpeedMode)
                        {
                            LOA_AddRefInLoadedList();
                        }
#ifndef JADEFUSION					    
                        ExitProcess(0);
#endif
					}
					else
					{
						INO_SaveLanguageFile();
					}
#ifdef JADEFUSION
                    if(EDI_gb_SlashLoadAndQuit || EDI_gb_SlashLKTM || EDI_gb_SlashLXMP || EDI_gb_SlashLXPK)
#else
					if(EDI_gb_SlashLoadAndQuit)
#endif
					{
                        ExitProcess(0);
                    }
                }
			}
		}
	}
#ifdef JADEFUSION
    else if (EDI_gb_SlashXPK)
    {
        // Pack the meshes in the basic worlds
        PackBasicWorlds();
        ExitProcess(0);
    }
    else if (EDI_gb_SlashXTX)
    {
        // Generate all textures
        Xe_GenerateAllDds();
        ExitProcess(0);
    }
#endif
	// Log utilisation
	if(!log)
	{
		log = 1;
		LOG_Use(BIG_gst.asz_Name);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::CamPlan(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CAM_tdst_Camera		mst_Cam;
	ULONG				ul_DM, ul_DF;
	BOOL				b_PrevSect;
	CPoint				pt;
	CRect				o_Rect;
	GDI_tdst_Device		*pst_Dev;
	MATH_tdst_Vector	v;
	BOOL				b_Res;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Position cam */
	MATH_CopyVector(&DW()->st_Origin[0], &DP()->mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix.T);
	MATH_CopyVector
	(
		&DW()->st_Vector[0],
		(MATH_tdst_Vector *) &DP()->mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix.Kx
	);
	DW()->aul_Color[0] = 0x00FFFFFF;
    DW()->ap_VectorGAO[0] = NULL;
	DW()->uc_Vector = 1;

	LINK_gb_AllRefreshEnable = FALSE;
	L_memcpy(&mst_Cam, &DDD()->st_Camera, sizeof(mst_Cam));
	DDD()->st_Camera.ul_Flags &= ~CAM_Cul_Flags_Perspective;
	DP()->Viewpoint_SetStandard(F3D_StdView_Top);
	DP()->ZoomExtendAll(&DDD()->st_Camera, DW());
	ul_DM = DDD()->ul_DrawMask;
	ul_DF = DDD()->ul_DisplayFlags;
	DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayCamera;
	DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplaySnd;
	DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayLight;
	DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayWaypoint;
	DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayWaypointInNetwork;
	DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayInvisible;
	DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowZDM;
	DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowZDE;
	DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_ShowCOB;
	DDD()->ul_DisplayFlags &= ~GDI_Cul_DF_DisplayHierarchy;
	DDD()->ul_DisplayFlags |= GDI_Cul_DF_ShowAIDebugVectors;
	DDD()->ul_DrawMask &= ~GDI_Cul_DM_Fogged;
	b_PrevSect = ENG_gb_ActiveSectorization;
	ENG_gb_ActiveSectorization = FALSE;
	LINK_gb_AllRefreshEnable = TRUE;

	LINK_Refresh();

	/* Loop until key is released */
	DP()->GetClientRect(&o_Rect);
	while(GetAsyncKeyState(VK_ADD) < 0)
	{
		b_Res = FALSE;
		GetCursorPos(&pt);
		DP()->ScreenToClient(&pt);
		if(!o_Rect.PtInRect(pt)) continue;

		pst_Dev = &DP()->mst_WinHandles.pst_DisplayData->st_Device;
		v.x = (float) pt.x / (float) pst_Dev->l_Width;
		v.y = 1.0f - ((float) pt.y / (float) pst_Dev->l_Height);
		if(DP()->b_PickDepth(&v, &v.z)) b_Res = TRUE;
		v.x = (float) pt.x;
		v.y = (float) pt.y;
		CAM_2Dto3D(&DP()->mst_WinHandles.pst_DisplayData->st_Camera, &v, &v);
		v.z += 2.0f;
	}

	DW()->uc_Vector = 0;
	L_memcpy(&DDD()->st_Camera, &mst_Cam, sizeof(mst_Cam));
	if(b_Res)
	{
		DDD()->st_Camera.st_Matrix.Kz = 0;
		MATH_NormalizeEqualVector((MATH_tdst_Vector *) &DDD()->st_Camera.st_Matrix.Kx);
		MATH_CrossProduct
		(
			(MATH_tdst_Vector *) &DDD()->st_Camera.st_Matrix.Jx,
			(MATH_tdst_Vector *) &DDD()->st_Camera.st_Matrix.Kx,
			(MATH_tdst_Vector *) &DDD()->st_Camera.st_Matrix.Ix
		);
		MATH_CopyVector(&DDD()->st_Camera.st_Matrix.T, &v);
	}
	else
	{
		L_memcpy(&DDD()->st_Camera, &mst_Cam, sizeof(mst_Cam));
	}

	DDD()->ul_DisplayFlags = ul_DF;
	DDD()->ul_DrawMask = ul_DM;
	ENG_gb_ActiveSectorization = b_PrevSect;
	LINK_Refresh();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::CamIso(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_dist, focal;
	MATH_tdst_Vector	v, v1;
	CRect				mo_ZoomRect;
	CAM_tdst_Camera		*pst_Cam;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Cam = &DDD()->st_Camera;
	if(pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
	{
		DP()->GetClientRect(&mo_ZoomRect);

		v.x = (float) mo_ZoomRect.left;
		v.x = (v.x / DP()->mst_WinHandles.pst_DisplayData->st_Device.l_Width);
		v.y = (float) mo_ZoomRect.top;
		v.y = 1.0f - (v.y / DP()->mst_WinHandles.pst_DisplayData->st_Device.l_Height);
		v1.x = (float) mo_ZoomRect.right;
		v1.x = (v1.x / DP()->mst_WinHandles.pst_DisplayData->st_Device.l_Width);
		v1.y = (float) mo_ZoomRect.bottom;
		v1.y = 1.0f - (v1.y / DP()->mst_WinHandles.pst_DisplayData->st_Device.l_Height);

		if(!DP()->b_PickOozRectPond(&v, &v1, &f_dist))
		{
			pst_Cam->ul_Flags &= ~CAM_Cul_Flags_Perspective;
			pst_Cam->f_FactorX = 1;
			pst_Cam->f_FactorY = 1;
			pst_Cam->f_IsoZoom = 1;
			return;
		}

		f_dist = 1.0f / f_dist;
		focal = pst_Cam->f_FieldOfVision;
		pst_Cam->ul_Flags &= ~CAM_Cul_Flags_Perspective;
		pst_Cam->f_IsoZoom = f_dist / (2.0f * pst_Cam->f_IsoFactor);
	}
	else
		pst_Cam->ul_Flags |= CAM_Cul_Flags_Perspective;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static EDIA_cl_UPDATEDialog *po_Dial;

void STRIP_SetBarreState(float Position)
{
	po_Dial->OnRefreshBarText(Position, "Compute");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EOUT_cl_Frame::ComputeStrips(BOOL _b_Strip)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	GRO_tdst_Struct		*pst_Gro;
	char				str[200];
	char				az[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = DW();
	if(!pst_World) return;

	po_Dial = new EDIA_cl_UPDATEDialog((char *) "");
	po_Dial->DoModeless();

	pst_AIEOT = &(pst_World->st_AllWorldObjects);
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GAO)) continue;
		if(!(pst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) continue;

		if(!pst_GAO->pst_Base) return;
		if(!pst_GAO->pst_Base->pst_Visu) return;

		pst_Gro = pst_GAO->pst_Base->pst_Visu->pst_Object;
		if(pst_Gro)
		{
			if(pst_Gro->i->ul_Type == GRO_Geometric)
			{
#if defined(_XENON_RENDER)
                if ((pst_GAO->pst_Base) && (pst_GAO->pst_Base->pst_Visu))
                {
                    GEO_ClearXenonMesh(pst_GAO->pst_Base->pst_Visu, (GEO_tdst_Object*)pst_Gro, FALSE, FALSE);
                }
#endif
				if(_b_Strip)
				{
					if(GEO_STRIP_Compute(pst_GAO,(GEO_tdst_Object *) pst_Gro, pst_GAO->pst_Base->pst_Visu->dul_VertexColors, FALSE , STRIP_SetBarreState ))
					{
						GEO_STRIP_ComputeStat((GEO_tdst_Object *) pst_Gro, str);
						GEO_STRIP_UnSetFlag(GEO_C_Strip_StatData, (GEO_tdst_Object *) pst_Gro);

						sprintf(az, "Strip %s (%s)", pst_GAO->sz_Name, str);
						LINK_PrintStatusMsg(az);
					}
					else if(!EDI_gb_NoVerbose)
					{
						sprintf(az, "Strip %s : The object doesn't have points, can't do this...", pst_GAO->sz_Name);
						ERR_X_Warning(0, az, NULL);
					}
				}
				else
				{
					if(((GEO_tdst_Object *) pst_Gro)->ulStripFlag & GEO_C_Strip_DataValid)
						GEO_STRIP_Delete((GEO_tdst_Object *) pst_Gro);
				}
#if defined(_XENON_RENDER)
                if ((pst_GAO->pst_Base) && (pst_GAO->pst_Base->pst_Visu))
                {
                    GEO_ResetXenonMesh(pst_GAO, pst_GAO->pst_Base->pst_Visu, (GEO_tdst_Object*)pst_Gro, FALSE, TRUE, FALSE);
                }
#endif
			}
			if(pst_Gro->i->ul_Type == GRO_GeoStaticLOD)
			{
				GEO_tdst_StaticLOD *p_stGEOLOD;
				ULONG LODCOUNTER;
				p_stGEOLOD = (GEO_tdst_StaticLOD *)pst_Gro;
				LODCOUNTER = p_stGEOLOD->uc_NbLOD;
				while (LODCOUNTER --)
				{
					if (p_stGEOLOD->dpst_Id[LODCOUNTER]->i->ul_Type == GRO_Geometric)
					{
#if defined(_XENON_RENDER)
                        if ((pst_GAO->pst_Base) && (pst_GAO->pst_Base->pst_Visu))
                        {
                            GEO_ClearXenonMesh(pst_GAO->pst_Base->pst_Visu, (GEO_tdst_Object*)p_stGEOLOD->dpst_Id[LODCOUNTER], FALSE, FALSE);
                        }
#endif
						if(_b_Strip)
						{
							if(GEO_STRIP_Compute(LODCOUNTER == 0 ? NULL : pst_GAO,(GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER], LODCOUNTER == 0 ? NULL : pst_GAO->pst_Base->pst_Visu->dul_VertexColors, FALSE , STRIP_SetBarreState ))
							{
								GEO_STRIP_ComputeStat((GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER], str);
								GEO_STRIP_UnSetFlag(GEO_C_Strip_StatData, (GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER]);

								sprintf(az, "Strip %s (%s) LOD LVL %d", pst_GAO->sz_Name, str ,LODCOUNTER);
								LINK_PrintStatusMsg(az);
							}
							else if(!EDI_gb_NoVerbose)
							{
								sprintf(az, "Strip %s : The object doesn't have points, can't do this...", pst_GAO->sz_Name);
								ERR_X_Warning(0, az, NULL);
							}
						}
						else
						{
							if(((GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER])->ulStripFlag & GEO_C_Strip_DataValid)
								GEO_STRIP_Delete((GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER]);
						}
#if defined(_XENON_RENDER)
                        if ((pst_GAO->pst_Base) && (pst_GAO->pst_Base->pst_Visu))
                        {
                            GEO_ResetXenonMesh(pst_GAO, pst_GAO->pst_Base->pst_Visu, (GEO_tdst_Object*)p_stGEOLOD->dpst_Id[LODCOUNTER], FALSE, TRUE, FALSE);
                        }
#endif
					}
				}
			}

		}
	}

	delete(po_Dial);

	LINK_Refresh();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG LODStrips_SetBarreState(float Position, char *Text)
{
    return po_Dial->OnRefreshBarText(Position, Text);
}

// Compute LOD from MRM and triangle strips (for all objects of world)
void EOUT_cl_Frame::ComputeLODStrips(BOOL _b_Strip)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	GRO_tdst_Struct		*pst_Gro;
	char				str[200];
	char				az[1024];
    float dfLevels[] = {.7f, .4f, .2f};
    float dfThresholds[] = {.7f, .4f, .2f};
    int lLevelNb = 4; 
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = DW();
	if(!pst_World) return;

	po_Dial = new EDIA_cl_UPDATEDialog((char *) "");
	po_Dial->DoModeless();

	pst_AIEOT = &(pst_World->st_AllWorldObjects);
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GAO)) continue;
		if(!(pst_GAO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) continue;

		if(!pst_GAO->pst_Base) return;
		if(!pst_GAO->pst_Base->pst_Visu) return;

		pst_Gro = pst_GAO->pst_Base->pst_Visu->pst_Object;
		if(pst_Gro)
		{
			if(pst_Gro->i->ul_Type == GRO_Geometric)
			{
				if(_b_Strip)
				{
                    if (GEO_MRM_Compute(pst_GAO,(GEO_tdst_Object *) pst_Gro, FALSE, LODStrips_SetBarreState))
                    {
                        // Change quality curve.
                        GEO_MRM_SetCurve((GEO_tdst_Object *) pst_Gro, 1.5f , 1.0f);
                        GEO_MRM_ComputeLevels(pst_GAO,(GEO_tdst_Object *) pst_Gro,FALSE, lLevelNb,dfLevels,dfThresholds,LODStrips_SetBarreState);
                        if(GEO_STRIP_Compute(pst_GAO,(GEO_tdst_Object *) pst_Gro, pst_GAO->pst_Base->pst_Visu->dul_VertexColors, FALSE , STRIP_SetBarreState ))
                        {
                            GEO_STRIP_ComputeStat((GEO_tdst_Object *) pst_Gro, str);
                            GEO_STRIP_UnSetFlag(GEO_C_Strip_StatData, (GEO_tdst_Object *) pst_Gro);

                            sprintf(az, "Strip %s (%s)", pst_GAO->sz_Name, str);
                            LINK_PrintStatusMsg(az);
                        }
                        else if(!EDI_gb_NoVerbose)
                        {
                            sprintf(az, "Strip %s : The object doesn't have points, can't do this...", pst_GAO->sz_Name);
                            ERR_X_Warning(0, az, NULL);
                        }
                    }
				}
				else
				{
                    GEO_MRM_DeleteLevels((GEO_tdst_Object *) pst_Gro);
					if(((GEO_tdst_Object *) pst_Gro)->ulStripFlag & GEO_C_Strip_DataValid)
						GEO_STRIP_Delete((GEO_tdst_Object *) pst_Gro);
				}
			}
			if(pst_Gro->i->ul_Type == GRO_GeoStaticLOD)
			{
				GEO_tdst_StaticLOD *p_stGEOLOD;
				ULONG LODCOUNTER;
				p_stGEOLOD = (GEO_tdst_StaticLOD *)pst_Gro;
				LODCOUNTER = p_stGEOLOD->uc_NbLOD;
				while (LODCOUNTER --)
				{
					if (p_stGEOLOD->dpst_Id[LODCOUNTER]->i->ul_Type == GRO_Geometric)
					{
						if(_b_Strip)
						{
                          	if (GEO_MRM_Compute(LODCOUNTER == 0 ? NULL : pst_GAO,(GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER], FALSE, LODStrips_SetBarreState))
                            {
                                GEO_MRM_SetCurve((GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER], 1.5f , 1.0f);
                                GEO_MRM_ComputeLevels(LODCOUNTER == 0 ? NULL : pst_GAO,(GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER],FALSE, lLevelNb,dfLevels,dfThresholds,LODStrips_SetBarreState);
							    if(GEO_STRIP_Compute(LODCOUNTER == 0 ? NULL : pst_GAO,(GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER], LODCOUNTER == 0 ? NULL : pst_GAO->pst_Base->pst_Visu->dul_VertexColors, FALSE , STRIP_SetBarreState ))
							    {
								    GEO_STRIP_ComputeStat((GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER], str);
								    GEO_STRIP_UnSetFlag(GEO_C_Strip_StatData, (GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER]);

								    sprintf(az, "Strip %s (%s) LOD LVL %d", pst_GAO->sz_Name, str ,LODCOUNTER);
								    LINK_PrintStatusMsg(az);
							    }
							    else if(!EDI_gb_NoVerbose)
							    {
							    	sprintf(az, "Strip %s : The object doesn't have points, can't do this...", pst_GAO->sz_Name);
							    	ERR_X_Warning(0, az, NULL);
							    }
                            }
						}
						else
						{
                            GEO_MRM_DeleteLevels((GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER]);
							if(((GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER])->ulStripFlag & GEO_C_Strip_DataValid)
								GEO_STRIP_Delete((GEO_tdst_Object *) p_stGEOLOD->dpst_Id[LODCOUNTER]);
						}
					}
				}
			}

		}
	}

	delete(po_Dial);

	LINK_Refresh();
}
#ifdef JADEFUSION
void EOUT_cl_Frame::PackOneDirectory(ULONG _ul_DirIndex, BOOL _b_IsRoot)
{
    if (_ul_DirIndex == BIG_C_InvalidIndex)
        return;

    if (!_b_IsRoot)
    {
        // Parse all the files in the directory to find a .wol (or .wow if no .wol was found)
        ULONG ul_WorldIndex = BIG_C_InvalidIndex;
        ULONG ul_FileIndex  = BIG_FirstFile(_ul_DirIndex);
        while (ul_FileIndex != BIG_C_InvalidIndex)
        {
            if (strstr(BIG_NameFile(ul_FileIndex), ".wol"))
            {
                ul_WorldIndex = ul_FileIndex;
            }
            else if (strstr(BIG_NameFile(ul_FileIndex), ".wow"))
            {
                if (ul_WorldIndex == BIG_C_InvalidIndex)
                {
                    ul_WorldIndex = ul_FileIndex;
                }
            }

            ul_FileIndex = BIG_NextFile(ul_FileIndex);
        }

        if (ul_WorldIndex != BIG_C_InvalidIndex)
        {
            // Close the current world (if any)
            CloseWorld();

            // Backup the LOA address table
            extern BAS_tdst_barray LOA_gst_Array;
            extern BAS_tdst_barray LOA_gst_ArrayAdr;
            extern int LOA_gi_CurRef;
            extern int LOA_gi_LastRef;

            BAS_tdst_barray st_ArrayBak;
            BAS_tdst_barray st_ArrayAdrBak;
            {
                st_ArrayBak.num  = LOA_gst_Array.num;
                st_ArrayBak.gran = LOA_gst_Array.gran;
                st_ArrayBak.size = LOA_gst_Array.size;
                st_ArrayBak.base = (BAS_tdst_Key*)L_malloc(st_ArrayBak.size * sizeof(BAS_tdst_Key));
                L_memcpy(st_ArrayBak.base, LOA_gst_Array.base, st_ArrayBak.size * sizeof(BAS_tdst_Key));

                st_ArrayAdrBak.num  = LOA_gst_ArrayAdr.num;
                st_ArrayAdrBak.gran = LOA_gst_ArrayAdr.gran;
                st_ArrayAdrBak.size = LOA_gst_ArrayAdr.size;
                st_ArrayAdrBak.base = (BAS_tdst_Key*)L_malloc(st_ArrayAdrBak.size * sizeof(BAS_tdst_Key));
                L_memcpy(st_ArrayAdrBak.base, LOA_gst_ArrayAdr.base, st_ArrayAdrBak.size * sizeof(BAS_tdst_Key));

                LOA_gi_CurRef = LOA_gi_LastRef = 0;
            }

            // Load and save the world so that the X360 packed data will be generated
            i_OnMessage(EDI_MESSAGE_SELFILE, 0, ul_WorldIndex);
#if defined(_XENON_RENDER)
            // Make sure the tangent space smoothing groups are applied
            DP()->XeComputeTangentSpaceSmoothing(0xffffffff);
#endif
            DP()->SaveWorld();
            CloseWorld();

            // Restore the LOA address table
            {
                L_free(LOA_gst_Array.base);
                L_free(LOA_gst_ArrayAdr.base);

                LOA_gst_Array.num  = st_ArrayBak.num;
                LOA_gst_Array.gran = st_ArrayBak.gran;
                LOA_gst_Array.size = st_ArrayBak.size;
                LOA_gst_Array.base = (BAS_tdst_Key*)L_malloc(st_ArrayBak.size * sizeof(BAS_tdst_Key));
                L_memcpy(LOA_gst_Array.base, st_ArrayBak.base, st_ArrayBak.size * sizeof(BAS_tdst_Key));

                LOA_gst_ArrayAdr.num  = st_ArrayAdrBak.num;
                LOA_gst_ArrayAdr.gran = st_ArrayAdrBak.gran;
                LOA_gst_ArrayAdr.size = st_ArrayAdrBak.size;
                LOA_gst_ArrayAdr.base = (BAS_tdst_Key*)L_malloc(st_ArrayAdrBak.size * sizeof(BAS_tdst_Key));
                L_memcpy(LOA_gst_ArrayAdr.base, st_ArrayAdrBak.base, st_ArrayAdrBak.size * sizeof(BAS_tdst_Key));
            }
        }

        // Next sub directory
        PackOneDirectory(BIG_NextDir(_ul_DirIndex), FALSE);
    }

    // Move on to the sub folders
    PackOneDirectory(BIG_SubDir(_ul_DirIndex), FALSE);
}

void EOUT_cl_Frame::PackBasicWorlds(void)
{
    if (!GDI_b_IsXenonGraphics())
        return;

    /*
    PackOneDirectory(BIG_ul_SearchDir("ROOT/EngineDatas/02 Modelisation Bank/_Kanada/_Decors"),      TRUE);
    PackOneDirectory(BIG_ul_SearchDir("ROOT/EngineDatas/02 Modelisation Bank/_Kanada/_Personnages"), TRUE);
    PackOneDirectory(BIG_ul_SearchDir("ROOT/EngineDatas/02 Modelisation Bank/_Kanada/_SFX"),         TRUE);
    PackOneDirectory(BIG_ul_SearchDir("ROOT/EngineDatas/02 Modelisation Bank/Decors"),               TRUE);
    PackOneDirectory(BIG_ul_SearchDir("ROOT/EngineDatas/02 Modelisation Bank/Interface"),            TRUE);
    */
    PackOneDirectory(BIG_ul_SearchDir("ROOT/EngineDatas/02 Modelisation Bank/Personnages"),          TRUE);
    PackOneDirectory(BIG_ul_SearchDir("ROOT/EngineDatas/02 Modelisation Bank/Objets"),               TRUE);
    /*
    PackOneDirectory(BIG_ul_SearchDir("ROOT/EngineDatas/02 Modelisation Bank/SFX"),                  TRUE);
    */
}
#endif
#endif /* ACTIVE_EDITORS */

