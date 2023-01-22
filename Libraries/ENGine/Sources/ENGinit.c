 /*$T ENGinit.c GC! 1.081 07/07/00 15:21:26 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Initialisations of the application and the engine */
#include "Precomp.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "BASe/BAS.h"
#include "SDK/Sources/LINks/LINK.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "INOut/INO.h"
#include "BIGfiles/BIG.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGopen.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "ENGinit.h"
#include "INOut/INOkeyboard.h"
#include "INOut/INOjoystick.h"
#include "INOut/INO.h"
#include "MATHs/MATH.h"
#include "AIinterp/Sources/AI.h"
#include "AIinterp/Sources/AImsg.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "TIMer/TIM.h"
#include "TABles/TABles.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/OBJects/OBJgizmo.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/INTersection/INTinit.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/COLlision/COL.h"
#include "ENGine/Sources/COLlision/COLinit.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLvars.h"
#include "ENGine/Sources/DYNamics/DYNinit.h"
#include "ENGine/Sources/DYNamics/DYN.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "ENGine/Sources/WAYs/WAY.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/EOT/EOTmain.h"
#include "ENGine/Sources/TEXT/TEXT.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/ANImation/ANI.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "ENGine/Sources/ACTions/ACTcompute.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"
#include "ENGvars.h"
#include "ENGcall.h"
#include "TIMer/PROfiler/PRO.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/Functions/AIfunctions_savephotos.h"
#include "GFX/GFX.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTstruct.h"
#include "GraphicDK\Sources\GFX\GFX.h"
#include "STRing/STRdata.h"
#include "AIinterp/Sources/AImsg.h"

#ifdef JADEFUSION
#include "SDK/Sources/Xenon/Live/Session.h"
#include "SDK/Sources/Xenon/Live/Notifications.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "BASe/BENch/BENch.h"
#include "Engine/Sources/Wind/WindManager.h"
#include "Xenon/MenuManager/MenuManager.h"
#endif

extern AI_tdst_GlobalMessageList	gast_GlobalMessages[C_GM_MaxTypes];

#include "Sound/Sources/SNDstruct.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/MTX.h"


#ifdef ACTIVE_EDITORS
#include "SELection/SELection.h"
#include "GEOmetric/GEODebugObject.h"
#ifdef JADEFUSION
#include "ENGine/Sources/Wind/WindManager.h"
#endif
#endif

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDstream.h"
#include "TIMer/TIMdefs.h"
#include "NETwork/Sources/NET.h"
#include "MC/MC_Manager.h"

#ifdef _GAMECUBE
#include "GameCube/GC_Stream.h"
#include "GXI_GC/GXI_init.h"
#endif

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeWeatherManager.h"
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern COL_tdst_GlobalVars	COL_gst_GlobalVars;
extern int					GRID_gi_Current;
extern float				TIM_gf_SpeedFactor;

#ifdef JADEFUSION
extern BOOL EDI_gb_ComputeMap;
#endif

/* désolé */
extern void STR_3DStringList_Clear( void );
extern void WOR_SectoReinit(WOR_tdst_World *);
extern void AI_GeneratedObject_Reinit( void );
/* vraiment, vraiment */



#if defined(_XBOX) || defined(_XENON)
HANDLE g_hHeap = NULL;
#endif // _XBOX/_XENON


/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Main rasters.
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
#ifdef RASTERS_ON
PRO_tdst_TrameRaster	MAI_gpst_RasterEng_Editor;
PRO_tdst_TrameRaster	MAI_gpst_RasterEng_EditorRaster;
#endif
#endif
#if defined(RASTERS_ON)

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */



/*
 =======================================================================================================================
    Aim:    Creation of rasters
 =======================================================================================================================
 */
