/*$T ENGvars.c GC!1.52 01/12/00 10:47:59 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    This file has all the global variables of the eng module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "GDInterface/GDInterface.h"
#include "ENGstruct.h"
#include "ENGvars.h"
#include "MATHs/MATH.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/*$2
 ---------------------------------------------------------------------------------------------------
    Aim:    Main Loop interface
 ---------------------------------------------------------------------------------------------------
 */

/* Pointer to input function */
void (*ENG_gp_Input) (void) = NULL;

/* Pointer to engine function */
void (*ENG_gp_Engine) (void) = NULL;

/* Pointer to display function */
void (*ENG_gp_Display) (HWND, GDI_tdst_DisplayData *) = NULL;

/*$2
 ---------------------------------------------------------------------------------------------------
    Aim:    Specific windows variables
 ---------------------------------------------------------------------------------------------------
 */

bool ENG_gb_ExitApplication                 = FALSE;     /* When force exiting application */
bool ENG_gb_ForceEndEngine                  = FALSE;      /* To exit engine, but for pause (editors) */
bool ENG_gb_ForcePauseEngine                = FALSE;     /* To pause engine */
bool ENG_gb_OneStepEngine                   = FALSE;      /* One step of the engine */
bool ENG_gb_ActivateUniverse                = TRUE;     /* Universe can be executed */
bool ENG_gb_MustRebuildDyn                  = FALSE;       /* Must rebuild dynamic in trame */
bool ENG_gb_EngineRunning                   = FALSE;
bool ENG_gb_AIRunning                       = FALSE;
bool ENG_gb_EVERunning                      = FALSE;
bool ENG_gb_ANIRunning                      = FALSE;
bool ENG_bg_FirstFrameSpeedRun              = FALSE;
bool ENG_gb_FirstFrame                      = FALSE;
bool ENG_gb_LimitFPS                        = TRUE;// cap/limit the fps to around 60 ~hogsy

HINSTANCE           MAI_gh_MainInstance;
HWND                MAI_gh_MainWindow;
MAI_tdst_WinHandles MAI_gst_MainHandles;

#ifdef _DEBUG
BOOL                 ENG_gb_TestMemory = FALSE;         /* if true test memory at end of trame */
#endif

#ifdef JADEFUSION
BOOL				ENG_gb_ProfileRunning = FALSE;		/* Profiling is running */
#endif
/*$2
 ---------------------------------------------------------------------------------------------------
    Main rasters.
 ---------------------------------------------------------------------------------------------------
 */

#ifdef RASTERS_ON
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_WinMsg;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_Input;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_OneLoop;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_OneCall;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_Display;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_Activation;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_Visibility;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_TablesManager;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_EOTcreation;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_AI;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_Events;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_SnP;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_ANI;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_DYN;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_REC;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_COL;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_ODE_COL;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_ODE_SOLVER;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_Synchro;
PRO_tdst_TrameRaster	ENG_gpst_RasterEng_EngineDt;
PRO_tdst_TrameRaster	ENG_gpst_RasterEng_EngineRealDt;
PRO_tdst_TrameRaster	ENG_gpst_RasterEng_EngineFrames;
PRO_tdst_TrameRaster	ENG_gpst_RasterEng_EditorsDt;
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_EditorRaster;
PRO_tdst_TrameRaster    ENG_gapst_RasterEng_User[ENG_C_NbUserRasters];

#ifdef ACTIVE_EDITORS
PRO_tdst_TrameRaster    ENG_gpst_RasterEng_Editors;
#endif
#endif /* RASTERS_ON */

/*$2
 ---------------------------------------------------------------------------------------------------
    Structure for initialisation
 ---------------------------------------------------------------------------------------------------
 */

MAI_tdst_InitStruct MAI_gst_InitStruct;

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
