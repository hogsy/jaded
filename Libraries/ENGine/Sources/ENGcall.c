/*$T ENGcall.c GC! 1.081 10/26/00 14:39:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "ENGine/Sources/DYNamics/DYNmain.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEMpro.h"
#include "TABles/TABles.h"
#include "GDInterface/GDInterface.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine/Sources/EOT/EOT.h"
#include "ENGine/Sources/RECalage/RECdyn.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/COLlision/COLmain.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#include "TIMer/TIMdefs.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "BIGfiles/BIGread.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

#include "BASe/BENch/BENch.h"

BOOL				ENG_gb_ActiveSectorization = TRUE;
BOOL				EDI_gb_ComputeMap = FALSE;
BOOL				EDI_gb_SlashC = FALSE;
extern				BOOL WOR_gb_FirstTrameAfterLoad;


ULONG ENG_gul_Loop = 0;	
extern unsigned int		WOR_gul_WorldKey;

extern void ANI_BeforeRendering(struct WOR_tdst_World_ *);
extern void WOR_ResetKillGO(void);	

BOOL		ENG_gb_InPause = FALSE;
BOOL		ENG_gb_ForceUnPause = FALSE;
UINT		ENG_gui_UnpauseMask = 0;
extern void MSG_GlobalProcess(void);
extern ULONG	AI_C_Callback;

#ifdef ODE_INSIDE
#include "ode_jade.h"
#endif


extern void AI_GeneratedObject_TimeToLive( void );

//extern void WatchDog_Start(void);
//extern void WatchDog_End(void);
/*
 =======================================================================================================================
    Aim:    This function call the engine for one world

    Note:   It is called by ENG_EngineCall for each active world
 =======================================================================================================================
 */
void ENG_OneWorldEngineCall(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_View		*pst_View;
	WOR_tdst_View		*pst_LastView;
	extern void			AI_EvalFunc_WORPreLoadTex_C(OBJ_tdst_GameObject *);
	TAB_tdst_PFelem		*pst_Elem;
	TAB_tdst_PFelem		*pst_LastElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	

#ifdef _GAMECUBE	
	{
		extern void CheckMediaError(void);
		CheckMediaError();
	}
#endif

	/* For loop thru activators */
	WOR_gpst_CurrentWorld = _pst_World;

   	AI_GeneratedObject_TimeToLive();
	

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 Secto
	 -------------------------------------------------------------------------------------------------------------------
	 */
	WOR_ComputeSector(_pst_World);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 Preload
	 -------------------------------------------------------------------------------------------------------------------
	 */
	

#ifndef ACTIVE_EDITORS
	AI_EvalFunc_WORPreLoadTex_C(AI_gpst_MainActors[0]);
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    3: we activate the objects of the world £
	    this is done only if the Refresh Time has been reached
	 -------------------------------------------------------------------------------------------------------------------
	 */
	
	ENG_gb_MustRebuildDyn = FALSE;
	
	if(_pst_World->b_ForceActivationRefresh)
	{
		if(ENG_gb_ForceUnPause)
		{
			/* Reset pause mode for all objects */
			pst_Elem = _pst_World->st_AllWorldObjects.p_Table;
			pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem;
			for(; pst_Elem < pst_LastElem; pst_Elem++)
			{
				if(TAB_b_IsAHole(pst_Elem->p_Pointer)) continue;
				if(OBJ_b_TestIdentityFlag((OBJ_tdst_GameObject *) (pst_Elem->p_Pointer), ENG_gui_UnpauseMask))
					OBJ_ClearControlFlag((OBJ_tdst_GameObject *) (pst_Elem->p_Pointer), OBJ_C_ControlFlag_InPause);
					
			
				/* --NEW ANTI BUG--- IF GO has not the ForceInactive Flag, we set it active (Camera Cut PB) */		
				if(!OBJ_b_TestControlFlag((OBJ_tdst_GameObject *) (pst_Elem->p_Pointer), OBJ_C_ControlFlag_ForceInactive))
					OBJ_SetStatusFlag((OBJ_tdst_GameObject *) (pst_Elem->p_Pointer), OBJ_C_StatusFlag_Active);
						
			}
		}
	
		PROPS2_StartRaster(&PROPS2_gst_WOR_World_ActivateObjects);
		WOR_World_ActivateObjects(_pst_World);
		PROPS2_StopRaster(&PROPS2_gst_WOR_World_ActivateObjects);
		
		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    4: We build the Engine Objects Tables (EOT) or (Tableaux d'objets moteurs (TOM) in french), but only when the activ
		    objects change
		 ---------------------------------------------------------------------------------------------------------------
		 */

		PROPS2_StartRaster(&PROPS2_gst_EOT_SetOfEOT_Build);
		EOT_SetOfEOT_Build(&_pst_World->st_EOT, &_pst_World->st_ActivObjects);
		PROPS2_StopRaster(&PROPS2_gst_EOT_SetOfEOT_Build);	

		_pst_World->b_ForceActivationRefresh = FALSE;
	}

	

	ENG_gb_ForceUnPause = FALSE;

	/* First Frame, we force the SnP Computation for AI first Frame. */
	if(ENG_gb_FirstFrame) 
	{
		INT_SnP_InsertionSort(_pst_World->pst_SnP, 0);
	}
	
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_SnP);
	PROPS2_StartRaster(&PROPS2_gst_ENG_OneWorldEngineCall1);

    /*$2 apply all generic modifier */
    MDF_MainApplyGen(&_pst_World->st_ActivObjects);   
	
	if(_pst_World->b_ForceBVRefresh)
	{
	
		INT_SnP_Refresh(_pst_World->pst_SnP, &_pst_World->st_ActivObjects);
		_pst_World->b_ForceBVRefresh = FALSE;
	
	}
	
	PROPS2_StopRaster(&PROPS2_gst_ENG_OneWorldEngineCall1);
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_SnP);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    We call events for each object
	 -------------------------------------------------------------------------------------------------------------------
	 */

