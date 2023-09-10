/*$T WORrender.c GC 1.134 04/22/04 14:16:04 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORrender.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "ENGine/Sources/OBJects/OBJgizmo.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"

#include "ENGine/Sources/ANImation/ANIrender.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/GRId/GRI_display.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_LAZY.h"

#include "GRObject/GROrender.h"
#include "WAYpoint/WAYpoint.h"
#include "WAYpoint/WAYlink.h"
#include "GDInterface/GDIrasters.h"
#include "GDInterface/GDIrequest.h"
#include "GEOmetric/GEOboundingvolume.h"
#include "GEOmetric/GEOzone.h"
#include "GEOmetric/GEO_LIGHTCUT.h"
#include "GFX/GFX.h"
#include "STRing/STRdata.h"
#include "SOFT/SOFTHelper.h"
#include "BASe/BAStypes.h"
#include "MATerial/MATShadow.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTcolor.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIengine.h"

#ifdef Active_CloneListe
#include "ENGine/Sources/OBJects/OBJinit.h"
#endif

#ifdef JADEFUSION
#include "GEOmetric/GEOstatistics.h"
#endif

#include "TIMer/PROfiler/PROPS2.h"
#include "BASe/BENch/BENch.h"
#include <assert.h>

#ifdef ACTIVE_EDITORS
#include "ENGine/Sources/COLlision/COLcob.h"
#include "SELection/SELection.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GAO.h"
#include "GEOmetric/GEODebugObject.h"
#endif

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeVertexShaderManager.h"
#include "XenonGraphics/XePixelShaderManager.h"
#include "XenonGraphics/XeGDInterface.h"
#include "XenonGraphics/XeShadowManager.h"
#include "XenonGraphics/XeUtils.h"
#include "XenonGraphics/XeAfterEffectManager.h"
#endif

#if defined(_XENON)
#include "Xenon/MenuManager/MenuManager.h"
#endif

#if defined(_XBOX)
#include "Gx8/Gx8Init.h"
#include "Gx8/Gx8water.h"
#include "Gx8/AfterFX/Gx8AfterFX_Def.h"
#include "Gx8/Gx8ShadowBuffer.h"
#include <xgraphics.h>//a effacer
#endif
#if defined(_PC_RETAIL)
#include "Dx9/Dx9Init.h"
#include "Dx9/Dx9Error.h"
#include "Dx9/AfterFX/Dx9AfterFX_Def.h"

/*
 * include "Dx9/Gx8ShadowBuffer.h" £
 * # include "Dx9/Gx8water.h"
 */
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern void					TIM_SynchroAdjustDisplay(void);

#ifdef PSX2_TARGET
extern void					Gsp_Before2D(void);
#elif defined(_GAMECUBE)
extern void					GXI_Before2D(void);
#endif
#if !defined(PSX2_TARGET) && !defined(_XBOX) && !defined(_GAMECUBE) && !defined(_XENON)
#include <gl/gl.h>
#endif
#ifdef ACTIVE_EDITORS
extern BOOL					sgb_EngineRender;
extern BOOL					OBJ_gb_DebugPhotoMode;
#endif

extern void					OGL_AE_Before2D();
extern void					OGL_AE_DebugInfo(WOR_tdst_World *_pst_World);

#if defined(USE_DOUBLE_RENDERING)
extern GDI_tdst_DisplayData gpst_GSP_stDD;
#endif
extern MATH_tdst_Vector		g_stGlobalNormPlaneUp;
extern MATH_tdst_Vector		g_stGlobalNormPlaneDown;
extern MATH_tdst_Vector		g_stGlobalNormPlaneLeft;
extern MATH_tdst_Vector		g_stGlobalNormPlaneRight;
extern void MODIFIER_SPG2_OneFrameCall();
extern void MODIFIER_FOGDYN_OneFrameCall();
#ifdef _GAMECUBE
#include "GXI_GC/GXI_def.h"
#include "GXI_GC/GXI_init.h"
#include "GXI_GC/GXI_shadows.h"
#endif



/* XBOX ONLY SHADOW BUFFER FUNCTIONS */
#if defined(_XBOX) || defined(_XENON)

/* Render all the shadow buffer from light point of view */
void				WOR_RenderAllShadowBuffer(WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD);
void				WOR_RenderShawowBufferDiffuse(WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD);
void				WOR_Clea_All_GO(WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD);

bool				WOR_CheckIfRecShadows(OBJ_tdst_GameObject *pst_GO);
bool				WOR_CheckIfCasShadows(OBJ_tdst_GameObject *pst_GO);
#endif

/*$4
 ***********************************************************************************************************************
    constants
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS
int					i_NumberOfSelectedLinks;
OBJ_tdst_GameObject *apst_Gizmo[500];
OBJ_tdst_GameObject *apst_Skels[500];
int					i_NumGizmos;
int					i_NumSkels;
ULONG				ul_SaveDrawMask;
BOOL				ENG_gb_DistCompute = TRUE;
F3D_tdst_PostIt		**WOR_gpt_AllPostIt = NULL;
ULONG				WOR_gul_AllPostIt = 0;
#endif
extern BOOL			ENG_gb_ActiveSectorization;

extern void			PROTEX_BeforeDraw(void);
extern void			WATER3D_BeforeDraw();

/*$4
 ***********************************************************************************************************************
    Macros for editor purpose
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    clear the Z buffer for View
 =======================================================================================================================
 */
#define M_Clear4View(_View_) \
	if(_View_ >= 4) OGL_Clear(GDI_Cl_ZBuffer, 0);

/*
 =======================================================================================================================
    save/restore draw mask
 =======================================================================================================================
 */
#define M4Edit_SaveDrawMask()		ul_SaveDrawMask = _pst_DD->ul_DrawMask;
#define M4Edit_RestoreDrawMask()	_pst_DD->ul_DrawMask = ul_SaveDrawMask;

/*
 =======================================================================================================================
    init number of selected links
 =======================================================================================================================
 */
#define M4Edit_InitNumberOfSelectedLinks() \
	i_NumberOfSelectedLinks = SEL_l_CountItem \
		( \
			_pst_World->pst_Selection, \
			SEL_C_SIF_Link \
		);

/*
 =======================================================================================================================
    init list of displayed bones and skeleton
 =======================================================================================================================
 */
#define M4Edit_InitBonesAndSkeletonDisplayList() \
	i_NumGizmos = 0; \
	i_NumSkels = 0;

/*
 =======================================================================================================================
    Add bones
 =======================================================================================================================
 */
#define M4Edit_AddBonesAndContinue(_GO_) \
	if \
	( \
		((_GO_)->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject) \
	&&	(ANI_b_GizmoHasTrack((_GO_))) \
	) \
	{ \
		apst_Gizmo[i_NumGizmos++] = _GO_; \
		return; \
	}

/*
 =======================================================================================================================
    Add skeleton
 =======================================================================================================================
 */
#define M4Edit_AddSkeleton(_GO_) \
	if((_GO_)->ul_EditorFlags & OBJ_C_EditFlags_DrawSkeleton) apst_Skels[i_NumSkels++] = _GO_;

/* test for engine/editor rendering */
#define M4Edit_IfEngineRender	sgb_EngineRender

/* test if gameobject is not a links */
#define M4Edit_IfNotLinks	(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Links))

/*
 =======================================================================================================================
    reset Compute RLI var (indicate that light are updating RLI or not
 =======================================================================================================================
 */
#define M4Edit_ResetLightComputeRLI()	LIGHT_gl_ComputeRLI = 0;

/*
 =======================================================================================================================
    shade selected (all not selected object are drawn in wireframe mode )
 =======================================================================================================================
 */
#define M4Edit_ShadeSelected() \
	if(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShadeSelected) \
	{ \
		if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) \
			_pst_DD->ul_DrawMask |= GDI_Cul_DM_NotWired; \
		else \
			_pst_DD->ul_DrawMask &= ~GDI_Cul_DM_NotWired; \
	}

#else /* ACTIVE_EDITORS */

#define M_Clear4View(_View_) \
	if(_View_) _pst_DD->st_GDI.pfnv_Clear(GDI_Cl_ZBuffer, 0);

#define M4Edit_SaveDrawMask()
#define M4Edit_RestoreDrawMask()
#define M4Edit_InitNumberOfSelectedLinks()
#define M4Edit_InitBonesAndSkeletonDisplayList()
#define M4Edit_AddBonesAndContinue(_GO_)
#define M4Edit_AddSkeleton(_GO_)
#define M4Edit_IfEngineRender	1
#define M4Edit_IfNotLinks		1
#define M4Edit_ResetLightComputeRLI()
#define M4Edit_ShadeSelected()
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Update viewpoint camera and camera data
 =======================================================================================================================
 */
void WOR_SetCam(WOR_tdst_View *pst_View)
{
#ifdef ACTIVE_EDITORS
	if (GDI_gpst_CurDD->pst_CamTarget)
	{
		MATH_tdst_Matrix	st_CamPosMat;
		MATH_MulMatrixMatrix(
			&st_CamPosMat,
			&GDI_gpst_CurDD->st_OffsetToCamTarget,
			GDI_gpst_CurDD->pst_CamTarget->pst_GlobalMatrix);
		CAM_SetCameraMatrix (&GDI_gpst_CurDD->st_Camera, &st_CamPosMat);
	}
	else if(GDI_gpst_CurDD->uc_EditorCamera && GDI_gpst_CurDD->pst_EditorCamObject)
	{
		CAM_SetCameraMatrix
		(
			&GDI_gpst_CurDD->st_Camera,
			((OBJ_tdst_GameObject *) (GDI_gpst_CurDD->pst_EditorCamObject))->pst_GlobalMatrix
		);
	}
	else if(GDI_gpst_CurDD->uc_EngineCamera)
	{
		CAM_SetCameraMatrix(&GDI_gpst_CurDD->st_Camera, &pst_View->st_ViewPoint);
	}

	if(GDI_gpst_CurDD->uc_LockObjectToCam && GDI_gpst_CurDD->pst_EditorCamObject)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	st_Mat;
		/*~~~~~~~~~~~~~~~~~~~~~~~*/

		CAM_SetObjectMatrixFromCam(&st_Mat, &GDI_gpst_CurDD->st_Camera.st_Matrix);
		OBJ_SetAbsoluteMatrix((OBJ_tdst_GameObject*)GDI_gpst_CurDD->pst_EditorCamObject, &st_Mat);
		OBJ_ComputeLocalWhenHie((OBJ_tdst_GameObject*)GDI_gpst_CurDD->pst_EditorCamObject);
	}

	if(GDI_gpst_CurDD->st_Camera.ul_Flags & CAM_Cul_Flags_Iso2)
	{
		if(!(GDI_gpst_CurDD->st_Camera.ul_Flags & CAM_Cul_Flags_Iso2Old))
		{
			GDI_gpst_CurDD->st_Camera.ul_Flags |= CAM_Cul_Flags_Iso2Old;
		}
	}
	else
	{
		GDI_gpst_CurDD->st_Camera.ul_Flags &= ~CAM_Cul_Flags_Iso2Old;
	}

#else
	CAM_SetCameraMatrix(&GDI_gpst_CurDD->st_Camera, &pst_View->st_ViewPoint);
#endif

	/* We set the DD Camera Field of vision. */
	GDI_gpst_CurDD->st_Camera.f_FieldOfVision = pst_View->st_DisplayInfo.f_FieldOfVision;
#ifdef JADEFUSION
#if defined(ACTIVE_EDITORS)
    GDI_gpst_CurDD->st_Camera.f_FieldOfVision *= 0.75f;
#endif
#endif
	GDI_gpst_CurDD->st_Camera.f_ViewportLeft = pst_View->st_DisplayInfo.f_ViewportLeft;
	GDI_gpst_CurDD->st_Camera.f_ViewportTop = pst_View->st_DisplayInfo.f_ViewportTop;
	GDI_gpst_CurDD->st_Camera.f_ViewportWidth = pst_View->st_DisplayInfo.f_ViewportWidth;
	GDI_gpst_CurDD->st_Camera.f_ViewportHeight = pst_View->st_DisplayInfo.f_ViewportHeight;

	GDI_gpst_CurDD->st_GDI.pfnv_SetProjectionMatrix(&GDI_gpst_CurDD->st_Camera);
	CAM_Inverse(&GDI_gpst_CurDD->st_Camera);
	SOFT_MatrixStack_Reset(&GDI_gpst_CurDD->st_MatrixStack, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
}

#ifdef ACTIVE_EDITORS

#ifdef ODE_INSIDE
extern UCHAR	COL_s_GhostOptimisation;
#endif

/*
 =======================================================================================================================
    Test if an object have to be render or not£ in editor we test presence of visu struct and colmap display£ in engine
    we only test presence of visu struct
 =======================================================================================================================
 */
BOOL WOR_b_MustRenderVisuGO(OBJ_tdst_GameObject *_pst_GO, GDI_tdst_DisplayData *_pst_DD)
{
#ifdef ODE_INSIDE
	if(COL_s_GhostOptimisation && (((_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) && _pst_GO->pst_Base->pst_ODE->ode_id_body && !dBodyIsEnabled (_pst_GO->pst_Base->pst_ODE->ode_id_body)))) 
		_pst_GO->ul_EditorFlags |= OBJ_C_EditFlags_GhostODE;
	else
		_pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_GhostODE;
#endif

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)) return FALSE;

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
	{
		if((_pst_DD->uc_ColMapDisplayMode & 0x80) && (_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCOB)) return TRUE;
		return(!(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCOB));
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_ColMap *pst_ColMap;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;

#ifdef ACTIVE_EDITORS
		if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ForceDisplayInColMapMode) return TRUE;
#endif
		if(pst_ColMap && pst_ColMap->dpst_Cob && (pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles))
		{
			return TRUE;
		}
		else
		{
			if((_pst_DD->uc_ColMapDisplayMode & 0x80) && (_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCOB)) return TRUE;
			return(!(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCOB));
		}
	}
}

#else
#define WOR_b_MustRenderVisuGO(a, b)	OBJ_b_TestIdentityFlag(a, OBJ_C_IdentityFlag_Visu)
#endif
#ifdef ACTIVE_EDITORS
extern void					GEO_OK3_Display(GDI_tdst_DisplayData *, OBJ_tdst_GameObject *, BOOL);
extern MDF_tdst_Modifier	*GAO_ModifierPhoto_Get(OBJ_tdst_GameObject *, BOOL);
extern BOOL					GAO_ModifierPhoto_LODAndFrameareOK(GAO_tdst_ModifierPhoto *);
extern OBJ_tdst_GameObject	*GAO_ModifierPhoto_SnapGOGet(OBJ_tdst_GameObject *, GAO_tdst_ModifierPhoto *, int);

/*
 =======================================================================================================================
 =======================================================================================================================
 */

void WOR_RenderFakePhotoFrame(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	Point3D[2], Point2D[2];
	MATH_tdst_Vector	*Quad[2];
	ULONG				ul_SaveColorconstant;
	ULONG				ul_SaveDrawMask;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_SaveDrawMask = _pst_DD->ul_CurrentDrawMask;
	ul_SaveColorconstant = _pst_DD->ul_ColorConstant;

	_pst_DD->ul_CurrentDrawMask = GDI_Cul_DM_All - GDI_Cul_DM_NotWired - GDI_Cul_DM_DontForceColor - GDI_Cul_DM_Lighted;
	_pst_DD->ul_ColorConstant = 0xFFFFFFFF;

	Quad[0] = &Point3D[0];
	Quad[1] = &Point3D[1];

	_pst_DD->st_GDI.pfnv_SetViewMatrix(&_pst_DD->st_Camera.st_InverseMatrix);

	/* Up Line */
	MATH_InitVector(&Point2D[0], _pst_DD->st_Camera.f_CenterX - 256.0f, _pst_DD->st_Camera.f_CenterY - 128.0f, 0.1f);
	MATH_InitVector(&Point2D[1], _pst_DD->st_Camera.f_CenterX + 256.0f, _pst_DD->st_Camera.f_CenterY - 128.0f, 0.1f);

	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[0], &Point2D[0]);
	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[1], &Point2D[1]);

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);

	/* Down Line */
	MATH_InitVector(&Point2D[0], _pst_DD->st_Camera.f_CenterX - 256.0f, _pst_DD->st_Camera.f_CenterY + 128.0f, 0.1f);
	MATH_InitVector(&Point2D[1], _pst_DD->st_Camera.f_CenterX + 256.0f, _pst_DD->st_Camera.f_CenterY + 128.0f, 0.1f);

	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[0], &Point2D[0]);
	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[1], &Point2D[1]);

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);

	/* Right Line */
	MATH_InitVector(&Point2D[0], _pst_DD->st_Camera.f_CenterX + 256.0f, _pst_DD->st_Camera.f_CenterY - 128.0f, 0.1f);
	MATH_InitVector(&Point2D[1], _pst_DD->st_Camera.f_CenterX + 256.0f, _pst_DD->st_Camera.f_CenterY + 128.0f, 0.1f);

	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[0], &Point2D[0]);
	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[1], &Point2D[1]);

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);

	/* Left Line */
	MATH_InitVector(&Point2D[0], _pst_DD->st_Camera.f_CenterX - 256.0f, _pst_DD->st_Camera.f_CenterY - 128.0f, 0.1f);
	MATH_InitVector(&Point2D[1], _pst_DD->st_Camera.f_CenterX - 256.0f, _pst_DD->st_Camera.f_CenterY + 128.0f, 0.1f);

	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[0], &Point2D[0]);
	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[1], &Point2D[1]);

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 0);
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 1);

	_pst_DD->ul_CurrentDrawMask = ul_SaveDrawMask;
	_pst_DD->ul_ColorConstant = ul_SaveColorconstant;
}

