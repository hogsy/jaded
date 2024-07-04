/*$T GDInterface.h GC! 1.081 08/08/00 10:10:54 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __GDINTERFACE_H__
#define __GDINTERFACE_H__

#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIwin.h"
#include "BASe/BASsys.h"
#include "MATHs/MATH.h"
#include "CAMera/CAMera.h"
#include "SOFT/SOFTMatrixStack.h"
#include "LIGHT/LIGHTstruct.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOzone.h"
#include "MATerial/MATstruct.h"
#include "WAYpoint/WAYlink.h"
#include "TEXture/TEXmanager.h"
#include "SOFT/SOFTstruct.h"

#if defined( __cplusplus ) && !defined( JADEFUSION )
extern "C"
{
#endif

	/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define GDI_Cl_ColorBuffer 0x1
#define GDI_Cl_ZBuffer     0x2

#define GDI_Cul_DM_UseTexture     0x00000001
#define GDI_Cul_DM_DontForceColor 0x00000002
#define GDI_Cul_DM_DontShowBV     0x00000004
#define GDI_Cul_DM_Draw           0x00000008
#define GDI_Cul_DM_NotWired       0x00000010
#define GDI_Cul_DM_UnlockedRLI    0x00000020
#define GDI_Cul_DM_UseRLI         0x00000040
#define GDI_Cul_DM_MaterialColor  0x00000080
#define GDI_Cul_DM_UseAmbient     0x00000100
#define GDI_Cul_DM_DoNotSort      0x00000200// Utile ?
#ifdef JADEFUSION
#	define GDI_Cul_DM_DontReceiveLM 0x00000400//XENON
#else
#	define GDI_Cul_DM_NoFacetMode 0x00000400// ex-GDI_Cul_DM_ComputeSpecular
#endif
#define GDI_Cul_DM_TestBackFace 0x00000800
#define GDI_Cul_DM_TwoSided     GDI_Cul_DM_TestBackFace

#ifdef JADEFUSION
//#define GDI_Cul_DM_DontCastLM   			0x00001000 //XENON
#	define GDI_Cul_DM_DynamicRLI 0x00001000//XENON
#else
#	define GDI_Cul_DM_NoShowRLIPlaceMode 0x00001000// Ex-GDI_Cul_DM_HidePoint
#endif

#define GDI_Cul_DM_NotInvertBF            0x00002000
#define GDI_Cul_DM_Fogged                 0x00004000
#define GDI_Cul_DM_Symetric               0x00008000
#define GDI_Cul_DM_ReceiveDynSdw          0x00010000
#define GDI_Cul_DM_ActiveSkin             0x00020000
#define GDI_Cul_DM_ReceiveShadowBuffer    0x00040000
#define GDI_Cul_DM_EmitShadowBuffer       0x00080000// Utile ?
#define GDI_Cul_DM_UseNormalMaterial      0x00100000
#define GDI_Cul_DM_UseBVForLightRejection 0x00200000// Utile ?
#define GDI_Cul_DM_DontUseAmbient2        0x00400000// Utile ?
#define GDI_Cul_DM_DontRecomputeNormales  0x00800000
#define GDI_Cul_DM_ZTest                  0x01000000
#define GDI_Cul_DM_DontScaleRLI           0x02000000
#define GDI_Cul_DM_NoAutoClone            0x04000000// GDI_Cul_DM_ClipReflectionPS2
//popowarning XENON
#define GDI_Cul_DM_UseMapLightSettings GDI_Cul_DM_NoAutoClone
#define GDI_Cul_DM_Lighted             0x08000000

#define GDI_Cul_DM_UseFog2 0x10000000
#ifdef JADEFUSION
#	define GDI_Cul_DM_DontForceSorted 0x20000000
#else
#	define GDI_Cul_DM_DontSort 0x20000000
#endif
#define GDI_Cul_DM_DontAttenuateLight 0x40000000
#define GDI_Cul_DM_ReflectOnWater     0x80000000// Inutilisé ?
#define GDI_Cul_DM_All                0xFFFFFFFF


/* Flag used in display Info */