//	PRO_StartTrameRaster(&ENG_gpst_RasterEng_Events);
	ENG_gb_EVERunning = TRUE;
	PROPS2_StartRaster(&PROPS2_gst_EVE_MainCall);
	EVE_MainCall(_pst_World);
	PROPS2_StopRaster(&PROPS2_gst_EVE_MainCall);
	ENG_gb_EVERunning = FALSE;
//	PRO_StopTrameRaster(&ENG_gpst_RasterEng_Events);
	
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    We call ANI
	 -------------------------------------------------------------------------------------------------------------------
	 */
	 
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_ANI);
	PROPS2_StartRaster(&PROPS2_gst_ANI_MainCall);
	ENG_gb_ANIRunning = TRUE;
	ANI_MainCall(_pst_World);
	ENG_gb_ANIRunning = FALSE;
	PROPS2_StopRaster(&PROPS2_gst_ANI_MainCall);
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_ANI);

	
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    We call AI for each object
	 -------------------------------------------------------------------------------------------------------------------
	 */
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_AI);
	ENG_gb_AIRunning = TRUE;
	PROPS2_StartRaster(&PROPS2_gst_AI_MainCall);		
	AI_MainCall(_pst_World);
	PROPS2_StopRaster(&PROPS2_gst_AI_MainCall);
	ENG_gb_AIRunning = FALSE;
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_AI);

	
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Hierarchy
	 -------------------------------------------------------------------------------------------------------------------
	 */
	PROPS2_StartRaster(&PROPS2_gst_OBJ_HierarchyMainCall);
	OBJ_HierarchyMainCall(_pst_World);
	PROPS2_StopRaster(&PROPS2_gst_OBJ_HierarchyMainCall);

	
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    We call the dynamics, using the Dyna EOT
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* In case of dynamic allocation or reallocation of dynamic struct in AI */
	if(ENG_gb_MustRebuildDyn)
	{
	
		EOT_SetOfEOT_OneBuild(&_pst_World->st_EOT.st_Dyna, OBJ_C_IdentityFlag_Dyna, &_pst_World->st_ActivObjects);
	
		ENG_gb_MustRebuildDyn = FALSE;
	}
	
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_DYN);
	PROPS2_StartRaster(&PROPS2_gst_DYN_MainCall);
	DYN_MainCall(_pst_World);
#ifdef ODE_INSIDE
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_ODE_SOLVER);
	ODE_MainCall(_pst_World);
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_ODE_SOLVER);
#endif
	PROPS2_StopRaster(&PROPS2_gst_DYN_MainCall);
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_DYN);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Collision
	 -------------------------------------------------------------------------------------------------------------------
	 */
