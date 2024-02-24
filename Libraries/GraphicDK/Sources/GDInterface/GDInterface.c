/*$T GDInterface.c GC!1.65 12/30/99 12:14:23 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "BASe/MEMory/MEM.h"
#include "BASe/MEMory/MEMpro.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/IMPort/IMPbase.h"
#include "BIGfiles/BIGfat.h"
#include "CAMera/CAMstruct.h"
#include "ENGine/Sources/ENGcall.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/GRId/GRI_display.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIresolution.h"
#include "GDInterface/GDIrasters.h"
#include "GDInterface/GDIrequest.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEOzone.h"
#include "MATerial/MATSprite.h"
#include "GRObject/GROrender.h"
#include "GRObject/GROedit.h"
#include "LINks/LINKmsg.h"
#include "MATerial/MATstruct.h"
#include "TEXture/TEXcubemap.h"

#include "OpenGL/Sources/OGLtex.h"
#include "OpenGL/Sources/OGLinit.h"
#include "OpenGL/Sources/OGLrequest.h"

#include "SELection/SELection.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTHelper.h"
#include "SOFT/SOFTPickingBuffer.h"
#include "STRing/STRstruct.h"
#include "PArticleGenerator/PAGstruct.h"
#include "GFX/GFX.h"
#include "TABles/TABles.h"
#include "TEXture/TEXmemory.h"
#include "TEXture/TEXstruct.h"
#include "TEXture/TEXprocedural.h"
#include "TEXture/TEXanimated.h"
#include "TEXture/TEXhardwareload.h"
#include "TEXture/TEXconvert.h"
#include "TEXture/TEX_MESH.h"

#include "TIMer/TIMdefs.h"
#include "TIMer/PROfiler/PROPS2.h"
#include "WAYpoint/WAYpoint.h"
#include "MATerial/MATShadow.h"
#include "SOFT/SOFTbackgroundimage.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#ifdef JADEFUSION
#	include "TEXture/TEXcubemap.h"
#	if defined( ACTIVE_EDITORS )
#		include "BIGfiles/BIGread.h"
#		include "BIGfiles/SAVing/SAVdefs.h"
#		include "XenonGraphics/XeScreenshotSeqBuilder.h"
#	endif
#endif

#if defined( PSX2_TARGET ) && defined( __cplusplus )
extern "C"
{
#endif
	/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

int GDI_gi_GDIType = 0;// OpenGL rendering

	LONG GDI_gl_ReadaptOperation                     = 0;
	float GDI_gaf_ScreenRation[ GDI_Cul_SRC_Number ] = { 1.0, 1.0, 0.75, 0.5625 };
	GDI_tdst_DisplayData *GDI_gpst_CurDD             = NULL;
	extern BOOL Gx8_gb_Init;
	extern BOOL LOA_gb_SpeedMode;
	extern void LOA_BeginSpeedMode( BIG_KEY _ul_Key );
	extern void LOA_EndSpeedMode( void );
	extern UINT SPG2_gb_Recompute;

	/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

	/*
 =======================================================================================================================
 =======================================================================================================================
 */
	void GDI_InitModule( void )
	{
#ifdef _GAMECUBE
		GXI_Init();
#endif

#if !defined( PSX2_TARGET ) && !defined( _GAMECUBE ) && !defined( _XBOX ) && !defined( _XENON ) /* no MAD with PS2, GC or XBOX */
#	ifdef ACTIVE_EDITORS
		MAD_Rasterize_Init();

		{
			extern BOOL TEX_b_ImportCallBack( char *_psz_BigPathName, char *_psz_BigFileName, char *_psz_ExternName );

			IMP_b_AddImportCallback( "ipu", TEX_b_ImportCallBack );
			IMP_b_AddImportCallback( "bik", TEX_b_ImportCallBack );
			IMP_b_AddImportCallback( "xmv", TEX_b_ImportCallBack );
			IMP_b_AddImportCallback( "mtx", TEX_b_ImportCallBack );
			IMP_b_AddImportCallback( "mpg", TEX_b_ImportCallBack );
			IMP_b_AddImportCallback( "m2v", TEX_b_ImportCallBack );
		}

#	endif
#endif
		GRO_Struct_InitInterfaces();
		MAT_Init();
		LIGHT_Init();
		GEO_Init();
		GEO_StaticLOD_Init();
		CAM_Init();
		WAY_Init();
		//SPL_Init();
		STR_Init();
		PAG_Init();
#ifndef PSX2_TARGET
		TEX_Memory_Init( 20000000 );
#else
	TEX_Memory_Init( 000000 );
#endif
		TEX_File_Init();
		TEX_List_Init( &TEX_gst_GlobalList );
#if defined( TEX_USE_CUBEMAPS )
		TEX_CubeMap_Init( 256 );
#endif// defined(TEX_USE_CUBEMAPS)
		TEX_Procedural_Init();
		SOFT_ZList_Init();
		GDI_Resolution_Init();
		SOFT_BackgroundImage_MainInit();
	}

	/*
 =======================================================================================================================
 =======================================================================================================================
 */
	void GDI_CloseModule( void )
	{
		GEO_DebugObject_Destroy();
		STR_Close();
		PAG_Close();
		GFX_End();
		TEX_Memory_Close();
#if defined( TEX_USE_CUBEMAPS )
		TEX_CubeMap_Free( TRUE );
#endif// defined(TEX_USE_CUBEMAPS)
		TEX_List_Free( &TEX_gst_GlobalList );
		GDI_Resolution_Close();
		SOFT_BackgroundImage_MainClose();
		MAT_SpriteGen_Close();
	}

	/*
 =======================================================================================================================
    Aim:    Create and initialize display data structure
 =======================================================================================================================
 */
	GDI_tdst_DisplayData *GDI_fnpst_CreateDisplayData( void )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GDI_tdst_DisplayData *pst_DD;
		extern BOOL EDI_gb_ComputeMap;
		extern WATER_Export_Struct stExportWaterParrams;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MEMpro_StartMemRaster();
		pst_DD = ( GDI_tdst_DisplayData * ) MEM_p_AllocAlign( sizeof( GDI_tdst_DisplayData ), 64 );
		L_memset( pst_DD, 0, sizeof( GDI_tdst_DisplayData ) );
		MEMpro_StopMemRaster( MEMpro_Id_GDI_fnpst_CreateDisplayData_1 );

		WTR_SetDefaulExp( &stExportWaterParrams );

		pst_DD->ul_DisplayFlags = 0;
		pst_DD->ul_DrawMask = pst_DD->ul_CurrentDrawMask = 0xFFFFFFFF - ( GDI_Cul_DM_MaterialColor + GDI_Cul_DM_NoAutoClone );
		pst_DD->f_RLIScale                               = 1.0f;
		pst_DD->pv_ShadowStack                           = NULL;
		pst_DD->pst_AdditionalMaterial                   = NULL;

	pst_DD->st_ScreenFormat.ul_Flags           = GDI_Cul_SFF_169BlackBand;
	pst_DD->st_ScreenFormat.f_PixelYoverX      = 1.0f;
	pst_DD->st_ScreenFormat.f_ScreenYoverX     = 1.0f;
	pst_DD->st_ScreenFormat.l_ScreenRatioConst = GDI_Cul_SRC_4over3;

#ifdef ACTIVE_EDITORS
		CAM_Engine_Init( &pst_DD->st_SplitViewCamera );
#endif
		CAM_Engine_Init( &pst_DD->st_Camera );

		// ADAVID April 6th 2005
		// Switched to statically-allocated SOFT MatrixStack to avoid memory alignment issues
		SOFT_l_MatrixStack_Create( &pst_DD->st_MatrixStack );

		LIGHT_List_Init( &pst_DD->st_LightList, 256 );
		TEX_Manager_Init( &pst_DD->st_TexManager );

		//    pst_DD->pst_ComputingBuffers = (SOFT_tdst_ComputingBuffers *) MEM_p_AllocAlign( sizeof( SOFT_tdst_ComputingBuffers ), 64);
		{
			extern SOFT_tdst_ComputingBuffers *SOFT_gp_Compute;
			pst_DD->pst_ComputingBuffers = ( SOFT_tdst_ComputingBuffers * ) SOFT_gp_Compute;
		}
		L_memset( pst_DD->pst_ComputingBuffers, 0, sizeof( SOFT_tdst_ComputingBuffers ) );

#ifdef _XENON_RENDER
		pst_DD->pst_ComputingBuffers->ComputedColors = pst_DD->pst_ComputingBuffers->staticComputedColors;
#endif

#ifdef ACTIVE_EDITORS

		WAY_Links_Init( &pst_DD->st_DisplayedLinks );
		GEO_Zone_Init( &pst_DD->st_DisplayedZones );

		pst_DD->pst_PickingBuffer = ( struct SOFT_tdst_PickingBuffer_ * ) MEM_p_Alloc( sizeof( SOFT_tdst_PickingBuffer ) );
		L_memset( pst_DD->pst_PickingBuffer, 0, sizeof( SOFT_tdst_PickingBuffer ) );
		pst_DD->pst_EditorSplitViewCamObject = NULL;
		pst_DD->uc_EngineSplitViewCamera     = 0;
		pst_DD->uc_EditorSplitViewCamera     = 0;
		pst_DD->pst_SplitViewCamTarget       = NULL;
		MATH_SetIdentityMatrix( &pst_DD->st_OffsetToSplitViewCamTarget );
		pst_DD->pst_EditorCamObject = NULL;
		pst_DD->uc_EngineCamera     = 0;
		pst_DD->uc_EditorCamera     = 0;
		pst_DD->pst_CamTarget       = NULL;
		MATH_SetIdentityMatrix( &pst_DD->st_OffsetToCamTarget );
		pst_DD->ul_GFXRenderMask = 0xFFFFFFFF;

		L_strcpy( pst_DD->sz_SnapshotName, "Capture" );
		pst_DD->uc_SnapshotFlag = GDI_Cc_CaptureType_One;

		pst_DD->pst_Helpers = ( struct SOFT_tdst_Helpers_ * ) MEM_p_Alloc( sizeof( SOFT_tdst_Helpers ) );
		SOFT_Helpers_Init( pst_DD->pst_Helpers, pst_DD );

		pst_DD->pst_GridOptions = ( struct GRID_tdst_DisplayOptions_ * ) MEM_p_Alloc( sizeof( GRID_tdst_DisplayOptions ) );
		L_memset( pst_DD->pst_GridOptions, 0, sizeof( GRID_tdst_DisplayOptions ) );

		pst_DD->pst_EditOptions = ( struct GRO_tdst_EditOptions_ * ) MEM_p_Alloc( sizeof( GRO_tdst_EditOptions ) );
		L_memset( pst_DD->pst_EditOptions, 0, sizeof( GRO_tdst_EditOptions ) );
		pst_DD->pst_EditOptions->ul_Flags             = GRO_Cul_EOF_Vertex | GRO_Cul_EOF_ToolAcceptMultiple | GRO_Cul_EOF_SelectVisible;
		pst_DD->pst_EditOptions->f_VertexBlendColor   = 1.0f;
		pst_DD->pst_EditOptions->ul_VertexPaintColor  = 0;
		pst_DD->pst_EditOptions->f_ScaleForNormals    = 1.0f;
		pst_DD->pst_EditOptions->ul_VertexColor[ 0 ]  = 0xFF0000FF;
		pst_DD->pst_EditOptions->ul_VertexColor[ 1 ]  = 0xFF00FFFF;
		pst_DD->pst_EditOptions->ul_VertexColor[ 2 ]  = 0xFF007FFF;
		pst_DD->pst_EditOptions->ul_VertexColor[ 3 ]  = 0xFF00FF7F;
		pst_DD->pst_EditOptions->ul_EdgeColor[ 0 ]    = 0xFFFF7F00;
		pst_DD->pst_EditOptions->ul_EdgeColor[ 1 ]    = 0xFFFFFF7F;
		pst_DD->pst_EditOptions->ul_EdgeColor[ 2 ]    = 0xFFFF7F7F;
		pst_DD->pst_EditOptions->ul_EdgeColor[ 3 ]    = 0xFFFFFFFF;
		pst_DD->pst_EditOptions->ul_FaceColor[ 0 ]    = 0xFF0000FF;
		pst_DD->pst_EditOptions->ul_FaceColor[ 1 ]    = 0xFF00FFFF;
		pst_DD->pst_EditOptions->ul_FaceColor[ 2 ]    = 0xFF007FFF;
		pst_DD->pst_EditOptions->ul_FaceColor[ 3 ]    = 0xFF00FF7F;
		pst_DD->pst_EditOptions->ul_NormalColor[ 0 ]  = 0xFF00FF00;
		pst_DD->pst_EditOptions->ul_NormalColor[ 1 ]  = 0xFF00FFFF;
		pst_DD->pst_EditOptions->f_VertexSize         = 1.0f;
		pst_DD->pst_EditOptions->f_EdgeSize           = 1.0f;
		pst_DD->pst_EditOptions->f_FaceSize           = 1.0f;
		pst_DD->pst_EditOptions->f_NormalSize         = 1.0f;
		pst_DD->pst_GetLightForGO                     = NULL;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 0 ]  = 0xFFFFFFFF;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 1 ]  = 0xFFFFFF00;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 2 ]  = 0xFFFF00FF;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 3 ]  = 0xFF00FFFF;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 4 ]  = 0xFFFF0000;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 5 ]  = 0xFF00FF00;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 6 ]  = 0xFF0000FF;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 7 ]  = 0xFFCFCF00;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 8 ]  = 0xFFCF00CF;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 9 ]  = 0xFF00CFCF;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 10 ] = 0xFFCF0000;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 11 ] = 0xFF00CF00;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 12 ] = 0xFF0000CF;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 13 ] = 0xFF9F9F00;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 14 ] = 0xFF9F009F;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 15 ] = 0xFF009F9F;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 16 ] = 0xFF9F0000;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 17 ] = 0xFF009F00;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 18 ] = 0xFF00009F;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 19 ] = 0xFF6F6F00;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 20 ] = 0xFF6F006F;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 21 ] = 0xFF006F6F;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 22 ] = 0xFF6F0000;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 23 ] = 0xFF006F00;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 24 ] = 0xFF00006F;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 25 ] = 0xFF3F3F00;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 26 ] = 0xFF3F003F;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 27 ] = 0xFF003F3F;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 28 ] = 0xFF3F0000;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 29 ] = 0xFF003F00;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 30 ] = 0xFF00003F;
		pst_DD->pst_EditOptions->ul_FaceIDColor[ 31 ] = 0xFF000000;
		pst_DD->pst_EditOptions->i_RLIAdjust_X        = -10000;
		pst_DD->pst_EditOptions->i_RLIAdjust_Y        = 0;
		pst_DD->pst_EditOptions->i_RLIAdjust_HideSub  = 1;
		pst_DD->pst_EditOptions->i_RLIAdjust_Preview  = 1;
		pst_DD->pst_EditOptions->i_RLIAdjust_Colorize = 0;
		pst_DD->pst_EditOptions->i_RLIAdjust_Mode     = 0;
		pst_DD->pst_EditOptions->i_UVMapper_X         = 0;
		pst_DD->pst_EditOptions->i_UVMapper_Y         = 0;
		pst_DD->pst_EditOptions->i_UVMapper_W         = 0;
		pst_DD->pst_EditOptions->i_UVMapper_H         = 0;

		pst_DD->uc_ColMapDisplayMode = 0;
		pst_DD->uc_ColMapShowSlope   = 0;