/* Graphic pipeline */
#define GDI_Cul_DI_SpecularIsComputed     0x00000001
#define GDI_Cul_DI_UseOneUVPerPoint       0x00000002
#define GDI_Cul_DI_RenderingTransparency  0x00000004
#define GDI_Cul_DI_DontSortObject         0x00000008
#define GDI_Cul_DI_FaceMap                0x00000010
#define GDI_Cul_DI_UseMipmap              0x00000020
#define GDI_Cul_DI_InvertMipmap           0x00000040
#define GDI_Cul_DI_ForceSquareTexture     0x00000080
#define GDI_Cul_DI_BlendRLI               0x00000100
#define GDI_Cul_DI_ForceSortTriangle      0x00000200
#define GDI_Cul_DI_UseSpecialVertexBuffer 0x00000400
#define GDI_Cul_DI_RenderingInterface     0x00000800
#define GDI_Cul_DI_ObjectHasBeenZAdded    0x00001000
#define GDI_Cul_DI_UpdateRLI              0x00002000
#define GDI_Cul_DI_UseLitAlphaShader      0x00004000
#define GDI_Cul_DI_ComputeShadowMap       0x00008000
#define GDI_Cul_DI_UpdateUV               0x00010000
#define GDI_Cul_DI_UseOriginalPoints      0x00020000
#define GDI_Cul_DI_RenderingDiffuse       0x00040000
#define GDI_Cul_DI_RenderingSpotSwadows   0x00080000
#define GDI_Cul_DI_RenderNOAnimation      0x00100000
#define GDI_Cul_DI_DoubleRendering_K      0x00040000
#define GDI_Cul_DI_DoubleRendering_I      0x00080000
#define GDI_Cul_DI_ShowSlope              0x00100000
#define GDI_Cul_DI_Noshadows              0x00200000
#ifdef JADEFUSION
#	define GDI_Cul_DI_RenderingGFX 0x00400000//XENON
#endif
/* Flag used in display Flag */

/* This will used only in editor */
#define GDI_Cul_DF_UsePickingBuffer         0x00000001
#define GDI_Cul_DF_DisplayLight             0x00000002
#define GDI_Cul_DF_DisplayCamera            0x00000004
#define GDI_Cul_DF_DisplayWaypoint          0x00000008
#define GDI_Cul_DF_DisplayWaypointInNetwork 0x00000010
#define GDI_Cul_DF_DisplayInvisible         0x00000020
#define GDI_Cul_DF_WaypointBoundingVolume   0x00000040
#define GDI_Cul_DF_DoNotRender              0x00000080
#define GDI_Cul_DF_ShowCurrentBV            0x00000100
#define GDI_Cul_DF_ShowAllBV                0x00000200// Inutilisé
#define GDI_Cul_DF_ShowNormals              0x00000400// Utile ?
#define GDI_Cul_DF_UpdateTrace              0x00000800
#define GDI_Cul_DF_Proportionnal            0x00001000
#define GDI_Cul_DF_DisplayHierarchy         0x00002000
#define GDI_Cul_DF_ShowZDM                  0x00004000
#define GDI_Cul_DF_ShowZDE                  0x00008000
#define GDI_Cul_DF_ShowCOB                  0x00010000
#define GDI_Cul_DF_ShowCurrentCOB           0x00020000
#define GDI_Cul_DF_ShowNormalsOfSelected    0x00040000// Utile ?
#define GDI_Cul_DF_SelectBone               0x00080000
#define GDI_Cul_DF_DrawCurve                0x00100000
#define GDI_Cul_DF_DoNotFlip                0x00200000
#define GDI_Cul_DF_DoNotClear               0x00400000
#define GDI_Cul_DF_ShowFaceNormal           0x00800000// Utile ?
#define GDI_Cul_DF_ShowAIDebugVectors       0x01000000
#define GDI_Cul_DF_Noshadows                0x02000000          // XENON
#define GDI_Cul_DF_ForceMaxDynLights        GDI_Cul_DF_Noshadows//XENON
#define GDI_Cul_DF_HideInterface            0x04000000
#define GDI_Cul_DF_ShadeSelected            0x08000000
#define GDI_Cul_DF_DisplaySnd               0x10000000
#define GDI_cul_DF_DepthReadBeforeFlip      0x20000000
#define GDI_cul_DF_DisplayOK3               0x40000000
#define GDI_Cul_DF_DisplayVisuWithoutGro    0x80000000// Utile ?

#define GDI_Cc_CaptureFlag_Type     0x03
#define GDI_Cc_CaptureType_One      0
#define GDI_Cc_CaptureType_Multiple 1
#define GDI_Cc_CaptureType_Infinit  2