extern void SOFT_TransformAndProject(MATH_tdst_Vector *, MATH_tdst_Vector *, LONG, CAM_tdst_Camera *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_RenderFakePhotoInfo
(
	GDI_tdst_DisplayData	*_pst_DD,
	OBJ_tdst_GameObject		*_pst_GO,
	GAO_tdst_ModifierPhoto	*_pst_Data
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_SnapGO;
	MATH_tdst_Vector	Point2D[2], Point3D[2];
	MATH_tdst_Vector	Center2D;
	MATH_tdst_Vector	*Quad[2];
	ULONG				ul_SaveColorconstant;
	ULONG				ul_SaveDrawMask;
	MATH_tdst_Vector	*pst_InfoLocalOffset;
	MATH_tdst_Vector	st_InfoGlobal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_SaveDrawMask = _pst_DD->ul_CurrentDrawMask;
	_pst_DD->ul_CurrentDrawMask = GDI_Cul_DM_All - GDI_Cul_DM_NotWired - GDI_Cul_DM_DontForceColor - GDI_Cul_DM_Lighted;
	
	ul_SaveColorconstant = _pst_DD->ul_ColorConstant;

	pst_SnapGO = GAO_ModifierPhoto_SnapGOGet(_pst_GO, _pst_Data, 1);

	pst_InfoLocalOffset = &_pst_Data->st_InfoOffset;
	MATH_TransformVector(&st_InfoGlobal, pst_SnapGO->pst_GlobalMatrix, pst_InfoLocalOffset);
	MATH_AddEqualVector(&st_InfoGlobal, &pst_SnapGO->pst_GlobalMatrix->T);

	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = &_pst_DD->st_Camera.st_InverseMatrix;
	SOFT_TransformAndProject(&Center2D, &st_InfoGlobal, 1, &_pst_DD->st_Camera);

	/* Inversion of the Titans !! Thanx to Vincent LHULLIERRRRR !!!!!! */
	Center2D.y = _pst_DD->st_Camera.f_Height - Center2D.y;

	if((Center2D.x == 0x80000000) || (Center2D.y == 0x80000000)) return;

	Quad[0] = &Point3D[0];
	Quad[1] = &Point3D[1];

	_pst_DD->st_GDI.pfnv_SetViewMatrix(&_pst_DD->st_Camera.st_InverseMatrix);
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 0);

	/* Up Line */
	MATH_InitVector(&Point2D[0], Center2D.x - 15.0f, Center2D.y + 10.0f, 0.1f);
	MATH_InitVector(&Point2D[1], Center2D.x + 15.0f, Center2D.y + 10.0f, 0.1f);

	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[0], &Point2D[0]);
	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[1], &Point2D[1]);
	
	_pst_DD->ul_ColorConstant = 0xFF00FF00;

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);

	/* Down Line */
	MATH_InitVector(&Point2D[0], Center2D.x - 15.0f, Center2D.y - 10.0f, 0.1f);
	MATH_InitVector(&Point2D[1], Center2D.x + 15.0f, Center2D.y - 10.0f, 0.1f);

	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[0], &Point2D[0]);
	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[1], &Point2D[1]);

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);

	/* Right Line */
	MATH_InitVector(&Point2D[0], Center2D.x + 15.0f, Center2D.y + 10.0f, 0.1f);
	MATH_InitVector(&Point2D[1], Center2D.x + 15.0f, Center2D.y - 10.0f, 0.1f);

	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[0], &Point2D[0]);
	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[1], &Point2D[1]);

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);

	/* Left Line */
	MATH_InitVector(&Point2D[0], Center2D.x - 15.0f, Center2D.y + 10.0f, 0.1f);
	MATH_InitVector(&Point2D[1], Center2D.x - 15.0f, Center2D.y - 10.0f, 0.1f);

	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[0], &Point2D[0]);
	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[1], &Point2D[1]);

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);

	/* Up Line */
	MATH_InitVector(&Point2D[0], Center2D.x - 15.0f, Center2D.y - 10.0f, 0.1f);
	MATH_InitVector(&Point2D[1], Center2D.x - 15.0f, Center2D.y + 10.0f, 0.1f);

	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[0], &Point2D[0]);
	CAM_2Dto3D(&_pst_DD->st_Camera, &Point3D[1], &Point2D[1]);

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLine, (ULONG) Quad);

	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 1);
	_pst_DD->ul_CurrentDrawMask = ul_SaveDrawMask;
	_pst_DD->ul_ColorConstant = ul_SaveColorconstant;
}

/*
 =======================================================================================================================
    Editor rendering: waypoint and links, hierarchical links, light, zone, BV, invisible
 =======================================================================================================================
 */
void WOR_RenderForEditors_Way_BV_Light_Invisible_Zone
(
	WOR_tdst_World			*_pst_World,
	GDI_tdst_DisplayData	*_pst_DD,
	OBJ_tdst_GameObject		*pst_GO
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WAY_tdst_Struct		*pst_Way;
	WAY_tdst_LinkList	*pst_LL, *pst_LastLL;
	WAY_tdst_Link		*pst_Link, *pst_LastLink;
	WAY_tdst_Link		st_Link;
	MDF_tdst_Modifier	*pst_Modifier;
	LONG				l;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- Waypoints and links ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypoint)
	{
		if
		(
			!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Links)
		&&	OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Waypoints)
		) GRO_RenderGro(pst_GO, &WAY_gst_GroStruct);
	}

	if(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_DisplayWaypointInNetwork)
	{
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Links))
		{
			if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Waypoints))
			{
				pst_Way = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;
				if(pst_Way)
				{
					pst_LL = pst_Way->pst_AllLinks;
					pst_LastLL = pst_LL + pst_Way->ul_Num;
					for(; pst_LL && pst_LL < pst_LastLL; pst_LL++)
					{
						if(pst_LL->pst_Network->ul_Flags & WAY_C_DisplayNet)
						{
							GRO_RenderGro(pst_GO, &WAY_gst_GroStruct);
							break;
						}
					}
				}
			}

			pst_Way = (WAY_tdst_Struct *) pst_GO->pst_Extended->pst_Links;
			if(pst_Way)
			{
				pst_LL = pst_Way->pst_AllLinks;
				pst_LastLL = pst_LL + pst_Way->ul_Num;
				for(; pst_LL && pst_LL < pst_LastLL; pst_LL++)
				{
					if(pst_LL->pst_Network->ul_Flags & WAY_C_DisplayNet)
					{
						pst_Link = pst_LL->pst_Links;
						pst_LastLink = pst_Link + pst_LL->ul_Num;
						for(; pst_Link < pst_LastLink; pst_Link++)
						{
							if((i_NumberOfSelectedLinks) && (SEL_RetrieveItem(_pst_World->pst_Selection, pst_Link)))
							{
								i_NumberOfSelectedLinks--;
								WAY_Link_Render(pst_GO, pst_Link, OBJ_C_EditFlags_Selected, 0, 0);
							}
							else
								WAY_Link_Render(pst_GO, pst_Link, 0, 0, 0);
						}
					}
				}
			}
		}
	}

	/*$1- Hierarchy links ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_DisplayHierarchy)
	{
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		{
			if(pst_GO->pst_Base->pst_Hierarchy->pst_Father)
			{
				if(SEL_RetrieveItemWithType(_pst_World->pst_Selection, pst_GO, SEL_C_SIF_HieLink))
					l = OBJ_C_EditFlags_Selected;
				else
					l = 0;

				st_Link.pst_Next = pst_GO;
				WAY_Link_Render(pst_GO->pst_Base->pst_Hierarchy->pst_Father, &st_Link, l, 1, 1);
			}

			if(pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit)
			{
				if(SEL_RetrieveItemWithType(_pst_World->pst_Selection, pst_GO, SEL_C_SIF_HieLink))
					l = OBJ_C_EditFlags_Selected;
				else
					l = 0;

				st_Link.pst_Next = pst_GO;
				WAY_Link_Render(pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit, &st_Link, l, 1, 0);
			}
		}
	}

	/*$1- Lights ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_DisplayLight)
	{
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Lights))
			GRO_RenderGro(pst_GO, pst_GO->pst_Extended->pst_Light);
	}

	/*$1- Invisible objects ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(pst_GO->ul_IdentityFlags & OBJ_C_VisibleObjectFlags))
	{
		if(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible)
		{
			if( GDI_WPShowStatusOn( _pst_DD, pst_GO->ul_InvisibleObjectIndex ) )
				GRO_RenderGro(pst_GO, &GRO_gst_Unknown);
		}
	}

	/*$1- Game object bounding volume ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_DoNotRender) && !(pst_GO->ul_AdditionalFlags & OBJ_C_EdAddFlags_HideBV) )
	{
		if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
		{
			if(SOFT_l_Helpers_MoversAreVisible(_pst_DD->pst_Helpers))
			{
				if((void *) pst_GO != SEL_pst_GetFirstItem(_pst_World->pst_Selection, SEL_C_SIF_Object))
					_pst_DD->ul_DisplayFlags |= GDI_Cul_DF_ShowCurrentBV;
			}
			else
				_pst_DD->ul_DisplayFlags |= GDI_Cul_DF_ShowCurrentBV;
		}

		if
		(
			(
				(
					(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
				||	(
						pst_GO
					&&	pst_GO->pst_Extended
					&&	pst_GO->pst_Extended->pst_Col
					&&	((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap
					&&	((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob
					&&	SEL_RetrieveItem
							(
								_pst_DD->pst_World->pst_Selection,
								*(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob)
							)
					)
				)
			)
		) _pst_DD->ul_DisplayFlags |= GDI_Cul_DF_ShowCurrentBV;

		if((pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowBV) || !(_pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_DontShowBV))
			_pst_DD->ul_DisplayFlags |= GDI_Cul_DF_ShowCurrentBV;

		if(_pst_DD->ul_DisplayFlags & (GDI_Cul_DF_ShowCurrentBV)) GEO_BoundingVolume_Display(_pst_DD, pst_GO);

#ifdef _XENON_RENDER
        if( pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowElementBV )
        {
            GEO_BoundingVolume_DisplayElementsBV( _pst_DD, pst_GO );
        }
#endif

	}

	/*$1- Zones ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		pst_GO->ul_IdentityFlags &
			(
				OBJ_C_IdentityFlag_ZDM |
				OBJ_C_IdentityFlag_ZDE |
				OBJ_C_IdentityFlag_ColMap |
				OBJ_C_IdentityFlag_Sound |
#ifdef ODE_INSIDE
				OBJ_C_IdentityFlag_ODE |
#endif
				OBJ_C_IdentityFlag_ExtendedObject
			)
	) GEO_Zone_Display(_pst_DD, pst_GO);

	/*$1- Visual ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if((_pst_DD->ul_DisplayFlags & GDI_cul_DF_DisplayOK3) && !(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCOB))
		GEO_OK3_Display(_pst_DD, pst_GO, FALSE);

	/*$1- Info Photos (Update and Display) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Modifier = GAO_ModifierPhoto_Get(pst_GO, TRUE);
	if(pst_Modifier && OBJ_gb_DebugPhotoMode)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GAO_tdst_ModifierPhoto	*pst_Data;
		OBJ_tdst_GameObject		*pst_SnapGO;
		ULONG					ul_SaveColorconstant;
		MATH_tdst_Vector		*pst_SphereLocalOffset;
		MATH_tdst_Vector		st_SphereGlobal;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Data = (GAO_tdst_ModifierPhoto *) pst_Modifier->p_Data;
		pst_SnapGO = GAO_ModifierPhoto_SnapGOGet(pst_Modifier->pst_GO, pst_Data, 0);

		pst_SphereLocalOffset = &pst_Data->st_SphereOffset;
		MATH_TransformVector(&st_SphereGlobal, pst_SnapGO->pst_GlobalMatrix, pst_SphereLocalOffset);

		MATH_AddEqualVector(&st_SphereGlobal, &pst_SnapGO->pst_GlobalMatrix->T);

		GEO_DebugObject_PushSphereMatrix(&_pst_DD->st_MatrixStack, &st_SphereGlobal, pst_Data->f_Radius);

		_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
		GDI_SetViewMatrix((*_pst_DD), _pst_DD->st_MatrixStack.pst_CurrentMatrix);

		ul_SaveColorconstant = _pst_DD->ul_ColorConstant;
		_pst_DD->ul_ColorConstant = (GAO_ModifierPhoto_LODAndFrameareOK(pst_Data)) ? 0xFF00FF00 : 0x000000FF;

		GEO_DebugObject_Draw
		(
			_pst_DD,
			GEO_DebugObject_Sphere,
			(GDI_Cul_DM_All - GDI_Cul_DM_NotWired - GDI_Cul_DM_DontForceColor - GDI_Cul_DM_Lighted),
			0,
			NULL
		);

		_pst_DD->ul_ColorConstant = ul_SaveColorconstant;
		GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);

		WOR_RenderFakePhotoInfo(_pst_DD, pst_GO, pst_Data);
	}
}

#else /* ACTIVE_EDITORS */

#define WOR_RenderForEditors_Way_BV_Light_Invisible_Zone(a, b, c)
#endif
#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Editor rendering:: skeletons, gizmo, ray, kilt, grid, secto
 =======================================================================================================================
 */
