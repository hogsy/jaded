/*$T ENGvars.h GC!1.68 01/04/00 12:12:43 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

/*
 * This file has all the global variables of the engine
 * declared as extern (for inclusions)
 */
#include "TIMer/PROfiler/PROdefs.h"
#include "BASe/BAStypes.h"
#include "GraphicDK/Sources/GDInterface/GDInterface.h"
#include "ENGstruct.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "MATHs/MATH.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Main Loop interface
 -----------------------------------------------------------------------------------------------------------------------
 */

/* Pointer to input function */
extern void (*ENG_gp_Input) (void);

/* Pointer to engine function */
extern void (*ENG_gp_Engine) (void);

/* Pointer to display function */
extern void (*ENG_gp_Display) (HWND, GDI_tdst_DisplayData *);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Aim:    Specific windows variables
 -----------------------------------------------------------------------------------------------------------------------
 */

extern BOOL                 ENG_gb_ExitApplication;     /* When force exiting application */
extern BOOL                 ENG_gb_ForceEndEngine;      /* To exit engine, but for pause (editors) */
extern BOOL                 ENG_gb_ForcePauseEngine;    /* To pause engine */
extern BOOL                 ENG_gb_OneStepEngine;       /* One step of the engine */
extern BOOL                 ENG_gb_ActivateUniverse;    /* Universe can be executed */
extern BOOL					ENG_gb_MustRebuildDyn;		/* Must rebuild dynamic in trame */
extern BOOL					ENG_gb_EngineRunning;
extern BOOL					ENG_gb_AIRunning;
extern BOOL					ENG_gb_EVERunning;
extern BOOL					ENG_gb_ANIRunning;
extern BOOL					ENG_bg_FirstFrameSpeedRun;
extern BOOL					ENG_gb_FirstFrame;
extern BOOL                 ENG_gb_LimitFPS;

extern HINSTANCE            MAI_gh_MainInstance;
extern HWND                 MAI_gh_MainWindow;
extern MAI_tdst_WinHandles  MAI_gst_MainHandles;

#ifdef _DEBUG
extern BOOL                 ENG_gb_TestMemory;          /* if true test memory at end of trame */
#endif

#ifdef JADEFUSION
extern BOOL					ENG_gb_ProfileRunning;		/* Profiling is running */
#endif
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Main rasters.
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef RASTERS_ON

#define ENG_C_NbUserRasters				10

extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_WinMsg;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_Input;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_OneCall;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_OneLoop;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_Display;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_Activation;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_Visibility;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_TablesManager;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_EOTcreation;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_AI;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_Events;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_SnP;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_COL;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_ODE_COL;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_ODE_SOLVER;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_REC;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_DYN;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_ANI;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_Synchro;
extern PRO_tdst_TrameRaster	ENG_gpst_RasterEng_EngineDt;
extern PRO_tdst_TrameRaster	ENG_gpst_RasterEng_EngineRealDt;
extern PRO_tdst_TrameRaster	ENG_gpst_RasterEng_EngineFrames;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_EditorRaster;
extern PRO_tdst_TrameRaster ENG_gapst_RasterEng_User[ENG_C_NbUserRasters];

#ifdef ACTIVE_EDITORS
extern PRO_tdst_TrameRaster	ENG_gpst_RasterEng_EditorsDt;
extern PRO_tdst_TrameRaster ENG_gpst_RasterEng_Editors;
#endif
#endif /* RASTERS_ON */

#ifdef JOYRECORDER
/*$2
 -----------------------------------------------------------------------------------------------------------------------
Joystick Recorder
-----------------------------------------------------------------------------------------------------------------------
*/
extern int INO_b_RecordInput;
extern int INO_b_PlayInput;
#endif
/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Structure for initialisation
 -----------------------------------------------------------------------------------------------------------------------
 */

extern MAI_tdst_InitStruct  MAI_gst_InitStruct;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
