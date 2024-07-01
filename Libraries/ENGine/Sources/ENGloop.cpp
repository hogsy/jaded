/*$T ENGloop.c GC! 1.081 07/03/02 11:04:38 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/CLIbrary/CLIwin.h"
#include "TABles/TABles.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGio.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ENGmsg.h"
#include "ENGine/Sources/ENGloop.h"
#include "ENGine/Sources/ENGcall.h"
#include "ENGine/Sources/WORld/WORrender.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "AIinterp/Sources/AIdebug.h"
#include "AIinterp/Sources/AIengine.h"
#include "LINks/LINKmsg.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "NETwork/sources/NET.h"
#if defined( _XBOX ) || defined( _XENON )
#	include "GX8/Gx8GPUMon.h"
#endif
#if defined( _XBOX )
#	include "DEModisk/DEModisk.h"
#endif

#if defined( _XENON )
#	include "Xenon/MenuManager/MenuManager.h"
#	include "Xenon/Live/RichPresence.h"
#	include "Xenon/Live/Notifications.h"
#	include "Xenon/Live/Achievements.h"
#	include "Xenon/Live/Session.h"
#	include "Xenon/Profile/Profile.h"
#endif

#ifdef JADEFUSION
#	include "BASe/BENch/BENch.h"
#endif
/*$2- editor ---------------------------------------------------------------------------------------------------------*/

#ifdef ACTIVE_EDITORS
#	include "BASe/ERRors/ERRasser.h"
#	include "EDIerrid.h"
#	include "LINKs/LINKtoed.h"
#	include "EDItors/Sources/SOuNd/SONutil.h"
#endif /* ACTIVE_EDITORS */

/*$2- PS2 + GC -------------------------------------------------------------------------------------------------------*/

//#if defined(PSX2_TARGET) || defined(_GAMECUBE) || defined(_XBOX)
#include "INOut/INOjoystick.h"
//#endif

/*$2------------------------------------------------------------------------------------------------------------------*/
#if defined( _XENON_RENDER )
#	include "XenonGraphics/XeBufferMgr.h"
#	include "XenonGraphics/XeRenderer.h"
#	include "XenonGraphics/XeSimpleRenderer.h"
#	include "XenonGraphics/XeGDInterface.h"

#	include "XenonGraphics/XeTrigger.h"
#endif

#include "BASe/BENch/BENch.h"
#include "INOut/INOkeyboard.h"
#include "INOut/INO.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDstream.h"
#include "TIMer/TIMdefs.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "TIMer/PROfiler/PROdisplay.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "TEXture/TEXprocedural.h"
#include "TEXture/TEXanimated.h"

#include "../../Shared/MainSharedSystem.h"
#include "../../Shared/Profiler.h"

/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

void DisplayAttach( GDI_tdst_DisplayData * );
void s_CheckResetRequest( void );
void ENG_ForceStartRasters( void );
extern "C" void FOGDYN_Reset( void );

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

extern void MEM_Defrag( int single );
extern "C" void SOFT_ZList_Clear( void );
extern "C" void MSG_GlobalReinit( void );

#if defined( PCWIN_TOOL )
extern void GDI_ChangeInterface( GDI_tdst_DisplayData *, ULONG ulNew );
extern int GDI_gi_GDIType;
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

BOOL sgb_DisplayRasters = FALSE;
BOOL sbg_FirstDisp      = FALSE;
extern UINT SPG2_gb_Recompute;
#ifndef ACTIVE_EDITORS
BOOL sgb_FullScreen   = FALSE;
int sgi_FullScreenRes = 0;
#else
extern "C" BOOL sgb_EngineRender = TRUE;
#endif
extern "C" BOOL ENG_gb_ForceAttach  = FALSE;
extern "C" BOOL ENG_gb_NeedToReinit = FALSE;
#if defined( USE_DOUBLE_RENDERING ) || defined( PSX2_TARGET )
ULONG ENG_gp_DoubleRendering       = 0;
ULONG ENG_gp_DoubleRenderingLocker = 0;
#endif

extern "C" ULONG ENG_gp_CameraCutHasBeenDetected = 0;

extern float TIM_gf_SynchroFrequency;
float TIM_gf_MainClockForTextureScrolling = 0.0f;
#ifndef PSX2_TARGET
HWND ENG_h_Rasters = 0;
#endif
#ifdef _FINAL_
float ENG_gf_TimeFinal;
BOOL ENG_gb_Raster = FALSE;
#endif

#ifdef JADEFUSION
extern BOOL ENG_gb_InPause;
#endif