void WOR_RenderForEditors_Skel_Gizmo_Ray_Kilt_Grid(WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int								igs;
	int								igz;
	ULONG							ul_SaveColor;
	ULONG							i,j;
	MATH_tdst_Vector				st_End, V[5];
	WOR_tdst_ShowVector				*pst_SV;
	GDI_tdst_Request_DrawLineEx		st_Line;
	WOR_tdst_Portal					*pst_Portal;
	WOR_tdst_Secto					*pst_Secto;
	GDI_tdst_Request_DrawQuad		st_Quad;
	GDI_tdst_Request_DrawPointEx	st_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- Skeletons ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(igs = 0; igs < i_NumSkels; igs++) SOFT_Helpers_CurveRenderSkel(apst_Skels[igs], _pst_DD);

	/*$1- Gizmos ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	_pst_DD->st_GDI.pfnv_SetViewMatrix(_pst_DD->st_Camera.pst_ObjectToCameraMatrix);
	ul_SaveColor = _pst_DD->ul_ColorConstant;
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 0);
	for(igz = 0; igz < i_NumGizmos; igz++)
	{
		if(TAB_ul_PFtable_GetElemIndexWithPointer(&_pst_World->st_AllWorldObjects, apst_Gizmo[igz]) != TAB_Cul_BadIndex)
		{
			SOFT_Helpers_CurveRenderBone(apst_Gizmo[igz], _pst_DD);
		}
	}

	_pst_DD->ul_ColorConstant = ul_SaveColor;
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DepthTest, 1);

	/*$1- Rays ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	if(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowAIDebugVectors)
	{
		_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
		_pst_DD->st_GDI.pfnv_SetViewMatrix(_pst_DD->st_Camera.pst_ObjectToCameraMatrix);

		ul_SaveColor = _pst_DD->ul_ColorConstant;

		if(_pst_World->uc_Vector >= WOR_Cte_DbgVectorRender) _pst_World->uc_Vector = 0;

		st_Line.f_Width = 2;
		st_Line.ul_Flags = GDI_Request_DrawLine_Arrow;
		st_Line.B = &st_End;
		
		st_Point.f_Size = 10;

		for(i = 0; i < _pst_World->uc_Vector; i++)
		{
			MATH_AddVector(&st_End, &_pst_World->st_Origin[i], &_pst_World->st_Vector[i]);
			st_Line.A = &_pst_World->st_Origin[i];
			st_Line.ul_Color = _pst_World->aul_Color[i];
            if (!_pst_World->ap_VectorGAO[i] || !_pst_World->ap_VectorGAO[i]->b_DontDisplayDebug)
    			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_Line);
		}

		pst_SV = _pst_World->ast_ShowVector;
		for(i = 0; i < _pst_World->uc_ShowVector; i++, pst_SV++)
		{
			_pst_DD->ul_ColorConstant = pst_SV->ul_Color;
			if(pst_SV->c_Vector)
			{
				st_Line.f_Width = 2;
				st_Line.ul_Flags = GDI_Request_DrawLine_Arrow;
				if(pst_SV->pst_Gao)
				{
					/* draw trace */
					for(j = 0; j < (ULONG) pst_SV->c_Remember; j++)
					{
						st_Line.A = pst_SV->st_RememberSrc + j;
						st_Line.B = pst_SV->st_RememberTgt + j;
						st_Line.ul_Color = COLOR_ul_Mul(pst_SV->ul_Color, ((float) (64 - j)) / ((float) 64));
						_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_Line);
					}

					/* draw current */
					st_Line.A = OBJ_pst_GetAbsolutePosition(pst_SV->pst_Gao);
					st_Line.B = &st_End;
					if(pst_SV->c_Local)
					{
						MATH_TransformVector(&st_End, OBJ_pst_GetAbsoluteMatrix(pst_SV->pst_Gao), pst_SV->pst_Vector);
						MATH_AddEqualVector(&st_End, st_Line.A);
					}
					else
						MATH_AddVector(&st_End, st_Line.A, pst_SV->pst_Vector);
					st_Line.ul_Color = pst_SV->ul_Color;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_Line);

					/* store for trace */
					if(ENG_gb_EngineRunning)
					{
						L_memmove(pst_SV->st_RememberSrc + 1, pst_SV->st_RememberSrc, 31 * sizeof(MATH_tdst_Vector));
						L_memmove(pst_SV->st_RememberTgt + 1, pst_SV->st_RememberTgt, 31 * sizeof(MATH_tdst_Vector));
						MATH_CopyVector(pst_SV->st_RememberSrc, st_Line.A);
						MATH_CopyVector(pst_SV->st_RememberTgt, st_Line.B);
						if(pst_SV->c_Remember < 32) pst_SV->c_Remember++;
					}
				}
			}
			else
			{
                if(!pst_SV->pst_Gao)
                {
                    /* draw trace */
                    for(j = 0; j < (ULONG) pst_SV->c_Remember; j++)
                    {
                        st_Point.ul_Color = COLOR_ul_Mul(pst_SV->ul_Color, ((float) (64 - j)) / ((float) 64));
                        st_Point.A = pst_SV->st_RememberSrc + j;
                        _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointEx, (ULONG) &st_Point );
                    }

                    /* draw current */
                    if(pst_SV->c_Local)
                    {
                        MATH_TransformVertex(&st_End, OBJ_pst_GetAbsoluteMatrix(pst_SV->pst_Gao), pst_SV->pst_Vector);
                        st_Point.A = &st_End;
                    }
                    else
                        st_Point.A = pst_SV->pst_Vector;

                    /* draw the point */
                    st_Point.ul_Color = pst_SV->ul_Color;
                    _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawPointEx, (ULONG) &st_Point );

                    /* draw a vertical line based on the point */
                    MATH_CopyVector( &V[0], st_Point.A );
                    V[0].z += 100.0f;
                    MATH_CopyVector( &V[1], st_Point.A );
                    V[1].z -= 100.0f;
                    st_Line.A = V;
                    st_Line.B = V+1;
                    st_Line.ul_Color = COLOR_ul_Mul( pst_SV->ul_Color, 0.5 );
                    _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) & st_Line);

                    /* store for trace */
                    if(ENG_gb_EngineRunning)
                    {
                        L_memmove(pst_SV->st_RememberSrc + 1, pst_SV->st_RememberSrc, 31 * sizeof(MATH_tdst_Vector));
                        MATH_CopyVector(pst_SV->st_RememberSrc, st_Point.A );
                        if(pst_SV->c_Remember < 32) pst_SV->c_Remember++;
                    }
                }
			}
		}

		for(i = 0; i < _pst_World->uc_Circle; i++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			SOFT_tdst_Ellipse	st_Ellipse;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(MATH_b_NulVector(&_pst_World->st_Normal[i]))
			{
				MATH_CopyVector(&st_Ellipse.st_A, MATH_pst_GetXAxis(&_pst_DD->st_Camera.st_Matrix));
				MATH_CopyVector(&st_Ellipse.st_B, MATH_pst_GetYAxis(&_pst_DD->st_Camera.st_Matrix));
			}
			else
			{
				/* For Depth Test */
				st_Ellipse.l_Flag |= 8;

				MATH_NormalizeEqualVector(&_pst_World->st_Normal[i]);

				if(MATH_f_DotProduct(&_pst_World->st_Normal[i], &MATH_gst_BaseVectorK) != 0)
				{
					MATH_CrossProduct(&st_Ellipse.st_A, &_pst_World->st_Normal[i], &MATH_gst_BaseVectorI);
					MATH_CrossProduct(&st_Ellipse.st_B, &_pst_World->st_Normal[i], &st_Ellipse.st_A);
				}
				else
				{
					MATH_CrossProduct(&st_Ellipse.st_A, &_pst_World->st_Normal[i], &MATH_gst_BaseVectorK);
					MATH_CrossProduct(&st_Ellipse.st_B, &_pst_World->st_Normal[i], &st_Ellipse.st_A);
				}
			}

			MATH_NormalizeEqualVector(&st_Ellipse.st_A);
			MATH_NormalizeEqualVector(&st_Ellipse.st_B);
			MATH_ScaleEqualVector(&st_Ellipse.st_A, _pst_World->af_Radius[i]);
			MATH_ScaleEqualVector(&st_Ellipse.st_B, _pst_World->af_Radius[i]);

			MATH_CopyVector((MATH_tdst_Vector *) &st_Ellipse.st_Center, &_pst_World->st_Center[i]);

			/* We dont want to draw the Alpha start point of the Ellipse ... */
			st_Ellipse.l_Flag |= 4;

			/* We dont want the "semi" circle effect. */
			st_Ellipse.l_Flag |= 1;

			/* Color */
			st_Ellipse.st_Center.color = _pst_World->aul_CircleColor[i];
            if (!_pst_World->ap_CircleGAO[i] || !_pst_World->ap_CircleGAO[i]->b_DontDisplayDebug)
    			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSoftEllipse, (ULONG) & st_Ellipse);
		}

		for(i = 0; i < _pst_World->uc_GeoForm; i++)
		{
            if (_pst_World->ap_GeoFormGAO[i] && _pst_World->ap_GeoFormGAO[i]->b_DontDisplayDebug)
                continue;

			switch(_pst_World->ai_GFType[i])
			{
			case WOR_Cte_DbgGFType_Sphere:
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					GDI_tdst_Request_DrawSphere st_Sphere;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					st_Sphere.pst_Center = &_pst_World->ast_GFPos[i];
					st_Sphere.f_Radius = _pst_World->af_GFVal1[i];
					st_Sphere.ul_SolidColor = _pst_World->aul_GFColor[i];
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawSphere, (ULONG) & st_Sphere);
				}
				break;
			case WOR_Cte_DbgGFType_Cone3D:
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					GDI_tdst_Request_DrawCone	st_Cone;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					st_Cone.pst_Pos = &_pst_World->ast_GFPos[i];
					st_Cone.pst_Axe = &_pst_World->ast_GFAxe1[i];
					st_Cone.ul_SolidColor = _pst_World->aul_GFColor[i];
					st_Cone.f_Alpha = _pst_World->af_GFVal1[i];
					st_Cone.ul_EdgeColor = _pst_World->aul_GFColor[i] | 0xFF000000;
					st_Cone.f_LineWidth = 2;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawCone, (ULONG) & st_Cone);
				}
				break;
			case WOR_Cte_DbgGFType_Cylinder:
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					GDI_tdst_Request_DrawCylinder	st_Cyl;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					st_Cyl.pst_Pos = &_pst_World->ast_GFPos[i];
					st_Cyl.pst_Axe = &_pst_World->ast_GFAxe1[i];
					st_Cyl.ul_SolidColor = _pst_World->aul_GFColor[i];
					st_Cyl.f_Radius = _pst_World->af_GFVal1[i];
					st_Cyl.ul_EdgeColor = _pst_World->aul_GFColor[i] | 0xFF000000;
					st_Cyl.f_LineWidth = 2;
					st_Cyl.ul_Flags = GDI_Request_DrawCylinder_Line;
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawCylinder, (ULONG) & st_Cyl);
				}
				break;
			case WOR_Cte_DbgGFType_2DRect:
				{
					V[0].x = V[3].x = _pst_World->ast_GFAxe1[i].x;
					V[0].y = V[1].y = _pst_World->ast_GFAxe1[i].y;
					V[1].x = V[2].x = _pst_World->ast_GFAxe2[i].x;
					V[2].y = V[3].y = _pst_World->ast_GFAxe2[i].y;
					
					for (j = 0; j < 4; j++)
					{
						V[ j ].x = _pst_DD->st_Camera.f_CenterX + (V[j].x - 0.5f) * _pst_DD->st_Camera.f_Width;
						V[ j ].y = _pst_DD->st_Camera.f_CenterY - (V[j].y - 0.5f) * _pst_DD->st_Camera.f_Height;
						V[ j ].z = 0.1f;
						CAM_2Dto3DCamera( &_pst_DD->st_Camera, &V[4], &V[j]);
						MATH_TransformVertex(&V[j], &_pst_DD->st_Camera.st_Matrix, &V[4]);
						st_Quad.V[j] = &V[j];
					}
					
					st_Quad.ul_Flags = GDI_Request_DrawQuad_Line | GDI_Request_DrawQuad_NoZWrite;
					st_Quad.ul_EdgeColor = _pst_World->aul_GFColor[i] | 0xFF000000;
					st_Quad.ul_SolidColor = _pst_World->aul_GFColor[i];
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawQuadEx, (ULONG) &st_Quad );
				}
				break;
			}
		}

		_pst_DD->ul_ColorConstant = ul_SaveColor;
	}

	/*$1- kilt (radiosity computing) ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GLV_DraweTheKilt(_pst_DD);

	/*$1- grid ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	PROPS2_StartRaster(&PROPS2_gst_GRI_Display);
	GRI_Display(_pst_World, _pst_World->pst_Grid, _pst_DD);
	GRI_Display(_pst_World, _pst_World->pst_Grid1, _pst_DD);
	PROPS2_StopRaster(&PROPS2_gst_GRI_Display);

	/*$1-PostIt ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < WOR_gul_AllPostIt; i++)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GDI_tdst_Request_DrawCylinder	st_Cyl;
		static MATH_tdst_Vector			axis;
		static MATH_tdst_Vector			pos;
		ULONG							col;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		col = 0x3FFFFFFF;
		if(WOR_gpt_AllPostIt[i]->ul_Flags & PIT_C_NoRead) col = 0x5F0000FF;

		axis.x = 0;
		axis.y = 0;
		axis.z = 0.8f;
		MATH_CopyVector(&pos, &WOR_gpt_AllPostIt[i]->st_Pos);
		pos.z += 0.35f;
		st_Cyl.pst_Pos = &pos;
		st_Cyl.pst_Axe = &axis;
		st_Cyl.ul_SolidColor = col;
		st_Cyl.f_Radius = 0.08f;
		st_Cyl.ul_EdgeColor = col;
		st_Cyl.f_LineWidth = 2;
		st_Cyl.ul_Flags = GDI_Request_DrawCylinder_Line;
		_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawCylinder, (ULONG) & st_Cyl);

		axis.x = 0;
		axis.y = 0;
		axis.z = 0.15f;
		MATH_CopyVector(&pos, &WOR_gpt_AllPostIt[i]->st_Pos);
		pos.z += 0.1f;
		st_Cyl.pst_Pos = &pos;
		st_Cyl.pst_Axe = &axis;
		st_Cyl.ul_SolidColor = col;
		st_Cyl.f_Radius = 0.1f;
		st_Cyl.ul_EdgeColor = col;
		st_Cyl.f_LineWidth = 2;
		st_Cyl.ul_Flags = GDI_Request_DrawCylinder_Line;
		_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawCylinder, (ULONG) & st_Cyl);
	}

	/*$1- secto ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_DD->st_Camera.pst_ObjectToCameraMatrix = _pst_DD->st_MatrixStack.pst_CurrentMatrix;
	_pst_DD->st_GDI.pfnv_SetViewMatrix(_pst_DD->st_Camera.pst_ObjectToCameraMatrix);

	st_Quad.ul_Flags = GDI_Request_DrawQuad_Line | GDI_Request_DrawQuad_NoZWrite | GDI_Request_DrawQuad_Normal;
	st_Quad.f_LineWidth = 2;
	_pst_DD->pst_Helpers->i_PortalNb = 0;

	for(pst_Secto = _pst_World->ast_AllSectos, i = 0; i < WOR_C_MaxSecto; i++, pst_Secto++)
	{
		if(!(pst_Secto->ul_Flags & WOR_CF_SectorValid)) continue;

		for(pst_Portal = pst_Secto->ast_Portals, j = 0; j < WOR_C_MaxSectoPortals; j++, pst_Portal++)
		{
			if(!(pst_Portal->uw_Flags & WOR_CF_PortalValid)) break;
			if(!(pst_Portal->uw_Flags & WOR_CF_PortalRender)) continue;

			if(pst_Portal->uw_Flags & WOR_CF_PortalPicked)
			{
				st_Quad.ul_SolidColor = 0x7FFFFF00;
				st_Quad.ul_EdgeColor = 0xFFFFFF00;
			}
			else if(pst_Portal->uw_Flags & WOR_CF_PortalPickable)
			{
				st_Quad.ul_SolidColor = 0x7F7F7F00;
				st_Quad.ul_EdgeColor = 0xFF7F7F00;
			}
			else
			{
				st_Quad.ul_SolidColor = 0x7FFF00FF;
				st_Quad.ul_EdgeColor = 0xFFFF00FF;
			}
#ifdef JADEFUSION
			if ((pst_Portal->uw_Flags & WOR_CF_PortalShare) && !GDI_b_IsXenonGraphics())
#else
			if(pst_Portal->uw_Flags & WOR_CF_PortalShare)
#endif
			{
				st_Quad.V[0] = &pst_Portal->vB;
				st_Quad.V[1] = &pst_Portal->vA;
			}
			else
			{
				st_Quad.V[0] = &pst_Portal->vA;
				st_Quad.V[1] = &pst_Portal->vB;
			}

			st_Quad.V[2] = &pst_Portal->vC;
			st_Quad.V[3] = &pst_Portal->vD;

			_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawQuadEx, (ULONG) & st_Quad);

			_pst_DD->pst_Helpers->apst_Portal[_pst_DD->pst_Helpers->i_PortalNb++] = pst_Portal;
		}
	}
}

#else /* ACTIVE_EDITORS */
#define WOR_RenderForEditors_Skel_Gizmo_Ray_Kilt_Grid(a, b)
#endif
#ifdef USE_DOUBLE_RENDERING

/*
 =======================================================================================================================
 =======================================================================================================================
 */