#	ifdef JADEFUSION
		pst_DD->ul_HighlightMode     = 0;
		pst_DD->b_PasteGaoProperties = FALSE;
#	endif

		pst_DD->uc_DrawPostIt = 0;

		pst_DD->l_WPShowStatus  = 0xFFFFFFFF;
		pst_DD->l_WPShowStatus2 = 0xFFFFFFFF;

		pst_DD->pst_BackgroundImage = ( struct SOFT_tdst_BackgroundImage_ * ) MEM_p_Alloc( sizeof( SOFT_tdst_BackgroundImage ) );
		pst_DD->pst_BackgroundImage->w_Texture = -1;
		if ( !( LOA_gb_SpeedMode || LOA_IsBinarizing() || EDI_gb_ComputeMap ) ) SOFT_BackgroundImage_Init( pst_DD->pst_BackgroundImage );

		/* additional texture for debug object */
		if ( !( LOA_gb_SpeedMode || LOA_IsBinarizing() || EDI_gb_ComputeMap ) ) GEO_DebugObject_AddTexture();

		pst_DD->pst_OceanExp = &stExportWaterParrams;
#endif

#if defined( _XENON_RENDER )
		pst_DD->LastBlendingMode = -1;
#endif

#ifdef RASTERS_ON
		pst_DD->pst_Raster = ( GDI_tdst_Rasters * ) MEM_p_Alloc( sizeof( GDI_tdst_Rasters ) );
#endif

#ifdef JADEFUSION
		pst_DD->ul_LightCullingFlags = ( GDI_Cul_Light_Cull_Element | GDI_Cul_Light_Cull_Frustum_Gao | GDI_Cul_Light_Cull_Frustum_Element );
#endif

#ifdef ACTIVE_EDITORS
		pst_DD->b_AntiAliasingBlur = FALSE;
#else
	pst_DD->b_AntiAliasingBlur = TRUE;
#endif

		return pst_DD;
	}

	/*
 =======================================================================================================================
    Aim:    Destroy display data
 =======================================================================================================================
 */
	void GDI_fnv_DestroyDisplayData( GDI_tdst_DisplayData *_pst_DD )
	{
		if ( _pst_DD )
		{
			GDI_gpst_CurDD = _pst_DD;

			SDW_Destroy();
			SOFT_MatrixStack_Destroy( &_pst_DD->st_MatrixStack );
			LIGHT_List_Close( &_pst_DD->st_LightList );
			///MEM_FreeAlign( _pst_DD->pst_ComputingBuffers );

#ifdef RASTERS_ON
			GDI_Rasters_Close( _pst_DD->pst_Raster );
			MEM_Free( _pst_DD->pst_Raster );
#endif
#ifdef ACTIVE_EDITORS
			WAY_Links_Close( &_pst_DD->st_DisplayedLinks );
			GEO_Zone_Close( &_pst_DD->st_DisplayedZones );
			MEM_Free( _pst_DD->pst_PickingBuffer );
			MEM_Free( _pst_DD->pst_Helpers );
			MEM_Free( _pst_DD->pst_GridOptions );
			MEM_Free( _pst_DD->pst_EditOptions );
			if ( !LOA_gb_SpeedMode ) SOFT_BackgroundImage_Close( _pst_DD->pst_BackgroundImage );
			MEM_Free( _pst_DD->pst_BackgroundImage );
#endif
			MEM_FreeAlign( _pst_DD );
		}
	}

	/*
 =======================================================================================================================
    Aim:    Init interface
 =======================================================================================================================
 */

#define M_INITGDI( GDI, DRV_EXT )                                                                        \
	GDI->pfnpv_InitDisplay         = ( void *(         *) ( void ) ) DRV_EXT##_pst_CreateDevice;                \
	GDI->pfnv_DesinitDisplay       = DRV_EXT##_DestroyDevice;                                            \
	GDI->pfnl_OpenDisplay          = DRV_EXT##_l_Init;                                                   \
	GDI->pfnl_CloseDisplay         = DRV_EXT##_l_Close;                                                  \
	GDI->pfnl_ReadaptDisplay       = DRV_EXT##_l_ReadaptDisplay;                                         \
	GDI->pfnv_SetViewMatrix        = DRV_EXT##_SetViewMatrix;                                            \
	GDI->pfnv_SetProjectionMatrix  = DRV_EXT##_SetProjectionMatrix;                                      \
	GDI->pfnl_DrawIndexedTriangles = DRV_EXT##_l_DrawElementIndexedTriangles;                            \
	GDI->pfnl_DrawIndexedSprites   = DRV_EXT##_l_DrawElementIndexedSprites;                              \
	GDI->pfnl_Request              = DRV_EXT##_l_Request;                                                \
	GDI->pfnv_SetTextureBlending   = ( void (   *)( ULONG, ULONG, ULONG ) ) DRV_EXT##_SetTextureBlending; \
	GDI->pfnl_InitTexture          = DRV_EXT##_l_Texture_Init;                                           \
	GDI->pfnv_LoadTexture          = DRV_EXT##_Texture_Load;                                             \
	GDI->pfnl_StoreTexture         = DRV_EXT##_l_Texture_Store;                                          \
	GDI->pfnv_LoadPalette          = DRV_EXT##_Palette_Load;                                             \
	GDI->pfnv_Set_Texture_Palette  = DRV_EXT##_Set_Texture_Palette;                                      \
	GDI->pfnv_SetTextureTarget     = DRV_EXT##_SetTextureTarget;                                         \
	GDI->pfnv_SetViewMatrix_SDW    = DRV_EXT##_SetViewMatrix_SDW;

	LONG GDI_fnl_InitInterface( GDI_tdst_Interface *pst_GDI, LONG _l_GDIType )
	{
		M_INITGDI( pst_GDI, OGL );
		return 1;
	}

#if defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL )

	void GDI_ChangeInterface( GDI_tdst_DisplayData *_pst_DD, ULONG ulNew )
	{
		GDI_tdst_Interface *pst_GDI;
		WOR_tdst_World *pst_SaveWorld;
		pst_GDI = &_pst_DD->st_GDI;

		pst_SaveWorld = _pst_DD->pst_World;
		GDI_l_DetachWorld( _pst_DD );

		pst_GDI->pfnl_CloseDisplay( _pst_DD );
		pst_GDI->pfnv_DesinitDisplay( _pst_DD->pv_SpecificData );

		switch ( ulNew )
		{
			case 0:
				M_INITGDI( pst_GDI, OGL );
				break;
		}
		_pst_DD->pv_SpecificData = OGL_pst_CreateDevice();
		_pst_DD->st_GDI.pfnl_OpenDisplay( _pst_DD->h_Wnd, _pst_DD );

		GDI_l_AttachWorld( _pst_DD, pst_SaveWorld );
	}

	ULONG GDI_GetInterface( GDI_tdst_DisplayData *_pst_DD )
	{
		GDI_tdst_Interface *pst_GDI;
		pst_GDI = &_pst_DD->st_GDI;
		return 0;
	}
#endif// defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL )

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef PSX2_TARGET
	extern void GSP_pst_InitDevice( void );

	LONG GDI_AttachDisplay( GDI_tdst_DisplayData *_pst_DD )
	{

		GDI_gpst_CurDD = _pst_DD;
		GDI_DetachDisplay( _pst_DD );

		GSP_pst_InitDevice();

		/* We want 70 % of the screen ---> 255 */
		_pst_DD->f_LODVal = ( 100.0f / 200.0f ) * 510.0f * ( ( _pst_DD->st_Device.l_Width > _pst_DD->st_Device.l_Height ) ? fInv( ( float ) _pst_DD->st_Device.l_Width ) : fInv( ( float ) _pst_DD->st_Device.l_Height ) );

		CAM_AssignCameraToDevice( &_pst_DD->st_ScreenFormat, &_pst_DD->st_Camera, _pst_DD->st_Device.l_Width, _pst_DD->st_Device.l_Height );

		if ( GDI_gl_ReadaptOperation )
		{
			return 1;
		}


		return _pst_DD->st_GDI.pfnl_OpenDisplay( _pst_DD );
	}
#else
#	ifdef _GAMECUBE
LONG GDI_AttachDisplay( GDI_tdst_DisplayData *_pst_DD )
{
	GDI_gpst_CurDD = _pst_DD;

	GDI_gpst_CurDD->st_Device.l_Width = GXI_Global_ACCESS( Xsize ) = FRAME_BUFFER_WIDTH;
	GDI_gpst_CurDD->st_Device.l_Height = GXI_Global_ACCESS( Ysize ) = FRAME_BUFFER_HEIGTH;

	/* We want 70 % of the screen ---> 255 */
	_pst_DD->f_LODVal = ( 100.0f / 200.0f ) * 510.0f * ( ( _pst_DD->st_Device.l_Width > _pst_DD->st_Device.l_Height ) ? fInv( ( float ) _pst_DD->st_Device.l_Width ) : fInv( ( float ) _pst_DD->st_Device.l_Height ) );

	CAM_AssignCameraToDevice( &_pst_DD->st_ScreenFormat, &_pst_DD->st_Camera, _pst_DD->st_Device.l_Width, _pst_DD->st_Device.l_Height );

	return 1;
}
#	else
#		if defined( _XBOX ) || defined( _XENON )
LONG GDI_AttachDisplay( GDI_tdst_DisplayData *_pst_DD, HWND _h_Wnd )
{
	LONG lReturnValue;
	UINT uiWidth, uiHeight;

	GDI_gpst_CurDD = _pst_DD;

#			ifdef JADEFUSION
	lReturnValue   = _pst_DD->st_GDI.pfnl_OpenDisplay( _h_Wnd, _pst_DD );
	g_oXeRenderer.GetBackbufferResolution( &uiWidth, &uiHeight );
	_pst_DD->st_Device.l_Width  = ( LONG ) uiWidth;
	_pst_DD->st_Device.l_Height = ( LONG ) uiHeight;

#			else
	_pst_DD->st_Device.l_Width  = 640;
	_pst_DD->st_Device.l_Height = 480;
#			endif

	/*    _pst_DD->st_Device.l_Width = 512;
    _pst_DD->st_Device.l_Height = 512;*/

	/* We want 70 % of the screen ---> 255 */
	_pst_DD->f_LODVal = ( 100.0f / 200.0f ) * 510.0f * ( ( _pst_DD->st_Device.l_Width > _pst_DD->st_Device.l_Height ) ? fInv( ( float ) _pst_DD->st_Device.l_Width ) : fInv( ( float ) _pst_DD->st_Device.l_Height ) );

	CAM_AssignCameraToDevice( &_pst_DD->st_ScreenFormat, &_pst_DD->st_Camera, _pst_DD->st_Device.l_Width, _pst_DD->st_Device.l_Height );

#			ifdef JADEFUSION
	return lReturnValue;
#			else
	return _pst_DD->st_GDI.pfnl_OpenDisplay( _h_Wnd, _pst_DD );
#			endif
}