static void s_CreateRasters(void)
{
	/*~~~~~~~~~~~~~~~~~~*/
	int		i;
	/*~~~~~~~~~~~~~~~~~~*/
	/*$off*/

	/* Init the raster that displays the current dt */
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_EngineDt, "Engine Infos", "Various Infos", "Engine Dt", PRO_E_Float, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_EngineRealDt, "Engine Infos", "Various Infos", "Engine Real Dt", PRO_E_Float, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_EngineFrames, "Engine Infos", "Various Infos", "Engine Frames", PRO_E_Float, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_Synchro, "Trame Loop", "Game Engine", "Synchro", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_WinMsg, "Trame Loop", "Game Engine", "Win Msg", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_Input, "Trame Loop", "Game Engine", "Input", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_Display, "Trame Loop", "Game Engine", "Display", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_Activation, "Trame Loop", "Game Engine", "Activation", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_Visibility, "Trame Loop", "Game Engine", "Visibility", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_TablesManager, "Trame Loop", "Game Engine", "Tables Manager", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_EOTcreation, "Trame Loop", "Game Engine", "EOT Creation", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_AI, "Trame Loop", "Game Engine", "AI", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_SnP, "Trame Loop", "Game Engine", "SnP", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_Events, "Trame Loop", "Game Engine", "Events", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_ANI, "Trame Loop", "Game Engine", "Animation", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_DYN, "Trame Loop", "Game Engine", "Dynamic", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_COL, "Trame Loop", "Game Engine", "Collision", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_ODE_COL, "Trame Loop", "Game Engine", "ODE Collision", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_ODE_SOLVER, "Trame Loop", "Game Engine", "ODE Solver", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_REC, "Trame Loop", "Game Engine", "Rec", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_OneCall, "Trame Loop", "Game Engine", "Game Engine", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_OneLoop, "Trame Loop", "Game Engine", "Full Loop", PRO_E_Time, 0);
	for(i = ENG_C_NbUserRasters - 1; i >= 0; i--)
		PRO_FirstInitTrameRaster(&ENG_gapst_RasterEng_User[i], "Trame Loop", "User", "User", PRO_E_Time, 0);

#ifdef ACTIVE_EDITORS
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_Editors, "Trame Loop", "Editors", "One Call", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_EditorRaster, "Trame Loop", "Editors", "Rasters", PRO_E_Time, 0);
	PRO_FirstInitTrameRaster(&ENG_gpst_RasterEng_EditorsDt, "Engine Infos", "Various Infos", "Editors Dt", PRO_E_Float, 0);

	for(i = 1; i < C_GM_MaxTypes; i++)
	{
		sprintf(gast_GlobalMessages[i].az_NameRaster, "Type %d", i);
		PRO_FirstInitTrameRaster
		(
			&gast_GlobalMessages[i].st_Raster,
			"Global Messages",
			gast_GlobalMessages[i].az_NameRaster,
			"",
			PRO_E_MsgGlob,
			i
		);
	}
#endif
	/*$on*/
}

#endif /* RASTERS_ON */

#ifdef ODE_INSIDE
extern  void LCP_StaticInit(void);
#endif
/*
 =======================================================================================================================
    Aim:    First init of the application
 =======================================================================================================================
 */
void ENG_InitApplication(void)
{
#if defined(_XBOX) || defined(_XENON)
	g_hHeap = HeapCreate (HEAP_GENERATE_EXCEPTIONS, 0xFFFF, 0);
#endif // _XBOX

#ifdef _GAMECUBE
    GC_StreamInitModule();
#endif
    
	/* Global structures */
#if defined(PSX2_TARGET) && defined(__CW__)
	BIG_gst.h_CLibFileHandle = (LONG) NULL;
#else
	BIG_gst.h_CLibFileHandle = NULL;
#endif
	L_memset(&MAI_gst_InitStruct, 0, sizeof(MAI_gst_InitStruct));
	L_memset(&MAI_gst_MainHandles, 0, sizeof(MAI_tdst_WinHandles));

	/* SDK modules */
	MEM_InitModule();
	
	PRO_InitModule();
#if defined(_XENON_PROFILE)
    XEInitializeProfile();
#endif

	BAS_InitModule();
	BIG_InitModule();
	INO_InitModule();
	AI_InitModule();
	WAY_InitModule();

	/* Engine modules */
	MATH_InitModule();
	TAB_InitModule();
	GDI_InitModule();
	MDF_Modifier_Init();
	DYN_InitModule();
	TEXT_InitModule();
	COL_InitModule();
	ANI_InitModule();
	LOA_LoadSpecialArray();
	MSG_GlobalFirstInit();

	/* Initialise the Universe, the worlds... */
	WOR_InitModule();

    /* photo buffer allocation */
    SAV_PhotoFirstInit();

#ifdef ACTIVE_EDITORS
	LINK_InitModule();
#endif
    MTX_InitModule();


#ifdef ODE_INSIDE
	LCP_StaticInit();
#endif

#ifdef JADEFUSION
    WIND_InitWindManager();

#ifdef _XENON
	g_MenuManager.Initialize();
	XOnlineStartup( );  //g_XeLiveSession.Initialize();
	g_XeNotificationManager.Initialize();
#endif
#endif
}

/*
 =======================================================================================================================
    Aim:    Last close of the application.
 =======================================================================================================================
 */