MATH_tdst_Matrix *WOR_GetSavedMatrixPtr(OBJ_tdst_GameObject *pst_GO)
{
	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_HasInitialPos))
	{
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
			return(pst_GO->pst_GlobalMatrix + 3);
		else
			return(pst_GO->pst_GlobalMatrix + 2);
	}
	else
	{
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
			return(pst_GO->pst_GlobalMatrix + 2);
		else
			return(pst_GO->pst_GlobalMatrix + 1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_CreateObjectHook(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*p_Supramat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * this function is used to set specific values in additionnnal matrix at creation
	 * to avoid interpolation
	 */
	p_Supramat = WOR_GetSavedMatrixPtr(pst_GO);
	*((ULONG *) p_Supramat) = 0xABADCAFF;
	*((ULONG *) (p_Supramat + 1)) = 0xABADCAFF;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_InterpoleMatrix_XX(MATH_tdst_Matrix *p_Dst, MATH_tdst_Matrix *p_Src1, MATH_tdst_Matrix *p_Src2, float fValue)
{
	MATH_BlendVector(MATH_pst_GetXAxis(p_Dst), MATH_pst_GetXAxis(p_Src1), MATH_pst_GetXAxis(p_Src2), fValue);
	MATH_BlendVector(MATH_pst_GetYAxis(p_Dst), MATH_pst_GetYAxis(p_Src1), MATH_pst_GetYAxis(p_Src2), fValue);
	MATH_BlendVector(MATH_pst_GetZAxis(p_Dst), MATH_pst_GetZAxis(p_Src1), MATH_pst_GetZAxis(p_Src2), fValue);
	MATH_NormalizeEqualVector(MATH_pst_GetXAxis(p_Dst));
	MATH_NormalizeEqualVector(MATH_pst_GetYAxis(p_Dst));
	MATH_NormalizeEqualVector(MATH_pst_GetZAxis(p_Dst));
	if((p_Src1->lType ^ p_Src2->lType) & MATH_Ci_Scale)
	{
		if(p_Src2->lType & MATH_Ci_Scale)
		{
			p_Src1->Sx = 1.0f;
			p_Src1->Sy = 1.0f;
			p_Src1->Sz = 1.0f;
		}
		else
		{
			p_Src2->Sx = 1.0f;
			p_Src2->Sy = 1.0f;
			p_Src2->Sz = 1.0f;
		}
	}

	MATH_BlendVector(&p_Dst->T, &p_Src1->T, &p_Src2->T, fValue);
	p_Dst->Sx = p_Src1->Sx + (p_Src2->Sx - p_Src1->Sx) * fValue;
	p_Dst->Sy = p_Src1->Sy + (p_Src2->Sy - p_Src1->Sy) * fValue;
	p_Dst->Sz = p_Src1->Sz + (p_Src2->Sz - p_Src1->Sz) * fValue;
	p_Dst->w = p_Src1->w + (p_Src2->w - p_Src1->w) * fValue;;
	p_Dst->lType = p_Src1->lType | p_Src2->lType;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
UINT WOR_DetectCameraCut(GDI_tdst_DisplayData *_pst_DD)
#else
u_int WOR_DetectCameraCut(GDI_tdst_DisplayData *_pst_DD)
#endif
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static MATH_tdst_Matrix		stSavedCameraMatrix;
	static float				fSaveSpeed;
	static MATH_tdst_Vector		stOld_Rotation;
	static MATH_tdst_Vector		stOld_Translation;
	MATH_tdst_Vector			stNew_Rotation;
	MATH_tdst_Vector			stNew_Translation;
	MATH_tdst_Vector			stLocal;
	MATH_tdst_Matrix			*p_Supramat;
	static OBJ_tdst_GameObject	*pst_OldFather;
	WOR_tdst_World				*_pst_World;
	u_int						RetVal;
	float						*CurrenFOV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_World = _pst_DD->pst_World;
	if(!_pst_World) return 0;

	p_Supramat = &_pst_DD->st_Camera.st_Matrix;
	CurrenFOV = &_pst_DD->st_Camera.f_FieldOfVision;

	if(_pst_World->pst_View)
	{
		p_Supramat = &_pst_World->pst_View->st_ViewPoint;
		CurrenFOV = &_pst_World->pst_View->st_DisplayInfo.f_FieldOfVision;
	}

#ifdef GSP_PS2_BENCH
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		extern ULONG	GLOBAL_LockCam;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(GLOBAL_LockCam)
		{
			MATH_CopyMatrix(p_Supramat, &stSavedCameraMatrix);
			return 0;
		}
	}
#endif
	RetVal = 0;
#define CAM_CUT_THRESHOLD	0.4f
	MATH_CrossProduct(&stNew_Rotation, MATH_pst_GetXAxis(p_Supramat), MATH_pst_GetXAxis(&stSavedCameraMatrix));
	MATH_CrossProduct(&stLocal, MATH_pst_GetYAxis(p_Supramat), MATH_pst_GetYAxis(&stSavedCameraMatrix));
	MATH_AddEqualVector(&stNew_Rotation, &stLocal);
	MATH_CrossProduct(&stLocal, MATH_pst_GetZAxis(p_Supramat), MATH_pst_GetZAxis(&stSavedCameraMatrix));
	MATH_AddEqualVector(&stNew_Rotation, &stLocal);

	if(_pst_World->pst_View->pst_Father != pst_OldFather) RetVal = 1;
	pst_OldFather = _pst_World->pst_View->pst_Father;

	if(MATH_f_Distance(&stNew_Rotation, &stOld_Rotation) > CAM_CUT_THRESHOLD) RetVal = 1;

	if
	(
		(MATH_f_DotProduct(MATH_pst_GetXAxis(p_Supramat), MATH_pst_GetXAxis(&stSavedCameraMatrix)) < 0.0f)
	||	(MATH_f_DotProduct(MATH_pst_GetYAxis(p_Supramat), MATH_pst_GetYAxis(&stSavedCameraMatrix)) < 0.0f)
	||	(MATH_f_DotProduct(MATH_pst_GetZAxis(p_Supramat), MATH_pst_GetZAxis(&stSavedCameraMatrix)) < 0.0f)
	) RetVal = 1;
	stOld_Rotation = stNew_Rotation;
	MATH_CopyMatrix(&stSavedCameraMatrix, p_Supramat);
	
	/* Philmippe KngKong : No more cinematic for the moment*/
	RetVal = 0;
	if(RetVal)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef PSX2_TARGET
		extern void GSP_SignalCameraCut(void);
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GSP_SignalCameraCut();
#endif
	}

	return RetVal;
}

void	GFX_Interpolate_All(GFX_tdst_List **, u_int, float);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_DoubleRenderingComputeOneGO(OBJ_tdst_GameObject *pst_GO, ULONG Mode, float fInterpoler)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*p_Supramat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Supramat = WOR_GetSavedMatrixPtr(pst_GO);

	if(!(pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled))
		WOR_InterpoleMatrix_XX(pst_GO->pst_GlobalMatrix, p_Supramat + 1, p_Supramat, fInterpoler);

	if(pst_GO->pst_Base)
	{
		if(pst_GO->pst_Base->pst_Hierarchy)
		{
			if(pst_GO->pst_Base->pst_Hierarchy->pst_Father->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible)
				WOR_DoubleRenderingComputeOneGO(pst_GO->pst_Base->pst_Hierarchy->pst_Father, Mode, fInterpoler);

			p_Supramat = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix_A;
			if(!(pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled))
			{
				WOR_InterpoleMatrix_XX
				(
					&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix,
					p_Supramat + 1,
					p_Supramat,
					fInterpoler
				);
			}

			OBJ_ComputeLocalWhenHie(pst_GO);
		}
	}

	if
	(
		(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	&&	pst_GO->pst_Base->pst_GameObjectAnim
	&&	pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
	)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		TAB_tdst_PFelem		*pst_Elem2, *pst_LastElem2;
		OBJ_tdst_GameObject *pst_GO2;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Elem2 = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
		pst_LastElem2 = TAB_pst_PFtable_GetLastElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
		for(; pst_Elem2 <= pst_LastElem2; pst_Elem2++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix	*p_Supramat2;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_GO2 = (OBJ_tdst_GameObject *) pst_Elem2->p_Pointer;
			if(TAB_b_IsAHole(pst_GO2)) continue;
			p_Supramat2 = WOR_GetSavedMatrixPtr(pst_GO2);

			if(!(pst_GO2->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled))
				WOR_InterpoleMatrix_XX(pst_GO2->pst_GlobalMatrix, p_Supramat2 + 1, p_Supramat2, fInterpoler);
			if(pst_GO2->pst_Base)
			{
				if(pst_GO2->pst_Base->pst_Hierarchy)
				{
					p_Supramat2 = &pst_GO2->pst_Base->pst_Hierarchy->st_LocalMatrix_A;
					if(!(pst_GO2->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled))
					{
						WOR_InterpoleMatrix_XX
						(
							&pst_GO2->pst_Base->pst_Hierarchy->st_LocalMatrix,
							p_Supramat2 + 1,
							p_Supramat2,
							fInterpoler
						);
					}
				}
			}

			MDF_InterpolateAll(pst_GO2, Mode, fInterpoler);
		}
	}

	MDF_InterpolateAll(pst_GO, Mode, fInterpoler);
}

static float	LastModeUsed = 0;
static float	fLastInterpolerUsed = 0.0f;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Compute_SetPos_In_Display_Hook(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*p_Supramat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Supramat = WOR_GetSavedMatrixPtr(pst_GO);

	/*
	 * This function is called AFTER the PosSet £
	 * p_Supramat = Matrix N £
	 * p_Supramat + 1 = Matrix N - 1
	 */
#ifdef WIN32
	if(MAI_gst_MainHandles.pst_DisplayData->ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_I)
#else
		if(gpst_GSP_stDD.ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_I)
#endif

		/* if (gpst_GSP_stDD.ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_I) */
		{
			/* Here, the current pos is considered as a wrong position */
			if(!LastModeUsed)
			{
				if(*(ULONG *) p_Supramat & 1) MATH_CopyMatrix(p_Supramat + 1, p_Supramat);
				MATH_CopyMatrix(p_Supramat, pst_GO->pst_GlobalMatrix);
			}

			WOR_InterpoleMatrix_XX(pst_GO->pst_GlobalMatrix, p_Supramat + 1, p_Supramat, fLastInterpolerUsed);
			MDF_InterpolateAll(pst_GO, LastModeUsed, fLastInterpolerUsed);
		}
		else
#ifdef WIN32
			if(MAI_gst_MainHandles.pst_DisplayData->ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_K)
#else
				if(gpst_GSP_stDD.ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_K)
#endif
				{
					MATH_CopyMatrix(pst_GO->pst_GlobalMatrix, p_Supramat);
					MDF_InterpolateAll(pst_GO, LastModeUsed, fLastInterpolerUsed);
				}

	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	{
		*(ULONG *) p_Supramat |= 1;
	}
	else
	{
		*(ULONG *) p_Supramat &= ~0xff;
		*(ULONG *) p_Supramat |= 0x14;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Compute_SetPos_Teleport(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*p_Supramat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Supramat = WOR_GetSavedMatrixPtr(pst_GO);
	(p_Supramat)->T = pst_GO->pst_GlobalMatrix->T;
	(p_Supramat + 1)->T = p_Supramat->T;
	if(pst_GO->pst_Base)
	{
		if(pst_GO->pst_Base->pst_Hierarchy)
		{
			p_Supramat = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix_A;
			(p_Supramat)->T = pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T;
			(p_Supramat + 1)->T = p_Supramat->T;
		}
	}

	* (ULONG *) p_Supramat |= 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_DoubleRenderingCompute(GDI_tdst_DisplayData *_pst_DD, ULONG Mode, float fInterpoler)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject		*pst_GO;
	WOR_tdst_World			*_pst_World;
	static MATH_tdst_Matrix stSaveCameraMatrix1;
	static MATH_tdst_Matrix stSaveCameraMatrix2;
	static float			FieldOFview1;
	static float			FieldOFview2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_DD->pst_World) return;

	fLastInterpolerUsed = fInterpoler;
	LastModeUsed = Mode;
	_GSP_BeginRaster(41);
	_pst_World = _pst_DD->pst_World;
	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects /* st_AllWorldObjects */ );
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects /* st_AllWorldObjects */ );
	if(Mode < 100)
	/* render I */
	{
		if(Mode == 0)
		{
			for(; pst_Elem <= pst_LastElem; pst_Elem++)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Matrix	*p_Supramat;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
				if(TAB_b_IsAHole(pst_GO)) continue;
				p_Supramat = WOR_GetSavedMatrixPtr(pst_GO);
				if((*(ULONG *) p_Supramat & 0xff) == 0x14) continue;	/* Display hook detection */

				if(*(ULONG *) p_Supramat == 0xABADCAFF)
				{
					MATH_CopyMatrix(p_Supramat, pst_GO->pst_GlobalMatrix);
					MATH_CopyMatrix(p_Supramat + 1, pst_GO->pst_GlobalMatrix);
				}

				MATH_CopyMatrix(p_Supramat + 1, p_Supramat);
				MATH_CopyMatrix(p_Supramat, pst_GO->pst_GlobalMatrix);
				*(ULONG *) p_Supramat |= 1;

				if(pst_GO->pst_Base)
				{
					if(pst_GO->pst_Base->pst_Hierarchy)
					{
						p_Supramat = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix_A;
						MATH_CopyMatrix(p_Supramat + 1, p_Supramat);
						MATH_CopyMatrix(p_Supramat, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
					}
				}

				if
				(
					(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
				&&	pst_GO->pst_Base->pst_GameObjectAnim
				&&	pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
				)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					TAB_tdst_PFelem		*pst_Elem2, *pst_LastElem2;
					OBJ_tdst_GameObject *pst_GO2;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_Elem2 = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
					pst_LastElem2 = TAB_pst_PFtable_GetLastElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
					for(; pst_Elem2 <= pst_LastElem2; pst_Elem2++)
					{
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
						MATH_tdst_Matrix	*p_Supramat2;
						/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

						pst_GO2 = (OBJ_tdst_GameObject *) pst_Elem2->p_Pointer;
						if(TAB_b_IsAHole(pst_GO2)) continue;
						p_Supramat2 = WOR_GetSavedMatrixPtr(pst_GO2);

						if(*(ULONG *) p_Supramat2 == 0xABADCAFF)
						{
							MATH_CopyMatrix(p_Supramat, pst_GO->pst_GlobalMatrix);
							MATH_CopyMatrix(p_Supramat + 1, pst_GO->pst_GlobalMatrix);
						}

						MATH_CopyMatrix(p_Supramat2 + 1, p_Supramat2);
						MATH_CopyMatrix(p_Supramat2, pst_GO2->pst_GlobalMatrix);
						if(pst_GO2->pst_Base)
						{
							if(pst_GO2->pst_Base->pst_Hierarchy)
							{
								p_Supramat2 = &pst_GO2->pst_Base->pst_Hierarchy->st_LocalMatrix_A;
								MATH_CopyMatrix(p_Supramat2 + 1, p_Supramat2);
								MATH_CopyMatrix(p_Supramat2, &pst_GO2->pst_Base->pst_Hierarchy->st_LocalMatrix);
							}
						}
					}
				}
			}
		}

		pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects /* st_AllWorldObjects */ );
		pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects /* st_AllWorldObjects */ );

		for(; pst_Elem <= pst_LastElem; pst_Elem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			WOR_DoubleRenderingComputeOneGO(pst_GO, Mode, fInterpoler);
		}
	}
	else
	/* render K */
	{
		for(; pst_Elem <= pst_LastElem; pst_Elem++)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Matrix	*p_Supramat;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			p_Supramat = WOR_GetSavedMatrixPtr(pst_GO);

			if((*(ULONG *) p_Supramat & 0xff) == 0x14) continue;		/* Display hook detection */
			if(Mode == 101)
			{
				MATH_CopyMatrix(p_Supramat, pst_GO->pst_GlobalMatrix);
				MATH_CopyMatrix(p_Supramat + 1, pst_GO->pst_GlobalMatrix);
			}
			else
				MATH_CopyMatrix(pst_GO->pst_GlobalMatrix, p_Supramat);
			*(ULONG *) p_Supramat |= 1;
			if(pst_GO->pst_Base)
			{
				if(pst_GO->pst_Base->pst_Hierarchy)
				{
					p_Supramat = &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix_A;
					if(Mode == 101)
					{
						MATH_CopyMatrix(p_Supramat, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
						MATH_CopyMatrix(p_Supramat + 1, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
					}
					else
					{
						MATH_CopyMatrix(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, p_Supramat);
					}
				}
			}

			if
			(
				(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
			&&	pst_GO->pst_Base->pst_GameObjectAnim
			&&	pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
			)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				TAB_tdst_PFelem		*pst_Elem2, *pst_LastElem2;
				OBJ_tdst_GameObject *pst_GO2;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_Elem2 = TAB_pst_PFtable_GetFirstElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
				pst_LastElem2 = TAB_pst_PFtable_GetLastElem(pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
				for(; pst_Elem2 <= pst_LastElem2; pst_Elem2++)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Matrix	*p_Supramat2;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_GO2 = (OBJ_tdst_GameObject *) pst_Elem2->p_Pointer;
					if(TAB_b_IsAHole(pst_GO2)) continue;
					p_Supramat2 = WOR_GetSavedMatrixPtr(pst_GO2);

					if(Mode == 101)
					{
						MATH_CopyMatrix(p_Supramat2, pst_GO2->pst_GlobalMatrix);
						MATH_CopyMatrix(p_Supramat2 + 1, pst_GO2->pst_GlobalMatrix);
					}
					else
						MATH_CopyMatrix(pst_GO2->pst_GlobalMatrix, p_Supramat2);
					if(pst_GO2->pst_Base)
					{
						if(pst_GO2->pst_Base->pst_Hierarchy)
						{
							p_Supramat2 = &pst_GO2->pst_Base->pst_Hierarchy->st_LocalMatrix_A;
							if(Mode == 101)
							{
								MATH_CopyMatrix(p_Supramat2, &pst_GO2->pst_Base->pst_Hierarchy->st_LocalMatrix);
								MATH_CopyMatrix(p_Supramat2 + 1, &pst_GO2->pst_Base->pst_Hierarchy->st_LocalMatrix);
							}
							else
								MATH_CopyMatrix(&pst_GO2->pst_Base->pst_Hierarchy->st_LocalMatrix, p_Supramat2);
						}
					}

					MDF_InterpolateAll(pst_GO2, Mode, fInterpoler);
				}
			}

			MDF_InterpolateAll(pst_GO, Mode, fInterpoler);
		}
	}
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	*p_Supramat;
		float				*CurrenFOV;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		p_Supramat = &_pst_DD->st_Camera.st_Matrix;
		CurrenFOV = &_pst_DD->st_Camera.f_FieldOfVision;

		if(_pst_World->pst_View)
		{
			p_Supramat = &_pst_World->pst_View->st_ViewPoint;
			CurrenFOV = &_pst_World->pst_View->st_DisplayInfo.f_FieldOfVision;
		}

		if(Mode < 100)
		/* Render I */
		{
			if(Mode == 0)
			{
				MATH_CopyMatrix(&stSaveCameraMatrix2, &stSaveCameraMatrix1);
				MATH_CopyMatrix(&stSaveCameraMatrix1, p_Supramat);
				FieldOFview2 = FieldOFview1;
				FieldOFview1 = *CurrenFOV;
			}

			WOR_InterpoleMatrix_XX(p_Supramat, &stSaveCameraMatrix2, &stSaveCameraMatrix1, fInterpoler);

			*CurrenFOV = FieldOFview2 + (FieldOFview1 - FieldOFview2) * fInterpoler;
		}
		else
			/* Render K */
		if(Mode == 101)
		{
			MATH_CopyMatrix(&stSaveCameraMatrix2, p_Supramat);
			MATH_CopyMatrix(&stSaveCameraMatrix1, p_Supramat);
			FieldOFview2 = FieldOFview1 = *CurrenFOV;
		}
		else
		{
			MATH_CopyMatrix(p_Supramat, &stSaveCameraMatrix1);
			*CurrenFOV = FieldOFview1;
		}

		_pst_DD->st_Camera.f_FieldOfVision = *CurrenFOV;
	}

	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects /* st_AllWorldObjects */ );
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects /* st_AllWorldObjects */ );
	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	*p_Supramat;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		p_Supramat = WOR_GetSavedMatrixPtr(pst_GO);
		*(ULONG *) p_Supramat |= 1;
	}

	GFX_Interpolate_All(&_pst_World->pst_GFX, Mode, fInterpoler);

	_GSP_EndRaster(41);
}
#else // Double rendering
UINT WOR_DetectCameraCut(GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	static MATH_tdst_Matrix		stSavedCameraMatrix;
	static float				fSaveSpeed;
	static MATH_tdst_Vector		stOld_Rotation;
	static MATH_tdst_Vector		stOld_Translation;
	MATH_tdst_Vector			stNew_Rotation;
	MATH_tdst_Vector			stLocal;
	MATH_tdst_Matrix			*p_Supramat;
	static OBJ_tdst_GameObject	*pst_OldFather;
	WOR_tdst_World				*_pst_World;
	UINT						RetVal;
	float						*CurrenFOV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_World = _pst_DD->pst_World;
	if(!_pst_World) return 0;

	p_Supramat = &_pst_DD->st_Camera.st_Matrix;
	CurrenFOV = &_pst_DD->st_Camera.f_FieldOfVision;

	if(_pst_World->pst_View)
	{
		p_Supramat = &_pst_World->pst_View->st_ViewPoint;
		CurrenFOV = &_pst_World->pst_View->st_DisplayInfo.f_FieldOfVision;
	}

	RetVal = 0;
#define CAM_CUT_THRESHOLD	0.4f
	MATH_CrossProduct(&stNew_Rotation, MATH_pst_GetXAxis(p_Supramat), MATH_pst_GetXAxis(&stSavedCameraMatrix));
	MATH_CrossProduct(&stLocal, MATH_pst_GetYAxis(p_Supramat), MATH_pst_GetYAxis(&stSavedCameraMatrix));
	MATH_AddEqualVector(&stNew_Rotation, &stLocal);
	MATH_CrossProduct(&stLocal, MATH_pst_GetZAxis(p_Supramat), MATH_pst_GetZAxis(&stSavedCameraMatrix));
	MATH_AddEqualVector(&stNew_Rotation, &stLocal);

	if(_pst_World->pst_View->pst_Father != pst_OldFather) RetVal = 1;
	pst_OldFather = _pst_World->pst_View->pst_Father;

	if(MATH_f_Distance(&stNew_Rotation, &stOld_Rotation) > CAM_CUT_THRESHOLD) RetVal = 1;

	if
	(
		(MATH_f_DotProduct(MATH_pst_GetXAxis(p_Supramat), MATH_pst_GetXAxis(&stSavedCameraMatrix)) < 0.0f)
	||	(MATH_f_DotProduct(MATH_pst_GetYAxis(p_Supramat), MATH_pst_GetYAxis(&stSavedCameraMatrix)) < 0.0f)
	||	(MATH_f_DotProduct(MATH_pst_GetZAxis(p_Supramat), MATH_pst_GetZAxis(&stSavedCameraMatrix)) < 0.0f)
	) RetVal = 1;
	stOld_Rotation = stNew_Rotation;
	MATH_CopyMatrix(&stSavedCameraMatrix, p_Supramat);
	return RetVal;
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *st_LastGOStack[8];
u32					ul_LastGOStackNumber = 0;

void WOR_Render_One_GO(WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *pst_GO)
{
#ifdef ACTIVE_EDITORS
		ERR_gpst_ContextGAO = pst_GO;
#endif

#ifdef JADEFUSION
		if((pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled)
#ifdef _XENON_RENDER
            // Let the object continue down the pipeline if it emits shadows.
            // We'll skip it if we need to later.
            && !(pst_GO->pst_Base && pst_GO->pst_Base->pst_Visu && (pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_EmitShadowBuffer))

#ifdef ACTIVE_EDITORS
            || (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden)
#endif
#endif
			)
#else // JADEFUSION
		if (pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Culled)
#endif
		{
#if defined(PSX2_TARGET) || defined(_GAMECUBE) || defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL) || defined(_PC_RETAIL)
			if(!(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_I))
			{
				AI_ExecCallback(pst_GO, AI_C_Callback_AfterBlend);
			}

#elif defined(_XBOX) || defined(_XENON)
			if
			(
				!(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)
			&&	!(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingSpotSwadows)
			&&	!(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingDiffuse)
			&&	!(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_I)
			)
			{
				AI_ExecCallback(pst_GO, AI_C_Callback_AfterBlend);
			}
#endif
			if
			(
				pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims
			&&	pst_GO->pst_Base
			&&	pst_GO->pst_Base->pst_GameObjectAnim
			)
				pst_GO->pst_Base->pst_GameObjectAnim->uc_Signal = 0;

#ifdef ACTIVE_EDITORS
            if (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
                WOR_RenderForEditors_Way_BV_Light_Invisible_Zone(_pst_World, _pst_DD, pst_GO);
#endif // ACTIVE_EDITORS

			return;
		}

#ifdef Active_CloneListe
#ifdef PSX2_TARGET	
		_pst_DD->ul_DrawMask &= ~GDI_Cul_DM_NoAutoClone;
		//_pst_DD->ul_DrawMask |= GDI_Cul_DM_NoAutoClone;
#endif
		if ( !(_pst_DD->ul_DrawMask & GDI_Cul_DM_NoAutoClone) && 
            !(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_ShowCOB ) && 
            (_pst_DD->ul_DrawMask & GDI_Cul_DM_NotWired))
		{
			//ICI POUR L'INSTANT a DEPLACER ??
			OBJ_GameObject_UpdateCloneListe();

			if (pst_GO->p_CloneNextGao )
				renderState_Cloned = 1;
			else renderState_Cloned =0;
		}
		else 
			renderState_Cloned =0;

#endif


		/* Modifier */
#if !defined(_XBOX) && !defined(_XENON)
		PROPS2_StartRaster(&PROPS2_gst_MDF_ApplyAllGao);
		MDF_ApplyAllGao(pst_GO);
		PROPS2_StopRaster(&PROPS2_gst_MDF_ApplyAllGao);

#else
		if(!(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingDiffuse))
		{
			PROPS2_StartRaster(&PROPS2_gst_MDF_ApplyAllGao);
			MDF_ApplyAllGao(pst_GO);
			PROPS2_StopRaster(&PROPS2_gst_MDF_ApplyAllGao);
		}
#endif

		/* Hierarchy */
		if
		(
			(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		&&	(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_SharedMatrix))
		) OBJ_ComputeGlobalWhenHie(pst_GO);

		/*
		 * shade selected £
		 * M4Edit_ShadeSelected();
		 */
#if defined(_XBOX) || (defined(_XENON) && defined(_GX8))
		{
			/*~~~~~~~~~~~~~*/
			bool	b_Culled;
			/*~~~~~~~~~~~~~*/

			if(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_ComputeShadowMap)
			{
#ifdef JADEFUSION
				b_Culled = OBJ_CullingObject(pst_GO, &_pst_DD->st_Camera);
#else		
				b_Culled = OBJ_CullingObject(pst_GO, &_pst_DD->st_Camera) != FALSE;
#endif		
				if(!b_Culled)
				{
					if(WOR_CheckIfRecShadows(pst_GO))
					{
						/* This object has not being culled */
						Gx8_SaveObjectInCurrentShadowBuffer((void *) pst_GO);
					}

					/* If don't cast shadow...don't render in the shdow buffer */
					if(!WOR_CheckIfCasShadows(pst_GO)) return;
				}
			}

			if(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingSpotSwadows)
			{
				if(!Gx8_IsObjectVisibleByCurrentShadowBuffer((void *) pst_GO))
				{
					return;
				}
			}

			if(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingDiffuse)
			{
				if(!Gx8_IsObjectVisibleByShadowBuffer((void *) pst_GO))
				{
					return;
				}
			}

			if(Gx8_IsDiffuseInTexture())
			{
				if(_pst_DD->ul_CurrentDrawMask & GDI_Cul_DM_Lighted)
				{
					if(Gx8_IsObjectVisibleByShadowBuffer((void *) pst_GO))
					{
						Gx8_SetDiffuseInTextureRendering(true);
					}
					else
					{
						Gx8_SetDiffuseInTextureRendering(false);
					}
				}
				else
					Gx8_SetDiffuseInTextureRendering(false);
			}
			else
				Gx8_SetDiffuseInTextureRendering(false);
		}
#endif
		if(WOR_b_MustRenderVisuGO(pst_GO, _pst_DD))
		{
			if(pst_GO->pst_Base && pst_GO->pst_Base->pst_Visu)
			{
				M4Edit_AddBonesAndContinue(pst_GO);

				if(pst_GO->pst_Base->pst_Visu->c_DisplayPos == 0)
				{
					_pst_DD->pst_AdditionalMaterial = pst_GO->pst_Base->pst_AddMaterial;
					PROPS2_StartRaster(&PROPS2_gst_GRO_Render);
#ifdef ACTIVE_EDITORS
					if(!(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject))
#endif
						GRO_Render(pst_GO);

					PROPS2_StopRaster(&PROPS2_gst_GRO_Render);
				}
			}
		}
		else if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims))
		{
			if(pst_GO->pst_Base && pst_GO->pst_Base->pst_GameObjectAnim)
			{
				_pst_DD->pst_AdditionalMaterial = pst_GO->pst_Base->pst_AddMaterial;
				PROPS2_StartRaster(&PROPS2_gst_ANI_Render);

#ifdef _XBOX
				if(pst_GO->playSkinning != _pst_DD->ul_RenderingCounter)
				{
					ANI_Render(pst_GO);
					PROPS2_StopRaster(&PROPS2_gst_ANI_Render);
					M4Edit_AddSkeleton(pst_GO);

					/* IMPORTANT..THIS HAVE TO BE "AFTER" THE ANI_RENDER CALL!!!!!!!!!!!!!!!!!!!!!!!!!! */
					pst_GO->playSkinning = _pst_DD->ul_RenderingCounter;
				}

				/* else { ANI_Render_XB(pst_GO,false); } */
#else
				ANI_Render(pst_GO);
				PROPS2_StopRaster(&PROPS2_gst_ANI_Render);
				M4Edit_AddSkeleton(pst_GO);
#endif
			}
		}
		else if( OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_ExtendedObject) &&
                 pst_GO->pst_Extended != NULL &&
                 pst_GO->pst_Extended->pst_Modifiers != NULL )
        {
            // GAOs with modifiers may need matrix refresh
            GAO_Render(pst_GO);
        }

		/* Modifier */