#		else
LONG GDI_AttachDisplay( GDI_tdst_DisplayData *_pst_DD, HWND _h_Wnd )
{
	/*~~~~~~~~~~~~~~~~*/
	RECT st_Rect;
	int i_OpenDisplay;
	/*~~~~~~~~~~~~~~~~*/

	if ( _pst_DD->h_Wnd != NULL )
	{
		i_OpenDisplay = 0;
		GDI_DetachDisplay( _pst_DD );
	}
	else
		i_OpenDisplay = 1;

	_pst_DD->h_Wnd = _h_Wnd;

	GetClientRect( _h_Wnd, &st_Rect );
	_pst_DD->st_Device.l_Width  = st_Rect.right - st_Rect.left;
	_pst_DD->st_Device.l_Height = st_Rect.bottom - st_Rect.top;

	/* We want 70 % of the screen ---> 255 */
	_pst_DD->f_LODVal = ( 100.0f / 200.0f ) * 510.0f * ( ( _pst_DD->st_Device.l_Width > _pst_DD->st_Device.l_Height ) ? fInv( ( float ) _pst_DD->st_Device.l_Width ) : fInv( ( float ) _pst_DD->st_Device.l_Height ) );

	CAM_AssignCameraToDevice( &_pst_DD->st_ScreenFormat, &_pst_DD->st_Camera, _pst_DD->st_Device.l_Width, _pst_DD->st_Device.l_Height );

#			ifdef ACTIVE_EDITORS
	if ( !_pst_DD->pst_PickingBuffer->dst_Pixel )
		SOFT_PickingBuffer_Init( _pst_DD->pst_PickingBuffer, _pst_DD->st_Device.l_Width, _pst_DD->st_Device.l_Height );
#			endif

	if ( GDI_gl_ReadaptOperation && !i_OpenDisplay ) return 1;

#			ifdef JADEFUSION
	{
		LONG lRetVal = _pst_DD->st_GDI.pfnl_OpenDisplay( _h_Wnd, _pst_DD );

#				if defined( _XENON_RENDER_PC )
		if ( !lRetVal && _pst_DD->st_GDI.pfnv_Clear == Xe_Clear )
		{
			GDI_PrepareAllForXenonGraphics();
		}
#				endif

		return lRetVal;
	}
#			else
	return _pst_DD->st_GDI.pfnl_OpenDisplay( _h_Wnd, _pst_DD );
#			endif
}

#		endif// _XBOX
#	endif    /* _GAMECUBE */
#endif        /* PSX2_TARGET */

	/*
 =======================================================================================================================
 =======================================================================================================================
 */
	LONG GDI_DetachDisplay( GDI_tdst_DisplayData *_pst_DD )
	{
#ifndef PSX2_TARGET
#	ifndef _GAMECUBE
#		if !defined( _XBOX ) && !defined( _XENON )
		LONG w, h;
		RECT st_Rect;

		if ( GDI_gl_ReadaptOperation )
		{
			GetClientRect( _pst_DD->h_Wnd, &st_Rect );
			w = st_Rect.right - st_Rect.left;
			h = st_Rect.bottom - st_Rect.top;
		}
#		endif// _XBOX
#	endif
#endif

		_pst_DD->h_Wnd = NULL;

#ifdef ACTIVE_EDITORS
		if ( GDI_gl_ReadaptOperation && ( w != _pst_DD->pst_PickingBuffer->l_Width || h != _pst_DD->pst_PickingBuffer->l_Height ) )
			SOFT_PickingBuffer_Close( _pst_DD->pst_PickingBuffer );
#endif

		if ( GDI_gl_ReadaptOperation ) return 1;

		GDI_l_DetachWorld( _pst_DD );
		return _pst_DD->st_GDI.pfnl_CloseDisplay( _pst_DD );
	}

	/*
 =======================================================================================================================
 =======================================================================================================================
 */

	LONG GDI_ReadaptDisplay( GDI_tdst_DisplayData *_pst_DD, HWND _h_Wnd )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_World *pst_SaveWorld;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GDI_gl_ReadaptOperation = 1;
#if ( defined( PSX2_TARGET ) || defined( _GAMECUBE ) )
		GDI_AttachDisplay( _pst_DD );
#else
	GDI_AttachDisplay( _pst_DD, _h_Wnd );
#endif
		GDI_gl_ReadaptOperation = 0;
#ifdef ACTIVE_EDITORS
		SOFT_PickingBuffer_Reinit( _pst_DD->pst_PickingBuffer, _pst_DD->st_Device.l_Width, _pst_DD->st_Device.l_Height );
#endif

//#ifdef ACTIVE_EDITORS
#if defined( ACTIVE_EDITORS ) && !defined( _XENON_RENDER_PC )
		if ( _pst_DD->st_TexManager.ul_Flags & TEX_Manager_ForceReload )
#endif
		{
#if defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL )
#	ifdef JADEFUSION
			if ( !GDI_b_IsXenonGraphics() )
#	endif
#endif
			{
				pst_SaveWorld = _pst_DD->pst_World;
				GDI_l_DetachWorld( _pst_DD );
			}
		}
		if ( !_pst_DD->st_GDI.pfnl_ReadaptDisplay( _h_Wnd, _pst_DD ) )
			return 0;
#if defined( ACTIVE_EDITORS ) && !defined( _XENON_RENDER_PC )
		if ( _pst_DD->st_TexManager.ul_Flags & TEX_Manager_ForceReload )
#endif
		{
#if defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL )
			if ( !GDI_b_IsXenonGraphics() )
#endif
			{
				GDI_l_AttachWorld( _pst_DD, pst_SaveWorld );
			}
		}
		return 1;
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef _GAMECUBE
	extern void GXI_BeforeDisplay( void );