#define GDI_Cc_CaptureFlag_Running       0x04
#define GDI_Cc_CaptureFlag_BIG           0x08
#define GDI_Cc_CaptureFlag_SaveInBf      0x10//XENON
#define GDI_Cc_CaptureFlag_ForCBM        0x20//XENON
#define GDI_Cc_CaptureFlag_ForSeqBuilder 0x40

/* Screen format flags */
#define GDI_Cul_SFF_CropToWindow 0x00000001
#define GDI_Cul_SFF_OccupyAll    0x00000002
#define GDI_Cul_SFF_ReferenceIsY 0x00000004
#define GDI_Cul_SFF_169BlackBand 0x00000008

/* Screen ration constant */
#define GDI_Cul_SRC_Customize 0
#define GDI_Cul_SRC_Square    1
#define GDI_Cul_SRC_4over3    2
#define GDI_Cul_SRC_16over9   3

#define GDI_Cul_SRC_Number 4

//XENON
/* Light Culling Flags */
#define GDI_Cul_Light_Cull_Element         ( 1 << 0 )
#define GDI_Cul_Light_Cull_Frustum_Gao     ( 1 << 1 )
#define GDI_Cul_Light_Cull_Frustum_Element ( 1 << 2 )

	/*$4
 ***********************************************************************************************************************
    globals
 ***********************************************************************************************************************
 */

	extern float GDI_gaf_ScreenRation[ GDI_Cul_SRC_Number ];
	extern int GDI_gi_GDIType;// current output

	/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */


	/*
 -----------------------------------------------------------------------------------------------------------------------
    Device capabilities
 -----------------------------------------------------------------------------------------------------------------------
 */
	typedef struct GDI_tdst_Capabilities_
	{
		LONG lDummy;
	} GDI_tdst_Capabilities;

	/*$off*/
	/*$2
 ---------------------------------------------------------------------------------------------------
	Interface functions
 ---------------------------------------------------------------------------------------------------
 */
	typedef struct GDI_tdst_Interface_
	{
		void *( *pfnpv_InitDisplay )( void );
		void ( *pfnv_DesinitDisplay )( void * );
		LONG( *pfnl_OpenDisplay )
		( HWND, void * );
		LONG( *pfnl_CloseDisplay )
		( void * );
		LONG( *pfnl_ReadaptDisplay )
		( HWND, void * );
		void ( *pfnv_SetViewMatrix )( MATH_tdst_Matrix * );
		void ( *pfnv_SetProjectionMatrix )( CAM_tdst_Camera * );
		LONG( *pfnl_DrawIndexedTriangles )
		( struct GEO_tdst_ElementIndexedTriangles_ *, GEO_Vertex *, GEO_tdst_UV *, ULONG );
		LONG( *pfnl_Request )
		( ULONG, ULONG );
#ifndef PSX2_TARGET
		void ( *pfnv_SetTextureBlending )( ULONG, ULONG, ULONG );
		void ( *pfnl_DrawIndexedSprites )( struct GEO_tdst_ElementIndexedSprite_ *, GEO_Vertex *, ULONG );
#endif
		LONG( *pfnl_InitTexture )
		( struct GDI_tdst_DisplayData_ *, ULONG );
		void ( *pfnv_LoadTexture )( struct GDI_tdst_DisplayData_ *, struct TEX_tdst_Data_ *, struct TEX_tdst_File_Desc_ *, ULONG );
		LONG( *pfnl_StoreTexture )
		( struct GDI_tdst_DisplayData_ *, struct TEX_tdst_Data_ *, struct TEX_tdst_File_Desc_ *, ULONG );
		void ( *pfnv_LoadPalette )( struct GDI_tdst_DisplayData_ *, struct TEX_tdst_Palette_ *, ULONG );
		void ( *pfnv_Set_Texture_Palette )( struct GDI_tdst_DisplayData_ *, ULONG _ulTexNum, ULONG IT, ULONG IP );

		void ( *pfnv_SetTextureTarget )( ULONG NumShadowTex, ULONG CLEAR );
		void ( *pfnv_SetViewMatrix_SDW )( struct MATH_tdst_Matrix_ *_pst_Matrix, float *Limits );

	} GDI_tdst_Interface;

//#ifndef __MATDRAW_C__
	extern int renderState_Shfited;
	extern int renderState_Cloned;