void ENG_CloseApplication(void)
{
#ifdef JADEFUSION    
	WIND_CloseWindManager();
#endif
	MTX_CloseModule();
	/* Close The engine modules */

#if defined(_XENON)
	g_MenuManager.Shutdown();
#endif

	DYN_CloseModule();
	WOR_CloseModule();
	TEXT_CloseModule();
    //SND_CloseModule();

	/* Close the SDK modules */
	MSG_GlobalDestroy();
	BAS_CloseModule();
	BIG_CloseModule();
	INO_CloseModule();
	BIG_FreeGlobalBuffer();
	AI_CloseModule();
	TAB_CloseModule();
	GDI_CloseModule();
	WAY_CloseModule();
	LOA_KillSpecialArray();


#if defined(_XENON_PROFILE)
    XEShutdownProfile();
#endif

    /* photo buffer desallocation */
    SAV_PhotoLastClose();

#ifdef _GAMECUBE
    GC_StreamCloseModule();
#endif    

#ifdef ACTIVE_EDITORS
	LINK_CloseModule();
#endif
	MEM_CloseModule();

#ifdef _XENON
	XOnlineCleanup( );  //g_XeLiveSession.Shutdown();
	g_XeNotificationManager.Shutdown();
#endif
}

/*
 =======================================================================================================================
    Aim:    Reinit of the engine
 =======================================================================================================================
 */
void ENG_InitEngine(void)
{
	/* Give the engine pointer to the main loop interface */
	ENG_gp_Engine = ENG_EngineCall;

	ENG_gb_LimitFPS = TRUE; // TODO: store in config ~hogsy
	ENG_gb_FirstFrame = TRUE;

	/* Reset the clock */
	TIM_Clock_Reset();
#ifdef _XBOX
		INO_Joystick_Init(MAI_gh_MainWindow);
#endif
	SND_InitModule();
	TIM_InitModule();
#ifndef _XBOX
	INO_Joystick_Init(MAI_gh_MainWindow);
#endif
	NET_Init();

	/* Create rasters */
#if defined(RASTERS_ON)
	s_CreateRasters();
#endif
	UNI_Status() = UNI_Cuc_Reset;
}

/*
 =======================================================================================================================
    Aim:    Last close of the engine

    Note:   Called by ENG_CloseApplication
 =======================================================================================================================
 */
void ENG_CloseEngine(void)
{
	INO_Joystick_Close();
    SND_CloseModule();
    SAV_PhotoLastClose();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ENG_Init(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ENG_Desinit(void)
{
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ENG_ReinitOneObject(OBJ_tdst_GameObject *_pst_GO, int _i_Type)
{
	/* Reset Culled Flag */
	_pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_StatusFlag_Culled;

	/* Hierarchy */
	if(_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Hierarchy)
	{
		if(_pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit)
			_pst_GO->pst_Base->pst_Hierarchy->pst_Father = _pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit;
		else
		{
			/* Dynamic Hierarchy Link must not be conserved after Reinit. */
			_pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Hierarchy;
			MEM_Free(_pst_GO->pst_Base->pst_Hierarchy);
			_pst_GO->pst_Base->pst_Hierarchy = NULL;
		}
	}

	/* Restore object (initial pos ) */
	if(_i_Type != UNI_Cuc_ForDuplicate)
		OBJ_Reinit(_pst_GO);

	/* Process */
	OBJ_ResetProcess
	(
		_pst_GO,
		WOR_World_GetWorldOfObject(_pst_GO)->ul_ProcessCounterAI,
		OBJ_C_ProcessedAI
	);
	OBJ_ResetProcess
	(
		_pst_GO,
		WOR_World_GetWorldOfObject(_pst_GO)->ul_ProcessCounterDyn,
		OBJ_C_ProcessedDyn
	);
	OBJ_ResetProcess
	(
		_pst_GO,
		WOR_World_GetWorldOfObject(_pst_GO)->ul_ProcessCounterHie,
		OBJ_C_ProcessedHie
	);

	/* AI */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_AI))
		AI_ReinitInstance((AI_tdst_Instance *) _pst_GO->pst_Extended->pst_Ai);

	/* Design Struct */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_DesignStruct))
	{
		_pst_GO->pst_Extended->pst_Design->ui_DesignFlags = _pst_GO->pst_Extended->pst_Design->ui_DesignFlagsInit;
	}

	/* Events */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Events)) EVE_ReinitData(_pst_GO->pst_Extended->pst_Events);

	/* Links */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Links))
		WAY_ReinitLinks((WAY_tdst_Struct *) _pst_GO->pst_Extended->pst_Links, _i_Type);

	/* Messages */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Msg))
	{
		extern void AI_EvalFunc_MSGClear_C(OBJ_tdst_GameObject *);
		AI_EvalFunc_MSGClear_C(_pst_GO);
	}

	/* BV */
	OBJ_ComputeBV(_pst_GO, OBJ_C_BV_ComputeIfNotLoaded, OBJ_C_BV_AABBox);

	/* Collisions */
	COL_Reinit(_pst_GO);

	/* Anims */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	{
		ANI_Reinit(_pst_GO);
		if(_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit) ACT_SetAction(_pst_GO, 0, 0, FALSE);
	}

	/* Reset the Dynamic flag */
	OBJ_ChangeIdentityFlags(_pst_GO, OBJ_ul_FlagsIdentityGet(_pst_GO) & (~OBJ_C_IdentityFlag_Dyna), OBJ_C_UnvalidFlags);

	/* Reset pause mode */
	_pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_InPause;

	/* Fix flags */
	_pst_GO->c_FixFlags &= 0xFC;
	_pst_GO->c_FixFlags |= (_pst_GO->c_FixFlags & 4) >> 2;
	_pst_GO->c_FixFlags |= (_pst_GO->c_FixFlags & 8) >> 2;

	/* Reset the "Anim dont touch" control flag */
	_pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_AnimDontTouch;
	_pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_SectoInvisible;
	_pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_SectoInactive;

	/* Force Invisible & Force Inactive */
	if(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInvisibleInit)
		_pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceInvisible;
	else
		_pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_ForceInvisible;
	if(_pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInactiveInit)
		_pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceInactive;
	else
		_pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_ForceInactive;

	/* Capacities */
	if(_pst_GO->pst_Extended) _pst_GO->pst_Extended->uw_Capacities = _pst_GO->pst_Extended->uw_CapacitiesInit;