/*$4
 ***********************************************************************************************************************
    MESSAGES
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DisplayAttach( GDI_tdst_DisplayData *_pst_DD )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static char asz_PrevName[ 100 ] = "";
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Attach world to display data : create texture */
	if ( MAI_gst_MainHandles.pst_World )
	{
		if (
		        ( _pst_DD->pst_World == NULL ) || ( _pst_DD->pst_World != MAI_gst_MainHandles.pst_World ) || ( L_strcmp( MAI_gst_MainHandles.pst_World->sz_Name, asz_PrevName ) ) || ( ENG_gb_ForceAttach ) )
		{
			ENG_gb_ForceAttach = FALSE;
			L_strcpy( asz_PrevName, MAI_gst_MainHandles.pst_World->sz_Name );

			/* GDI_ChangeInterface( _pst_DD, 1 ); */
			GDI_l_AttachWorld( _pst_DD, MAI_gst_MainHandles.pst_World );

			_pst_DD->pst_World = MAI_gst_MainHandles.pst_World;

			/* Copy default camera into view 0 viewpoint */
#ifdef ACTIVE_EDITORS
			CAM_SetObjectMatrixFromCam(
			        &_pst_DD->pst_World->pst_View->st_ViewPoint,
			        &_pst_DD->pst_World->st_CameraPosSave );
#endif
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_Display( HWND h, GDI_tdst_DisplayData *_pst_DD )
{
	JADED_PROFILER_START();

	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	extern void MEM_Defrag( int );
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Attach world */

	DisplayAttach( _pst_DD );

	GDI_BeforeDisplay( _pst_DD );

	_pst_DD->pst_World = MAI_gst_MainHandles.pst_World;
	if ( _pst_DD->pst_World )
	{
		_pst_DD->pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas = _pst_DD;
		WOR_Render( _pst_DD->pst_World, _pst_DD );
	}

	GDI_AfterDisplay( _pst_DD );

	JADED_PROFILER_END();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void s_CheckResetRequest( void )
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_InitBeforeTrame( void )
{
	JADED_PROFILER_START();

	s_CheckResetRequest();

	ENG_gp_Display = jaded::sys::launchOperations.editorMode ? nullptr : s_Display;

	ENG_gp_Input = INO_Update;
	if ( UNI_Status() != UNI_Cuc_Ready )
	{
		switch ( UNI_Status() )
		{
			case UNI_Cuc_Reset:
				PROPS2_StartRaster( &PROPS2_gst_AI_Reset );
				AI_Reset();
				PROPS2_StopRaster( &PROPS2_gst_AI_Reset );

				PROPS2_StartRaster( &PROPS2_gst_WOR_Universe_Open );
				WOR_Universe_Open( BIG_UniverseKey() );
				PROPS2_StopRaster( &PROPS2_gst_WOR_Universe_Open );
				break;

			default:
				PROPS2_StartRaster( &PROPS2_gst_ENG_ReinitOneWorld );
				AI_ReinitUniverse();
				ENG_ReinitOneWorld( MAI_gst_MainHandles.pst_World, UNI_Status() );
				PROPS2_StopRaster( &PROPS2_gst_ENG_ReinitOneWorld );
				break;
		}

		UNI_Status() = UNI_Cuc_Ready;
	}

#if defined( _DEBUG ) && ( defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL ) )
#	ifndef MEM_OPT
	if ( ENG_gb_TestMemory ) MEM_CheckAllocatedBlocks( 0 );
#	endif
#endif

	JADED_PROFILER_END();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_DesInitAfterTrame( void )
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static BOOL sfnb_EndGame( void )
{
	if ( ENG_gb_ExitApplication )
	{
		return TRUE;
	}
	if ( ENG_gb_ForceEndEngine )
	{
		return TRUE;
	}
	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Handles windows messages while the engine is running

    Note:   It is here that youy can edit while the engine is running
 =======================================================================================================================
 */
void s_HandleWinMessages( void )
{
	if ( !jaded::sys::launchOperations.editorMode )
	{
		return;
	}

	JADED_PROFILER_START();

	MSG msg;
	float f_StartTimeEditors;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Windows messages */
	PRO_StartTrameRaster( &ENG_gpst_RasterEng_WinMsg );
	f_StartTimeEditors = TIM_f_Clock_TrueRead();

#ifdef ACTIVE_EDITORS
	sgb_EngineRender = FALSE;
	while ( PeekMessage( &msg, 0, 0, 0xFFFFFFFF, PM_REMOVE ) )
	{
		if ( !LINK_b_ProcessEngineWndMsg( &msg ) ) break;
		if ( !LINK_PreTranslateMessage( &msg ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		if (
		        ( ( GetAsyncKeyState( VK_LBUTTON ) < 0 ) || ( GetAsyncKeyState( VK_RBUTTON ) < 0 ) ) && ( GetAsyncKeyState( VK_SPACE ) >= 0 ) ) break;
	}

	sgb_EngineRender = TRUE;
#else /* EDITOR */
	while ( PeekMessage( &msg, 0, 0, 0xFFFFFFFF, PM_REMOVE ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

#endif /* EDITOR */
	TIM_gf_EditorTime += ( TIM_f_Clock_TrueRead() - f_StartTimeEditors );

	PRO_StopTrameRaster( &ENG_gpst_RasterEng_WinMsg );

	JADED_PROFILER_END();
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#if defined( _XBOX ) || defined( _XENON )

#	if defined( _XENON )

// ***********
//    Xenon
// ***********

// display rasters : Back + Y + left trigger
#		define Mb_Cnd_RastersDisplay ( INO_b_Joystick_IsButtonDown( eXeButton_Back ) && INO_b_Joystick_IsButtonDown( eXeButton_Y ) && INO_b_Joystick_IsButtonDown( eXeButton_Trigger_Left ) )

// reset map : Back + A + left trigger
#		define Mb_Cnd_WorldsReset ( INO_b_Joystick_IsButtonDown( eXeButton_Back ) && INO_b_Joystick_IsButtonDown( eXeButton_A ) && INO_b_Joystick_IsButtonDown( eXeButton_Trigger_Left ) )

// change rasters category : Back + down + left trigger
#		define Mb_Cnd_RastersChangeCateg ( INO_b_Joystick_IsButtonDown( eXeButton_Back ) && INO_b_Joystick_IsButtonDown( eXeButton_DPad_Down ) && INO_b_Joystick_IsButtonDown( eXeButton_Trigger_Left ) )

// test backface : Back + B + right trigger
#		define Mb_Cnd_RastersTestBackFace ( INO_b_Joystick_IsButtonDown( eXeButton_Back ) && INO_b_Joystick_IsButtonDown( eXeButton_B ) && INO_b_Joystick_IsButtonDown( eXeButton_Trigger_Right ) )

// display wired : Back + A + right trigger
#		define Mb_Cnd_RastersDisplayWired ( INO_b_Joystick_IsButtonDown( eXeButton_Back ) && INO_b_Joystick_IsButtonDown( eXeButton_A ) && INO_b_Joystick_IsButtonDown( eXeButton_Trigger_Right ) )

// depth read before flip : Back + Y + right trigger
#		define Mb_Cnd_RastersDepthReadBeforeFlip ( INO_b_Joystick_IsButtonDown( eXeButton_Back ) && INO_b_Joystick_IsButtonDown( eXeButton_Y ) && INO_b_Joystick_IsButtonDown( eXeButton_Trigger_Right ) )
#	else

// **********
//    Xbox
// **********

// display rasters : Back + black + left trigger
#		define Mb_Cnd_RastersDisplay             ( INO_b_Joystick_IsButtonDown( e_Back ) && INO_b_Joystick_IsButtonDown( e_ButtonBlack ) && INO_b_Joystick_IsButtonDown( e_LeftTrigger ) )

// reset map : Back + A + left trigger
#		define Mb_Cnd_WorldsReset                ( INO_b_Joystick_IsButtonDown( e_Back ) && INO_b_Joystick_IsButtonDown( e_ButtonA ) && INO_b_Joystick_IsButtonDown( e_LeftTrigger ) )

// change rasters category : Back + down + left trigger
#		define Mb_Cnd_RastersChangeCateg         ( INO_b_Joystick_IsButtonDown( e_Back ) && INO_b_Joystick_IsButtonDown( e_DPadDown ) && INO_b_Joystick_IsButtonDown( e_LeftTrigger ) )

// test backface : Back + B + right trigger
#		define Mb_Cnd_RastersTestBackFace        ( INO_b_Joystick_IsButtonDown( e_Back ) && INO_b_Joystick_IsButtonDown( e_ButtonB ) && INO_b_Joystick_IsButtonDown( e_RightTrigger ) )

// display wired : Back + A + right trigger
#		define Mb_Cnd_RastersDisplayWired        ( INO_b_Joystick_IsButtonDown( e_Back ) && INO_b_Joystick_IsButtonDown( e_ButtonA ) && INO_b_Joystick_IsButtonDown( e_RightTrigger ) )

// depth read before flip : Back + Y + right trigger
#		define Mb_Cnd_RastersDepthReadBeforeFlip ( INO_b_Joystick_IsButtonDown( e_Back ) && INO_b_Joystick_IsButtonDown( e_ButtonY ) && INO_b_Joystick_IsButtonDown( e_RightTrigger ) )

#	endif

static void XB_s_EngineCheat( void )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#	ifdef RASTERS_ON
	PRO_tdst_TrameRaster *pst_Categ;
#	endif
	TAB_tdst_PFelem *pst_CurrentElem;
	TAB_tdst_PFelem *pst_EndElem;
	WOR_tdst_World *pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Reinit all worlds */
	if ( Mb_Cnd_WorldsReset )
	{
		SOFT_ZList_Clear();
		MSG_GlobalReinit();
		AI_ReinitUniverse();
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem( &WOR_gst_Universe.st_WorldsTable );
		pst_EndElem     = TAB_pst_PFtable_GetLastElem( &WOR_gst_Universe.st_WorldsTable );
		for ( ; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++ )
		{
			pst_World = ( WOR_tdst_World * ) pst_CurrentElem->p_Pointer;
			if ( TAB_b_IsAHole( pst_World ) ) continue;
			ENG_ReinitOneWorld( pst_World, UNI_Cuc_TotalInit );
		}

		while ( Mb_Cnd_WorldsReset )
			INO_Joystick_Update();
	}

#	ifdef RASTERS_ON

	/* Display rasters */

	if ( Mb_Cnd_RastersDisplay )
	{
		if ( !sbg_FirstDisp )
		{
			sbg_FirstDisp = TRUE;
		}
		else
		{
			sgb_DisplayRasters = sgb_DisplayRasters ? FALSE : TRUE;
			if ( !sgb_DisplayRasters ) sbg_FirstDisp = FALSE;


			mpst_CurrentCategory    = _PRO_gpst_FirstTrameRaster;
			mpst_CurrentSubCategory = NULL;
			mpst_CurrentName        = NULL;
		}

		while ( Mb_Cnd_RastersDisplay )
			INO_Joystick_Update();
	}


	// swap Test back face flag
	if ( Mb_Cnd_RastersTestBackFace )
	{
		if ( MAI_gst_MainHandles.pst_World && MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )
		{
			if ( ( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask & GDI_Cul_DM_TestBackFace )
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask &= ~GDI_Cul_DM_TestBackFace;
			}
			else
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask |= GDI_Cul_DM_TestBackFace;
			}
		}

		while ( Mb_Cnd_RastersTestBackFace )
			INO_Joystick_Update();
	}

	// Wire
	if ( Mb_Cnd_RastersDisplayWired )
	{
		if ( MAI_gst_MainHandles.pst_World && MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )
		{
			if ( ( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask & GDI_Cul_DM_NotWired )
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask &= ~GDI_Cul_DM_NotWired;
			}
			else
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask |= GDI_Cul_DM_NotWired;
			}
		}

		while ( Mb_Cnd_RastersDisplayWired )
			INO_Joystick_Update();
	}

	// Depth Read Before Flip
	if ( Mb_Cnd_RastersDepthReadBeforeFlip )
	{
		if ( MAI_gst_MainHandles.pst_World && MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )
		{
			if ( ( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DisplayFlags & GDI_cul_DF_DepthReadBeforeFlip )
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DisplayFlags &= ~GDI_cul_DF_DepthReadBeforeFlip;
			}
			else
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DisplayFlags |= GDI_cul_DF_DepthReadBeforeFlip;
			}
		}

		while ( Mb_Cnd_RastersDepthReadBeforeFlip )
			INO_Joystick_Update();
	}

	// Rasters : Change name
	if ( Mb_Cnd_RastersChangeCateg )
	{
		if ( sgb_DisplayRasters )
		{
			pst_Categ = mpst_CurrentName;
			if ( !pst_Categ )
				pst_Categ = _PRO_gpst_FirstTrameRaster;
			else
			{
				do
				{
					pst_Categ = pst_Categ->pst_NextRaster;
					if ( !pst_Categ ) pst_Categ = _PRO_gpst_FirstTrameRaster;
				} while ( ( pst_Categ != mpst_CurrentName ) && ( !L_strcmpi( pst_Categ->psz_Name, mpst_CurrentName->psz_Name ) ) );
			}

			mpst_CurrentName = pst_Categ;
		}

		while ( Mb_Cnd_RastersChangeCateg )
			INO_Joystick_Update();
	}

#	endif// RASTERS_ON
}

#else /* GC + PS2 + _XBOX */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern "C" int AI2C_ai2Ccan;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32_s_EngineCheat( void )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#	ifdef RASTERS_ON
	PRO_tdst_TrameRaster *pst_Categ;
#	endif
#	ifndef ACTIVE_EDITORS
	TAB_tdst_PFelem *pst_CurrentElem;
	TAB_tdst_PFelem *pst_EndElem;
	WOR_tdst_World *pst_World;
#	endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Full screen */
#	ifndef ACTIVE_EDITORS

	/* Reinit all worlds */
	if ( GetAsyncKeyState( VK_F6 ) < 0 )
	{
		SOFT_ZList_Clear();
		MSG_GlobalReinit();
		AI_ReinitUniverse();
		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem( &WOR_gst_Universe.st_WorldsTable );
		pst_EndElem     = TAB_pst_PFtable_GetLastElem( &WOR_gst_Universe.st_WorldsTable );
		for ( pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++ )
		{
			pst_World = ( WOR_tdst_World * ) pst_CurrentElem->p_Pointer;
			if ( TAB_b_IsAHole( pst_World ) ) continue;
			ENG_ReinitOneWorld( pst_World, UNI_Cuc_TotalInit );
		}

		while ( GetAsyncKeyState( VK_F6 ) < 0 )
			;
	}

#	endif

	if ( ( GetAsyncKeyState( VK_F10 ) < 0 ) && ( GetAsyncKeyState( VK_SHIFT ) < 0 ) && ( GetAsyncKeyState( VK_CONTROL ) < 0 ) )
	{
		if ( AI2C_ai2Ccan == 0 )
			AI2C_ai2Ccan = 1;
		else
			AI2C_ai2Ccan = 0;
		while ( GetAsyncKeyState( VK_F10 ) < 0 )
			;
	}

	/* Display rasters */
#	ifdef RASTERS_ON
	if ( GetAsyncKeyState( VK_F1 ) < 0 )
	{
		if ( !sbg_FirstDisp )
		{
			sbg_FirstDisp = TRUE;
			while ( GetAsyncKeyState( VK_F1 ) < 0 )
				;
		}
		else
		{
			sgb_DisplayRasters = sgb_DisplayRasters ? FALSE : TRUE;
			if ( !sgb_DisplayRasters ) sbg_FirstDisp = FALSE;
			while ( GetAsyncKeyState( VK_F1 ) < 0 )
				;
			mpst_CurrentCategory    = _PRO_gpst_FirstTrameRaster;
			mpst_CurrentSubCategory = NULL;
			mpst_CurrentName        = NULL;
			if ( ENG_h_Rasters ) InvalidateRect( ENG_h_Rasters, NULL, TRUE );
		}
	}

#		ifndef ACTIVE_EDITORS

	/* swap Test back face flag */
	if ( GetAsyncKeyState( VK_F2 ) < 0 )
	{
		if ( MAI_gst_MainHandles.pst_World && MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )
		{
			if ( ( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask & GDI_Cul_DM_TestBackFace )
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask &= ~GDI_Cul_DM_TestBackFace;
			}
			else
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask |= GDI_Cul_DM_TestBackFace;
			}
		}

		while ( GetAsyncKeyState( VK_F2 ) < 0 )
			;
	}

	/* Wire */
	if ( GetAsyncKeyState( VK_F3 ) < 0 )
	{
		if ( MAI_gst_MainHandles.pst_World && MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )
		{
			if ( ( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask & GDI_Cul_DM_NotWired )
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask &= ~GDI_Cul_DM_NotWired;
			}
			else
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DrawMask |= GDI_Cul_DM_NotWired;
			}
		}

		while ( GetAsyncKeyState( VK_F3 ) < 0 )
			;
	}

	/* Wire */
	if ( GetAsyncKeyState( VK_F5 ) < 0 )
	{
		if ( MAI_gst_MainHandles.pst_World && MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )
		{
			if ( ( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DisplayFlags & GDI_cul_DF_DepthReadBeforeFlip )
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DisplayFlags &= ~GDI_cul_DF_DepthReadBeforeFlip;
			}
			else
			{
				( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas )->ul_DisplayFlags |= GDI_cul_DF_DepthReadBeforeFlip;
			}
		}

		while ( GetAsyncKeyState( VK_F5 ) < 0 )
			;
	}

#			if defined( PCWIN_TOOL )
	if ( GetAsyncKeyState( VK_F11 ) < 0 )
	{
		GDI_gi_GDIType = 1 - GDI_gi_GDIType;
		GDI_ChangeInterface(
		        ( ( GDI_tdst_DisplayData * ) MAI_gst_MainHandles.pst_World->pst_View[ 0 ].st_DisplayInfo.pst_DisplayDatas ),
		        GDI_gi_GDIType );
		while ( GetAsyncKeyState( VK_F11 ) < 0 )
			;
	}
#			endif

#		endif
	/* Rasters : Change name */
	if ( GetAsyncKeyState( VK_F4 ) < 0 )
	{
		if ( sgb_DisplayRasters )
		{
			pst_Categ = mpst_CurrentName;
			if ( !pst_Categ )
				pst_Categ = _PRO_gpst_FirstTrameRaster;
			else
			{
				do
				{
					pst_Categ = pst_Categ->pst_NextRaster;
					if ( !pst_Categ ) pst_Categ = _PRO_gpst_FirstTrameRaster;
				} while ( ( pst_Categ != mpst_CurrentName ) && ( !L_strcmpi( pst_Categ->psz_Name, mpst_CurrentName->psz_Name ) ) );
			}

			mpst_CurrentName = pst_Categ;
		}

		while ( GetAsyncKeyState( VK_F4 ) < 0 )
			;
		if ( ENG_h_Rasters ) InvalidateRect( ENG_h_Rasters, NULL, TRUE );
	}

#	endif
}

#endif /* GAMECUBE + PS2 + XBOX */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_EngineCheat( void )
{

#if defined( PSX2_TARGET )
	PS2_s_EngineCheat();
#elif defined( _GAMECUBE )
	GC_s_EngineCheat();
#elif defined( _XBOX ) || defined( _XENON )
	XB_s_EngineCheat();
#else
	win32_s_EngineCheat();
#endif
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

#if defined( ACTIVE_EDITORS )

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void EDI_s_EngineCheatFinal( void )
{
}

#else

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void win32_s_EngineCheatFinal( void )
{
	/*~~~~~~~~~~~~*/
	int i;
	DEVMODE devMode;
	LONG l_Width, l_Height;
	/*~~~~~~~~~~~~*/

	/* Fullscreen */
	if ( ( GetAsyncKeyState( VK_MENU ) < 0 ) && ( GetAsyncKeyState( VK_RETURN ) < 0 ) )
	{
		if ( sgi_FullScreenRes != 3 )// (!sgb_FullScreen)
		{
			sgb_FullScreen = TRUE;
			sgi_FullScreenRes++;
#	ifdef JADEFUSION
#		if defined( PCWIN_TOOL )
			if ( GDI_b_IsXenonGraphics() )
			{
				GDI_ReadaptDisplay( GDI_gpst_CurDD, MAI_gst_MainHandles.h_DisplayWindow );
			}
			else
#		endif
#	endif
			{
				switch ( sgi_FullScreenRes )
				{
					case 1:
						l_Width  = 640;
						l_Height = 480;
						break;
					case 2:
						l_Width  = 800;
						l_Height = 600;
						break;
					case 3:
						l_Width  = 1024;
						l_Height = 768;
						break;
						//case 4: l_Width = 1280; l_Height = 1024; break;
				}
				for ( i = 0;; i++ )
				{
					if ( !EnumDisplaySettings( NULL, i, &devMode ) ) break;
					if (
					        ( ( LONG ) devMode.dmBitsPerPel == 32 ) && ( ( LONG ) devMode.dmPelsWidth == l_Width ) && ( ( LONG ) devMode.dmPelsHeight == l_Height ) )
					{
						if ( i > 1 )
						{
							ChangeDisplaySettings( NULL, 0 );
							ShowWindow( MAI_gh_MainWindow, SW_NORMAL );
						}
						ChangeDisplaySettings( &devMode, CDS_FULLSCREEN );
						ShowWindow( MAI_gh_MainWindow, SW_SHOWMAXIMIZED );
						return;
					}
				}
			}
		}
		else
		{
			sgb_FullScreen    = FALSE;
			sgi_FullScreenRes = 0;
			ChangeDisplaySettings( NULL, 0 );
			ShowWindow( MAI_gh_MainWindow, SW_NORMAL );
			MoveWindow( MAI_gh_MainWindow, 0, 0, 640, 480, TRUE );
		}

		while ( ( GetAsyncKeyState( VK_MENU ) < 0 ) || ( GetAsyncKeyState( VK_RETURN ) < 0 ) )
			;
	}

	/* Cheatcodes */
#	ifdef _FINAL_
	if ( GetAsyncKeyState( VK_CONTROL ) >= 0 ) return;
	if ( GetAsyncKeyState( VK_SPACE ) >= 0 ) return;

	if ( GetAsyncKeyState( VK_F1 ) < 0 )
	{
		ENG_gb_Raster = ENG_gb_Raster ? FALSE : TRUE;
		while ( GetAsyncKeyState( VK_F1 ) < 0 )
			;
	}

#	endif
}

#endif /* PS2 + GC */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void s_EngineCheatFinal( void )
{
#if defined( ACTIVE_EDITORS )
	EDI_s_EngineCheatFinal();
#else
	win32_s_EngineCheatFinal();
#endif
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifndef _FINAL_

extern "C" extern float TIM_gf_realdt;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static void s_DisplayRasters( void )
{
	/*~~~~~~~~~~~*/
#	if ( !defined( PSX2_TARGET ) && !defined( _GAMECUBE ) && !defined( _XBOX ) && !defined( _XENON ) )
	char az[ 50 ];
	RECT r;
#	endif
	/*~~~~~~~~~~~*/

#	ifdef RASTERS_ON

#		if defined( _XBOX ) || defined( _XENON )
// rasters are displayed in function GDI_BeforeDisplay(...)
#		else// _XBOX

#			ifdef PSX2_TARGET
	if ( !sgb_DisplayRasters ) return;
	PRO_OneTrameEnding( 0 );

	/*~~~~~~~~~~~~~~~~*/
#			else
#				ifdef _GAMECUBE
#				else
	WNDCLASS x_Class;
	HDC hdc;
	/*~~~~~~~~~~~~~~~~*/

	if ( !ENG_h_Rasters )
	{
		x_Class.style         = 0;
		x_Class.lpfnWndProc   = DefWindowProc;
		x_Class.cbClsExtra    = 0;
		x_Class.cbWndExtra    = 0;
		x_Class.hInstance     = GetModuleHandle( NULL );
		x_Class.hIcon         = NULL;
		x_Class.hCursor       = NULL;
		x_Class.hbrBackground = CreateSolidBrush( GetSysColor( COLOR_BTNFACE ) );
		x_Class.lpszMenuName  = NULL;
		x_Class.lpszClassName = "Rasters";
		RegisterClass( &x_Class );

		ENG_h_Rasters = CreateWindowEx(
		        WS_EX_CLIENTEDGE | WS_EX_TOOLWINDOW,
		        "Rasters",
		        "Rasters",
		        WS_CAPTION | WS_POPUP | WS_BORDER | WS_THICKFRAME,
		        5,
		        5,
		        150,
		        50,
		        MAI_gh_MainWindow,
		        0,
		        GetModuleHandle( NULL ),
		        0 );
	}

	hdc = GetDC( ENG_h_Rasters );
	if ( !sgb_DisplayRasters )
	{
		if ( sbg_FirstDisp )
		{
			SetBkColor( hdc, GetSysColor( COLOR_BTNFACE ) );
			ShowWindow( ENG_h_Rasters, SW_SHOW );
			if ( TIM_gf_dt * TIM_gf_SynchroFrequency > 1.0f ) SetTextColor( hdc, RGB( 255, 0, 0 ) );
			sprintf( az, "%f", TIM_gf_dt * TIM_gf_SynchroFrequency );
			r.left = r.top = 5;
			r.right = r.bottom = 100;
			DrawText( hdc, az, strlen( az ), &r, 0 );

			sprintf( az, "%f", TIM_gf_realdt * TIM_gf_SynchroFrequency );
			r.top += 20;
			r.bottom += 20;
			DrawText( hdc, az, strlen( az ), &r, 0 );
		}
		else
		{
			ShowWindow( ENG_h_Rasters, SW_HIDE );
		}

		ReleaseDC( ENG_h_Rasters, hdc );
		return;
	}

	PRO_OneTrameEnding( ENG_h_Rasters, hdc, 0 );
	ReleaseDC( ENG_h_Rasters, hdc );
#				endif
#			endif
#		endif// _XBOX
#	endif    // RASTERS_ON
}

#endif /* _FINAL_ */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ENG_ForceStartRasters( void )
{
	/*~~*/
#ifdef RASTERS_ON
	int i;
	/*~~*/

	ENG_gpst_RasterEng_EngineDt.uw_StartCount      = 0;
	ENG_gpst_RasterEng_EngineRealDt.uw_StartCount  = 0;
	ENG_gpst_RasterEng_EngineFrames.uw_StartCount  = 0;
	ENG_gpst_RasterEng_Synchro.uw_StartCount       = 0;
	ENG_gpst_RasterEng_WinMsg.uw_StartCount        = 0;
	ENG_gpst_RasterEng_Input.uw_StartCount         = 0;
	ENG_gpst_RasterEng_Display.uw_StartCount       = 0;
	ENG_gpst_RasterEng_Activation.uw_StartCount    = 0;
	ENG_gpst_RasterEng_Visibility.uw_StartCount    = 0;
	ENG_gpst_RasterEng_TablesManager.uw_StartCount = 0;
	ENG_gpst_RasterEng_EOTcreation.uw_StartCount   = 0;
	ENG_gpst_RasterEng_AI.uw_StartCount            = 0;
	ENG_gpst_RasterEng_Events.uw_StartCount        = 0;
	ENG_gpst_RasterEng_SnP.uw_StartCount           = 0;
	ENG_gpst_RasterEng_ANI.uw_StartCount           = 0;
	ENG_gpst_RasterEng_DYN.uw_StartCount           = 0;
	ENG_gpst_RasterEng_COL.uw_StartCount           = 0;
	ENG_gpst_RasterEng_ODE_COL.uw_StartCount       = 0;
	ENG_gpst_RasterEng_ODE_SOLVER.uw_StartCount    = 0;
	ENG_gpst_RasterEng_REC.uw_StartCount           = 0;
	ENG_gpst_RasterEng_OneCall.uw_StartCount       = 0;
	ENG_gpst_RasterEng_OneLoop.uw_StartCount       = 0;

	for ( i = 0; i < ENG_C_NbUserRasters; i++ ) ENG_gapst_RasterEng_User[ i ].uw_StartCount = 0;

#	ifdef ACTIVE_EDITORS
	ENG_gpst_RasterEng_Editors.uw_StartCount      = 0;
	ENG_gpst_RasterEng_EditorRaster.uw_StartCount = 0;
	ENG_gpst_RasterEng_EditorsDt.uw_StartCount    = 0;
#	endif
#endif
}

extern "C" UINT WOR_DetectCameraCut( GDI_tdst_DisplayData *_pst_DD );
#ifdef _DEBUG
extern "C" void MEM_dbg_FindLastAllocatedCluster( void );
#endif

/*
 =======================================================================================================================
    Aim:    One frame of the engine main loop
 =======================================================================================================================
 */
static void s_OneTrame( void )
{
	JADED_PROFILER_START();

#ifdef _FINAL_
	if ( ENG_gb_Raster ) ENG_gf_TimeFinal = TIM_f_Clock_TrueRead();
#endif

	PRO_StartTrameRaster( &ENG_gpst_RasterEng_OneLoop );
	s_HandleWinMessages();

	/*
	 * We update the main clock (it calculates the time between two frames and the
	 * time since last reset of the main clock )
	 */
	TIM_Clock_Update();
	if ( ENG_gb_NeedToReinit )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World *pst_World;
		TAB_tdst_PFelem *pst_CurrentElem;
		TAB_tdst_PFelem *pst_EndElem;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		SOFT_ZList_Clear();
		MSG_GlobalReinit();
		AI_ReinitUniverse();

#ifdef _XENON_RENDER
		Xe_InvalidateRenderLists();
#endif

		pst_CurrentElem = TAB_pst_PFtable_GetFirstElem( &WOR_gst_Universe.st_WorldsTable );
		pst_EndElem     = TAB_pst_PFtable_GetLastElem( &WOR_gst_Universe.st_WorldsTable );
		for ( ; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++ )
		{
			pst_World = ( WOR_tdst_World * ) pst_CurrentElem->p_Pointer;
			if ( TAB_b_IsAHole( pst_World ) ) continue;

			ENG_ReinitOneWorld( pst_World, UNI_Cuc_TotalInit );
		}
#ifdef JADEFUSION
		void SPG2Holder_Modifier_Prepare();
		SPG2Holder_Modifier_Prepare();
#endif
	}

	ENG_gb_NeedToReinit = FALSE;

	if ( TIM_gf_MainClockForTextureScrolling > 1024.0f )
		TIM_gf_MainClockForTextureScrolling -= 1024.0f;

	/* Sound & textures */
	if ( MAI_gst_MainHandles.pst_DisplayData )
	{
		jaded::sys::profiler.StartProfiling( "SND_Update" );
		SND_Update( &MAI_gst_MainHandles.pst_DisplayData->st_Camera.st_Matrix );
		jaded::sys::profiler.EndProfiling( "SND_Update" );

		jaded::sys::profiler.StartProfiling( "TEX_Procedural_Update" );
		TEX_Procedural_Update( MAI_gst_MainHandles.pst_DisplayData );
		jaded::sys::profiler.EndProfiling( "TEX_Procedural_Update" );

		jaded::sys::profiler.StartProfiling( "TEX_Anim_Update" );
		TEX_Anim_Update( MAI_gst_MainHandles.pst_DisplayData );
		jaded::sys::profiler.EndProfiling( "TEX_Anim_Update" );
	}

#ifdef JADEFUSION
	// Handle Pause Delay

	if ( g_iReinitPauseDelay > 0 )
	{
		g_iReinitPauseDelay--;
	}

	if ( g_iReinitPauseScheduled && ( g_iReinitPauseDelay == 0 ) )
	{
		AI_EvalFunc_WORPause_C( 0, 0xFFFFFFFF );
		g_iReinitPauseScheduled = FALSE;
	}
#endif

#ifndef _FINAL_
	s_EngineCheat();
#endif

#ifndef ACTIVE_EDITORS
	s_EngineCheatFinal();
#endif

	/* Read inputs */
	_GSP_BeginRaster( 15 );

	if ( ENG_gp_Input && ( !ENG_gb_ForcePauseEngine ) )
	{
		PRO_StartTrameRaster( &ENG_gpst_RasterEng_Input );
		PROPS2_StartRaster( &PROPS2_gst_ENG_gp_Input );

		ENG_gp_Input();

		PROPS2_StopRaster( &PROPS2_gst_ENG_gp_Input );
		PRO_StopTrameRaster( &ENG_gpst_RasterEng_Input );
	}

	NET_ServerUpdate();
	NET_PlayerUpdate();

	_GSP_EndRaster( 15 );

	/* Engine */

	_GSP_BeginRaster( 18 );
	if ( ( ENG_gp_Engine ) && ( !ENG_gb_ForcePauseEngine ) )
	{

		PRO_StartTrameRaster( &ENG_gpst_RasterEng_OneCall );
		PROPS2_StartRaster( &PROPS2_gst_ENG_gp_Engine );

		ENG_gp_Engine();

		PROPS2_StopRaster( &PROPS2_gst_ENG_gp_Engine );
		PRO_StopTrameRaster( &ENG_gpst_RasterEng_OneCall );
	}
	_GSP_EndRaster( 18 );

	/* Display */
#ifndef JADEFUSION
	_GSP_BeginRaster( 19 );
#endif

	if ( ENG_gp_Display && MAI_gst_MainHandles.pst_DisplayData )
	{

		PRO_StartTrameRaster( &ENG_gpst_RasterEng_Display );
		PROPS2_StartRaster( &PROPS2_gst_ENG_gp_Display );
		{
#ifdef USE_DOUBLE_RENDERING
			//xenon ?? extern UINT WOR_DetectCameraCut(GDI_tdst_DisplayData *_pst_DD);
			extern u_int WOR_DetectCameraCut( GDI_tdst_DisplayData * _pst_DD );
			extern void WOR_DoubleRenderingCompute( GDI_tdst_DisplayData * _pst_DD, ULONG Mode, float fFactor );
			ENG_gp_CameraCutHasBeenDetected = 0;


			if ( WOR_DetectCameraCut( MAI_gst_MainHandles.pst_DisplayData ) )
			{
				ENG_gp_DoubleRenderingLocker    = 0x4;
				ENG_gp_CameraCutHasBeenDetected = 1;
			}


			if ( ENG_gp_DoubleRenderingLocker )
			{
				ENG_gp_DoubleRenderingLocker--;
				WOR_DoubleRenderingCompute( MAI_gst_MainHandles.pst_DisplayData, 101, 0.0f );// Save
			}


			if ( ENG_gp_DoubleRendering && ( !ENG_gp_DoubleRenderingLocker ) )
			{
				u_int CounterR;
				float SaveRLIScale;

				ULONG SaveRLIColor;

				SaveRLIScale = MAI_gst_MainHandles.pst_DisplayData->f_RLIScale;
				SaveRLIColor = MAI_gst_MainHandles.pst_DisplayData->ul_RLIColorDest;

				CounterR = ENG_gp_DoubleRendering;
				TIM_gf_dt /= ( float ) ( ENG_gp_DoubleRendering + 1 );
				for ( CounterR = 0; CounterR < ENG_gp_DoubleRendering; CounterR++ )
				{
					MAI_gst_MainHandles.pst_DisplayData->ul_DisplayInfo &= ~( GDI_Cul_DI_DoubleRendering_K | GDI_Cul_DI_DoubleRendering_I );
					MAI_gst_MainHandles.pst_DisplayData->ul_DisplayInfo |= GDI_Cul_DI_DoubleRendering_I;

					WOR_DoubleRenderingCompute( MAI_gst_MainHandles.pst_DisplayData, CounterR, ( float ) ( CounterR + 1 ) / ( float ) ( ENG_gp_DoubleRendering + 1 ) );

					TIM_gf_MainClockForTextureScrolling += TIM_gf_dt;
					MAI_gst_MainHandles.pst_DisplayData->f_RLIScale      = SaveRLIScale;
					MAI_gst_MainHandles.pst_DisplayData->ul_RLIColorDest = SaveRLIColor;
					ENG_gp_Display( MAI_gst_MainHandles.h_DisplayWindow, MAI_gst_MainHandles.pst_DisplayData );
					/* textures update */
					if ( MAI_gst_MainHandles.pst_DisplayData )
					{

						_GSP_BeginRaster( 20 );
						TEX_Procedural_Update( MAI_gst_MainHandles.pst_DisplayData );
						TEX_Anim_Update( MAI_gst_MainHandles.pst_DisplayData );
						_GSP_EndRaster( 20 );
					}
				}

				MAI_gst_MainHandles.pst_DisplayData->ul_DisplayInfo &= ~( GDI_Cul_DI_DoubleRendering_K | GDI_Cul_DI_DoubleRendering_I );
				MAI_gst_MainHandles.pst_DisplayData->ul_DisplayInfo |= GDI_Cul_DI_DoubleRendering_K;
				WOR_DoubleRenderingCompute( MAI_gst_MainHandles.pst_DisplayData, 100, 1.0f );// 100 Mean K

				TIM_gf_MainClockForTextureScrolling += TIM_gf_dt;
				MAI_gst_MainHandles.pst_DisplayData->f_RLIScale      = SaveRLIScale;
				MAI_gst_MainHandles.pst_DisplayData->ul_RLIColorDest = SaveRLIColor;
				ENG_gp_Display( MAI_gst_MainHandles.h_DisplayWindow, MAI_gst_MainHandles.pst_DisplayData );

				MAI_gst_MainHandles.pst_DisplayData->ul_DisplayInfo &= ~( GDI_Cul_DI_DoubleRendering_K | GDI_Cul_DI_DoubleRendering_I );
				TIM_gf_dt *= ( float ) ( ENG_gp_DoubleRendering + 1 );
			}
			else
#else
			{
				ENG_gp_CameraCutHasBeenDetected = 0;
				if ( WOR_DetectCameraCut( MAI_gst_MainHandles.pst_DisplayData ) )
				{
					ENG_gp_CameraCutHasBeenDetected = 1;
				}
			}
#endif
			{
				TIM_gf_MainClockForTextureScrolling += TIM_gf_dt;
				ENG_gp_Display( MAI_gst_MainHandles.h_DisplayWindow, MAI_gst_MainHandles.pst_DisplayData );
			}
		}

		PROPS2_StopRaster( &PROPS2_gst_ENG_gp_Display );
		PRO_StopTrameRaster( &ENG_gpst_RasterEng_Display );
	}

	FOGDYN_Reset();

	/* Inform editors that a trame is ending... This will not display the rasters ! */
#ifdef ACTIVE_EDITORS
	PRO_StartTrameRaster( &ENG_gpst_RasterEng_Editors );
	LINK_OneTrameEnding();
	PRO_StopTrameRaster( &ENG_gpst_RasterEng_Editors );
	ENG_bg_FirstFrameSpeedRun = FALSE;
#endif /* ACTIVE_EDITORS */

	/* Display rasters (engine mode) */
	PRO_StartTrameRaster( &ENG_gpst_RasterEng_EditorRaster );
#ifndef _FINAL_
	s_DisplayRasters();
#endif

#ifdef ACTIVE_EDITORS
	LINK_DisplayRasters();
#endif
	PRO_StopTrameRaster( &ENG_gpst_RasterEng_EditorRaster );

#ifndef ACTIVE_EDITORS
	MEM_Defrag( 1 );
#endif

#ifdef _DEBUG
	MEM_dbg_FindLastAllocatedCluster();
#endif

#ifndef _FINAL_
#	ifdef MEM_OPT
	{
		void MEM_vConditionnallyLogHeap();
		MEM_vConditionnallyLogHeap();
	}
#	endif// MEM_OPT
#endif    // _FINAL

	/* End of loop */
	PRO_StopTrameRaster( &ENG_gpst_RasterEng_OneLoop );

	JADED_PROFILER_END();
}

/*
 =======================================================================================================================
    Aim:    The engine main loop...
 =======================================================================================================================
 */

#define TICKS_PER_SECOND 60U
#define SKIP_TICKS       ( 1000U / TICKS_PER_SECOND )
#define MAX_FRAMESKIP    5U

void ENG_Loop( void )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	static float f_StartTimeEngineStopped = 0;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ENG_gb_EngineRunning = TRUE;

	_Try_
#ifdef ACTIVE_EDITORS

	        /* If we enter the engine loop again, we mesure the total time spend in the editors */
	        if ( f_StartTimeEngineStopped ) TIM_gf_EditorTime += ( TIM_f_Clock_TrueRead() - f_StartTimeEngineStopped );

	/* When break or step by step, change context */
	if ( AI_gb_ExitByBreak && !AI_gb_ErrorWhenBreak ) L_longjmp( AI_gst_ContextIn, 1 );
	AI_gb_ExitByBreak    = FALSE;
	AI_gb_ErrorWhenBreak = FALSE;

	/* When a break is raised, go here */
	if ( L_setjmp( AI_gst_ContextOut ) == 1 )
	{
		ENG_gb_AIRunning     = FALSE;
		ENG_gb_EngineRunning = FALSE;

		if ( jaded::sys::launchOperations.editorMode )
		{
			ESON_PauseAll( TRUE );
		}
		else
		{
			SND_StopAll( 0 );
		}

		f_StartTimeEngineStopped = TIM_f_Clock_TrueRead();
		_Return_( ; );
	}

#endif

	ENG_gb_ExitApplication  = FALSE;
	ENG_gb_ForceEndEngine   = FALSE;
	ENG_gb_ForcePauseEngine = FALSE;

	ENG_ForceStartRasters();

	INO_Joystick_Acquire();

	while ( !sfnb_EndGame() )
	{
		jaded::sys::profiler.StartProfiling( "Main Loop" );

		static uint64_t nextTick = 0;
		if ( nextTick == 0 )
		{
			nextTick = SDL_GetTicks64();
		}

		s_InitBeforeTrame();

		if ( !ENG_gb_LimitFPS || ENG_gb_OneStepEngine )
		{
			s_OneTrame();
		}
		else
		{
			uint64_t loops = 0;
			while ( SDL_GetTicks64() > nextTick && loops < MAX_FRAMESKIP )
			{
				s_OneTrame();

				nextTick += SKIP_TICKS;
				loops++;
			}
		}

		//double delta = ( double ) ( SDL_GetTicks64()  + SKIP_TICKS - nextTick ) / ( double ) ( SKIP_TICKS );
		// hogsy: TODO - move render loop and other crap down here, so we operate that EVERY tick

		s_DesInitAfterTrame();

		jaded::sys::profiler.EndProfiling( "Main Loop" );

#ifdef ACTIVE_EDITORS
		if ( ENG_gb_OneStepEngine ) break;
#endif
	}

	_Catch_
#ifdef ACTIVE_EDITORS
	        AI_gb_ExitByBreak = FALSE;
	AI_gb_ErrorWhenBreak      = TRUE;
#endif
	_End_
#ifdef ACTIVE_EDITORS

	INO_Joystick_Unacquire();

	/* Reset step info */
	ENG_gb_OneStepEngine = FALSE;
	ENG_gb_AIRunning     = FALSE;

	/*
	 * We stop the engine (F5 just pressed), so we start mesuring the time spend
	 * without the engine running
	 */
	f_StartTimeEngineStopped = TIM_f_Clock_TrueRead();
#endif

	if ( jaded::sys::launchOperations.editorMode )
	{
		ESON_PauseAll( TRUE );
	}
	else
	{
		SND_StreamPrefetchFlushAll();
		SND_StopAll( 0 );
	}

	ENG_gb_EngineRunning = FALSE;

	s_CheckResetRequest();
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