//	WatchDog_Start();	
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_COL);
	PROPS2_StartRaster(&PROPS2_gst_COL_MainCall);
	COL_MainCall(_pst_World);
	PROPS2_StopRaster(&PROPS2_gst_COL_MainCall);
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_COL);
    
//	WatchDog_End();
    

#ifdef ODE_INSIDE
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_ODE_COL);
	dJointGroupEmpty (_pst_World->ode_joint_col_ode);
	_pst_World->ode_contacts_num_ode = 0;
	dSpaceCollide(_pst_World->ode_id_space, (void *) _pst_World, COL_ODEPrimitive_Callback);
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_ODE_COL);
#endif

	
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Recalage
	 -------------------------------------------------------------------------------------------------------------------
	 */
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_REC);
	PROPS2_StartRaster(&PROPS2_gst_REC_MainCall);
	REC_MainCall(_pst_World, REC_ComputeAllReports);
	PROPS2_StopRaster(&PROPS2_gst_REC_MainCall);
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_REC);

	
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Hierarchy
	 -------------------------------------------------------------------------------------------------------------------
	 */
	PROPS2_StartRaster(&PROPS2_gst_OBJ_HierarchyMainCall);
	OBJ_HierarchyMainCall(_pst_World);
	PROPS2_StopRaster(&PROPS2_gst_OBJ_HierarchyMainCall);
	
	
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_AI);
	PROPS2_StartRaster(&PROPS2_gst_AI_ExecCallbackAll);
	AI_ExecCallbackAll(_pst_World, AI_C_Callback_AfterRec);
 	AI_RunContext(_pst_World, AI_C_Callback_AfterRec);
	PROPS2_StopRaster(&PROPS2_gst_AI_ExecCallbackAll);
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_AI);

	/* In case of dynamic allocation or reallocation of dynamic struct in AI */
	if(ENG_gb_MustRebuildDyn)
	{
	
		EOT_SetOfEOT_OneBuild(&_pst_World->st_EOT.st_Dyna, OBJ_C_IdentityFlag_Dyna, &_pst_World->st_ActivObjects);
		ENG_gb_MustRebuildDyn = FALSE;
	
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Hierarchy
	 -------------------------------------------------------------------------------------------------------------------
	 */
	PROPS2_StartRaster(&PROPS2_gst_OBJ_HierarchyMainCall);
	OBJ_HierarchyMainCall(_pst_World);
	PROPS2_StopRaster(&PROPS2_gst_OBJ_HierarchyMainCall);
	

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    We make the World views follow their father
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_View = _pst_World->pst_View;

	pst_LastView = pst_View + _pst_World->ul_NbViews;
	PROPS2_StartRaster(&PROPS2_gst_ViewPointModificator);
	for(; pst_View < pst_LastView; pst_View++)
	{
	
		if((pst_View->st_DisplayInfo.pst_DisplayDatas) && (pst_View->pfnv_ViewPointModificator))
			pst_View->pfnv_ViewPointModificator(pst_View);
	}
	PROPS2_StopRaster(&PROPS2_gst_ViewPointModificator);

    /*$2 unapply all generic modifier */
    MDF_MainUnApplyGen(&_pst_World->st_ActivObjects);
	
    /*$2
	 -------------------------------------------------------------------------------------------------------------------
	    We make the objects of the world visible £
	    this is done only if the Visibility Refresh Time has been reached
	 -------------------------------------------------------------------------------------------------------------------
	 */
	if(_pst_World->b_ForceVisibilityRefresh)
	{
		/* We make the objects visible */
		PROPS2_StartRaster(&PROPS2_gst_WOR_World_MakeObjectsVisible);
		WOR_World_MakeObjectsVisible(_pst_World);
		PROPS2_StopRaster(&PROPS2_gst_WOR_World_MakeObjectsVisible);
		_pst_World->b_ForceVisibilityRefresh = FALSE;
	
	}
	
    /*$2
	 -------------------------------------------------------------------------------------------------------------------
	 Animation blends
	 -------------------------------------------------------------------------------------------------------------------
	 */
	ANI_BeforeRendering(_pst_World);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	 -------------------------------------------------------------------------------------------------------------------
	 */
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_AI);
	PROPS2_StartRaster(&PROPS2_gst_AI_ExecCallbackAll);
	AI_C_Callback = 1;
 	AI_RunContext(_pst_World, AI_C_Callback_AfterBlend);
	AI_C_Callback = 0; 	
	PROPS2_StopRaster(&PROPS2_gst_AI_ExecCallbackAll);
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_AI);

	/* Si la secto est en mode spécial, besoin de refresh maintenant (HACK KONG DEGEULASSE) */
	/*if(_pst_World->ul_CurrentSector == 667 && _pst_World->b_ForceVisibilityRefresh)
	{
		// We make the objects visible 
		PROPS2_StartRaster(&PROPS2_gst_WOR_World_MakeObjectsVisible);
		WOR_World_MakeObjectsVisible(_pst_World);
		PROPS2_StopRaster(&PROPS2_gst_WOR_World_MakeObjectsVisible);
		_pst_World->b_ForceVisibilityRefresh = FALSE;
	
	}*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    We make the World views follow their father
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_View = _pst_World->pst_View;

	pst_LastView = pst_View + _pst_World->ul_NbViews;
	PROPS2_StartRaster(&PROPS2_gst_ViewPointModificator);
	for(; pst_View < pst_LastView; pst_View++)
	{
	
		if((pst_View->st_DisplayInfo.pst_DisplayDatas) && (pst_View->pfnv_ViewPointModificator))
			pst_View->pfnv_ViewPointModificator(pst_View);
	}
	PROPS2_StopRaster(&PROPS2_gst_ViewPointModificator);

	ENG_gul_Loop++;	
}

/*
 =======================================================================================================================
    Aim:    The main Engine funtion

    Note:   Called by function "s_OneTrame" (pointed by ENG_p_Engine()) £
            funtion pointer is initialised in function "ENG_FirstInitEngine"
 =======================================================================================================================
 */