//#endif
	LONG OGL_l_DrawElementIndexedTriangles( GEO_tdst_ElementIndexedTriangles *_pst_Element, GEO_Vertex *_pst_Point, GEO_tdst_UV *_pst_UV, ULONG ulnumberOfPoints );
	LONG OGL_l_ShiftDrawElementIndexedTriangles( GEO_tdst_ElementIndexedTriangles *_pst_Element, GEO_Vertex *_pst_Point, MATH_tdst_Vector *_pst_Normal, GEO_tdst_UV *_pst_UV, ULONG ulnumberOfPoints );
	LONG OGL_l_CloneDrawElementIndexedTriangles( GEO_tdst_ElementIndexedTriangles *_pst_Element, GEO_Vertex *_pst_Point, MATH_tdst_Vector *_pst_Normal, GEO_tdst_UV *_pst_UV, ULONG ulnumberOfPoints );
	/*$on*/


	////////////////////////////////////////////////////////////////////////////////////////

	extern BOOL OGL_gb_Init;
#define GDI_DrawIndexedTriangles( GDIi, Tris, Vrtx, Normals, UV, ulNbPoints )          \
	if ( renderState_Shfited )                                                         \
		OGL_l_ShiftDrawElementIndexedTriangles( Tris, Vrtx, Normals, UV, ulNbPoints ); \
	else if ( renderState_Cloned )                                                     \
		OGL_l_CloneDrawElementIndexedTriangles( Tris, Vrtx, Normals, UV, ulNbPoints ); \
	else                                                                               \
		GDIi.st_GDI.pfnl_DrawIndexedTriangles( Tris, Vrtx, UV, ulNbPoints );

#define GDI_DrawIndexedSprites( GDIi, Tris, Vrtx, ulNbPoints ) GDIi.st_GDI.pfnl_DrawIndexedSprites( Tris, Vrtx, ulNbPoints )
#define GDI_SetTextureBlending( GDIi, VA, VB, VC )             GDIi.st_GDI.pfnv_SetTextureBlending( VA, VB, VC )
#define GDI_SetTextureTarget( GDIi, NST, CL )                  GDIi.st_GDI.pfnv_SetTextureTarget( NST, CL )
#define GDI_SetViewMatrix( GDIi, TTMatrix )                    GDIi.st_GDI.pfnv_SetViewMatrix( TTMatrix )
#define GDI_SetViewMatrix_SDW( GDIi, MAT, Lim )                GDIi.st_GDI.pfnv_SetViewMatrix_SDW( MAT, Lim )

	////////////////////////////////////////////////////////////////////////////////////////

#if defined( ACTIVE_EDITORS ) && defined( _XENON_RENDER )
	void GDI_Xe_ForceLoadLastTexture( void );