#if !defined(_XBOX) && !defined(_XENON)
		PROPS2_StartRaster(&PROPS2_gst_MDF_UnApplyAllGao);
		MDF_UnApplyAllGao(pst_GO);
		PROPS2_StopRaster(&PROPS2_gst_MDF_UnApplyAllGao);
#else
		if(!(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingDiffuse))
		{
			PROPS2_StartRaster(&PROPS2_gst_MDF_UnApplyAllGao);
			MDF_UnApplyAllGao(pst_GO);
			PROPS2_StopRaster(&PROPS2_gst_MDF_UnApplyAllGao);
		}
#endif
		_pst_DD->pst_AdditionalMaterial = NULL;

		_pst_DD->ul_DisplayFlags &= ~GDI_Cul_DF_ShowCurrentBV;
		_pst_DD->ul_DisplayFlags &= ~GDI_Cul_DF_ShowCurrentCOB;

		WOR_RenderForEditors_Way_BV_Light_Invisible_Zone(_pst_World, _pst_DD, pst_GO);

#if defined(_XENON_RENDER)

        // Backup the camera transform needed for deferred shadowing
        MATH_tdst_Matrix temp;
        MATH_MakeOGLMatrix(&temp, &GDI_gpst_CurDD->st_Camera.st_Matrix);
        *(g_pXeContextManagerEngine->GetCameraMatrix()) = *(D3DXMATRIX*)(&temp);

#if defined(ACTIVE_EDITORS)
        if(!(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_DisplayLight))
#endif
        {
            if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Lights))
                GRO_RenderGro(pst_GO, pst_GO->pst_Extended->pst_Light);
        }
#endif

#if !defined(_XBOX) && !defined(_XENON)

		/*$1- Gizmo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		OBJ_Gizmo_Update(pst_GO);

		/*$1- trace ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		GRO_RenderTrace(pst_GO);
#else
		if(!(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_RenderingDiffuse))
		{
			OBJ_Gizmo_Update(pst_GO);
			GRO_RenderTrace(pst_GO);
		}
#endif
#ifdef Active_CloneListe
	//desactive toute la chaine de clone deja rendu
#ifndef PSX2_TARGET	
	if ( !(_pst_DD->ul_DrawMask & GDI_Cul_DM_NoAutoClone) )
#endif	
	if ( pst_GO->p_CloneNextGao )
	{
		OBJ_tdst_GameObject *_pst_GOChaine;
		_pst_GOChaine = pst_GO; 
		while (_pst_GOChaine)
		{
			if (_pst_GOChaine->ulMoreCloneInfo & 1) // <- Has Been Drawn
			{
				_pst_GOChaine->ulMoreCloneInfo &=~1;
				_pst_GOChaine->ul_StatusAndControlFlags |= OBJ_C_StatusFlag_Culled;//*/
			}
			_pst_GOChaine=_pst_GOChaine->p_CloneNextGao;
			if (_pst_GOChaine == pst_GO) _pst_GOChaine = NULL;
		}
	}
	renderState_Cloned = 0;
#endif
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
u32 WOR_MUST_BE_SWAP(OBJ_tdst_GameObject *pA,OBJ_tdst_GameObject *pB)
{
	if (TAB_b_IsAHole(pA) || TAB_b_IsAHole(pB)) return 0;

	if (OBJ_b_TestIdentityFlag(pA, OBJ_C_IdentityFlag_Visu))
	{
		if (OBJ_b_TestIdentityFlag(pB, OBJ_C_IdentityFlag_Visu))
		{
			if ((u32)pA->pst_Base->pst_Visu->c_DisplayOrder > (u32)pB->pst_Base->pst_Visu->c_DisplayOrder)
			{
				return 1;
			} 
		} else return 1;
	} 
	return 0;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Render_All_GO(WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_GO = NULL;
	TAB_tdst_PFelem		*pst_LasPreviousElem;
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
		extern ULONG ShowNormals;
		ULONG SaveShowNormals;
#endif
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	ERR_gpst_ContextGAO = NULL;
	ERR_gpsz_ContextString = "Render pass 1";
#endif
	_GSP_BeginRaster(41);

#if defined(_XENON_RENDER) && defined(ACTIVE_EDITORS)
    if( _pst_DD->bLogLightUsage )
    {
        // Put the log in the same folder as a frame capture
        char * pc_Cur = NULL;
        char szLogFileName[256];

        strcpy( szLogFileName, _pst_DD->sz_SnapshotName );

        pc_Cur = strrchr( szLogFileName, '/' );
        if (pc_Cur == NULL)
            pc_Cur = strrchr( szLogFileName, '\\' );
        if (pc_Cur == NULL)
            L_strcpy(szLogFileName, "light_usage_log.txt");
        else
        {
            *(pc_Cur+1) = 0;
            CreateDirectory( szLogFileName, NULL );
            strcpy( pc_Cur + 1, "light_usage_log.txt" );
        }

        g_oXeLightUsageLogger.BeginLog( szLogFileName, _pst_World->sz_Name );
    }
#endif

	/* Render GameObject */
	if(!(_pst_DD->ul_DisplayInfo & GDI_Cul_DI_DoubleRendering_I))
	{
		pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
		pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);
		for(; pst_Elem <= pst_LastElem; pst_Elem++)
		{
			/* get and test game object */
			pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			
			OBJ_M_EdAddFlags_RefreshBeforeDisplay( pst_GO );

#ifdef ACTIVE_EDITORS
			ERR_gpst_ContextGAO = pst_GO;
#endif
			pst_GO->ul_StatusAndControlFlags |= OBJ_C_StatusFlag_Culled;

			/* compute culling */
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	st_Dist;
				float				f_Dist;
				BOOL				b_Culled;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(M4Edit_IfNotLinks)
				{
					/* Distance camera / object */
					MATH_SubVector(&st_Dist, &pst_GO->pst_GlobalMatrix->T, &_pst_World->pst_View->st_ViewPoint.T);
					f_Dist = MATH_f_SqrNormVector(&st_Dist);

					b_Culled = OBJ_CullingObject(pst_GO, &_pst_DD->st_Camera);
					if(!OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_AlwaysVisible) && b_Culled)
					{
						pst_GO->uc_LOD_Vis = 0;
						if(M4Edit_IfEngineRender) continue;
					}
					else
					{
						pst_GO->uc_LOD_Vis = OBJ_uc_CalcLODVis
							(
								pst_GO,
								_pst_World->pst_View,
#ifdef JADEFUSION
								0.0f // not used in the function : MATH_f_SqrNormVector(&st_Dist)
#else
								MATH_f_SqrNormVector(&st_Dist)
#endif
							);
					}
				}
			}

			if(M4Edit_IfEngineRender)
			{	/* Hierarchy */
				if
				(
					(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
				&&	(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_SharedMatrix))
				) OBJ_ComputeGlobalWhenHie(pst_GO);
			}

#ifdef ACTIVE_EDITORS
			if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Hidden)
			{
				if
				(
					pst_GO->ul_IdentityFlags &
						(
							OBJ_C_IdentityFlag_ZDM |
							OBJ_C_IdentityFlag_ZDE |
							OBJ_C_IdentityFlag_ColMap |
							OBJ_C_IdentityFlag_Sound |
#ifdef ODE_INSIDE
							OBJ_C_IdentityFlag_ODE |
#endif
							OBJ_C_IdentityFlag_ExtendedObject
						)
				)
				{
					GEO_Zone_Display(_pst_DD, pst_GO);
				}

				continue;
			}
#endif
#ifdef PSX2_TARGET
			if(!OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_AlwaysVisible))
				if (pst_GO->uc_LOD_Vis < 5) continue;
#endif
			if (pst_GO->ucCullingVisibility > pst_GO->uc_LOD_Vis) continue;

			pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_StatusFlag_Culled;
		}
	}

	_GSP_EndRaster(41);

	/* : : */
#ifdef ACTIVE_EDITORS
	ERR_gpsz_ContextString = "Render pass 2";
#endif