void ENG_EngineCall(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			ul_NbWorlds;
	TAB_tdst_PFelem *pst_CurrentElem;
	TAB_tdst_PFelem *pst_EndElem;
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	PROPS2_StartRaster(&PROPS2_gst_ENG_EngineCall1);

 	
	/*
	 * We Call the tables manager who removes the holes of all the tables when
	 * necessary
	 */
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_TablesManager);
    
	TAB_TablesManager_Call();

	/* Update if necessary the value of the CPU clock */
#if !defined(PSX2_TARGET) && !defined(_GAMECUBE) /* CPU clock update is made with TIM_Clock_Update */
	TIM_UpdateCPUClockFrequency();
#endif
	
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_TablesManager);

	PROPS2_StopRaster(&PROPS2_gst_ENG_EngineCall1);
	PROPS2_StartRaster(&PROPS2_gst_ENG_EngineCall2);

	/* Total reinit */

	/* Execute universe AI */
	if(ENG_gb_ActivateUniverse && WOR_gst_Universe.pst_AI)
	{
	
		AI_gpst_CurrentInstance = AI_gpst_CurrentInstanceUltra = WOR_gst_Universe.pst_AI;
		ENG_gb_AIRunning = TRUE;
		AI_RunInstance(WOR_gst_Universe.pst_AI);
		ENG_gb_AIRunning = FALSE;
	
	}

	PROPS2_StopRaster(&PROPS2_gst_ENG_EngineCall2);
	PROPS2_StartRaster(&PROPS2_gst_ENG_EngineCall3);

	/* Loop trhu the worlds to call the main engine for each active world */
	ul_NbWorlds = 0;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{

		pst_World = (WOR_tdst_World *) pst_CurrentElem->p_Pointer;
		if(!TAB_b_IsAHole(pst_World))
		{
			/* We call the engine for each active world */
			if(WOR_b_Universe_IsWorldActive(ul_NbWorlds))
			{
				PROPS2_StartRaster(&PROPS2_gst_ENG_OneWorldEngineCall);
   				ENG_OneWorldEngineCall(pst_World); 
				PROPS2_StopRaster(&PROPS2_gst_ENG_OneWorldEngineCall);
			}
		}

		ul_NbWorlds++;
	}

	MSG_GlobalProcess();
	WOR_ResetKillGO(); 
	
	WOR_gb_FirstTrameAfterLoad = FALSE;
	ENG_gb_FirstFrame = FALSE;
	PROPS2_StopRaster(&PROPS2_gst_ENG_EngineCall3);   
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