#endif
	/*
 -----------------------------------------------------------------------------------------------------------------------
    Device data
 -----------------------------------------------------------------------------------------------------------------------
 */
	typedef struct GDI_tdst_Device_
	{
		LONG l_Width;
		LONG l_Height;
#ifdef ACTIVE_EDITORS
		LONG Vx, Vy, Vw, Vh;
#endif
	} GDI_tdst_Device;

	typedef struct GDI_tdst_ScreenFormat_
	{
		ULONG ul_Flags;
		float f_PixelYoverX;
		float f_ScreenYoverX;
		float f_BX, f_BY, f_BW, f_BH;
		LONG l_ScreenRatioConst;
	} GDI_tdst_ScreenFormat;

	typedef struct GDI_tdst_HighlightColors_
	{
		ULONG ul_Colors[ 5 ];
	} GDI_tdst_HighlightColors;

	typedef struct GDI_tdst_ProfilingInformation_
	{
		unsigned int numBatches;
		unsigned int numRequests;
	} GDI_tdst_ProfilingInformation_;

	/*
 -----------------------------------------------------------------------------------------------------------------------
    Display data
 -----------------------------------------------------------------------------------------------------------------------
 */
	typedef struct GDI_tdst_DisplayData_
	{
		GDI_tdst_Interface st_GDI;
		GDI_tdst_Capabilities st_Caps;

		HWND h_Wnd;

		ULONG ul_DrawMask;
		ULONG ul_DisplayInfo;
		GEO_Vertex *p_Current_Vertex_List; /* Philippe,  pour eviter le test use special-Vertexbuffer */
		ULONG ul_CurrentDrawMask;
		ULONG ul_ColorConstant;
		float f_LODVal;

		ULONG *pul_RLI_Used;
		ULONG *pul_RLIBlend;
		float f_RLIBlend;
		float f_RLIScale;
		ULONG ul_RLIColorDest;
		unsigned short *pus_ReorderBuffer;

		float f_LightAttenuation;

		struct MAT_tdst_MultiTexture_ *pst_CurrentMat;

		struct MAT_tdst_MTLevel_ *pst_CurrentMLTTXLVL;
#ifdef JADEFUSION
		struct OBJ_tdst_GameObject_ *pst_CurrentAnim;
#endif
		struct OBJ_tdst_GameObject_ *pst_CurrentGameObject;
		struct GEO_tdst_Object_ *pst_CurrentGeo;

		struct GRO_tdst_Struct_ *pst_AdditionalMaterial;

		GDI_tdst_Device st_Device;
		GDI_tdst_ScreenFormat st_ScreenFormat;
		CAM_tdst_Camera st_Camera;
		CAM_tdst_Camera st_SaveCamera;
		MATH_tdst_Vector st_SunSource;
		LIGHT_tdst_List st_LightList;
		OBJ_tdst_SingleBV *pst_BVForLight;
		SOFT_tdst_MatrixStack st_MatrixStack;
		SOFT_tdst_ComputingBuffers *pst_ComputingBuffers;//ONLY_PSX2_ALIGNED(16);
		SOFT_tdst_FogParams st_Fog;                      //0
		SOFT_tdst_FogParams st_Fog1;                     //1er
		SOFT_tdst_FogParams st_Fog2;                     //2eme
		TEX_tdst_Manager st_TexManager;

		ULONG ul_DisplayFlags;

		void *pv_SpecificData;
		struct WOR_tdst_World_ *pst_World;
		void *pv_ShadowStack;

		unsigned char uc_ForceNumFrames;

#ifdef ACTIVE_EDITORS
		struct SOFT_tdst_PickingBuffer_ *pst_PickingBuffer;
		WAY_tdst_GraphicLinks st_DisplayedLinks;
		GEO_tdst_GraphicZones st_DisplayedZones;
		void *pst_EditorCamObject;
		unsigned char uc_EngineCamera;
		unsigned char uc_EditorCamera;
		unsigned char uc_LockObjectToCam;
		struct OBJ_tdst_GameObject_ *pst_CamTarget;// The object the camera is looking at
		MATH_tdst_Matrix st_OffsetToCamTarget;
		unsigned char uc_SnapshotFlag;
		unsigned char uc_EditBounding;
		unsigned char uc_EditODE;
		unsigned char uc_StaticBV;
		unsigned char uc_ColMapDisplayMode;
		unsigned char uc_ColMapShowSlope;
		unsigned char uc_DrawPostIt;
		ULONG uc_SnapshotCounter;
		ULONG uc_SnapshotCurrent;
		char sz_SnapshotName[ 255 ];
		char sz_SnapshotPath[ 255 ];
		struct SOFT_tdst_Helpers_ *pst_Helpers;
		struct GRID_tdst_DisplayOptions_ *pst_GridOptions;
		struct GRO_tdst_EditOptions_ *pst_EditOptions;
		LONG l_WPShowStatus;
		LONG l_WPShowStatus2;
		struct SOFT_tdst_BackgroundImage_ *pst_BackgroundImage;
		ULONG ul_GFXRenderMask;
		ULONG ul_WiredMode;
		ULONG ul_USer2;
		ULONG ul_WPFlags;
		float f_WPSize;
		struct OBJ_tdst_GameObject_ *pst_GetLightForGO;
		struct WATER_Export_Struct_ *pst_OceanExp;
#	ifdef JADEFUSION
		bool bLogLightUsage;
		unsigned char uc_EnableElementCulling;
		bool b_ShowVertexColor;
		bool b_DisableBaseMap;
		bool b_DisableMoss;
		bool b_DisableLightmap;

		ULONG ul_HighlightMode;
		GDI_tdst_HighlightColors st_HighlightColors;

		BOOL b_PasteGaoProperties;
#	endif

		BOOL b_DisplayDebugSelected;

		// Camera for second active view in split mode (see WOR_View_SplitView).
		// These datas will be used by the split view (flags WOR_Cuc_View_SplitView set).
		CAM_tdst_Camera st_SplitViewCamera;
		CAM_tdst_Camera st_SaveSplitViewCamera;
		void *pst_EditorSplitViewCamObject;
		unsigned char uc_EngineSplitViewCamera;
		unsigned char uc_EditorSplitViewCamera;
		unsigned char uc_LockObjectToSplitViewCam;
		struct OBJ_tdst_GameObject_ *pst_SplitViewCamTarget;// The object the camera is looking at
		MATH_tdst_Matrix st_OffsetToSplitViewCamTarget;
#endif

		struct GDI_tdst_Rasters_ *pst_Raster;

		/* Graphic Context save variables */

		/* Should beonly used for opengl */
		ULONG LastDrawMask;
		ULONG LastTextureUsed;
		ULONG LastBlendingMode;
		ULONG ul_RenderingCounter;


		ULONG ShowAEInEngine;
		ULONG ShowAEEditor;
		ULONG GlobalMul2X;
		float GlobalMul2XFactor;
		ULONG ShowAlphaBuffer;
		ULONG ClearAlphaForGlowAtNextLoad;
		ULONG DrawGraphicDebugInfo;
		u32 TRI_ALarm;  // Alarm when too many visible triangles
		u32 DRAW_ALarm; // Alarm when ("triangle nb"/"call to draw triangles") < DRW_ALarm
		u32 OBJ_ALarm;  // Alarm when too many objects.
		u32 SPG_ALarm;  // Alarm when too many active SPG
		u32 SMALL_ALarm;// Display red triangles when geometry has less than SMALL_ALarm triangles.
		u32 ShowHistogramm;
		u32 GlobalXInvert;
		u32 DisplayMemInfo;
		u32 DisplayTriInfo;
		u32 ColorCostIA;
		u32 ColorCostIAThresh;

		BOOL b_AntiAliasingBlur;

#ifdef JADEFUSION
		ULONG ul_LightCullingFlags;
		BOOL b_DrawWideScreenBands;
		BOOL b_EnableColorCorrection;

#	ifdef _XENON_RENDER
		CHAR g_cZListCurrentDisplayOrder;
#	endif
#endif

		GDI_tdst_ProfilingInformation_ profilingInformation;
	} GDI_tdst_DisplayData ONLY_PSX2_ALIGNED( 64 );

	extern GDI_tdst_DisplayData *GDI_gpst_CurDD;