#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
	if (ShowNormals) 
	{
		extern u32 ColorCost_GlobalTime_IA;
		extern u32 ColorCost_MaxTime_IA;

		extern u32 ColorCost_GlobalTime_GRAPHIC;
		extern u32 ColorCost_MaxTime_GRAPHIC;

	
		ULONG MAXTICK_E,MAXTICK_G,MAXTICK_GL;
		
		ColorCost_GlobalTime_IA = ColorCost_GlobalTime_GRAPHIC = 0;
		MAXTICK_E = MAXTICK_G = 0;
		/* 1 - Compute max GRAPHIC */
		pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);
		pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
		for(; pst_Elem <= pst_LastElem; pst_Elem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			ColorCost_GlobalTime_GRAPHIC += pst_GO->DrawTick;
			ColorCost_MaxTime_GRAPHIC = MAXTICK_G = lMax(MAXTICK_G , pst_GO->DrawTick);
			pst_GO->NumberOfTris = 0;
		}
		/* 2 - Compute max AI */
		pst_LastElem = TAB_pst_PFtable_GetLastElem(&(_pst_World->st_EOT.st_AI));
		pst_Elem = TAB_pst_PFtable_GetFirstElem(&(_pst_World->st_EOT.st_AI));
		for(; pst_Elem <= pst_LastElem; pst_Elem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			ColorCost_GlobalTime_IA += pst_GO->EngineTick;
			ColorCost_MaxTime_IA = MAXTICK_E = lMax(MAXTICK_E , pst_GO->EngineTick);
		}
		if (!MAXTICK_E) MAXTICK_E = 1;
		if (!MAXTICK_G) MAXTICK_G = 1;
		MAXTICK_GL = MAXTICK_E + MAXTICK_G;
		/* 2 - Compute pst_GO->LastDrawTick */
		pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);
		pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
		for(; pst_Elem <= pst_LastElem; pst_Elem++)
		{
			ULONG Results;
			pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
#define COLORCOST_BLENDER 2
			Results = pst_GO->LastDrawTick;
			Results -= Results >> COLORCOST_BLENDER;
			Results += ((pst_GO->DrawTick << 8) / MAXTICK_G)>>COLORCOST_BLENDER;
			pst_GO->LastDrawTick = lMin(Results , 255);
			pst_GO->DrawTick = 0;
		}
		/* 2 - Compute pst_GO->LastEngineTick */
		pst_LastElem = TAB_pst_PFtable_GetLastElem(&(_pst_World->st_EOT.st_AI));
		pst_Elem = TAB_pst_PFtable_GetFirstElem(&(_pst_World->st_EOT.st_AI));
		for(; pst_Elem <= pst_LastElem; pst_Elem++)
		{
			ULONG Results;
			pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			Results = pst_GO->LastEngineTick;
			Results -= Results >> COLORCOST_BLENDER;
			Results += ((pst_GO->EngineTick << 8) / MAXTICK_E)>>COLORCOST_BLENDER;
			pst_GO->LastEngineTick = lMin(Results , 255);
			pst_GO->EngineTick = 0;
		}
	}
#endif
#endif

	pst_LasPreviousElem = NULL;
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
	SaveShowNormals = ShowNormals;
#endif
#endif
	ul_LastGOStackNumber = 0;

	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);
	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
		extern u8	CurrentTicksInGlobals;
		extern u8	CurrentTicksInDraw;
		extern u8	CurrentTicksInEngine;
		ULONG 		CurrentTicksNum;
		u32			CurrentNumberOfTris;
		static ULONG LASTMAXTICK = 0;
#endif
#endif
#ifdef ACTIVE_EDITORS
		u32			CurrentNumberOfTris;
		extern u32 Stats_ulNumberOfTRiangles;
#endif
		/* DJ TEMP */
		pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);

		pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;

		if(TAB_b_IsAHole(pst_GO)) continue;

		/* Philippe BEGIN */
		/* Philippe Special draw for JAKE */
		/* Avoid to pass throught the walls.*/
//		if ( (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_DrawAtEnd)/* || (pst_GO->ul_MyKey == 0x72006AB4 /* _pnj_Jack */) )
		if (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_DrawAtEnd)
		{
			// Do the spacial thing 
			pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_DrawAtEnd;
			if (ul_LastGOStackNumber < 8)
			{
				st_LastGOStack[ul_LastGOStackNumber++] = pst_GO;
				continue;
			}
		}//*/
		/* Philippe END */
#ifdef ACTIVE_EDITORS
		CurrentNumberOfTris = Stats_ulNumberOfTRiangles;
#endif		
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
		ShowNormals = SaveShowNormals;
		if (ShowNormals) 
		{
			extern void ColorCost_AE_AddAnObject(OBJ_tdst_GameObject *p_GO);
			CurrentTicksInDraw = pst_GO->LastDrawTick;
			CurrentTicksInEngine = pst_GO->LastEngineTick;
			CurrentTicksInGlobals = lMin(pst_GO->LastEngineTick + pst_GO->LastDrawTick , 255);
			CurrentTicksNum = scePcGetCounter0(); // Not closed
			CurrentNumberOfTris = GspGlobal_ACCESS(Tnum);
			if ((ShowNormals & 3) == 3) // Global
			{
				if (CurrentTicksInGlobals < 100) ShowNormals = 0;
				else ColorCost_AE_AddAnObject(pst_GO);
			} else 
			if ((ShowNormals & 3) == 2) // Draw
			{
				if (CurrentTicksInGlobals < 50) ShowNormals = 0;
				else ColorCost_AE_AddAnObject(pst_GO);
			} else 
			if ((ShowNormals & 3) == 1) // Engine
			{
				if (CurrentTicksInEngine < 10) ShowNormals = 0;
				else ColorCost_AE_AddAnObject(pst_GO);
			}
		}
#endif
#endif
		
		WOR_Render_One_GO(_pst_World, _pst_DD, pst_GO);
#ifdef ACTIVE_EDITORS
		if(OBJ_gb_DebugPhotoMode) WOR_RenderFakePhotoFrame(_pst_DD);
		pst_GO->NumberOfTris = Stats_ulNumberOfTRiangles - CurrentNumberOfTris;
#endif

#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
		ShowNormals = SaveShowNormals;
		if (ShowNormals) 
		{
			CurrentTicksNum = scePcGetCounter0() - CurrentTicksNum; // Not closed
			pst_GO->DrawTick += CurrentTicksNum;
			pst_GO->NumberOfTris = GspGlobal_ACCESS(Tnum) - CurrentNumberOfTris;		
		}
#endif
#endif
/*		if (pst_LasPreviousElem)
		{
			if (WOR_MUST_BE_SWAP((OBJ_tdst_GameObject *) pst_LasPreviousElem->p_Pointer,pst_GO))
			{
				TAB_tdst_PFelem		SWAP;
				SWAP = *pst_LasPreviousElem;
				*pst_LasPreviousElem = *pst_Elem;
				*pst_Elem = SWAP;
			}
			
		}
		pst_LasPreviousElem = pst_Elem;//*/
	}
#ifdef PSX2_TARGET
#ifdef GSP_PS2_BENCH
		ShowNormals = SaveShowNormals;
#endif
#endif
#if defined(_XENON_RENDER) && defined(ACTIVE_EDITORS)
    if( _pst_DD->bLogLightUsage )
    {
        g_oXeLightUsageLogger.EndLog( );
        _pst_DD->bLogLightUsage = false;
    }
#endif	
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Render_3D(int iNumView, WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD)
{
	/* We update the culling variables. */
	CAM_Engine_ComputePlans(&_pst_DD->st_Camera);
	OBJ_UpdateCullingVars(&_pst_DD->st_Camera);

	/*
	 * clear the zbuffer (we clear before each view except first one in engine, and
	 * the 4 first ont in editor
	 */
	M_Clear4View(iNumView);

	/* Shadows */
#if defined(_XBOX)
	if(!((Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData)->bShadowBuffer)
#endif
		SDW_UpdatePositions();

	/* Earth,Wind & fire */
	PROTEX_BeforeDraw();

	ul_LastGOStackNumber = 0;

	WATER3D_BeforeDraw();

	/* Render objects */
	WOR_Render_All_GO(_pst_World, _pst_DD);

#ifdef _GAMECUBE
#ifdef USE_HARDWARE_LIGHTS
	GXI_Global_ACCESS(LightMask) = GX_LIGHT_NULL;
#endif
#endif

	/* Graphic FX */
	GFX_Render(&_pst_World->pst_GFX, 0);

	WOR_RenderForEditors_Skel_Gizmo_Ray_Kilt_Grid(_pst_World, _pst_DD);
	M4Edit_ResetLightComputeRLI();

	/*$1- Z list ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	PROPS2_StartRaster(&PROPS2_gst_SOFT_ZList_Send);
#ifdef _GAMECUBE
#ifdef USE_HARDWARE_LIGHTS
	GXI_Global_ACCESS(LightMask) = GX_LIGHT_NULL;
#endif
#endif
	SOFT_ZList_Send();

	if (ul_LastGOStackNumber)
	{
		float PushValue;
		PushValue = 5.0f;
		GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PushZBuffer, *(u32 *) &PushValue);
		while (ul_LastGOStackNumber--)
		{
			WOR_Render_One_GO(_pst_World, _pst_DD, st_LastGOStack[ul_LastGOStackNumber]);
		}
		SOFT_ZList_Send();
		ul_LastGOStackNumber=0;
	}

	GAO_ModifierLazy_UnapplyAll();
	MODIFIER_SPG2_OneFrameCall();
	MODIFIER_FOGDYN_OneFrameCall();
}

#if defined(_XBOX)

/*
 =======================================================================================================================
    function used only for the cheat code (can be deleted)
 =======================================================================================================================
 */
void WOR_GetLookAtMatrix(MATH_tdst_Matrix *p_LookAtMatrix, MATH_tdst_Matrix *p_LightMatrix, MATH_tdst_Vector *p_Target)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Sight;
	MATH_tdst_Vector	st_Up;
	MATH_tdst_Vector	st_XVector;
	MATH_tdst_Vector	st_YVector;
	MATH_tdst_Vector	st_Translation, st_Distance;
	float				fDist;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SetIdentityMatrix(p_LookAtMatrix);

	MATH_SubVector(&st_Sight, MATH_pst_GetTranslation(p_LightMatrix), p_Target);

	MATH_NormalizeVector(&st_Sight, &st_Sight);
	MATH_CopyVector(MATH_pst_GetZAxis(p_LookAtMatrix), &st_Sight);
	MATH_InitVector(&st_Up, 0, 1, 0);
	MATH_CrossProduct(&st_XVector, &st_Up, &st_Sight);
	MATH_NormalizeVector(&st_XVector, &st_XVector);
	MATH_CopyVector(MATH_pst_GetXAxis(p_LookAtMatrix), &st_XVector);
	MATH_CrossProduct(&st_YVector, &st_XVector, &st_Sight);
	MATH_CopyVector(MATH_pst_GetYAxis(p_LookAtMatrix), &st_YVector);

	/* Compute Translation */
	MATH_CopyVector(&st_Translation, MATH_pst_GetTranslation(p_LightMatrix));
	MATH_SubVector(&st_Distance, p_Target, &st_Translation);
	fDist = MATH_f_Distance(&st_Translation, p_Target);

	/* fMult = fDist; */
	MATH_SetTranslation(p_LookAtMatrix, &st_Translation);

	MATH_RotateMatrix_AroundLocalXAxis(p_LookAtMatrix, 3.14f / 2.0f);
}

/*
 =======================================================================================================================
    function used only for the cheat code (can be deleted)
 =======================================================================================================================
 */
void WOR_BuildSBLightMatrix(Gx8_tdst_SpecificData *pst_SD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	vLightSource;
	MATH_tdst_Vector	vLightTarget;
	MATH_tdst_Matrix	mTmp;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < pst_SD->iNOfSBLights; i++)
	{
		MATH_CopyVector(&vLightTarget, OBJ_pst_GetAbsolutePosition(AI_EvalFunc_AIMainActorGet_C(0)));

		switch(i)
		{
		case 1:		vLightTarget.x += 3; break;
		case 2:		vLightTarget.x -= 3; break;
		case 3:		vLightTarget.y += 3; break;
		case 4:		vLightTarget.y -= 3; break;
		default:	break;
		}

		vLightSource.x = vLightTarget.x;
		vLightSource.y = vLightTarget.y + 2;
		vLightSource.z = vLightTarget.z + 3;

		MATH_SetIdentityMatrix(&mTmp);
		MATH_SetTranslation(&mTmp, &vLightSource);

		WOR_GetLookAtMatrix(&pst_SD->mLightMatrix[i], &mTmp, &vLightTarget);
	}
}

/*
 =======================================================================================================================
    this function checks if some of the active lights use shadow buffer
 =======================================================================================================================
 */
void WOR_CheckForShadowBuffer(GDI_tdst_DisplayData *_pst_DD, Gx8_tdst_SpecificData *pst_SD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject **ppst_LightNode;
	int					i;
	LIGHT_tdst_Light	*LIGHT_gpst_Cur;
	extern bool			bShadowBufferCheat;
	extern int			iNOfShadowBufferCheat;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ppst_LightNode = _pst_DD->st_LightList.dpst_Light;

	if(bShadowBufferCheat)
	{
		pst_SD->iNOfSBLights = iNOfShadowBufferCheat;
		pst_SD->bShadowBuffer = true;

		WOR_BuildSBLightMatrix(pst_SD);
	}
	else
	{
		pst_SD->iNOfSBLights = 0;
		pst_SD->bShadowBuffer = FALSE;

		for(i = 0; i < (int) _pst_DD->st_LightList.ul_Current; i++)
		{
			LIGHT_gpst_Cur = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;

			/* if ((LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_EmitRTShadows) != 0) */
			if((LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_UseShadowBuffer) != 0)
			{
				if(pst_SD->iNOfSBLights < ShadowBuffer_MaxLights)
				{
					pst_SD->bShadowBuffer = true;

					/*
					 * MATH_SetIdentityMatrix(&mTmp); MATH_SetTranslation(&mTmp,
					 * MATH_pst_GetTranslation((*ppst_LightNode)->pst_GlobalMatrix));
					 * WOR_GetLookAtMatrix( &pst_SD->mLightMatrix[pst_SD->iNOfSBLights], &mTmp,
					 * OBJ_pst_GetAbsolutePosition(AI_EvalFunc_AIMainActorGet_C(0)));
					 */
					MATH_CopyMatrix(&pst_SD->mLightMatrix[pst_SD->iNOfSBLights], (*ppst_LightNode)->pst_GlobalMatrix);

					pst_SD->iNOfSBLights++;
				}
			}

			ppst_LightNode++;
		}
	}
}

#if !defined(AI_EvalFunc_AIMainActorGet_C)
//extern OBJ_tdst_GameObject	*AI_EvalFunc_AIMainActorGet_C(int);
#endif
#endif
#ifdef _XBOX
#include "Gx8/Gx8shadowbuffer.h"
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_Render(WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem					*pst_Elem, *pst_LastElem;
	WOR_tdst_View					*pst_View;
	WOR_tdst_View					*pst_LastView;
	int								iNumView;
	LIGHT_tdst_Light				*pst_Light;
	//SOFT_tdst_FogParams				st_Fog;
	OBJ_tdst_GameObject				*pst_GO;
#if defined(_XBOX)
	Gx8_tdst_SpecificData			*pst_SD;
	MATH_tdst_Matrix				m_cameraViewPoint;
	int								iNumSBLight;
	extern u_int					gAE_Status;
	extern GSP_AfterEffectParams	gAE_Params;
	extern bool						bSmallViewport;
#endif

#ifdef XENONVIDEOSTATISTICS
    XeGOStatistics                  *XeStats = XeGOStatistics::Instance();
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // Reinit all stats for this frame
    XeStats->InitStats();
#endif

#if defined(_PC_RETAIL)
	pst_SD = (Dx9_tdst_SpecificData *) _pst_DD->pv_SpecificData;

	/* set render target to textures */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		IDirect3DSurface9	*pImageSurface;
		IDirect3DSurface9	*pDepthStencilSurface;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		IDirect3DTexture9_GetSurfaceLevel(pst_SD->pImageRenderTarget, 0, /* level */ &pImageSurface);
		CHK_D3D(IDirect3DDevice9_SetRenderTarget(pst_SD->pD3DDevice, 0, /* first render target */ pImageSurface));

		CHK_D3D(IDirect3DDevice9_SetDepthStencilSurface(pst_SD->pD3DDevice, pst_SD->pDepthStencilSurface, ));

		CHK_D3D
		(
			IDirect3DDevice9_Clear
				(
					pst_SD->pD3DDevice,
					0,		/* rects count */
					NULL,	/* rects ptr */
					D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, /* TODO: try to avoid D3DCLEAR_TARGET */
					0,		/* color */
					1.0f,	/* z */
					0		/* stencil */
				)
		);
	}
#endif