#endif
	void GDI_BeforeDisplay( GDI_tdst_DisplayData *_pst_DD )
	{
		ULONG ul_Color;

		GDI_gpst_CurDD = _pst_DD;

		ImGuiInterface_NewFrame();

		PRO_StartTrameRaster( &_pst_DD->pst_Raster->st_BeforeDisplay );

#ifdef _XENON_RENDER
#	if defined( ACTIVE_EDITORS )
		if ( GDI_b_IsXenonGraphics() )
#	endif
		{
			Xe_BeforeDisplay();
		}
#endif

		// Clear the profiling information
		memset( &_pst_DD->profilingInformation, 0, sizeof( GDI_tdst_ProfilingInformation_ ) );

		if ( !( _pst_DD->ul_DisplayFlags & GDI_Cul_DF_DoNotRender ) )
		{
			ul_Color = ( _pst_DD->pst_World ) ? _pst_DD->pst_World->ul_BackgroundColor : 0;
			if ( _pst_DD->ul_DisplayFlags & GDI_Cul_DF_DoNotClear )
			{
				OGL_Clear( GDI_Cl_ZBuffer, ul_Color );
				_pst_DD->ul_DisplayFlags &= ~GDI_Cul_DF_DoNotClear;
			}
			else
			{
				OGL_Clear( GDI_Cl_ColorBuffer | GDI_Cl_ZBuffer, ul_Color );
			}

			_pst_DD->st_GDI.pfnl_Request( GDI_Cul_Request_SetFogParams, ( ULONG ) &_pst_DD->st_Fog );
			_pst_DD->ul_RenderingCounter++;
		}

#ifdef ACTIVE_EDITORS
		if ( _pst_DD->pst_World )
			MATH_CopyMatrix( &_pst_DD->pst_World->st_CameraPosSave, &_pst_DD->st_Camera.st_Matrix );
		GEO_DebugObject_SetLightMatrix( _pst_DD );
#endif
		PRO_SetRasterLong( &_pst_DD->pst_Raster->st_RenderingCount, _pst_DD->ul_RenderingCounter );
		PRO_SetRasterLong( &_pst_DD->pst_Raster->st_ViewportWidth, _pst_DD->st_Device.l_Width );
		PRO_SetRasterLong( &_pst_DD->pst_Raster->st_ViewportHeight, _pst_DD->st_Device.l_Height );

		CAM_AssignCameraToDevice( &_pst_DD->st_ScreenFormat, &_pst_DD->st_Camera, _pst_DD->st_Device.l_Width, _pst_DD->st_Device.l_Height );
		_pst_DD->st_GDI.pfnv_SetProjectionMatrix( &_pst_DD->st_Camera );
		CAM_Inverse( &_pst_DD->st_Camera );
		SOFT_MatrixStack_Reset( &_pst_DD->st_MatrixStack, &_pst_DD->st_Camera.st_InverseMatrix );

#ifdef ACTIVE_EDITORS
		if ( _pst_DD->ul_DisplayFlags & GDI_Cul_DF_UsePickingBuffer )
		{
			SOFT_PickingBuffer_Clear( _pst_DD->pst_PickingBuffer );
			SOFT_PickingBuffer_SetSelWindow( _pst_DD->pst_PickingBuffer );
		}
#endif

		LIGHT_List_Reset( &_pst_DD->st_LightList );

#ifdef ACTIVE_EDITORS
		WAY_Links_Clear( &_pst_DD->st_DisplayedLinks );
		GEO_Zone_Clear( &_pst_DD->st_DisplayedZones );

		/* check texture number */
		if ( ( _pst_DD->pst_World ) && ( _pst_DD->st_GDI.pfnl_Request( GDI_Cul_Request_NumberOfTextures, TEX_gst_GlobalList.l_NumberOfTextures ) ) )
			GDI_l_AttachWorld( _pst_DD, _pst_DD->pst_World );
#endif

		PRO_StopTrameRaster( &_pst_DD->pst_Raster->st_BeforeDisplay );

// Display XBox rasters
#if ( !defined( _XENON ) && defined( _XBOX ) && defined( RASTERS_ON ) )
		if ( sgb_DisplayRasters )
		{
			extern void Gx8_SetRastersRenderState( void );
			static LARGE_INTEGER s_stLastPerfCounterValue  = { 0 };
			static float gs_fPerformanceTimerTicksPerFrame = 0.0f;
			LARGE_INTEGER xPerfCounterAfterPresent;
			float fLastFrameLength;

#	ifdef XBOX_TUNING
			XBOX_vUpdatePerfResultMenu();
#	endif// XBOX_TUNING

			if ( gs_fPerformanceTimerTicksPerFrame == 0.0f )
			{
				LARGE_INTEGER stFrequency_;
				QueryPerformanceFrequency( &stFrequency_ );// in counts per second
				gs_fPerformanceTimerTicksPerFrame = ( ( float ) ( _int64 ) stFrequency_.QuadPart ) / 60.0f /* or 50 ? */;
			}

			QueryPerformanceCounter( &xPerfCounterAfterPresent );
			fLastFrameLength         = ( ( float ) ( ( _int64 ) ( xPerfCounterAfterPresent.QuadPart ) - ( _int64 ) ( s_stLastPerfCounterValue.QuadPart ) ) ) / gs_fPerformanceTimerTicksPerFrame;
			s_stLastPerfCounterValue = xPerfCounterAfterPresent;

			Gx8_SetRastersRenderState();
#	if !defined( _XENON )

			IDirect3DDevice8_SetTextureStageState( GX8_M_SD( GDI_gpst_CurDD )->mp_D3DDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
			IDirect3DDevice8_SetTextureStageState( GX8_M_SD( GDI_gpst_CurDD )->mp_D3DDevice, 0, D3DTSS_TEXCOORDINDEX, 0 );
			IDirect3DDevice8_SetTextureStageState( GX8_M_SD( GDI_gpst_CurDD )->mp_D3DDevice, 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
			IDirect3DDevice8_SetTextureStageState( GX8_M_SD( GDI_gpst_CurDD )->mp_D3DDevice, 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			IDirect3DDevice8_SetTextureStageState( GX8_M_SD( GDI_gpst_CurDD )->mp_D3DDevice, 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			IDirect3DDevice8_SetTextureStageState( GX8_M_SD( GDI_gpst_CurDD )->mp_D3DDevice, 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
			IDirect3DDevice8_SetTextureStageState( GX8_M_SD( GDI_gpst_CurDD )->mp_D3DDevice, 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
			IDirect3DDevice8_SetTextureStageState( GX8_M_SD( GDI_gpst_CurDD )->mp_D3DDevice, 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
			IDirect3DDevice8_SetTextureStageState( GX8_M_SD( GDI_gpst_CurDD )->mp_D3DDevice, 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
			IDirect3DDevice8_SetTextureStageState( GX8_M_SD( GDI_gpst_CurDD )->mp_D3DDevice, 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

			XB_fn_vBeginDrawText();
#	endif

			if ( sgb_DisplayRasters )
			{
#	if !defined( _XENON )
				XB_fn_vDisplayInfoRaster( fLastFrameLength );
#	endif
				PRO_OneTrameEnding( 0 );
			}
#	if !defined( _XENON )
			XB_fn_vEndDrawText();
#	endif
		}
#endif// (_XBOX && RASTERS_ON))
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
	void GDI_CopyToWindow4BigSnap( char *Src, char *Dst, ULONG W, ULONG H, ULONG DPos )
	{
		Dst += ( ( DPos >> 2 ) & 3 ) * W * 3;
		Dst += ( DPos & 3 ) * 3 * W * 4 * H;
		while ( H-- )
		{
			ULONG Counter;
			Counter = W;
			while ( Counter-- )
			{
				*( Dst++ ) = *( Src++ );
				*( Dst++ ) = *( Src++ );
				*( Dst++ ) = *( Src++ );
			}
			Dst -= W * 3;
			Dst += ( W << 2 ) * 3;
		}
	}

	ULONG g_ul_BIG_SNAPSHOT_COUNTER = 0;
#endif

	void GDI_AfterDisplay( GDI_tdst_DisplayData *_pst_DD )
	{
#if !defined( _XENON ) && defined( _XBOX )

		Gx8_EndReflectionDraw();

#endif

		PRO_StartTrameRaster( &_pst_DD->pst_Raster->st_AfterDisplay );
		GDI_gpst_CurDD = _pst_DD;

		/* testing buffer for some GFX : lens flare */
		if ( _pst_DD->ul_DisplayFlags & GDI_cul_DF_DepthReadBeforeFlip )
			if ( _pst_DD->pst_World )
				GFX_Test( _pst_DD->pst_World->pst_GFX );

		ImGuiInterface_Render();

		/* flip */
		if ( !( _pst_DD->ul_DisplayFlags & ( GDI_Cul_DF_DoNotRender | GDI_Cul_DF_DoNotFlip ) ) )
			OGL_Flip();
		else
			_pst_DD->ul_DisplayFlags &= ~GDI_Cul_DF_DoNotFlip;

		/* Recalc SPG2 */
		SPG2_gb_Recompute--;
		if ( SPG2_gb_Recompute == 0 )
		{
			extern void SPG2_SpecialGridHasChanged();
			SPG2_SpecialGridHasChanged();
		}
		else if ( SPG2_gb_Recompute < 0 )
		{
			SPG2_gb_Recompute = 0;
		}

#ifdef ACTIVE_EDITORS
#	ifdef JADEFUSION
		if ( ( _pst_DD->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Running ) && ( _pst_DD->uc_SnapshotFlag & GDI_Cc_CaptureFlag_ForSeqBuilder ) )
		{
			//skip screenshot

			if ( ( _pst_DD->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Type ) == GDI_Cc_CaptureType_One )
				_pst_DD->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_Running;
		}
		else if ( ( _pst_DD->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Running ) || g_ul_BIG_SNAPSHOT_COUNTER )
#	else
		if ( ( _pst_DD->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Running ) || g_ul_BIG_SNAPSHOT_COUNTER )
#	endif
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			HBITMAP hbm;
			HDC hdc, hcompatibledc;
			BITMAPINFO st_BmpInfo;
			char sz_Name[ 260 ];
			void *bitmap;
			TEX_tdst_File_Desc st_Tex, st_TexResize;
			float f_StartTimeEditors;
			LONG l_width;

			static char *BIGSnap = NULL;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			f_StartTimeEditors = TIM_f_Clock_TrueRead();

			l_width = _pst_DD->st_Device.Vw;
			BAS_ZERO( &st_BmpInfo, sizeof( BITMAPINFO ) );
			st_BmpInfo.bmiHeader.biWidth       = l_width & ( ~3 );
			st_BmpInfo.bmiHeader.biHeight      = _pst_DD->st_Device.Vh;
			st_BmpInfo.bmiHeader.biPlanes      = 1;
			st_BmpInfo.bmiHeader.biBitCount    = 24;
			st_BmpInfo.bmiHeader.biSize        = sizeof( BITMAPINFOHEADER );
			st_BmpInfo.bmiHeader.biCompression = BI_RGB;

			hdc           = GetDC( _pst_DD->h_Wnd );
			hbm           = CreateDIBSection( hdc, &st_BmpInfo, DIB_RGB_COLORS, ( void           **) &bitmap, NULL, 0 );
			hcompatibledc = CreateCompatibleDC( hdc );

			if ( SelectObject( hcompatibledc, hbm ) )
			{
				BitBlt( hcompatibledc, 0, 0, _pst_DD->st_Device.Vw, _pst_DD->st_Device.Vh, hdc, _pst_DD->st_Device.Vx, _pst_DD->st_Device.Vy, SRCCOPY );
			}

			st_Tex.uw_Width         = ( unsigned short ) _pst_DD->st_Device.Vw & ( ~3 );
			st_Tex.uw_Height        = ( unsigned short ) _pst_DD->st_Device.Vh;
			st_Tex.uc_BPP           = 24;
			st_Tex.uc_PaletteBPC    = 0;
			st_Tex.uw_PaletteLength = 0;
			st_Tex.p_Bitmap         = bitmap;

			if ( g_ul_BIG_SNAPSHOT_COUNTER )
			{
				if ( !BIGSnap )
					BIGSnap = ( char * ) malloc( ( 3 * st_Tex.uw_Width * st_Tex.uw_Height ) << 4 );
				if ( BIGSnap )
				{
					GDI_CopyToWindow4BigSnap( ( char * ) bitmap, BIGSnap, _pst_DD->st_Device.Vw, _pst_DD->st_Device.Vh, g_ul_BIG_SNAPSHOT_COUNTER & 0x3f );
					if ( ( g_ul_BIG_SNAPSHOT_COUNTER & 0x3f ) == 0 )
					{
						while ( 1 )
						{
							sprintf( sz_Name, "%s%04d_BIG.tga", _pst_DD->sz_SnapshotName, _pst_DD->uc_SnapshotCurrent );
							if ( _access( sz_Name, 0 ) )
								break;
							_pst_DD->uc_SnapshotCurrent++;
						}
						st_Tex.uw_Width <<= 2;
						st_Tex.uw_Height <<= 2;
						st_Tex.uc_BPP           = 24;
						st_Tex.uc_PaletteBPC    = 0;
						st_Tex.uw_PaletteLength = 0;
						st_Tex.p_Bitmap         = BIGSnap;

						TEX_l_File_SaveTga( sz_Name, &st_Tex );
						free( BIGSnap );
						BIGSnap = NULL;
					}
				}
			}
			else
			{
				if ( _pst_DD->uc_SnapshotFlag & GDI_Cc_CaptureFlag_SaveInBf )
				{
					st_Tex.ul_AMask = 0xFF000000;
					st_Tex.ul_RMask = 0x000000FF;
					st_Tex.ul_GMask = 0x0000FF00;
					st_Tex.ul_BMask = 0x00FF0000;

					L_memcpy( &st_TexResize, &st_Tex, sizeof( TEX_tdst_File_Desc ) );
					if ( st_Tex.uw_Height > st_Tex.uw_Width )
					{
						if ( st_Tex.uw_Height > 128 )
						{
							st_TexResize.uw_Height = 128;
							st_TexResize.uw_Width  = ( unsigned short ) ( ( ( float ) st_Tex.uw_Width / ( float ) st_Tex.uw_Height ) * 128.0 );
						}
					}
					else
					{
						if ( st_Tex.uw_Width > 128 )
						{
							st_TexResize.uw_Width  = 128;
							st_TexResize.uw_Height = ( unsigned short ) ( ( ( float ) st_Tex.uw_Height / ( float ) st_Tex.uw_Width ) * 128.0 );
						}
					}
					if ( ( st_TexResize.uw_Width != st_Tex.uw_Width ) || ( st_TexResize.uw_Height != st_Tex.uw_Height ) )
					{
						st_TexResize.p_Bitmap = ( void * ) L_malloc( 4 * 128 * 128 );
						TEX_l_ConvertSize( &st_Tex, &st_TexResize );
						TEX_ul_File_SaveTgaInBF( _pst_DD->sz_SnapshotPath, _pst_DD->sz_SnapshotName, &st_TexResize );
						L_free( st_TexResize.p_Bitmap );
					}
					else
						TEX_ul_File_SaveTgaInBF( _pst_DD->sz_SnapshotPath, _pst_DD->sz_SnapshotName, &st_Tex );
				}
				else if ( _pst_DD->uc_SnapshotFlag & GDI_Cc_CaptureFlag_ForCBM )
				{
					TEX_ul_File_SaveTgaInBF( _pst_DD->sz_SnapshotPath, _pst_DD->sz_SnapshotName, &st_Tex );
				}
				else
				{
					while ( 1 )
					{

						sprintf( sz_Name, "%s%04d.tga", _pst_DD->sz_SnapshotName, _pst_DD->uc_SnapshotCurrent );
						if ( _access( sz_Name, 0 ) )
							break;
						_pst_DD->uc_SnapshotCurrent++;
					}
					TEX_l_File_SaveTga( sz_Name, &st_Tex );
				}
			}


			DeleteObject( hbm );
			DeleteDC( hcompatibledc );
			ReleaseDC( _pst_DD->h_Wnd, hdc );

			_pst_DD->uc_SnapshotCurrent++;
			if ( ( _pst_DD->uc_SnapshotFlag & GDI_Cc_CaptureFlag_Type ) == GDI_Cc_CaptureType_One )
				_pst_DD->uc_SnapshotFlag &= ~GDI_Cc_CaptureFlag_Running;

			TIM_gf_EditorTime += ( TIM_f_Clock_TrueRead() - f_StartTimeEditors );
		}

#endif
		PRO_StopTrameRaster( &_pst_DD->pst_Raster->st_AfterDisplay );
	}

	/*
 =======================================================================================================================
 =======================================================================================================================
 */
	LONG GDI_l_DetachWorld( GDI_tdst_DisplayData *_pst_DD )
	{
		if ( _pst_DD->pst_World == NULL ) return 1;
		GDI_gpst_CurDD = _pst_DD;

		OGL_Texture_Unload( _pst_DD );
		TEX_Procedural_Reinit( _pst_DD );
		TEX_Anim_Reinit( _pst_DD );

#if defined( TEX_USE_CUBEMAPS )
		TEX_CubeMap_HardwareUnload();
#endif

		/* Detach world */
		_pst_DD->pst_World = NULL;
		return 1;
	}

/*
 =======================================================================================================================
    Debugging : log to a file key loaded
 =======================================================================================================================
 */

//#define  DEBUG_ATTACHWORLD
#ifdef DEBUG_ATTACHWORLD

#	define M_4Debug_LogTexVar()    \
		static int LogTexCount = 0; \
		LogTexCount++;

#	define M_4Debug_LogTex( _i_, _key_ )                         \
		{                                                         \
			L_FILE x_File;                                        \
			char sz_Name[ 64 ];                                   \
                                                                  \
			sprintf( sz_Name, "x:/TexLog%02d.txt", LogTexCount ); \
			x_File = L_fopen( sz_Name, "at" );                    \
			if ( x_File )                                         \
			{                                                     \
				fprintf( x_File, "%03d -> %08X\n", _i_, _key_ );  \
				fclose( x_File );                                 \
			}                                                     \
		}

#else

#	define M_4Debug_LogTexVar()
#	define M_4Debug_LogTex( a, b )

#endif

	/* if a rawpal texture comes here that means that it is not supported (ex : only non supported palette and raw) */
	/* so we have to convert it */
	void GDI_TransformUnsupportedRawPal( GDI_tdst_DisplayData *_pst_DD, TEX_tdst_Data *_pst_Tex, TEX_tdst_File_Desc *_pst_TexDesc, TEX_tdst_File_Desc *_pst_RawDesc )
	{
		TEX_tdst_Data *pst_RawTex;
		TEX_tdst_Palette *pst_Pal;
		ULONG *pul_Dest, ul_Size;
		UCHAR *puc_Src;

		//pst_RawTex = TEX_gst_GlobalList.dst_Texture + _pst_TexDesc->st_Params.ul_Params[0];
		//pst_Pal = TEX_gst_GlobalList.dst_Palette + _pst_TexDesc->st_Params.ul_Params[1];
		pst_RawTex = TEX_gst_GlobalList.dst_Texture + _pst_TexDesc->w_TexRaw;
		pst_Pal    = TEX_gst_GlobalList.dst_Palette + _pst_TexDesc->w_TexPal;

		/* special for font */
		if ( _pst_RawDesc->st_Params.uw_Flags & TEX_FP_ThisIsAFont )
			STR_FontAddAlternativeKey( pst_RawTex->ul_Key, _pst_Tex->ul_Key );

		/* conversion */
		if ( TEX_l_File_GetContent( pst_RawTex->ul_Key, _pst_RawDesc ) )
		{
			ul_Size = _pst_TexDesc->st_Params.uw_Width * _pst_TexDesc->st_Params.uw_Height;
			TEX_M_File_Alloc( _pst_TexDesc->p_Bitmap, 4 * ul_Size, void );

			pul_Dest = ( ULONG * ) _pst_TexDesc->p_Bitmap;
			puc_Src  = ( UCHAR  *) _pst_RawDesc->p_Bitmap;

			if ( _pst_DD->st_TexManager.ul_Flags & TEX_Manager_RGB2BGR )
			{
				if ( _pst_RawDesc->uc_BPP == 4 )
					TEX_Convert_4To32SwapRB( pul_Dest, puc_Src, pst_Pal->pul_Color, ul_Size );
				else if ( _pst_RawDesc->uc_BPP == 8 )
					TEX_Convert_8To32SwapRB( pul_Dest, puc_Src, pst_Pal->pul_Color, ul_Size );
			}
			else
			{
				if ( _pst_RawDesc->uc_BPP == 4 )
					TEX_Convert_4To32( pul_Dest, puc_Src, pst_Pal->pul_Color, ul_Size );
				else if ( _pst_RawDesc->uc_BPP == 8 )
					TEX_Convert_8To32( pul_Dest, puc_Src, pst_Pal->pul_Color, ul_Size );
			}
			_pst_TexDesc->uc_BPP = 32;
			TEX_M_File_Free( _pst_RawDesc->p_Bitmap );
		}
	}//*/


	/*
 =======================================================================================================================
 =======================================================================================================================
 */
	LONG GDI_l_AttachWorld( GDI_tdst_DisplayData *_pst_DD, WOR_tdst_World *_pst_World )
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		TEX_tdst_Data *pst_Tex;
		char *pc_UsedTexture;
		LONG l_Size, l_NbTextures, l_NbStartTextures;
		TEX_tdst_File_Desc *dst_TexDesc, *pst_CurTexDesc, *pst_OtherDesc;
		int i, i_Tex, i_Restart, i_Type, i_SecondPass, i_Start;
		LONG l_GetInfoOk;
		void *p_Data;
		MAT_tdst_SpriteGen *pst_SG;
		short w_Texture;
		USHORT uw_Flags;
		TEX_tdst_Animated *pst_Ani;
		UCHAR uc_Number;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		M_4Debug_LogTexVar();

		if ( !_pst_DD ) return 0;

		LOA_StartLoadRaster( LOA_Cte_LDI_AttachWorld );
		if ( _pst_DD->pst_World != NULL ) GDI_l_DetachWorld( _pst_DD );
		GDI_gpst_CurDD = _pst_DD;

		/* Attach world */
		_pst_DD->pst_World = _pst_World;

		/* reinit fog : dont force no fog */
		_pst_DD->st_Fog.c_Flag = 0;

		/* reinit ZList */
		SOFT_ZList_Clear();

		/* The end if we attach a dummy world */
		if ( _pst_World == NULL )
		{
			LOA_StopLoadRaster( LOA_Cte_LDI_AttachWorld );
			return 1;
		}

		/* is there textures ? */
		l_NbTextures = TEX_gst_GlobalList.l_NumberOfTextures;
		if ( l_NbTextures == 0 )
		{
			LOA_StopLoadRaster( LOA_Cte_LDI_AttachWorld );
			return 1;
		}

		LOA_BeginSpeedMode( 0xFF800000 );

		/* create temporary texture data */
		l_Size      = l_NbTextures * sizeof( TEX_tdst_File_Desc );
		dst_TexDesc = ( TEX_tdst_File_Desc * ) MEM_p_VMAlloc( l_Size );
		L_memset( dst_TexDesc, 0, l_Size );

		/* Get used textures */
		GDI_M_TimerReset( GDI_f_Delay_AttachWorld_TextureUsed );
		GDI_M_TimerStart( GDI_f_Delay_AttachWorld_TextureUsed );
		pc_UsedTexture    = MAT_pc_GetAllUsedTextureIndex( _pst_DD, l_NbTextures );
		l_NbStartTextures = l_NbTextures;
		GDI_M_TimerStop( GDI_f_Delay_AttachWorld_TextureUsed );

		/* Load all textures informations */
		GDI_M_TimerReset( GDI_f_Delay_AttachWorld_TextureInfos );
		GDI_M_TimerStart( GDI_f_Delay_AttachWorld_TextureInfos );
		PROPS2_StartRaster( &PROPS2_gst_OGL_l_AttachWorld2 );
		i_Start      = 0;
		i_SecondPass = 0;
		//i_AnotherFirstPass = 0;
		w_Texture = -1;

	OGL_TextureInformationLoad_StartAgain:

		for ( i = i_Start; i < l_NbTextures; i++ )
		{
			pst_Tex        = TEX_gst_GlobalList.dst_Texture + i;
			pst_CurTexDesc = dst_TexDesc + i;

			if ( i_SecondPass || ( pc_UsedTexture[ i ] != 2 ) )
				pst_CurTexDesc->uw_DescFlags = TEX_Cuw_DF_VeryBadBoy;

			if ( i_SecondPass || ( pc_UsedTexture[ i ] == 1 ) )
			{
				if ( !i_SecondPass ) pc_UsedTexture[ i ] = 2;
				PROPS2_StartRaster( &PROPS2_gst_TEX_l_File_GetInfo );

				M_4Debug_LogTex( i, pst_Tex->ul_Key );

				l_GetInfoOk = TEX_l_File_GetInfo( pst_Tex->ul_Key, pst_CurTexDesc, TRUE );
				PROPS2_StopRaster( &PROPS2_gst_TEX_l_File_GetInfo );
				if ( l_GetInfoOk )
				{
					/* sprite generator texture */
					if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_SprFile )
					{
						TEX_gst_GlobalList.dst_Texture[ i ].uw_Flags |= TEX_uw_IsSpriteGen;
						pst_SG                                                    = ( MAT_tdst_SpriteGen                                                    *) pst_CurTexDesc->st_Params.ul_Params[ 0 ];
						*( ULONG * ) &TEX_gst_GlobalList.dst_Texture[ i ].w_Width = ( ULONG ) pst_SG;

						/* id of new texture added */
						w_Texture = pst_SG->s_TextureIndex;
					}
					else
					{
						/* texture is used and exist, delete the flag TEX_Cuw_DF_VeryBadBoy */
						pst_CurTexDesc->uw_DescFlags &= ~TEX_Cuw_DF_VeryBadBoy;
						if ( !( _pst_DD->st_TexManager.ul_Flags & TEX_Manager_UseMipmap ) )
							pst_CurTexDesc->st_Params.uw_Flags &= ~TEX_FP_MipmapOn;

						/* procedural texture */
						if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_ProFile )
						{
							i_Type            = ( int ) pst_CurTexDesc->st_Params.ul_Params[ 0 ];
							p_Data            = ( void            *) pst_CurTexDesc->st_Params.ul_Params[ 1 ];
							uw_Flags          = ( USHORT ) pst_CurTexDesc->st_Params.ul_Params[ 2 ];
							pst_Tex->w_Height = pst_CurTexDesc->uw_Height;
							pst_Tex->w_Width  = pst_CurTexDesc->uw_Width;
							PROPS2_StartRaster( &PROPS2_gst_TEX_Procedural_Add );
							TEX_Procedural_Add( pst_Tex->w_Index, i_Type, uw_Flags, p_Data );
							PROPS2_StopRaster( &PROPS2_gst_TEX_Procedural_Add );
						}

						/* animated texture */
						else if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_AniFile )
						{
							TEX_gst_GlobalList.dst_Texture[ i ].uw_Flags |= TEX_uw_Ani;
							uc_Number = ( UCHAR ) pst_CurTexDesc->st_Params.ul_Params[ 0 ];
							p_Data    = ( void    *) pst_CurTexDesc->st_Params.ul_Params[ 1 ];
							uw_Flags  = ( USHORT ) pst_CurTexDesc->st_Params.ul_Params[ 2 ];
							pst_Ani   = TEX_pst_Anim_Add( pst_Tex->w_Index, uc_Number, uw_Flags, p_Data );

							if ( pst_Ani->uc_Number )
							{
								i_Restart = 0;
								for ( i_Tex = 0; i_Tex < ( int ) pst_Ani->uc_Number; i_Tex++ )
								{
									w_Texture                         = TEX_w_List_AddTexture( &TEX_gst_GlobalList, pst_Ani->dst_Tex[ i_Tex ].ul_Key, 1 );
									pst_Ani->dst_Tex[ i_Tex ].w_Index = w_Texture;
									if ( ( w_Texture < l_NbStartTextures ) && ( pc_UsedTexture[ w_Texture ] == 0 ) )
									{
										pc_UsedTexture[ w_Texture ] = 1;
										if ( w_Texture < i )
											i_Restart = 1;
									}
								}

								pst_Tex->w_Height = pst_Ani->dst_Tex->w_Index;
								w_Texture         = -1;
								if ( i_Restart )
									goto OGL_TextureInformationLoad_StartAgain;
							}
						}

						/* raw pal association */
						else if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_RawPalFile )
						{
							/* id of new texture added */
							TEX_gst_GlobalList.dst_Texture[ i ].uw_Flags |= TEX_uw_RawPal;
							w_Texture = TEX_w_Manager_ChooseGoodInTex( &_pst_DD->st_TexManager, pst_CurTexDesc );
						}
#if defined( _XENON_RENDER )
						else if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_XeDDSFile )
						{
							pst_CurTexDesc->uc_BPP      = 0x80;
							pst_CurTexDesc->uc_FinalBPP = 0x80;
						}
#endif
					}

					/* 
                 * if texture used by sprite generator (or rawpal file) exist we have to do some particular treatment 
                 * if texture is new nothing to do here, it's treated in second pass 
                 * else we set it's flag UsedTexture to one and if it's index is less thant current one
                 * we set flag i_AnotherFirstPass to watch another time all primary textures
                 */
					if ( w_Texture != -1 )
					{
						if ( ( w_Texture < l_NbStartTextures ) && ( pc_UsedTexture[ w_Texture ] == 0 ) )
						{
							pc_UsedTexture[ w_Texture ] = 1;
							if ( w_Texture < i )
								goto OGL_TextureInformationLoad_StartAgain;
						}
						w_Texture = -1;
					}
				}
			}
		}

		/* test if some more texture has been added (by sprite generator for example) */
		if ( l_NbTextures != TEX_gst_GlobalList.l_NumberOfTextures )
		{
			/* reallocate and init additional temporary structure data */
			PROPS2_StartRaster( &PROPS2_gst_tex_create_realloc );
			l_Size      = TEX_gst_GlobalList.l_NumberOfTextures * sizeof( TEX_tdst_File_Desc );
			dst_TexDesc = ( TEX_tdst_File_Desc * ) MEM_p_VMRealloc( dst_TexDesc, l_Size );
			PROPS2_StopRaster( &PROPS2_gst_tex_create_realloc );
			for ( i = l_NbTextures; i < TEX_gst_GlobalList.l_NumberOfTextures; i++ )
				L_memset( dst_TexDesc + i, 0, sizeof( TEX_tdst_File_Desc ) );

			/* init and go for a second pass */
			i_SecondPass = 1;
			i_Start      = l_NbTextures;
			l_NbTextures = TEX_gst_GlobalList.l_NumberOfTextures;
			goto OGL_TextureInformationLoad_StartAgain;
		}

		/* load palettes */
		for ( i = 0; i < TEX_gst_GlobalList.l_NumberOfPalettes; i++ )
		{
			TEX_File_LoadPalette( TEX_gst_GlobalList.dst_Palette + i, ( _pst_DD->st_TexManager.ul_Flags & TEX_Manager_RGB2BGR ) );
		}
#if defined( _XENON_RENDER )
		// Redo the texture association for textures that must be converted by the Xenon texture manager
		for ( i = 0; i < TEX_gst_GlobalList.l_NumberOfTextures; ++i )
		{
			pst_Tex        = TEX_gst_GlobalList.dst_Texture + i;
			pst_CurTexDesc = dst_TexDesc + i;

			if ( pst_CurTexDesc->uw_DescFlags & TEX_Cuw_DF_VeryBadBoy ) continue;

			if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_RawPalFile )
			{
				if ( pst_CurTexDesc->w_TexFlags & TEX_XenonNeedsConversion )
				{
					ULONG ulSizeBK = dst_TexDesc[ pst_CurTexDesc->w_TexTC ].st_Tex.st_XeProperties.ul_FileSize;

					L_memcpy( &dst_TexDesc[ pst_CurTexDesc->w_TexTC ].st_Tex.st_XeProperties,
					          &pst_CurTexDesc->st_Tex.st_XeProperties, sizeof( TEX_tdst_XenonFileTex ) );

					dst_TexDesc[ pst_CurTexDesc->w_TexTC ].st_Tex.st_XeProperties.ul_FileSize = ulSizeBK;
					dst_TexDesc[ pst_CurTexDesc->w_TexTC ].w_TexFlags |= TEX_XenonConvert;
					dst_TexDesc[ pst_CurTexDesc->w_TexTC ].uw_DescFlags |= TEX_Cuw_DF_XenonFileAsBitmap;
				}
			}
		}
#endif
		/* another pass on texture for tex file */
		/*
     * another pass on texture for tex file:
     *      on va positionner le flag VeryBadBoy pour ces textures dans les cas suivants
     *          raw + pal dispo et acceptée ou TrueColor dispo et accepté
     *      on va le laisser si on doit passer en TrueColor un raw+pal (palette non acceptée et pas de True color dispo)
     */
		for ( i = 0; i < TEX_gst_GlobalList.l_NumberOfTextures; i++ )
		{
			pst_Tex        = TEX_gst_GlobalList.dst_Texture + i;
			pst_CurTexDesc = dst_TexDesc + i;

			if ( pst_CurTexDesc->uw_DescFlags & TEX_Cuw_DF_VeryBadBoy ) continue;

			if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_RawPalFile )
			{
				if ( pst_CurTexDesc->w_TexFlags & TEX_TakeTrueColor )
				{
					/* mode TrueColor avec texture TrueColor dispo */
					pst_Tex->w_TexTC = pst_CurTexDesc->w_TexTC;
					pst_Tex->w_Dummy = -1;
					pst_CurTexDesc->uw_DescFlags |= TEX_Cuw_DF_VeryBadBoy;
				}
				else /* palette */
				{
					pst_Tex->w_TexRaw = pst_CurTexDesc->w_TexRaw;
					pst_Tex->w_TexPal = pst_CurTexDesc->w_TexPal;
					if ( pst_CurTexDesc->w_TexFlags & TEX_ChangeToTrueColor )
					{
						if ( pst_Tex->w_TexRaw == -1 )
						{
							/* mode aucune donnée dispo */
							pst_CurTexDesc->uw_DescFlags |= TEX_Cuw_DF_VeryBadBoy;
						}
						else
						{
							/* mode TrueColor mais seulement pal + raw */
							pst_OtherDesc = dst_TexDesc + pst_Tex->w_TexRaw;
							pst_OtherDesc->uw_DescFlags |= TEX_Cuw_DF_VeryBadBoy;
							pst_CurTexDesc->st_Params.uc_Format = pst_OtherDesc->st_Params.uc_Format;
							pst_CurTexDesc->st_Params.uw_Width  = pst_OtherDesc->st_Params.uw_Width;
							pst_CurTexDesc->st_Params.uw_Height = pst_OtherDesc->st_Params.uw_Height;
							TEX_File_DevelopParams( pst_CurTexDesc );
						}
					}
					else
					{
						/* mode raw + pal avec raw + pal dispo */
						pst_CurTexDesc->uw_DescFlags |= TEX_Cuw_DF_VeryBadBoy;
					}
				}
			}
		}

		PROPS2_StopRaster( &PROPS2_gst_OGL_l_AttachWorld2 );

		/* special for background image (editor only ) */
		SOFT_BackGroundImage_SetTextureInfo( _pst_DD->pst_BackgroundImage, dst_TexDesc );

		GDI_M_TimerStop( GDI_f_Delay_AttachWorld_TextureInfos );

		/* Compute all textures compression */
		GDI_M_TimerReset( GDI_f_Delay_AttachWorld_TextureManager );
		GDI_M_TimerStart( GDI_f_Delay_AttachWorld_TextureManager );

		PROPS2_StartRaster( &PROPS2_gst_TEX_Manager_Reinit );
		TEX_Manager_Reinit( &_pst_DD->st_TexManager );
		PROPS2_StopRaster( &PROPS2_gst_TEX_Manager_Reinit );

		PROPS2_StartRaster( &PROPS2_gst_TEX_Manager_ComputeCompression );
		TEX_Manager_ComputeCompression( &_pst_DD->st_TexManager, dst_TexDesc, l_NbTextures, 0 );
		TEX_Manager_ComputeCompression( &_pst_DD->st_TexManager, dst_TexDesc, l_NbTextures, 1 );
		PROPS2_StopRaster( &PROPS2_gst_TEX_Manager_ComputeCompression );

		GDI_M_TimerStop( GDI_f_Delay_AttachWorld_TextureManager );

		GDI_M_TimerReset( GDI_f_Delay_AttachWorld_TextureCreate );
		GDI_M_TimerReset( GDI_f_Delay_AttachWorld_TextureCreate_GetContent );
		GDI_M_TimerReset( GDI_f_Delay_AttachWorld_TextureCreate_Compress );
		GDI_M_TimerReset( GDI_f_Delay_AttachWorld_TextureCreate_Convert24To32 );
		GDI_M_TimerReset( GDI_f_Delay_AttachWorld_TextureCreate_Convert32Colors );
		GDI_M_TimerReset( GDI_f_Delay_AttachWorld_TextureCreate_LoadHard );
		GDI_M_TimerReset( GDI_f_Delay_AttachWorld_TextureCreate_Mipmap );
		GDI_M_TimerStart( GDI_f_Delay_AttachWorld_TextureCreate );

		PROPS2_StartRaster( &PROPS2_gst_OGL_l_AttachWorld3 );

		/* Create textures */
		if ( _pst_DD->st_GDI.pfnl_InitTexture( _pst_DD, l_NbTextures ) )
		{
			_pst_DD->st_TexManager.l_TextureNumber = l_NbTextures;

			/* Create textures */
			pst_Tex        = TEX_gst_GlobalList.dst_Texture;
			pst_CurTexDesc = dst_TexDesc;

			/* ======================================================================== */
			/* Firstly load All bitmaps (included 8bit & 4Bits Texture)					*/
			/* ======================================================================== */
			for ( i = 0; i < l_NbTextures; i++, pst_Tex++, pst_CurTexDesc++ )
			{
#ifdef JADEFUSION
#	if defined( ACTIVE_EDITORS )
				extern BOOL EDI_gb_ComputeMap;
				if ( EDI_gb_ComputeMap && ( pst_Tex->uw_Flags & TEX_uw_RawPal ) )
				{
					BIG_INDEX ul_File;
					char az_Path[ BIG_C_MaxLenPath ];
					char az_File[ BIG_C_MaxLenName ];
					TEX_tdst_File_Desc t_Desc;
					extern unsigned int WOR_gul_WorldKey;
					ULONG ul_World;

					ul_World = BIG_ul_SearchKeyToFat( WOR_gul_WorldKey );
					if ( ul_World != BIG_C_InvalidIndex )
					{
#		if defined( _XENON_RENDER )
						sprintf( az_Path, "%s", TEX_Csz_X360TexturesPath );
						sprintf( az_File, "0x%08x.xtx", pst_Tex->ul_Key );
#		else
						BIG_ComputeFullName( BIG_ParentFile( ul_World ), az_Path );
						L_strcat( az_Path, "/BinTextures" );
						sprintf( az_File, "%x", pst_Tex->ul_Key );
#		endif

						ul_File = BIG_ul_SearchKeyToFat( pst_Tex->ul_Key );
						if ( ul_File != BIG_C_InvalidIndex )
						{
							L_memcpy( &t_Desc, pst_Tex, sizeof( TEX_tdst_File_Desc ) );
							if ( ( pst_CurTexDesc->st_Tex.st_XeProperties.ul_NativeTexture != 0 ) &&
							     ( pst_CurTexDesc->st_Tex.st_XeProperties.ul_NativeTexture != BIG_C_InvalidKey ) )
							{
								ULONG ul_Pos = BIG_ul_SearchKeyToPos( pst_CurTexDesc->st_Tex.st_XeProperties.ul_NativeTexture );
								ULONG ul_Size;
								CHAR *pBuffer = BIG_pc_ReadFileTmpMustFree( ul_Pos, &ul_Size );

								SAV_Begin( az_Path, az_File );
								SAV_Buffer( pBuffer, ul_Size );
								SAV_ul_End();

								L_free( pBuffer );
							}
							else if ( ( pst_CurTexDesc->st_Tex.st_XeProperties.ul_OriginalTexture != 0 ) &&
							          ( pst_CurTexDesc->st_Tex.st_XeProperties.ul_OriginalTexture != BIG_C_InvalidKey ) )
							{
								TEX_XeGenerateDDS( &pst_CurTexDesc->st_Tex.st_XeProperties, az_Path, az_File );
							}
						}
					}
				}
#	endif
#endif
				if ( pst_CurTexDesc->uw_DescFlags & TEX_Cuw_DF_VeryBadBoy ) continue;
				if ( pst_Tex->uw_Flags & TEX_uw_Ani ) continue;

				if ( ( pst_CurTexDesc->st_Params.uw_Flags & TEX_FP_Interface )
#ifndef _GAMECUBE
				     && ( _pst_DD->st_TexManager.ul_Flags & TEX_Manager_StoreInterfaceTex )
#endif
				)
					_pst_DD->st_TexManager.l_RamTextureNumber++;
				else
					_pst_DD->st_TexManager.l_VRamTextureNumber++;
				TEX_Manager_AddInfo( &_pst_DD->st_TexManager, pst_Tex, pst_CurTexDesc );

				if (
				        ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_RawPalFile ) && !( pst_CurTexDesc->uw_DescFlags & TEX_Cuw_DF_SpecialNoLoad ) )
				{
					/* if a rawpal texture comes here that means that it is not supported (ex : only non supported palette and raw) */
					/* so we have to convert it */
					//pst_OtherDesc = dst_TexDesc + pst_CurTexDesc->st_Params.ul_Params[0];
					pst_OtherDesc = dst_TexDesc + pst_CurTexDesc->w_TexRaw;
					GDI_TransformUnsupportedRawPal( _pst_DD, pst_Tex, pst_CurTexDesc, pst_OtherDesc );
				}

				MEMpro_StartMemRaster();
				TEX_Hardware_Load( _pst_DD, pst_Tex, pst_CurTexDesc, i );
				MEMpro_StopMemRaster( MEMpro_Id_OGL_ul_Texture_Create );
			}

			/* ======================================================================== */
			/* Secondly load All Palettes */
			/* ======================================================================== */

			for ( i = 0; i < TEX_gst_GlobalList.l_NumberOfPalettes; i++ )
			{
				_pst_DD->st_GDI.pfnv_LoadPalette( _pst_DD, TEX_gst_GlobalList.dst_Palette + i, i );
			}

			/* ======================================================================== */
			/* Thirdly Associate pallettes & textures */
			/* ======================================================================== */

			pst_Tex        = TEX_gst_GlobalList.dst_Texture;
			pst_CurTexDesc = dst_TexDesc;
			for ( i = 0; i < l_NbTextures; i++, pst_Tex++, /*pul_Texture++, */ pst_CurTexDesc++ )
			{
				ULONG palette, tex;
				/* Is it a raw pal? */
				if ( pst_CurTexDesc->uc_FinalBPP > 8 ) continue;
				if ( pst_Tex->uw_Flags & TEX_uw_RawPal )
				{
					/* What is the pal? */
					palette = pst_Tex->w_Width;
					/* What is the tex? */
					tex           = pst_Tex->w_Height;
					pst_OtherDesc = dst_TexDesc + tex;// les infos + d'autres dans TEX_gst_GlobalList.dst_Texture + tex
					_pst_DD->st_GDI.pfnv_Set_Texture_Palette( _pst_DD, i, tex, palette );
				}
			}

			/* ======================================================================== */
			/* procedural : les textures procédurales d'interface doivent utiliser      
			/*          le buffer mémoire ou est stockée la texture                     
			/* ======================================================================== */
			TEX_Procedural_AfterLoad( _pst_DD );
		}

		PROPS2_StopRaster( &PROPS2_gst_OGL_l_AttachWorld3 );
		GDI_M_TimerStop( GDI_f_Delay_AttachWorld_TextureCreate );

		/* free local data */
		MEM_Free( pc_UsedTexture );
		MEM_Free( dst_TexDesc );

		/* unload palette */
		if ( TEX_gst_GlobalList.l_NumberOfPalettes )
		{
			for ( i = 0; i < TEX_gst_GlobalList.l_NumberOfPalettes; i++ )
			{
				if ( TEX_gst_GlobalList.dst_Palette[ i ].uc_Flags & TEX_uc_KeepPalInMem ) continue;
				if ( ( TEX_gst_GlobalList.dst_Palette + i )->pul_Color )
					TEX_M_File_Free( ( TEX_gst_GlobalList.dst_Palette + i )->pul_Color );
				( TEX_gst_GlobalList.dst_Palette + i )->pul_Color = NULL;
			}
			TEX_gst_GlobalList.l_NumberOfPalettes = 0;
		}

#ifdef ACTIVE_EDITORS
		if ( _pst_DD->st_TexManager.ul_Flags & TEX_Manager_DumpBench )
		{
			/*~~~~~~~~~~~~~~~~~*/
			char sz_Text[ 256 ];
			int i;
			ULONG ul_Index;
			/*~~~~~~~~~~~~~~~~~*/

			GDI_M_TimerPrint( "Get texture used      ", GDI_f_Delay_AttachWorld_TextureUsed );
			GDI_M_TimerPrint( "Get texture infos     ", GDI_f_Delay_AttachWorld_TextureInfos );
			GDI_M_TimerPrint( "Texture manager       ", GDI_f_Delay_AttachWorld_TextureManager );
			GDI_M_TimerPrint( "Create textures       ", GDI_f_Delay_AttachWorld_TextureCreate );
			GDI_M_TimerPrint( "....Get content       ", GDI_f_Delay_AttachWorld_TextureCreate_GetContent );
			GDI_M_TimerPrint( "....Compress          ", GDI_f_Delay_AttachWorld_TextureCreate_Compress );
			GDI_M_TimerPrint( "....Convert 24 to 32  ", GDI_f_Delay_AttachWorld_TextureCreate_Convert24To32 );
			GDI_M_TimerPrint( "....Convert 32 colors ", GDI_f_Delay_AttachWorld_TextureCreate_Convert32Colors );
			GDI_M_TimerPrint( "....Load hard         ", GDI_f_Delay_AttachWorld_TextureCreate_LoadHard );
			GDI_M_TimerPrint( "....compute Mipmap    ", GDI_f_Delay_AttachWorld_TextureCreate_Mipmap );

			sprintf( sz_Text, "Interface Texture      : %d", _pst_DD->st_TexManager.l_NbTextureInterface );
			LINK_PrintStatusMsg( sz_Text );
			for ( i = 0; i < _pst_DD->st_TexManager.l_NbTextureInterface; i++ )
			{
				ul_Index = BIG_ul_SearchKeyToFat( _pst_DD->st_TexManager.aul_TextureInterface[ i ] );
				if ( ul_Index == BIG_C_InvalidIndex )
					sprintf( sz_Text, "....[%08X] bad ref", _pst_DD->st_TexManager.aul_TextureInterface[ i ] );
				else
					sprintf( sz_Text, "....[%08X] %s (%d x %d)->(%d x %d)", BIG_FileKey( ul_Index ), BIG_NameFile( ul_Index ),
					         _pst_DD->st_TexManager.auw_TIW[ i ],
					         _pst_DD->st_TexManager.auw_TIH[ i ],
					         _pst_DD->st_TexManager.auw_TICW[ i ],
					         _pst_DD->st_TexManager.auw_TICH[ i ] );
				LINK_PrintStatusMsg( sz_Text );
			}
		}
#endif

		STR_FontResolveAlternativeKey();

		WOR_World_Attached( _pst_World );

#if defined( TEX_USE_CUBEMAPS )
		TEX_CubeMap_HardwareLoad();
#endif

		LOA_EndSpeedMode();
		LOA_StopLoadRaster( LOA_Cte_LDI_AttachWorld );
		return 1;
	}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
	void GDI_SwapCameras( GDI_tdst_DisplayData *_pst_DD )
	{
#	ifndef JADEFUSION//POPO WARNING a RECASTER !!!!!
		CAM_tdst_Camera st_TempCam;
		void *p_Temp;
		unsigned char uc_Temp;
		struct GEO_tdst_Object_ *pst_TempObj;
		MATH_tdst_Matrix st_TempMatrix;

		L_memcpy( &st_TempCam, &_pst_DD->st_Camera, sizeof( CAM_tdst_Camera ) );
		L_memcpy( &_pst_DD->st_Camera, &_pst_DD->st_SplitViewCamera, sizeof( CAM_tdst_Camera ) );
		L_memcpy( &_pst_DD->st_SplitViewCamera, &st_TempCam, sizeof( CAM_tdst_Camera ) );

		L_memcpy( &st_TempCam, &_pst_DD->st_SaveCamera, sizeof( CAM_tdst_Camera ) );
		L_memcpy( &_pst_DD->st_SaveCamera, &_pst_DD->st_SaveSplitViewCamera, sizeof( CAM_tdst_Camera ) );
		L_memcpy( &_pst_DD->st_SaveSplitViewCamera, &st_TempCam, sizeof( CAM_tdst_Camera ) );

		p_Temp                                = _pst_DD->pst_EditorCamObject;
		_pst_DD->pst_EditorCamObject          = _pst_DD->pst_EditorSplitViewCamObject;
		_pst_DD->pst_EditorSplitViewCamObject = p_Temp;

		uc_Temp                           = _pst_DD->uc_EngineCamera;
		_pst_DD->uc_EngineCamera          = _pst_DD->uc_EngineSplitViewCamera;
		_pst_DD->uc_EngineSplitViewCamera = uc_Temp;

		uc_Temp                           = _pst_DD->uc_EditorCamera;
		_pst_DD->uc_EditorCamera          = _pst_DD->uc_EditorSplitViewCamera;
		_pst_DD->uc_EditorSplitViewCamera = uc_Temp;

		uc_Temp                              = _pst_DD->uc_LockObjectToCam;
		_pst_DD->uc_LockObjectToCam          = _pst_DD->uc_LockObjectToSplitViewCam;
		_pst_DD->uc_LockObjectToSplitViewCam = uc_Temp;

		pst_TempObj                     = ( GEO_tdst_Object                     *) _pst_DD->pst_CamTarget;
		_pst_DD->pst_CamTarget          = _pst_DD->pst_SplitViewCamTarget;
		_pst_DD->pst_SplitViewCamTarget = ( OBJ_tdst_GameObject * ) pst_TempObj;

		L_memcpy( &st_TempMatrix, &_pst_DD->st_OffsetToCamTarget, sizeof( MATH_tdst_Matrix ) );
		L_memcpy( &_pst_DD->st_OffsetToCamTarget, &_pst_DD->st_OffsetToSplitViewCamTarget, sizeof( MATH_tdst_Matrix ) );
		L_memcpy( &_pst_DD->st_OffsetToSplitViewCamTarget, &st_TempMatrix, sizeof( MATH_tdst_Matrix ) );
#	endif
	}

	/*
 =======================================================================================================================
 =======================================================================================================================
 */
	void GDI_CopyMainCameraToSplitViewCamera( GDI_tdst_DisplayData *_pst_DD )
	{
		L_memcpy( &_pst_DD->st_SplitViewCamera, &_pst_DD->st_Camera, sizeof( CAM_tdst_Camera ) );
		L_memcpy( &_pst_DD->st_SaveSplitViewCamera, &_pst_DD->st_SaveCamera, sizeof( CAM_tdst_Camera ) );

		_pst_DD->pst_EditorSplitViewCamObject = _pst_DD->pst_EditorCamObject;
		_pst_DD->uc_EngineSplitViewCamera     = _pst_DD->uc_EngineCamera;
		_pst_DD->uc_EditorSplitViewCamera     = _pst_DD->uc_EditorCamera;
		_pst_DD->uc_LockObjectToSplitViewCam  = _pst_DD->uc_LockObjectToCam;
		_pst_DD->pst_SplitViewCamTarget       = _pst_DD->pst_CamTarget;

		L_memcpy( &_pst_DD->st_OffsetToSplitViewCamTarget, &_pst_DD->st_OffsetToCamTarget, sizeof( MATH_tdst_Matrix ) );
	}

	/*
 =======================================================================================================================
 =======================================================================================================================
 */
	void GDI_AttachMainCamToObj( GDI_tdst_DisplayData *_pst_DD, struct OBJ_tdst_GameObject_ *_pst_GO )
	{
		MATH_tdst_Matrix st_TransfoMat, st_CamPosMat;

		if ( !_pst_GO ) return;
		_pst_DD->pst_CamTarget = _pst_GO;

		CAM_SetCamMatrixFromObject( &st_CamPosMat, &_pst_DD->st_Camera.st_Matrix );
		MATH_SetType( &st_CamPosMat, MATH_l_GetType( &st_TransfoMat ) );
		MATH_InvertMatrix( &st_TransfoMat, _pst_GO->pst_GlobalMatrix );
		MATH_MulMatrixMatrix( &_pst_DD->st_OffsetToCamTarget, &st_CamPosMat, &st_TransfoMat );
	}

	/*
 =======================================================================================================================
 =======================================================================================================================
 */
	void GDI_DetachMainCamFromObj( GDI_tdst_DisplayData *_pst_DD )
	{
		_pst_DD->pst_CamTarget = NULL;
	}