#ifdef ACTIVE_EDITORS
	LINK_UpdatePointer(_pst_GO);
#endif

	/* Sound */
	SND_DetachObject(_pst_GO);
}

extern float	AI_gf_Epsilon;
extern COL_tdst_GlobalVars COL_gst_GlobalVars;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ENG_InitOneWorld(WOR_tdst_World *_pst_World)
{
	ENG_gb_FirstFrame = TRUE;

	/* Sector */
	WOR_SectoReinit(_pst_World);

	/* Sets the number of collision reports to 0. */
	_pst_World->ul_NbReports = 0;

	/* Sets the number of last ray crossed objects to 0. */
	_pst_World->ul_NbCrossed = 0;

	/* Sets the view's father to NULL. */
	if(_pst_World->pst_View) _pst_World->pst_View->pst_Father = NULL;

	/* Init networks */
	WAY_ComputePoundNetworks(_pst_World);

	/* Reinit grid */
	if(_pst_World->pst_Grid) GRI_Reinit(_pst_World->pst_Grid);
	if(_pst_World->pst_Grid1) GRI_Reinit(_pst_World->pst_Grid1);

	/* Reinit the global Epsilon of AI */
	AI_gf_Epsilon = Cf_EpsilonBig;

	COL_gst_GlobalVars.f_CornerCosAngle = 0.6f;
}

extern void MEM_Defrag(int);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ENG_ReinitOneWorld(WOR_tdst_World *_pst_World, int _i_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_PFElem, *pst_PFLastElem;
	OBJ_tdst_GameObject *pst_GO;
	ULONG				i;
	char				c_Pause;
	extern void			ResetPreloadTexAll(void);
#ifdef ACTIVE_EDITORS
	extern void			OGL_AE_Reinit();
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_World) return;

	TIM_gf_SpeedFactor = 1.0f;
#ifdef ACTIVE_EDITORS
	OGL_AE_Reinit();
#endif


	/* Sector */
	WOR_SectoReinit(_pst_World);

	/* Construct tables now */
	_pst_World->ul_ProcessCounterHie = 0;
	_pst_World->ul_ProcessCounterAI = 0;
	_pst_World->ul_ProcessCounterDyn = 0;

#ifdef ODE_INSIDE
	dJointGroupEmpty(_pst_World->ode_joint_dyna);
	dJointGroupEmpty(_pst_World->ode_joint_col_jade);
	dJointGroupEmpty(_pst_World->ode_joint_col_ode);
	dJointGroupEmpty(_pst_World->ode_joint_ode);

	_pst_World->ode_contacts_num_jade = 0;
	_pst_World->ode_contacts_num_ode = 0;