#ifdef _XENON_RENDER
    // set ambient colors for the world
    g_oVertexShaderMgr.SetAmbientColor(0, XeConvertColor(_pst_World->ul_AmbientColor));
    g_oVertexShaderMgr.SetAmbientColor(1, XeConvertColor(_pst_World->ul_AmbientColor2));

    // set global mul 2X
    g_oPixelShaderMgr.EnableGlobalMul2X(_pst_DD->GlobalMul2X != 0);
    g_oPixelShaderMgr.SetGlobalMul2XFactor(_pst_DD->GlobalMul2XFactor);

    // set global RLI scale/offset
    g_pXeContextManagerEngine->SetGlobalRLIScaleAndOffset(_pst_World->f_XeRLIScale, _pst_World->f_XeRLIOffset);

    g_oXeShadowManager.ClearLights();
    g_oXeShadowManager.SetGaussianStrength(_pst_World->f_XeGaussianStrength);
    BOOL isGlowActivated = _pst_World->f_XeGlowIntensity > 0.0f;
    g_oAfterEffectManager.SetParams( AE_COLORDIFFUSION, isGlowActivated, 0, _pst_World->f_XeGlowLuminosityMin );
    g_oAfterEffectManager.SetParams( AE_COLORDIFFUSION, isGlowActivated, 1, _pst_World->f_XeGlowLuminosityMax );
    g_oAfterEffectManager.SetParams( AE_COLORDIFFUSION, isGlowActivated, 2, _pst_World->f_XeGlowIntensity );
    g_oAfterEffectManager.SetParams( AE_COLORDIFFUSION, isGlowActivated, 3, *((float*)&_pst_World->ul_XeGlowColor) );
    g_oAfterEffectManager.SetParams( AE_COLORDIFFUSION, isGlowActivated, 4, _pst_World->f_XeGlowZNear );
    g_oAfterEffectManager.SetParams( AE_COLORDIFFUSION, isGlowActivated, 5, _pst_World->f_XeGlowZFar );
    g_oAfterEffectManager.SetGodRayIntensity( _pst_World->f_XeGodRayIntensity, _pst_World->ul_XeGodRayIntensityColor );
    g_oAfterEffectManager.SetParams( AE_XINVERT, _pst_DD->GlobalXInvert, 0, 0.0f);

    // default fog
    //modif Yoann le fog default est st_Fog1
	_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_SetFogParams, (ULONG) & _pst_DD->st_Fog);//temporaire2
	//_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_SetFogParams, (ULONG) & _pst_DD->st_Fog);
#endif

	MAI_gst_MainHandles.pst_DisplayData = _pst_DD;
	PROPS2_StartRaster(&PROPS2_gst_WOR_Render);
	if(_pst_DD)
	{
#ifdef RASTERS_ON
		GDI_Rasters_Reset(_pst_DD->pst_Raster);
#endif
		GDI_gpst_CurDD = _pst_DD;
		iNumView = 0;

		/* CARLONE...THIS IS NEW "SHADOW BUFFER" CODE!! */
#if defined(_XBOX)
		{
#if defined(USE_SHADOW_BUFFER)
			SaveDeviceSettings();

			/* Render all the shadow buffers */
			WOR_RenderAllShadowBuffer(_pst_World, _pst_DD);

			/* Render the "diffuse" shadow buffer texture */
			WOR_RenderShawowBufferDiffuse(_pst_World, _pst_DD);

			RestoreDeviceSettings();
#endif
		}
#endif
#if defined(_XBOX)
		/* CARLONE...RETURN TO SMALL VIEPORT */
		//Yoann -->false
		bSmallViewport = false;
#endif

		/* END OF NEW "SHADOW BUFFER" CODE BY PHILLIPPE!! */
		pst_View = _pst_World->pst_View;
		pst_LastView = pst_View + _pst_World->ul_NbViews;

		M4Edit_SaveDrawMask();

		for(; pst_View < pst_LastView; pst_View++)
		{
			if((pst_View->uc_Flags & WOR_Cuc_View_Activ) && (pst_View->st_DisplayInfo.pst_DisplayDatas == _pst_DD))
			{

#ifdef ACTIVE_EDITORS
				if (pst_View->uc_Flags & WOR_Cuc_View_SplitView) GDI_SwapCameras( GDI_gpst_CurDD );
#endif

				_pst_World->pst_CurrentView = pst_View;

				/* Compute current camera */
				PROPS2_StartRaster(&PROPS2_gst_WOR_SetCam);
				WOR_SetCam(pst_View);
				PROPS2_StopRaster(&PROPS2_gst_WOR_SetCam);

				/* We update the culling variables. */
				CAM_Engine_ComputePlans(&_pst_DD->st_Camera);

				OBJ_UpdateCullingVars(&_pst_DD->st_Camera);

				/*
				 * clear the zbuffer (we clear before each view except first one in engine, and
				 * the 4 first ont in editor
				 */
				M_Clear4View(iNumView);

				/*
				 * init editor var : number of selected links, displayed skel and gizmos are set
				 * to 0
				 */
				M4Edit_InitNumberOfSelectedLinks();
				M4Edit_InitBonesAndSkeletonDisplayList();

				PROPS2_StartRaster(&PROPS2_gst_WOR_Render1);
				PRO_StartTrameRaster(&_pst_DD->pst_Raster->st_BuildLightList);

				/*
				 * Build list of light with all light within EOT light Keep a separated list of
				 * light in case someone have to put manually a light
				 */
				LIGHT_List_Reset(&_pst_DD->st_LightList);

				pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_Lights);
				pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_Lights);
				
				{
				//bool FIRSTFog = 0;
				_pst_DD->st_Fog2.c_Flag = 0;
				for(; pst_Elem <= pst_LastElem; pst_Elem++)
				{
					pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
					if(TAB_b_IsAHole(pst_GO)) {
#ifdef ACTIVE_EDITORS
						if (pst_View->uc_Flags & WOR_Cuc_View_SplitView) GDI_SwapCameras( GDI_gpst_CurDD );
#endif
						continue;
					}

					pst_Light = (LIGHT_tdst_Light *) pst_GO->pst_Extended->pst_Light;
					if(pst_Light && (pst_Light->ul_Flags & LIGHT_Cul_LF_Active))
					{
						if((pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Fog)
						{
#ifdef JADEFUSION
							if(!(_pst_DD->st_Fog1.c_Flag & SOFT_C_ForceNoFog))
#else
							if(!(_pst_DD->st_Fog1.c_Flag & SOFT_C_ForceNoFog) && pst_Light->st_Fog.b_FogNumber1)
#endif
							{
								_pst_DD->st_Fog1.c_Mode = 0;//avant: st_Fog.c_Mode = 0;
								_pst_DD->st_Fog1.f_Start = pst_Light->st_Fog.f_Start;
								_pst_DD->st_Fog1.f_End = pst_Light->st_Fog.f_End;
								_pst_DD->st_Fog1.ul_Color = pst_Light->ul_Color;
								_pst_DD->st_Fog1.f_Density = pst_Light->st_Fog.f_Density;
								_pst_DD->st_Fog1.c_Flag = SOFT_C_FogActive;
								//_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_SetFogParams, (ULONG) & st_Fog);//avant
								_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_SetFogParams, (ULONG) & _pst_DD->st_Fog1);//active le 1er par defaut
								//FIRSTFog ++;
#ifdef JADEFUSION	
								_pst_DD->st_Fog1.f_PitchAttenuationMin = pst_Light->st_Fog.f_PitchAttenuationMin;
                                _pst_DD->st_Fog1.f_PitchAttenuationMax = pst_Light->st_Fog.f_PitchAttenuationMax;
                                _pst_DD->st_Fog1.f_PitchAttenuationIntensity = pst_Light->st_Fog.f_PitchAttenuationIntensity;
#endif							
							}
#ifndef JADEFUSION
							// 2eme Parametre de fog
							else if(!(_pst_DD->st_Fog2.c_Flag & SOFT_C_ForceNoFog) && !pst_Light->st_Fog.b_FogNumber1)
							{
								_pst_DD->st_Fog2.c_Mode = 0;
								_pst_DD->st_Fog2.f_Start = pst_Light->st_Fog.f_Start;
								_pst_DD->st_Fog2.f_End = pst_Light->st_Fog.f_End;
								_pst_DD->st_Fog2.ul_Color = pst_Light->ul_Color;
								_pst_DD->st_Fog2.f_Density = pst_Light->st_Fog.f_Density;
								_pst_DD->st_Fog2.c_Flag = SOFT_C_FogActive;
								//_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_SetFogParams, (ULONG) & st_Fog);
								//FIRSTFog ++;
#ifdef JADEFUSION				
								_pst_DD->st_Fog2.f_PitchAttenuationMin = pst_Light->st_Fog.f_PitchAttenuationMin;
                                _pst_DD->st_Fog2.f_PitchAttenuationMax = pst_Light->st_Fog.f_PitchAttenuationMax;
                                _pst_DD->st_Fog2.f_PitchAttenuationIntensity = pst_Light->st_Fog.f_PitchAttenuationIntensity;
#endif		
							}
#endif
						}
						else if(!pst_GO->uc_Secto || !ENG_gb_ActiveSectorization)
						{
#ifdef _XBOX
							if(!(pst_Light->ul_Flags & LIGHT_Cul_LF_NotReplaceShadowBuffer))
#elif (defined(_XENON_RENDER) && defined(XERENDER_USE_XENON_LIGHTS))
                            if(pst_Light->ul_Flags & LIGHT_Cul_LF_ExtendedShadowLight)
                            {
                                // Send this shadow light to Xenon renderer
                                Xe_AddShadowLight(pst_GO);
						        //LIGHT_List_AddLight(&_pst_DD->st_ExtendedShadowLightList, pst_GO);
                            }
                            if(pst_Light->ul_Flags & LIGHT_Cul_LF_ExtendedLight)
#else
								if(!(pst_Light->ul_Flags & LIGHT_Cul_LF_UseShadowBuffer))
#endif
								{
									if ((pst_Light->ul_Flags & (LIGHT_Cul_LF_ExclusiveLight | LIGHT_Cul_LF_Type)) == (LIGHT_Cul_LF_Omni | LIGHT_Cul_LF_ExclusiveLight))
										_pst_DD->st_LightList.ulContainExclusiveLights++;
									LIGHT_List_AddLight(&_pst_DD->st_LightList, pst_GO);
								}
						}
					}
				}
				}//light
#ifdef JADEFUSION
                LIGHT_ResetAllSpotCullingBV( &_pst_DD->st_LightList );
                LIGHT_List_Sort(&_pst_DD->st_LightList );
#endif
				PRO_StopTrameRaster(&_pst_DD->pst_Raster->st_BuildLightList);
				PRO_SetRasterLong(&_pst_DD->pst_Raster->st_NbLights, _pst_DD->st_LightList.ul_Current);
				PROPS2_StopRaster(&PROPS2_gst_WOR_Render1);

#if defined(_XBOX)
				/*
				 * NO MORE SHADOW BUFFERE HERE WOR_CheckForShadowBuffer(_pst_DD, pst_SD); // ...
				 * if is it so ... if (pst_SD->bShadowBuffer) { MATH_tdst_Matrix mTmp; // ... for
				 * each light render the scene with black shadows
				 * for(iNumSBLight=0;iNumSBLight<pst_SD->iNOfSBLights;iNumSBLight++) { // set
				 * light POV MATH_CopyMatrix( &m_cameraViewPoint, &pst_View->st_ViewPoint);
				 * MATH_CopyMatrix( &pst_View->st_ViewPoint, &pst_SD->mLightMatrix[iNumSBLight]);
				 * PROPS2_StartRaster(&PROPS2_gst_WOR_SetCam); WOR_SetCam(pst_View);
				 * PROPS2_StopRaster(&PROPS2_gst_WOR_SetCam); // direct x initialization
				 * Gx8_ShadowBuffer_LightRender_Init(pst_SD); // render in the SB
				 * pst_SD->bRenderingFromLight=true; WOR_Render_3D(iNumView,_pst_World,_pst_DD);
				 * pst_SD->bRenderingFromLight=false; // restore camera POV MATH_CopyMatrix(
				 * &pst_View->st_ViewPoint, &m_cameraViewPoint);
				 * PROPS2_StartRaster(&PROPS2_gst_WOR_SetCam); WOR_SetCam(pst_View);
				 * PROPS2_StopRaster(&PROPS2_gst_WOR_SetCam); // compute UV matrix
				 * CAM_SetObjectMatrixFromCam( &mTmp, &pst_SD->mLightMatrix[iNumSBLight] );
				 * MATH_InvertMatrix( &pst_SD->mSBTextureMatrix, &mTmp ); // directx init
				 * Gx8_ShadowBuffer_CameraRender_Init(pst_SD,
				 * pst_SD->pShadowBufferSpot[iNumSBLight]); // render with black shadows
				 * WOR_Render_3D(iNumView,_pst_World,_pst_DD); // restore directx state
				 * Gx8_ShadowBuffer_ResetRenderStates(pst_SD); } }
				 */
				AE_ManageAETexturesAllocation(pst_SD);
				
				if(gAE_Status & GSP_Status_AE_DB)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					float	f_SaveNear, f_SaveFar;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					Gx8_DepthBlurZBufferRender_Init(pst_SD);

					f_SaveNear = _pst_DD->st_Camera.f_NearPlane;
					f_SaveFar = _pst_DD->st_Camera.f_FarPlane;

					/* pst_DD->st_Camera.f_NearPlane=gAE_Params.Depth_Blur_ZStart + 0.5f; */
					_pst_DD->st_Camera.f_NearPlane = 20.0f; /* setprojection divide it by 20 = 1 */

					_pst_DD->st_Camera.f_FarPlane = gAE_Params.Depth_Blur_ZEnd * 2;

					/* Compute current camera */
					PROPS2_StartRaster(&PROPS2_gst_WOR_SetCam);
					WOR_SetCam(pst_View);
					PROPS2_StopRaster(&PROPS2_gst_WOR_SetCam);

					WOR_Render_3D(iNumView, _pst_World, _pst_DD);

					_pst_DD->st_Camera.f_NearPlane = f_SaveNear;
					_pst_DD->st_Camera.f_FarPlane = f_SaveFar;

					Gx8_DepthBlurZBufferRender_ResetStates(pst_SD);
				}

				// deactive pour kingkong cet effet...
				/*if(gAE_Status & GSP_Status_AE_DBN)
				{
					//~~~~~~~~~~~~~~~~~~~~~~~~~~//
					float	f_SaveNear, f_SaveFar;
					//~~~~~~~~~~~~~~~~~~~~~~~~~~//

					Gx8_NearBlurZBufferRender_Init(pst_SD);

					f_SaveNear = _pst_DD->st_Camera.f_NearPlane;
					f_SaveFar = _pst_DD->st_Camera.f_FarPlane;

					_pst_DD->st_Camera.f_NearPlane = 10.0f; // setprojection divide it by 20 = 0.5
					_pst_DD->st_Camera.f_FarPlane = gAE_Params.Depth_Blur_Near;

					// Compute current camera
					PROPS2_StartRaster(&PROPS2_gst_WOR_SetCam);
					WOR_SetCam(pst_View);
					PROPS2_StopRaster(&PROPS2_gst_WOR_SetCam);

					WOR_Render_3D(iNumView, _pst_World, _pst_DD);

					_pst_DD->st_Camera.f_NearPlane = f_SaveNear;
					_pst_DD->st_Camera.f_FarPlane = f_SaveFar;

					Gx8_NearBlurZBufferRender_ResetStates(pst_SD);
				}*/
#endif /* XBOX */

				/* Compute current camera */
				PROPS2_StartRaster(&PROPS2_gst_WOR_SetCam);
				WOR_SetCam(pst_View);
				PROPS2_StopRaster(&PROPS2_gst_WOR_SetCam);

#ifdef _XBOX
				if(GetCastingShadows())
				{
					Gx8_PrepareMatrixDiffuseRendering();
				}

				/* Prepare stuff for water reflection map */
				Gx8_BeginReflectionDraw();

				/*
				 * Prepare stuff for water refraction mapping £
				 * Gx8_BeginRefractionDraw();
				 */
#endif
#ifdef _PC_RETAIL
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					void	Dx9_ResetDrawReflection(void);
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					Dx9_ResetDrawReflection();
				}
#endif

				/*
				 * WOR_Render_3D(iNumView,_pst_World,_pst_DD); £
				 * Shadows
				 */
				SDW_UpdatePositions();

				/* Earth,Wind & fire */
				PROTEX_BeforeDraw();

				WATER3D_BeforeDraw();

				/* Render */
				WOR_Render_All_GO(_pst_World, _pst_DD);

				/* Graphic FX */
				GFX_Render(&_pst_World->pst_GFX, 0);

				WOR_RenderForEditors_Skel_Gizmo_Ray_Kilt_Grid(_pst_World, _pst_DD);
				M4Edit_ResetLightComputeRLI();

#ifdef _XBOX
				if(GetCastingShadows())
				{
					Gx8_ReleaseMatrixDiffuseRendering();
				}
#endif

				/*$1- Z list ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				PROPS2_StartRaster(&PROPS2_gst_SOFT_ZList_Send);

				SOFT_ZList_Send();

				if (ul_LastGOStackNumber)
				{
#ifdef _XENON_RENDER
                    g_pXeContextManagerEngine->SetDoublePassZOverwrite(TRUE);
#endif
					float PushValue;
					PushValue = 5.0f;
					WOR_SetCam(pst_View);
					GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_PushZBuffer, *(u32 *) &PushValue);
					while (ul_LastGOStackNumber--)
					{
				
						WOR_Render_One_GO(_pst_World, _pst_DD, st_LastGOStack[ul_LastGOStackNumber]);
					}
					SOFT_ZList_Send();
					ul_LastGOStackNumber=0;
#ifdef _XENON_RENDER
                    g_pXeContextManagerEngine->SetDoublePassZOverwrite(FALSE);
#endif
				}


				GAO_ModifierLazy_UnapplyAll();
				MODIFIER_SPG2_OneFrameCall();
				MODIFIER_FOGDYN_OneFrameCall();

				PROPS2_StopRaster(&PROPS2_gst_SOFT_ZList_Send);

#if defined(ACTIVE_EDITORS) || defined(PCWIN_TOOL)
				OGL_AE_Before2D();
				/* 16 / 9eme black band */
				if(_pst_DD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_169BlackBand)
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_Display169BlackBand, 0);

				OGL_AE_DebugInfo(_pst_World);