#endif
	static BOOL GDI_sb_XenonGraphics = FALSE;

#if !( defined( _XENON_RENDER ) && !( defined( ACTIVE_EDITORS ) || defined( PCWIN_TOOL ) ) )
	BOOL GDI_b_IsXenonGraphics( void )
	{
		return GDI_sb_XenonGraphics;
	}
#endif

#if defined( PSX2_TARGET ) && defined( __cplusplus )
}
#endif

#if defined( ACTIVE_EDITORS ) && defined( _XENON_RENDER )

void GDI_Xe_ForceLoadLastTexture( void )
{
	TEX_tdst_File_Desc *dst_TexDesc;
	TEX_tdst_File_Desc *pst_CurTexDesc;
	TEX_tdst_File_Desc *pst_OtherDesc;
	TEX_tdst_Data *pst_Tex;
	TEX_tdst_Animated *pst_Ani;
	MAT_tdst_SpriteGen *pst_SG;
	CHAR *pc_UsedTextures;
	int i_Type;
	void *p_Data;
	USHORT uw_Flags;
	UCHAR uc_Number;
	int i_Restart;
	int i_Tex;
	LONG l_FirstTexture;
	LONG l_FirstPalette;
	LONG l_Size;
	LONG l_StartTexture;
	LONG l_SecondPass;
	LONG l_MaxTextures;
	SHORT w_Texture;
	LONG l_GetInfoOk;
	LONG i;

	if ( !GDI_b_IsXenonGraphics() )
		return;

	if ( TEX_gst_GlobalList.l_NumberOfTextures < 2 )
		return;

	if ( GDI_gpst_CurDD->pst_World == NULL )
		return;

	// Absolute references
	l_FirstTexture = TEX_gst_GlobalList.l_NumberOfTextures - 2;
	l_StartTexture = l_FirstTexture;
	l_FirstPalette = TEX_gst_GlobalList.l_NumberOfPalettes;
	l_MaxTextures  = TEX_gst_GlobalList.l_NumberOfTextures;

	LOA_BeginSpeedMode( 0xFF800000 );

	// Create the texture information structure
	l_Size      = l_MaxTextures * sizeof( TEX_tdst_File_Desc );
	dst_TexDesc = ( TEX_tdst_File_Desc * ) MEM_p_Alloc( l_Size );
	L_memset( dst_TexDesc, 0, l_Size );

	// We assume that all current textures must remain loaded and that the new texture is used..
	pc_UsedTextures = ( CHAR * ) MEM_p_Alloc( l_MaxTextures * sizeof( CHAR ) );
	L_memset( pc_UsedTextures, 2, l_FirstTexture );
	L_memset( pc_UsedTextures + l_FirstTexture, 1, l_MaxTextures - l_FirstTexture );

	// Load information about the new textures
	l_StartTexture = l_FirstTexture;
	l_SecondPass   = 0;
	w_Texture      = -1;

XE_ForceLoadLastTexture_Start:

	for ( i = l_StartTexture; i < l_MaxTextures; ++i )
	{
		pst_Tex        = TEX_gst_GlobalList.dst_Texture + i;
		pst_CurTexDesc = dst_TexDesc + i;

		if ( l_SecondPass || ( pc_UsedTextures[ i ] != 2 ) )
			pst_CurTexDesc->uw_DescFlags = TEX_Cuw_DF_VeryBadBoy;

		if ( l_SecondPass || ( pc_UsedTextures[ i ] == 1 ) )
		{
			if ( !l_SecondPass )
				pc_UsedTextures[ i ] = 2;

			l_GetInfoOk = TEX_l_File_GetInfo( pst_Tex->ul_Key, pst_CurTexDesc, TRUE );
			if ( l_GetInfoOk )
			{
				// Sprite generator texture
				if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_SprFile )
				{
					TEX_gst_GlobalList.dst_Texture[ i ].uw_Flags |= TEX_uw_IsSpriteGen;
					pst_SG                                                    = ( MAT_tdst_SpriteGen                                                    *) pst_CurTexDesc->st_Params.ul_Params[ 0 ];
					*( ULONG * ) &TEX_gst_GlobalList.dst_Texture[ i ].w_Width = ( ULONG ) pst_SG;

					// Id of the new texture added
					w_Texture = pst_SG->s_TextureIndex;
				}
				else
				{
					// Texture is used and exist, delete the flag TEX_Cuw_DF_VeryBadBoy
					pst_CurTexDesc->uw_DescFlags &= ~TEX_Cuw_DF_VeryBadBoy;
					if ( !( GDI_gpst_CurDD->st_TexManager.ul_Flags & TEX_Manager_UseMipmap ) )
						pst_CurTexDesc->st_Params.uw_Flags &= ~TEX_FP_MipmapOn;

					// Procedural texture
					if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_ProFile )
					{
						i_Type            = ( int ) pst_CurTexDesc->st_Params.ul_Params[ 0 ];
						p_Data            = ( void            *) pst_CurTexDesc->st_Params.ul_Params[ 1 ];
						uw_Flags          = ( USHORT ) pst_CurTexDesc->st_Params.ul_Params[ 2 ];
						pst_Tex->w_Height = pst_CurTexDesc->uw_Height;
						pst_Tex->w_Width  = pst_CurTexDesc->uw_Width;
						TEX_Procedural_Add( pst_Tex->w_Index, i_Type, uw_Flags, p_Data );
					}

					// Animated texture
					else if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_AniFile )
					{
						TEX_gst_GlobalList.dst_Texture[ i ].uw_Flags |= TEX_uw_Ani;
						uc_Number = ( UCHAR ) pst_CurTexDesc->st_Params.ul_Params[ 0 ];
						p_Data    = ( void    *) pst_CurTexDesc->st_Params.ul_Params[ 1 ];
						uw_Flags  = ( USHORT ) pst_CurTexDesc->st_Params.ul_Params[ 2 ];
						pst_Ani   = TEX_pst_Anim_Add( pst_Tex->w_Index, uc_Number, uw_Flags, p_Data );

						if ( pst_Ani->uc_Number )
						{
							i_Restart = 0;
							for ( i_Tex = 0; i_Tex < ( int ) pst_Ani->uc_Number; i_Tex++ )
							{
								w_Texture                         = TEX_w_List_AddTexture( &TEX_gst_GlobalList, pst_Ani->dst_Tex[ i_Tex ].ul_Key, 1 );
								pst_Ani->dst_Tex[ i_Tex ].w_Index = w_Texture;
							}

							pst_Tex->w_Height = pst_Ani->dst_Tex->w_Index;
							w_Texture         = -1;
							if ( i_Restart )
								goto XE_ForceLoadLastTexture_Start;
						}
					}

					// Raw pal association
					else if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_RawPalFile )
					{
						// Id of new texture added
						TEX_gst_GlobalList.dst_Texture[ i ].uw_Flags |= TEX_uw_RawPal;
						w_Texture = TEX_w_Manager_ChooseGoodInTex( &GDI_gpst_CurDD->st_TexManager, pst_CurTexDesc );
					}

					// Xenon DDS file
					else if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_XeDDSFile )
					{
						pst_CurTexDesc->uc_BPP      = 0x80;
						pst_CurTexDesc->uc_FinalBPP = 0x80;
					}
				}
			}
		}
	}

	// Test if some more texture has been added (by sprite generator for example)
	if ( l_MaxTextures != TEX_gst_GlobalList.l_NumberOfTextures )
	{
		// Reallocate and init additional temporary structure data
		l_Size      = TEX_gst_GlobalList.l_NumberOfTextures * sizeof( TEX_tdst_File_Desc );
		dst_TexDesc = ( TEX_tdst_File_Desc * ) MEM_p_Realloc( dst_TexDesc, l_Size );
		for ( i = l_MaxTextures; i < TEX_gst_GlobalList.l_NumberOfTextures; i++ )
			L_memset( dst_TexDesc + i, 0, sizeof( TEX_tdst_File_Desc ) );

		/* init and go for a second pass */
		l_SecondPass   = 1;
		l_StartTexture = l_MaxTextures;
		l_MaxTextures  = TEX_gst_GlobalList.l_NumberOfTextures;
		goto XE_ForceLoadLastTexture_Start;
	}

	// Load the palettes
	for ( i = l_FirstPalette; i < TEX_gst_GlobalList.l_NumberOfPalettes; i++ )
	{
		TEX_File_LoadPalette( TEX_gst_GlobalList.dst_Palette + i,
		                      ( GDI_gpst_CurDD->st_TexManager.ul_Flags & TEX_Manager_RGB2BGR ) );
	}


	// Redo the texture association for textures that must be converted by the Xenon texture manager
	for ( i = l_FirstTexture; i < TEX_gst_GlobalList.l_NumberOfTextures; ++i )
	{
		pst_Tex        = TEX_gst_GlobalList.dst_Texture + i;
		pst_CurTexDesc = dst_TexDesc + i;

		if ( pst_CurTexDesc->uw_DescFlags & TEX_Cuw_DF_VeryBadBoy ) continue;

		if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_RawPalFile )
		{
			if ( pst_CurTexDesc->w_TexFlags & TEX_XenonNeedsConversion )
			{
				ULONG ulSizeBK = dst_TexDesc[ pst_CurTexDesc->w_TexTC ].st_Tex.st_XeProperties.ul_FileSize;

				L_memcpy( &dst_TexDesc[ pst_CurTexDesc->w_TexTC ].st_Tex.st_XeProperties,
				          &pst_CurTexDesc->st_Tex.st_XeProperties, sizeof( TEX_tdst_XenonFileTex ) );

				dst_TexDesc[ pst_CurTexDesc->w_TexTC ].st_Tex.st_XeProperties.ul_FileSize = ulSizeBK;
				dst_TexDesc[ pst_CurTexDesc->w_TexTC ].w_TexFlags |= TEX_XenonConvert;
				dst_TexDesc[ pst_CurTexDesc->w_TexTC ].uw_DescFlags |= TEX_Cuw_DF_XenonFileAsBitmap;
			}
		}
	}

	// Another pass on texture for tex files
	//     on va positionner le flag VeryBadBoy pour ces textures dans les cas suivants
	//         raw + pal dispo et acceptée ou TrueColor dispo et accepté
	//     on va le laisser si on doit passer en TrueColor un raw+pal (palette non acceptée et pas de True color dispo)
	for ( i = l_FirstTexture; i < TEX_gst_GlobalList.l_NumberOfTextures; i++ )
	{
		pst_Tex        = TEX_gst_GlobalList.dst_Texture + i;
		pst_CurTexDesc = dst_TexDesc + i;

		if ( pst_CurTexDesc->uw_DescFlags & TEX_Cuw_DF_VeryBadBoy ) continue;

		if ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_RawPalFile )
		{
			if ( pst_CurTexDesc->w_TexFlags & TEX_TakeTrueColor )
			{
				// Mode TrueColor avec texture TrueColor dispo
				pst_Tex->w_TexTC = pst_CurTexDesc->w_TexTC;
				pst_Tex->w_Dummy = -1;
				pst_CurTexDesc->uw_DescFlags |= TEX_Cuw_DF_VeryBadBoy;
			}
			else// Palette
			{
				pst_Tex->w_TexRaw = pst_CurTexDesc->w_TexRaw;
				pst_Tex->w_TexPal = pst_CurTexDesc->w_TexPal;
				if ( pst_CurTexDesc->w_TexFlags & TEX_ChangeToTrueColor )
				{
					if ( pst_Tex->w_TexRaw == -1 )
					{
						// Mode aucune donnée dispo
						pst_CurTexDesc->uw_DescFlags |= TEX_Cuw_DF_VeryBadBoy;
					}
					else
					{
						// Mode TrueColor mais seulement pal + raw
						pst_OtherDesc = dst_TexDesc + pst_Tex->w_TexRaw;
						pst_OtherDesc->uw_DescFlags |= TEX_Cuw_DF_VeryBadBoy;
						pst_CurTexDesc->st_Params.uc_Format = pst_OtherDesc->st_Params.uc_Format;
						pst_CurTexDesc->st_Params.uw_Width  = pst_OtherDesc->st_Params.uw_Width;
						pst_CurTexDesc->st_Params.uw_Height = pst_OtherDesc->st_Params.uw_Height;
						TEX_File_DevelopParams( pst_CurTexDesc );
					}
				}
				else
				{
					// Mode raw + pal avec raw + pal dispo
					pst_CurTexDesc->uw_DescFlags |= TEX_Cuw_DF_VeryBadBoy;
				}
			}
		}
	}

	// Compute all textures compression
	TEX_Manager_ComputeCompression( &GDI_gpst_CurDD->st_TexManager, dst_TexDesc + l_FirstTexture, l_MaxTextures - l_FirstTexture, 0 );
	TEX_Manager_ComputeCompression( &GDI_gpst_CurDD->st_TexManager, dst_TexDesc + l_FirstTexture, l_MaxTextures - l_FirstTexture, 1 );

	// Resize the texture array to add the new ones
	Xe_Texture_Resize( l_MaxTextures );

	// Create the textures
	{
		GDI_gpst_CurDD->st_TexManager.l_TextureNumber = l_MaxTextures;

		// Create textures
		pst_Tex        = TEX_gst_GlobalList.dst_Texture + l_FirstTexture;
		pst_CurTexDesc = dst_TexDesc + l_FirstTexture;

		// Firstly load All bitmaps (included 8bit & 4Bits Texture)
		for ( i = l_FirstTexture; i < l_MaxTextures; i++, pst_Tex++, pst_CurTexDesc++ )
		{
			if ( pst_CurTexDesc->uw_DescFlags & TEX_Cuw_DF_VeryBadBoy ) continue;
			if ( pst_Tex->uw_Flags & TEX_uw_Ani ) continue;

			if ( ( pst_CurTexDesc->st_Params.uw_Flags & TEX_FP_Interface ) &&
			     ( GDI_gpst_CurDD->st_TexManager.ul_Flags & TEX_Manager_StoreInterfaceTex ) )
				GDI_gpst_CurDD->st_TexManager.l_RamTextureNumber++;
			else
				GDI_gpst_CurDD->st_TexManager.l_VRamTextureNumber++;
			TEX_Manager_AddInfo( &GDI_gpst_CurDD->st_TexManager, pst_Tex, pst_CurTexDesc );

			if ( ( pst_CurTexDesc->st_Params.uc_Type == TEX_FP_RawPalFile ) &&
			     !( pst_CurTexDesc->uw_DescFlags & TEX_Cuw_DF_SpecialNoLoad ) )
			{
				// If a rawpal texture comes here that means that it is not supported (ex : only non supported palette and raw)
				// so we have to convert it
				pst_OtherDesc = dst_TexDesc + pst_CurTexDesc->w_TexRaw;
				GDI_TransformUnsupportedRawPal( GDI_gpst_CurDD, pst_Tex, pst_CurTexDesc, pst_OtherDesc );
			}

			TEX_Hardware_Load( GDI_gpst_CurDD, pst_Tex, pst_CurTexDesc, i );
		}

		// Secondly load All Palettes
		for ( i = l_FirstPalette; i < TEX_gst_GlobalList.l_NumberOfPalettes; i++ )
		{
			GDI_gpst_CurDD->st_GDI.pfnv_LoadPalette( GDI_gpst_CurDD, TEX_gst_GlobalList.dst_Palette + i, i );
		}

		// Thirdly Associate pallettes & textures
		pst_Tex        = TEX_gst_GlobalList.dst_Texture + l_FirstTexture;
		pst_CurTexDesc = dst_TexDesc + l_FirstTexture;
		for ( i = l_FirstTexture; i < l_MaxTextures; i++, pst_Tex++, pst_CurTexDesc++ )
		{
			ULONG palette, tex;
			// Is it a raw pal?
			if ( pst_CurTexDesc->uc_FinalBPP > 8 ) continue;
			if ( pst_Tex->uw_Flags & TEX_uw_RawPal )
			{
				// What is the pal?
				palette = pst_Tex->w_Width;
				// What is the tex?
				tex           = pst_Tex->w_Height;
				pst_OtherDesc = dst_TexDesc + tex;// les infos + d'autres dans TEX_gst_GlobalList.dst_Texture + tex
				GDI_gpst_CurDD->st_GDI.pfnv_Set_Texture_Palette( GDI_gpst_CurDD, i, tex, palette );
			}
		}

		// Procedural : les textures procédurales d'interface doivent utiliser
		//              le buffer mémoire ou est stockée la texture
		TEX_Procedural_AfterLoad( GDI_gpst_CurDD );
	}

	// Free local data
	MEM_Free( pc_UsedTextures );
	MEM_Free( dst_TexDesc );

	// Unload palette
	if ( TEX_gst_GlobalList.l_NumberOfPalettes )
	{
		for ( i = l_FirstPalette; i < TEX_gst_GlobalList.l_NumberOfPalettes; i++ )
		{
			if ( TEX_gst_GlobalList.dst_Palette[ i ].uc_Flags & TEX_uc_KeepPalInMem ) continue;
			if ( ( TEX_gst_GlobalList.dst_Palette + i )->pul_Color )
				TEX_M_File_Free( ( TEX_gst_GlobalList.dst_Palette + i )->pul_Color );
			( TEX_gst_GlobalList.dst_Palette + i )->pul_Color = NULL;
		}
		TEX_gst_GlobalList.l_NumberOfPalettes = 0;
	}

	STR_FontResolveAlternativeKey();

	LOA_EndSpeedMode();
}

#endif