#endif

	ENG_gb_FirstFrame = TRUE;
	GFX_gpst_Current = NULL;
	GRID_gi_Current = 0;

	/* Reinit AI */
	L_memset(AI_gpst_MainActors, 0, sizeof(AI_gpst_MainActors));
	for(i = 0; i < AI_CXT_Max; i++) AI_gai_Differed[i] = 0;
	AI_Reset();

	/* Clear sectors */
	ResetPreloadTexAll();

	/* Sets the number of collision reports to 0. */
	_pst_World->ul_NbReports = 0;

	/* Sets the number of last ray crossed objects to 0. */
	_pst_World->ul_NbCrossed = 0;

	/* Destroy all added views and split view */
#ifdef ACTIVE_EDITORS
	WOR_View_UnsplitView(_pst_World);
	for(i = 4; i < _pst_World->ul_NbViews; i++)
	{
		WOR_View_Close(&_pst_World->pst_View[i]);
		_pst_World->pst_View[i].uc_Flags &= ~WOR_Cuc_View_Activ;
		_pst_World->pst_View[i].pfnv_ViewPointModificator = NULL;
	}

	_pst_World->ul_NbViews = 4;
#else
	for(i = 1; i < _pst_World->ul_NbViews; i++)
	{
		WOR_View_Close(&_pst_World->pst_View[i]);
		_pst_World->pst_View[i].uc_Flags &= ~WOR_Cuc_View_Activ;
		_pst_World->pst_View[i].pfnv_ViewPointModificator = NULL;
	}

	_pst_World->ul_NbViews = 1;
#endif

	/* Sets the view's father to NULL. */
	_pst_World->pst_View->pst_Father = NULL;

	/* Reinit all objects */
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Generated))
		{
			c_Pause = pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_InPause ? 1 : 0;
			if((_i_Type != UNI_Cuc_InitInPause) || (c_Pause))
			{
				ENG_ReinitOneObject(pst_GO, _i_Type);
				if((_i_Type == UNI_Cuc_InitInPause) && (c_Pause)) pst_GO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_InPause;
			}
		}
	}

	/* Destroy them if they were generated */
	pst_PFElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_PFLastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	for(; pst_PFElem <= pst_PFLastElem; pst_PFElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_PFElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Generated))
		{
			WOR_World_DetachObject(_pst_World, pst_GO);
			INT_SnP_DetachObject(pst_GO, _pst_World);
#ifdef ACTIVE_EDITORS
			// Skeleton Display Case.
			if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
				pst_GO->pst_Base->pst_Visu->pst_Object = NULL;
#endif

			OBJ_GameObject_Remove(pst_GO, 1);
#ifdef ACTIVE_EDITORS
			SEL_DelItem(_pst_World->pst_Selection, pst_GO);
#endif
		}
	}

	/* Reinit networks */
	WAY_ComputePoundNetworks(_pst_World);

	/* Reinit grid */
	if(_pst_World->pst_Grid) GRI_Reinit(_pst_World->pst_Grid);
	if(_pst_World->pst_Grid1) GRI_Reinit(_pst_World->pst_Grid1);

    /* Reinit GFX */
    GFX_Free( &_pst_World->pst_GFX, _i_Type );
    GFX_Free( &_pst_World->pst_GFXInterface, _i_Type );
    SPG2_ReinitWorldHook();

	/* Int */
	INT_SnP_Reinit(_pst_World);

	/* No more deleted infos */
	_pst_World->i_NumDelObj = 0;

	COL_gst_GlobalVars.f_CornerCosAngle = 0.6f;

	/* Reinit the global Epsilon of AI */
	AI_gf_Epsilon = Cf_EpsilonBig;

    /* reinit io */
    INO_Reinit();
    STRDATA_ReinitAll();

    /* reinit photo */
    SAV_PhotoReinit();

    /* 3D String */
    STR_3DStringList_Clear();

    /* fog : reinit force no fog to false */
    if (GDI_gpst_CurDD)
		GDI_gpst_CurDD->st_Fog.c_Flag = GDI_gpst_CurDD->st_Fog1.c_Flag = GDI_gpst_CurDD->st_Fog2.c_Flag = 0;

	WOR_World_ActivateObjects(_pst_World);
	EOT_SetOfEOT_Build(&_pst_World->st_EOT, &_pst_World->st_ActivObjects);
	for(i = 0; i < _pst_World->ul_NbViews; i++) _pst_World->pst_View[i].uc_Flags |= WOR_Cuc_View_RecomputeTables;
	WOR_World_MakeObjectsVisible(_pst_World);

	/* sound reinit */
	SND_ReinitOneWorld(_i_Type);
	
	/* Reinit table of duplicated objects */
	AI_GeneratedObject_Reinit();

#if defined(_XENON_RENDER)
	g_oXeWeatherManager.ReInit();
#endif

	MEM_Defrag(0);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