#elif defined(_PC_RETAIL)
#else
				/* 16 / 9eme black band */
				if(_pst_DD->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_169BlackBand)
					_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_Display169BlackBand, 0);
#endif

				/*$1- take a snap ? ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					extern void AI_EvalFunc_VIEWSnapshot_DoIt(void);
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					AI_EvalFunc_VIEWSnapshot_DoIt();
				}

				/*$1- Graphic FX for interface ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(!(_pst_DD->ul_DisplayFlags & GDI_Cul_DF_HideInterface))
				{
					/* if(_pst_World->pst_GFXInterface) */
					{
						/* charge les textures de l'interface */
						_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_LoadInterfaceTex, 0);

						/* Clear le ZBuffer */
#ifndef _XENON 
						if(!(_pst_DD->ul_DisplayFlags & GDI_cul_DF_DepthReadBeforeFlip))
						{
							OGL_Clear(GDI_Cl_ZBuffer, 0);
						}
#endif
						/* sauve la camera */
						L_memcpy(&GDI_gpst_CurDD->st_SaveCamera, &GDI_gpst_CurDD->st_Camera, sizeof(CAM_tdst_Camera));

						/* met une matrice identité pour la camera */
						CAM_SetCameraMatrix(&GDI_gpst_CurDD->st_Camera, &MATH_gst_IdentityMatrix);
						GDI_gpst_CurDD->st_GDI.pfnv_SetProjectionMatrix(&GDI_gpst_CurDD->st_Camera);
						CAM_Inverse(&GDI_gpst_CurDD->st_Camera);
						SOFT_MatrixStack_Reset
						(
							&GDI_gpst_CurDD->st_MatrixStack,
							&GDI_gpst_CurDD->st_Camera.st_InverseMatrix
						);

						/* display interface */
						_pst_DD->ul_DisplayInfo |= GDI_Cul_DI_RenderingInterface;

#ifdef _XENON_RENDER
						// interface is on top of all
						GDI_gpst_CurDD->g_cZListCurrentDisplayOrder = XERENDER_INTERFACE_DEPTH;
#endif

						if(_pst_World->pst_GFXInterface) GFX_Render(&(_pst_World->pst_GFXInterface), 1);

						/* display strings */
						STRDATA_Render();

#ifdef _XENON_RENDER
						GDI_gpst_CurDD->g_cZListCurrentDisplayOrder = 0;
#endif

						_pst_DD->ul_DisplayInfo &= ~GDI_Cul_DI_RenderingInterface;

						/* restaure la camera */
						L_memcpy(&GDI_gpst_CurDD->st_Camera, &GDI_gpst_CurDD->st_SaveCamera, sizeof(CAM_tdst_Camera));

						/* decharge les texture de l'interface */
						_pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_UnloadInterfaceTex, 0);
					}
				}
#ifdef ACTIVE_EDITORS
				if (pst_View->uc_Flags & WOR_Cuc_View_SplitView) GDI_SwapCameras( GDI_gpst_CurDD );
#endif
			}

			iNumView++;
		}

		M4Edit_RestoreDrawMask();

		/* Restore cam to view 0 */
		PROPS2_StartRaster(&PROPS2_gst_WOR_SetCam);
		WOR_SetCam(_pst_World->pst_View);
		PROPS2_StopRaster(&PROPS2_gst_WOR_SetCam);

		GDI_gpst_CurDD = NULL;
	}

	PROPS2_StopRaster(&PROPS2_gst_WOR_Render);
#ifdef XENONVIDEOSTATISTICS
    XeStats->SortAndCleanStats();
#endif
}

/*
 =======================================================================================================================
    Aim: Returns the % of screen covered by the BV of an object in the given view Note: 0= culled; 255= full screen or
    bigger
 =======================================================================================================================
 */
UCHAR OBJ_uc_CalcLODVis(OBJ_tdst_GameObject *_pst_GO, WOR_tdst_View *_pst_View, float _f_SqrObjDist)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		st_Dist, st_Center;
	float					f_SqrProjRadius;
	float					f_ProjRadius;
	float					f_SqrObjDist;
	MATH_tdst_Vector		st_Diag;
	GDI_tdst_DisplayData	*pst_DD;
	void					*pst_BV;
	UCHAR					uc_Result;
#ifndef _GAMECUBE
	float					f_Result;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(PSX2_TARGET) || defined(_GAMECUBE)
	pst_DD = GDI_gpst_CurDD;
#else
	pst_DD = (GDI_tdst_DisplayData *) _pst_View->st_DisplayInfo.pst_DisplayDatas;
#endif
	if(pst_DD)
	{
		/* Distance camera / object */
		OBJ_BV_ComputeCenter(_pst_GO, &st_Center);
		MATH_SubVector(&st_Dist, &st_Center, &_pst_View->st_ViewPoint.T);
		f_SqrObjDist = MATH_f_SqrNormVector(&st_Dist);

		pst_BV = _pst_GO->pst_BV;
		if(OBJ_BV_IsSphere(pst_BV))
		{
			f_SqrProjRadius = (fSqr(OBJ_f_BV_GetRadius(pst_BV)) * fSqr(pst_DD->st_Camera.f_FactorX)) / f_SqrObjDist;
		}
		else
		{
			MATH_SubVector(&st_Diag, OBJ_pst_BV_GetGMax(pst_BV), OBJ_pst_BV_GetGMin(pst_BV));
			f_SqrProjRadius = MATH_f_SqrNormVector(&st_Diag) * fSqr(pst_DD->st_Camera.f_FactorX) / f_SqrObjDist;
		}

		f_ProjRadius = pst_DD->f_LODVal * fOptSqrt(f_SqrProjRadius);
		if(_pst_GO->uc_LOD_Dist) f_ProjRadius *= _pst_GO->uc_LOD_Dist / 255.0f;

#if defined(_GAMECUBE) || defined(_XENON)
		uc_Result = (UCHAR) fMin(255.0f, f_ProjRadius);
#else
		f_Result = fMin(255.0f, f_ProjRadius) + 12533760.0f;
		uc_Result = *(UCHAR *) &f_Result;
#endif

		/* We want to keep the 0 value for culled object only. */
		if(uc_Result == 0) uc_Result = 1;

		return uc_Result;
	}
	else
		return(UCHAR) 255;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
#ifdef _XBOX

/*
 =======================================================================================================================
    include "Gx8/Gx8shadowbuffer.h" JUST FOR XBOX SHADOW BUFFERS Clean up all the game object's flags
 =======================================================================================================================
 */
void WOR_Clea_All_GO(WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);

	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		pst_GO->playSkinning = true;
	}
}

/*
 =======================================================================================================================
    Render all the shadow buffer from light point of view
 =======================================================================================================================
 */
void WOR_RenderAllShadowBuffer(WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_GO;
	LIGHT_tdst_Light	*pst_Light;
	float				lightAngle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Prepare lights */
	LIGHT_List_Reset(&_pst_DD->st_LightList);		/* No light for this rendering */
	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_Lights);
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_Lights);

	/* By default performs skinning */
	_pst_DD->ul_DisplayInfo &= (~GDI_Cul_DI_RenderNOAnimation);

	/* Tell the engine I'm rendering shadow buffers */
	_pst_DD->ul_DisplayInfo |= GDI_Cul_DI_ComputeShadowMap;

	/* By default no shadow buffer */
	SetCastingShadows(false);

	/* Prepare device to rendere in SHADOW BUFFER */
	Gx8_PrepareDeviceForSBRendering();

	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;
		pst_Light = (LIGHT_tdst_Light *) pst_GO->pst_Extended->pst_Light;

		if
		(
			pst_Light
		&&	(pst_Light->ul_Flags & LIGHT_Cul_LF_Active)
		&&	(pst_Light->ul_Flags & LIGHT_Cul_LF_UseShadowBuffer)
		)
		/*
		 * if(pst_Light && (pst_Light->ul_Flags & LIGHT_Cul_LF_Active) &&
		 * (pst_Light->ul_Flags & LIGHT_Cul_LF_EmitRTShadows))
		 */
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			WOR_tdst_View	st_ShadowView;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			st_ShadowView = *_pst_World->pst_View;

			/* Set the view as from the light source (CARLONE) */
			st_ShadowView.st_ViewPoint = (*(pst_GO->pst_GlobalMatrix));

			/* Get the light angle */
			lightAngle = pst_Light->st_Spot.f_BigAlpha * 2;

			/*
			 * lightAngle=pst_Light->st_Spot.f_BigAlpha; £
			 * Save the light field of view
			 */
			st_ShadowView.st_DisplayInfo.f_FieldOfVision = lightAngle;

			_pst_World->pst_CurrentView = &st_ShadowView;

			/* Set the shadow buffer as target */
			if(Gx8_SBSetTarget(&st_ShadowView.st_ViewPoint, (void *) pst_GO, lightAngle))
			{
				/* Tell the pipeline this frame we will use shadow buffer */
				SetCastingShadows(true);

				ENG_gb_ActiveSectorization = FALSE; /* No sectorisation for this rendering */
				_pst_World->pst_CurrentView = &st_ShadowView;

				/* Compute current camera */
				WOR_SetCam(_pst_World->pst_CurrentView);

				/* We update the culling variables. */
				CAM_Engine_ComputePlans(&_pst_DD->st_Camera);
				OBJ_UpdateCullingVars(&_pst_DD->st_Camera);

				/* Preculling BV mask */
				WOR_Render_All_GO(_pst_World, _pst_DD);

				/*
				 * After the first rendering...don' t need no more to perform skinning!!!!!!!!!!!! £
				 * CARLONE REMOVE SOFT_ZList_Send();
				 */
				SOFT_ZList_Clear();
				_pst_DD->ul_DisplayInfo |= GDI_Cul_DI_RenderNOAnimation;
				ENG_gb_ActiveSectorization = TRUE;
			}	/* END IF SETTING SHADOW BUFFER IS OK */
		}		/* END IF LIGHT CAST SHADOW */
	}			/* END FOR EACH LIGHT */

	/* Release rendering settings for shadow buffer */
	Gx8_ReleaseDeviceForSBRendering();

	/* Tell the engine I'm not more drawing shadow map */
	_pst_DD->ul_DisplayInfo &= (~GDI_Cul_DI_ComputeShadowMap);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_RenderShawowBufferDiffuse(WOR_tdst_World *_pst_World, GDI_tdst_DisplayData *_pst_DD)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_GO;
	LIGHT_tdst_Light	*pst_Light;
	SOFT_tdst_FogParams st_Fog;
	ULONG				savedAmbientColor;
	ULONG				savedAmbientColor2;
	WOR_tdst_View		*pst_View;
	WOR_tdst_View		*pst_LastView;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!GetCastingShadows()) return;

	Gx8_PrepareDeviceForSBDiffuseRendering();

	/*
	 * Restore camera (I changed it for light view rendering) £
	 * _pst_World->pst_CurrentView=_pst_World->pst_View; RESTORE CAMERA
	 */
	pst_View = _pst_World->pst_View;
	pst_LastView = pst_View + _pst_World->ul_NbViews;

	for(; pst_View < pst_LastView; pst_View++)
	{
		if((pst_View->uc_Flags & WOR_Cuc_View_Activ) && (pst_View->st_DisplayInfo.pst_DisplayDatas == _pst_DD))
		{
			_pst_World->pst_CurrentView = pst_View;
			WOR_SetCam(pst_View);
		}
	}

	/*
	 * savedAmbientColor=_pst_World->ul_AmbientColor;
	 * savedAmbientColor2=_pst_World->ul_AmbientColor2;
	 * _pst_World->ul_AmbientColor=0x00000000;
	 * _pst_World->ul_AmbientColor2=0x00000000; £
	 * STEP 1) Draw objects with just the diffuse component £
	 * Tell the engine I'm drawing Just the diffuse of the scene
	 */
	_pst_DD->ul_DisplayInfo |= GDI_Cul_DI_RenderingDiffuse;

	LIGHT_List_Reset(&_pst_DD->st_LightList);
	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_Lights);
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_Lights);
	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		pst_Light = (LIGHT_tdst_Light *) pst_GO->pst_Extended->pst_Light;

		/* Don't use the light that cast shadows for this firs lighting step */
		if
		(
			pst_Light
		&&	(pst_Light->ul_Flags & LIGHT_Cul_LF_Active)
		&&	!(pst_Light->ul_Flags & LIGHT_Cul_LF_UseShadowBuffer)
		)
		/*
		 * if(pst_Light && (pst_Light->ul_Flags & LIGHT_Cul_LF_Active) &&
		 * !(pst_Light->ul_Flags & LIGHT_Cul_LF_EmitRTShadows))
		 */
		{
			/*
			 * CARLONE REMOVE £
			 * continue;
			 */
			if((pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Fog)
			{	/* if ( !(_pst_DD->st_Fog.c_Flag & SOFT_C_ForceNoFog ) ) { st_Fog.c_Mode = 0; st_Fog.f_Start
				 * pst_Light->st_Fog.f_Start; st_Fog.f_End = pst_Light->st_Fog.f_End; st_Fog.ul_Color
				 * pst_Light->ul_Color; st_Fog.f_Density = pst_Light->st_Fog.f_Density; st_Fog.c_Flag SOFT_C_FogActive;
				 * _pst_DD->st_GDI.pfnl_Request(GDI_Cul_Request_SetFogParams, (ULONG) & st_Fog); }
				 * £
				 * NO FOG */
				continue;
			}
/*			else if(!OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_SecInvisible) || !ENG_gb_ActiveSectorization)
			{
				LIGHT_List_AddLight(&_pst_DD->st_LightList, pst_GO);
			}*/
		}
	}

	ENG_gb_ActiveSectorization = FALSE; /* No sectorisation for this rendering */

	/* WOR_SetCam(_pst_World->pst_CurrentView); */
	CAM_Engine_ComputePlans(&_pst_DD->st_Camera);
	OBJ_UpdateCullingVars(&_pst_DD->st_Camera);

	/* CARLONE REMOVE */
	SOFT_ZList_Clear();
	WOR_Render_All_GO(_pst_World, _pst_DD);

	SOFT_ZList_Clear();
	ENG_gb_ActiveSectorization = TRUE;

	/* Reset the diffuse rendering */
	_pst_DD->ul_DisplayInfo &= (~GDI_Cul_DI_RenderingDiffuse);

	/*
	 * END OF STEP 1 STEP 2) Draw objects with all the light map on and add the result
	 * on the frame buffer £
	 * Gx8_SetBias(15);
	 */
	LIGHT_List_Reset(&_pst_DD->st_LightList);

	/* No more ambient light...i don't want to add it again */
	savedAmbientColor = _pst_World->ul_AmbientColor;
	savedAmbientColor2 = _pst_World->ul_AmbientColor2;
	_pst_World->ul_AmbientColor = 0x00000000;
	_pst_World->ul_AmbientColor2 = 0x00000000;

	/* Tell the pipeline...I' drawing spot with shadows */
	_pst_DD->ul_DisplayInfo |= GDI_Cul_DI_RenderingSpotSwadows;

	/* Tell the pipeline it need to calculate matrix for shadow buffer */
	((Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData)->needSBMatrix = true;

	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_Lights);
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_Lights);
	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		pst_Light = (LIGHT_tdst_Light *) pst_GO->pst_Extended->pst_Light;

		/* Don't use the light that cast shadows for this firs lighting step */
		if
		(
			pst_Light
		&&	(pst_Light->ul_Flags & LIGHT_Cul_LF_Active)
		&&	pst_Light->ul_Flags & LIGHT_Cul_LF_UseShadowBuffer
		&&	pst_Light->ul_Flags & LIGHT_Cul_LF_Spot
		)
		/*
		 * if(pst_Light && (pst_Light->ul_Flags & LIGHT_Cul_LF_Active) &&
		 * pst_Light->ul_Flags & LIGHT_Cul_LF_EmitRTShadows)
		 */
		{
			if((pst_Light->ul_Flags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Fog)
			{
				/* DO NOTHING */
			}
		}
	}

	Gx8_ResetShadowBufferDeviceSettings();

	/*
	 * Gx8_SetBias(0); £
	 * Restore origina ambient color
	 */
	_pst_World->ul_AmbientColor = savedAmbientColor;
	_pst_World->ul_AmbientColor2 = savedAmbientColor2;

	/* No more shadow spot rendering */
	_pst_DD->ul_DisplayInfo &= (~GDI_Cul_DI_RenderingSpotSwadows);

	/* No more matrix for shadow buffer */
	((Gx8_tdst_SpecificData *) _pst_DD->pv_SpecificData)->needSBMatrix = false;

	/* END */
	Gx8_ReleaseDeviceForSBDiffuseRendering();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bool WOR_CheckIfRecShadows(OBJ_tdst_GameObject *pst_GO)
{
	/*
	 * TRICK..FALSE==TRUE...TO BE CHANGED £
	 * If no visu....always cast shadows
	 */
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
	{
		return true;
	}

	return(!(pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_ReceiveShadowBuffer));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
bool WOR_CheckIfCasShadows(OBJ_tdst_GameObject *pst_GO)
{
	/*
	 * TRICK..FALSE==TRUE...TO BE CHANGED £
	 * If no visu....always cast shadows
	 */
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
	{
		return true;
	}

	return(!(pst_GO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_EmitShadowBuffer));
}
#endif