#define GDI_gpst_CurDD_SPR ( *GDI_gpst_CurDD )

#define GDI_WPShowStatusOn( M_DD, M_index ) \
	( ( M_index < 32 ) ? ( M_DD->l_WPShowStatus & ( 1 << M_index ) ) : ( M_DD->l_WPShowStatus2 & ( 1 << ( M_index - 32 ) ) ) )

	/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

	GDI_tdst_DisplayData *GDI_fnpst_CreateDisplayData( void );
	void GDI_fnv_DestroyDisplayData( GDI_tdst_DisplayData * );
	LONG GDI_fnl_InitInterface( GDI_tdst_Interface *, LONG _l_GDIType );

	LONG GDI_AttachDisplay( GDI_tdst_DisplayData *, HWND );
	LONG GDI_DetachDisplay( GDI_tdst_DisplayData * );
	LONG GDI_ReadaptDisplay( GDI_tdst_DisplayData *, HWND );

	void GDI_InitModule( void );
	void GDI_CloseModule( void );

	void GDI_BeforeDisplay( GDI_tdst_DisplayData * );
	void GDI_AfterDisplay( GDI_tdst_DisplayData * );

	LONG GDI_l_DetachWorld( GDI_tdst_DisplayData * );
	LONG GDI_l_AttachWorld( GDI_tdst_DisplayData *, struct WOR_tdst_World_ * );

#ifdef ACTIVE_EDITORS
	void GDI_SwapCameras( GDI_tdst_DisplayData * );
	void GDI_CopyMainCameraToSplitViewCamera( GDI_tdst_DisplayData * );
	void GDI_AttachMainCamToObj( GDI_tdst_DisplayData *, struct OBJ_tdst_GameObject_ * );
	void GDI_DetachMainCamFromObj( GDI_tdst_DisplayData * );
#endif

#if defined( __cplusplus ) && !defined( JADEFUSION )
}
#endif
#endif /* __GDINTERFACE_H__ */
