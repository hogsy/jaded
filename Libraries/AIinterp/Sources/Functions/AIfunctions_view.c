/*$T AIfunctions_view.c GC! 1.100 03/20/01 14:01:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "CAMera/CAMera.h"
#include "SOFT/SOFTlinear.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/WORld/WORinit.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "TEXture/TEXfile.h"
#include "TEXture/TEXconvert.h"
#include "../../../Dlls/JPEGLIB/INTERFCE.H"
#include "BIGfiles/BIGio.h"
#include "INOut/INOsaving.h"
#include "INOut/INOsaving.h"
#include "GDInterface/GDIrequest.h"
#include "AIinterp/Sources/Functions/AIfunctions_savephotos.h"

#ifdef _XENON
#include "Xenon/VideoManager/VideoManager.h"
#endif

#if (defined(PSX2_TARGET) || defined(_GAMECUBE)) && defined(__cplusplus)
extern "C"
{
#endif
extern ULONG	IMG_Analyser(WOR_tdst_World *, int );
extern ULONG	IMG_Analyser_Bis(WOR_tdst_World *, int );

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_VIEWAnalyser_C(OBJ_tdst_GameObject *pst_GO, int i_Mission)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return 0;
    if (i_Mission <= 0) return IMG_Analyser_Bis( pst_World, i_Mission );
	return IMG_Analyser(pst_World, i_Mission);
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWAnalyser(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Mission;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Mission = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_VIEWAnalyser_C(pst_GO, i_Mission));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_VIEWAssignObject_C(OBJ_tdst_GameObject *pst_GO, int i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Old;
	WOR_tdst_View		*pst_View;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Get world of the object */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return NULL;

	AI_Check((ULONG) i_Num < pst_World->ul_NbViews, "Invalid view number");
	pst_View = pst_World->pst_View;
	pst_Old = pst_View[i_Num].pst_Father;
	pst_View[i_Num].pst_Father = pst_GO;
	return pst_Old;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWAssignObject(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Num = AI_PopInt();
	AI_PushGameObject(AI_EvalFunc_VIEWAssignObject_C(pst_GO, i_Num));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_VIEWCreate(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	WOR_tdst_World		*pst_World;
	int					i_Num;
	int					i_Par;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Par = AI_PopInt();
	i_Par = AI_PopInt();
	i_Par = AI_PopInt();
	i_Par = AI_PopInt();
	i_Par = AI_PopInt();

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return ++_pst_Node;
	i_Num = WOR_View_Create(pst_World);

	/* Assign the same display data as the first view */
#if defined(PSX2_TARGET) || defined(_GAMECUBE)
	pst_World->pst_View[i_Num].st_DisplayInfo.pst_DisplayDatas = GDI_gpst_CurDD;
#else
	pst_World->pst_View[i_Num].st_DisplayInfo.pst_DisplayDatas = pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas;
#endif
	AI_PushInt(i_Num);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_VIEWDestroy(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Num;
	WOR_tdst_World		*pst_World;
	WOR_tdst_View		*pst_View;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Num = AI_PopInt();

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return ++_pst_Node;

	AI_Check((ULONG) i_Num < pst_World->ul_NbViews, "Bad view index (too large)");
	pst_View = pst_World->pst_View;
	AI_Check(pst_View[i_Num].pfnv_ViewPointModificator, "Bad view index (hole)");

	/*
	 * Disactivate the view. Set pfnv_ViewPointModificator to NULL to indicate that
	 * the view is destroyed.
	 */
	WOR_View_Close(&pst_View[i_Num]);
	pst_View[i_Num].uc_Flags &= ~WOR_Cuc_View_Activ;
	pst_View[i_Num].pfnv_ViewPointModificator = NULL;

	return ++_pst_Node;
}

#ifdef ACTIVE_EDITORS
static OBJ_tdst_GameObject	ast_DummyObj[10];
static MATH_tdst_Matrix		ast_DummyMat[10];
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_VIEWGetObject_C(int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject		*pst_GO;
	WOR_tdst_View			*pst_View;
	WOR_tdst_World			*pst_World;
#ifdef ACTIVE_EDITORS
	GDI_tdst_DisplayData	*pst_DisplayDatas;

#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	pst_View = pst_World->pst_View;
	pst_GO = pst_View[_i_Num].pst_Father;

	/* Simulate an object in editor camera */
#ifdef ACTIVE_EDITORS
	pst_DisplayDatas = (GDI_tdst_DisplayData *) pst_View[_i_Num].st_DisplayInfo.pst_DisplayDatas;
	if(pst_DisplayDatas)
	{
		if
		(
			(!pst_DisplayDatas->uc_EngineCamera || !pst_GO)
		&&	(!pst_DisplayDatas->uc_EditorCamera || !pst_DisplayDatas->pst_EditorCamObject)
		)
		{
			pst_GO = &ast_DummyObj[_i_Num];
			pst_GO->sz_Name = "Dummy Obj For Camera";
			pst_GO->ul_IdentityFlags = 0;
			pst_GO->pst_GlobalMatrix = &ast_DummyMat[_i_Num];
			if(pst_DisplayDatas) CAM_SetObjectMatrixFromCam(pst_GO->pst_GlobalMatrix, &pst_DisplayDatas->st_Camera.st_Matrix);
		}
	}
#endif
	return pst_GO;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWGetObject(AI_tdst_Node *_pst_Node)
{
	AI_PushGameObject(AI_EvalFunc_VIEWGetObject_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_VIEWFocaleSet_C(int _i_Num, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	OBJ_tdst_GameObject		*pst_GO;
	GDI_tdst_DisplayData	*pst_DisplayDatas;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return;

	if(pst_World->pst_View[_i_Num].st_DisplayInfo.f_FieldOfVision == f) return;
	pst_World->pst_View[_i_Num].st_DisplayInfo.f_FieldOfVision = f;
	pst_World->pst_View[_i_Num].pfnv_ViewPointModificator(&pst_World->pst_View[_i_Num]);

#if defined(PSX2_TARGET) || defined(_GAMECUBE)
	pst_DisplayDatas = GDI_gpst_CurDD;
#else
	pst_DisplayDatas = (GDI_tdst_DisplayData *) pst_World->pst_View[_i_Num].st_DisplayInfo.pst_DisplayDatas;
#endif
	if(pst_DisplayDatas)
	{
		pst_DisplayDatas->st_Camera.f_FieldOfVision = f;

		CAM_AssignCameraToDevice
		(
			&pst_DisplayDatas->st_ScreenFormat,
			&pst_DisplayDatas->st_Camera,
			pst_DisplayDatas->st_Device.l_Width,
			pst_DisplayDatas->st_Device.l_Height
		);

		GDI_gpst_CurDD = pst_DisplayDatas;
		pst_DisplayDatas->st_GDI.pfnv_SetProjectionMatrix(&pst_DisplayDatas->st_Camera);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWFocaleSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~*/
	float	f;
	/*~~~~~~*/

	f = AI_PopFloat();
	AI_EvalFunc_VIEWFocaleSet_C(AI_PopInt(), f);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_VIEWFocaleGet_C(int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return 0;

	return pst_World->pst_View[_i_Num].st_DisplayInfo.f_FieldOfVision;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWFocaleGet(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_VIEWFocaleGet_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_VIEWViewportPosSet_C(int _i_Num, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	OBJ_tdst_GameObject *pst_GO;
	float				w, h;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return;

	w = pst_World->pst_View[_i_Num].st_DisplayInfo.f_ViewportWidth;
	h = pst_World->pst_View[_i_Num].st_DisplayInfo.f_ViewportHeight;

	WOR_View_SetViewport(&pst_World->pst_View[_i_Num], v->x, v->y, w, h);
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWViewportPosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v;
	/*~~~~~~~~~~~~~~~~~~~*/

	v = AI_PopVectorPtr();
	AI_EvalFunc_VIEWViewportPosSet_C(AI_PopInt(), v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_VIEWViewportPosGet_C(int _i_Num, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(pst_World)
	{
		v->x = pst_World->pst_View[_i_Num].st_DisplayInfo.f_ViewportLeft;
		v->y = pst_World->pst_View[_i_Num].st_DisplayInfo.f_ViewportTop;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWViewportPosGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	AI_EvalFunc_VIEWViewportPosGet_C(AI_PopInt(), &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_VIEWViewportSizeSet_C(int _i_Num, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	OBJ_tdst_GameObject *pst_GO;
	float				x, y;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return;

	x = pst_World->pst_View[_i_Num].st_DisplayInfo.f_ViewportLeft;
	y = pst_World->pst_View[_i_Num].st_DisplayInfo.f_ViewportTop;

	WOR_View_SetViewport(&pst_World->pst_View[_i_Num], x, y, v->x, v->y);
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWViewportSizeSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v;
	/*~~~~~~~~~~~~~~~~~~~*/

	v = AI_PopVectorPtr();
	AI_EvalFunc_VIEWViewportSizeSet_C(AI_PopInt(), v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_VIEWViewportSizeGet_C(int _i_Num, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(pst_World)
	{
		v->x = pst_World->pst_View[_i_Num].st_DisplayInfo.f_ViewportWidth;
		v->y = pst_World->pst_View[_i_Num].st_DisplayInfo.f_ViewportHeight;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWViewportSizeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	AI_EvalFunc_VIEWViewportSizeGet_C(AI_PopInt(), &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_VIEW3dWorldTo2d_C(int _i_Num, MATH_tdst_Vector *_3d, MATH_tdst_Vector *_2d)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	CAM_tdst_Camera			*pst_Cam;
	OBJ_tdst_GameObject		*pst_GO;
	GDI_tdst_DisplayData	*pst_DisplayDatas;
	WOR_tdst_WorldDisplay	*pst_DisplayInfo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(pst_World)
	{
		pst_DisplayInfo = &pst_World->pst_View[_i_Num].st_DisplayInfo;
#if defined(PSX2_TARGET) || defined(_GAMECUBE)
		pst_DisplayDatas = GDI_gpst_CurDD;
#else
		pst_DisplayDatas = (GDI_tdst_DisplayData *) pst_DisplayInfo->pst_DisplayDatas;
#endif
		if(pst_DisplayDatas)
		{
            if (pst_DisplayDatas->ul_DisplayInfo & GDI_Cul_DI_RenderingInterface)
                pst_Cam = &pst_DisplayDatas->st_SaveCamera;
            else
			    pst_Cam = &pst_DisplayDatas->st_Camera;
			pst_Cam->pst_ObjectToCameraMatrix = &pst_Cam->st_InverseMatrix;
			SOFT_TransformAndProject(_2d, _3d, 1, pst_Cam);
			_2d->x = 0.5f + (_2d->x - pst_Cam->f_CenterX) / pst_Cam->f_Width;
			_2d->y = 0.5f + (pst_Cam->f_CenterY - _2d->y) / pst_Cam->f_Height;
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEW3dWorldTo2d(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	src, dst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&src);
	AI_EvalFunc_VIEW3dWorldTo2d_C(AI_PopInt(), &src, &dst);
	AI_PushVector(&dst);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_VIEW2dTo3d_C(int _i_Num, MATH_tdst_Vector *_2d, MATH_tdst_Vector *_3d)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
    CAM_tdst_Camera			*pst_Cam;
	OBJ_tdst_GameObject		*pst_GO;
	GDI_tdst_DisplayData	*pst_DisplayDatas;
	WOR_tdst_WorldDisplay	*pst_DisplayInfo;
	MATH_tdst_Vector		v;
	MATH_tdst_Vector		v2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	MATH_CopyVector(&v2, _2d);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(pst_World)
	{
		pst_DisplayInfo = &pst_World->pst_View[_i_Num].st_DisplayInfo;
#if defined(PSX2_TARGET) || defined(_GAMECUBE)
		pst_DisplayDatas = GDI_gpst_CurDD;
#else
		pst_DisplayDatas = (GDI_tdst_DisplayData *) pst_DisplayInfo->pst_DisplayDatas;
#endif
		if(pst_DisplayDatas)
		{
            if (pst_DisplayDatas->ul_DisplayInfo & GDI_Cul_DI_RenderingInterface)
                pst_Cam = &pst_DisplayDatas->st_SaveCamera;
            else
			    pst_Cam = &pst_DisplayDatas->st_Camera;

			//v2.x *= pst_DisplayDatas->st_Camera.f_Width;
			//v2.y = (1.0f - v2.y) * pst_DisplayDatas->st_Camera.f_Height;
            
            v2.x = pst_Cam->f_CenterX + (v2.x - 0.5f) * pst_Cam->f_Width;
			v2.y = pst_Cam->f_CenterY - (v2.y - 0.5f) *  pst_Cam->f_Height;
			
            CAM_2Dto3DCamera( pst_Cam, &v, &v2);
			MATH_TransformVertex(_3d, &pst_Cam->st_Matrix, &v);
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEW2dTo3d(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	src, dst;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&src);
	AI_EvalFunc_VIEW2dTo3d_C(AI_PopInt(), &src, &dst);
	AI_PushVector(&dst);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_VIEWWidth_C(int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	OBJ_tdst_GameObject		*pst_GO;
	GDI_tdst_DisplayData	*pst_DisplayDatas;
	WOR_tdst_WorldDisplay	*pst_DisplayInfo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return 0;

	pst_DisplayInfo = &pst_World->pst_View[_i_Num].st_DisplayInfo;
#if defined(PSX2_TARGET) || defined(_GAMECUBE)
	pst_DisplayDatas = GDI_gpst_CurDD;
#else
	pst_DisplayDatas = (GDI_tdst_DisplayData *) pst_DisplayInfo->pst_DisplayDatas;
#endif
	if(pst_DisplayDatas)
		return pst_DisplayDatas->st_Device.l_Width;
	else
		return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWWidth(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_VIEWWidth_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_VIEWHeight_C(int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	OBJ_tdst_GameObject		*pst_GO;
	GDI_tdst_DisplayData	*pst_DisplayDatas;
	WOR_tdst_WorldDisplay	*pst_DisplayInfo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return 0;

	pst_DisplayInfo = &pst_World->pst_View[_i_Num].st_DisplayInfo;
#if defined(PSX2_TARGET) || defined(_GAMECUBE)
	pst_DisplayDatas = GDI_gpst_CurDD;
#else
	pst_DisplayDatas = (GDI_tdst_DisplayData *) pst_DisplayInfo->pst_DisplayDatas;
#endif
	if(pst_DisplayDatas)
		return pst_DisplayDatas->st_Device.l_Width;
	else
		return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWHeight(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_VIEWHeight_C(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns wether the current GameObject is culled or not within the given View.
 =======================================================================================================================
 */
int AI_EvalFunc_VIEWCulled_C(OBJ_tdst_GameObject *pst_GO, int i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	GDI_tdst_DisplayData	*pst_DisplayDatas;
	WOR_tdst_WorldDisplay	*pst_DisplayInfo;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return 0;

	pst_DisplayInfo = &pst_World->pst_View[i_Num].st_DisplayInfo;
	pst_World->pst_View[i_Num].pfnv_ViewPointModificator(&pst_World->pst_View[i_Num]);

#if defined(PSX2_TARGET) || defined(_GAMECUBE)
	pst_DisplayDatas = GDI_gpst_CurDD;
#else
	pst_DisplayDatas = (GDI_tdst_DisplayData *) pst_DisplayInfo->pst_DisplayDatas;
#endif
	if(!pst_DisplayDatas) return 0;
	CAM_SetCameraMatrix(&pst_DisplayDatas->st_Camera, &pst_World->pst_View[i_Num].st_ViewPoint);
	MATH_InvertMatrix(&pst_DisplayDatas->st_Camera.st_InverseMatrix, &pst_DisplayDatas->st_Camera.st_Matrix);

	/* We update the culling variables. */
	CAM_Engine_ComputePlans(&pst_DisplayDatas->st_Camera);
	OBJ_UpdateCullingVars(&pst_DisplayDatas->st_Camera);

	if(OBJ_CullingObject(pst_GO, &pst_DisplayDatas->st_Camera)) return 1;
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWCulled(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_VIEWCulled_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

extern void MEM_FreeTmp(void *p);

#ifdef _GAMECUBE
#define Photo_MaxSize		1792
#else
#define Photo_MaxSize		4096
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int VIEWSNAP_Slot = -1;

void AI_EvalFunc_VIEWSnapshot_DoIt()
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World				*pst_World;
	OBJ_tdst_GameObject			*pst_GO;
	GDI_tdst_DisplayData		*pst_DisplayDatas;
	WOR_tdst_WorldDisplay		*pst_DisplayInfo;
	GDI_tdst_Request_RWPixels	st_Capture;
	//char						sz_Name[260];
    int                         _i_Slot, i_Quality;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	
    if (VIEWSNAP_Slot == -1) return;
    _i_Slot = VIEWSNAP_Slot;
    VIEWSNAP_Slot = -1;
    

	AI_M_GetCurrentObject(pst_GO);
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(!pst_World) return;
		

	pst_DisplayInfo = &pst_World->pst_View[0].st_DisplayInfo;
#if defined(PSX2_TARGET) || defined(_GAMECUBE)
	pst_DisplayDatas = GDI_gpst_CurDD;
#else
	pst_DisplayDatas = (GDI_tdst_DisplayData *) pst_DisplayInfo->pst_DisplayDatas;
#endif
	if(!pst_DisplayDatas) return;

	MEM_ResetTmpMemory();
	MEM_gb_EnableTmp = TRUE;    
	
	GDI_gpst_CurDD = pst_DisplayDatas;

	st_Capture.c_Write = 0;
	//st_Capture.c_Buffer = 1;
    st_Capture.c_Buffer = 0;

	if(pst_DisplayDatas->st_Device.l_Width < 512)
	{
		st_Capture.x = 0;
		st_Capture.w = pst_DisplayDatas->st_Device.l_Width;
		st_Capture.w -= st_Capture.w % 4;
	}
	else
	{
		st_Capture.x = (pst_DisplayDatas->st_Device.l_Width - 512) / 2;
		st_Capture.w = 512;
	}

	if(pst_DisplayDatas->st_Device.l_Height < 256)
	{
		st_Capture.y = 0;
		st_Capture.h = pst_DisplayDatas->st_Device.l_Height;
	}
	else
	{
		st_Capture.y = (pst_DisplayDatas->st_Device.l_Height - 256) / 2;
		st_Capture.h = 256;
	}

	st_Capture.p_Bitmap = (char *) MEM_p_AllocTmp(st_Capture.w * st_Capture.h * 3);
	pst_DisplayDatas->st_GDI.pfnl_Request(GDI_Cul_Request_ReadScreen, (ULONG) & st_Capture);
	
	if((st_Capture.w == 512) && (st_Capture.h == 256))
	{
		TEX_Compress24_Xo2((unsigned char*)st_Capture.p_Bitmap, (unsigned char*)st_Capture.p_Bitmap, 512, 256);
		TEX_Compress24_Yo2((unsigned char*)st_Capture.p_Bitmap, (unsigned char*)st_Capture.p_Bitmap, 256, 256);
		st_Capture.w = 256;
		st_Capture.h = 128;
	}

	/* TGA Save */

	/*$F
    {
        TEX_tdst_File_Desc			st_Tex;

	    st_Tex.uw_Width = (unsigned short) st_Capture.w;
	    st_Tex.uw_Height = (unsigned short) st_Capture.h;
	    st_Tex.uc_BPP = 24;
	    st_Tex.uc_PaletteBPC = 0;
	    st_Tex.uw_PaletteLength = 0;
	    st_Tex.p_Bitmap = st_Capture.p_Bitmap;

	    sprintf(sz_Name, "photo%03d.tga", _i_Slot);
	    TEX_l_File_SaveTga(sz_Name, &st_Tex);
    }

    /* JPEG save */
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		JPEGDATA	st_JpegData;
//#ifndef PSX2_TARGET
		//L_FILE		x_File;
//#endif		
		char		*pc_Buffer, *pc_Cur;
#if !defined(_XBOX) && !defined(_XENON)
		char		*pc_Src;
		int			i, j;
#endif // _XBOX
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pc_Buffer = (char *) MEM_p_AllocTmp(st_Capture.w * st_Capture.h * 3);
		pc_Cur = pc_Buffer;

#ifdef PSX2_TARGET
		/* no vertical mirror and inversion of colours */
		pc_Src = st_Capture.p_Bitmap;
		for(j = 0; j < st_Capture.h; j++)
		{
			for(i = 0; i < st_Capture.w; i++)
			{
				*pc_Cur++ = pc_Src[2];
				*pc_Cur++ = pc_Src[1];
				*pc_Cur++ = pc_Src[0];
				pc_Src += 3;
			}
		}

#else

#if defined(_XBOX) || defined(_XENON)
		/* no vertical mirror and no inversion of colours */
		memcpy (pc_Cur, st_Capture.p_Bitmap, st_Capture.h * st_Capture.w * 3);

#else // _XBOX

		/* vertical mirror and inversion of colours */
		for(j = 0; j < st_Capture.h; j++)
		{
			pc_Src = ((char *) st_Capture.p_Bitmap) + (((st_Capture.h - 1) - j) * st_Capture.w * 3);
			for(i = 0; i < st_Capture.w; i++)
			{
				*pc_Cur++ = pc_Src[2];
				*pc_Cur++ = pc_Src[1];
				*pc_Cur++ = pc_Src[0];
				pc_Src += 3;
			}
		}

#endif // _XBOX
#endif

		i_Quality = 70;
        do
        {
		    L_memset(&st_JpegData, 0, sizeof(JPEGDATA));
		    st_JpegData.width = st_Capture.w;
		    st_JpegData.height = st_Capture.h;
		    st_JpegData.ptr = (unsigned char*)pc_Buffer;
		    st_JpegData.quality = i_Quality;
		    st_JpegData.output_file = (void *) st_Capture.p_Bitmap;
		    lJADE_JPEG_INPOUT_FILE_LENGHT = 0;
		    JpegWrite(&st_JpegData);
            i_Quality -= 5;
        }
        while( lJADE_JPEG_INPOUT_FILE_LENGHT > Photo_MaxSize );
		SAV_PhotoSet(_i_Slot, st_Capture.p_Bitmap, lJADE_JPEG_INPOUT_FILE_LENGHT);
		MEM_Free(pc_Buffer);
	}

	MEM_Free(st_Capture.p_Bitmap);

	MEM_gb_EnableTmp = FALSE;    
}
/**/
void AI_EvalFunc_VIEWSnapshot_C(int _i_Slot)
{
    VIEWSNAP_Slot = _i_Slot;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWSnapshot(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_VIEWSnapshot_C(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_VIEWDrawMaskSet_C( int i_One, int i_Zero )
{
    GDI_tdst_DisplayData		*pst_DisplayDatas;

    pst_DisplayDatas = GDI_gpst_CurDD;
    if (!pst_DisplayDatas) return;

    pst_DisplayDatas->ul_DrawMask |= i_One;
    pst_DisplayDatas->ul_DrawMask &= ~i_Zero;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWDrawMaskSet( AI_tdst_Node *_pst_Node )
{
    int i_One, i_Zero;

    i_Zero = AI_PopInt();
    i_One = AI_PopInt();
    AI_EvalFunc_VIEWDrawMaskSet_C( i_One, i_Zero );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_VIEWForceNoFog_C( int i_Value )
{
    GDI_tdst_DisplayData		*pst_DisplayDatas;

    pst_DisplayDatas = GDI_gpst_CurDD;
    if (!pst_DisplayDatas) return;

    if (i_Value)
        pst_DisplayDatas->st_Fog.c_Flag |= SOFT_C_ForceNoFog;
    else    
        pst_DisplayDatas->st_Fog.c_Flag &= ~SOFT_C_ForceNoFog;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWForceNoFog( AI_tdst_Node *_pst_Node )
{
    AI_EvalFunc_VIEWForceNoFog_C( AI_PopInt() );
    return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_VIEWSetMode_C( int i_Value )
{
#ifdef PSX2_TARGET
	extern u32 ps2_ScreenMode;
#endif
#ifdef _XBOX
	extern u_int32 TVMode;
#endif
#ifdef _GAMECUBE
	extern int GXI_iTVMode;
#endif // _GAMECUBE


	GDI_tdst_DisplayData		*pst_DisplayDatas;
    
    pst_DisplayDatas = GDI_gpst_CurDD;
    if (!pst_DisplayDatas) return 0;
    
    if (i_Value == 0)
    {
#ifdef PSX2_TARGET
		return ps2_ScreenMode + 1;
#endif
#ifdef _GAMECUBE
		return GXI_iTVMode + 1;
#endif	
#ifdef _XBOX
/*	if (TVMode==0) 
		return 1;
	else if (TVMode==1) 
		return 2; 
	else if (TVMode==2) 
		return 2;
	else return 0;*/
		return TVMode;
#endif
		if (pst_DisplayDatas->st_ScreenFormat.l_ScreenRatioConst == GDI_Cul_SRC_4over3)
		{
			if ( pst_DisplayDatas->st_ScreenFormat.ul_Flags & GDI_Cul_SFF_169BlackBand )
				return 1;
			return 2;
		}
		else if (pst_DisplayDatas->st_ScreenFormat.l_ScreenRatioConst == GDI_Cul_SRC_16over9)
			return 3;
		return 0;
	}
#ifdef _XBOX
	if ( TVMode==3 ) return GDI_Cul_SRC_16over9;
#endif

	if (i_Value == 1)
	{
#ifdef _GAMECUBE
		GXI_SwitchDisplay(0);
		return GDI_Cul_SRC_4over3;
#endif	
#ifdef PSX2_TARGET
		ps2_ScreenMode = 0;
		return GDI_Cul_SRC_4over3;
#endif
#ifdef _XBOX
		TVMode=1;
		Gx8_SwitchDisplay();
		return GDI_Cul_SRC_4over3;
#endif		
		pst_DisplayDatas->st_ScreenFormat.l_ScreenRatioConst = GDI_Cul_SRC_4over3;
		pst_DisplayDatas->st_ScreenFormat.ul_Flags |= GDI_Cul_SFF_169BlackBand;
		return pst_DisplayDatas->st_ScreenFormat.l_ScreenRatioConst;
	}
			
    if (i_Value == 2)
    {
#ifdef _GAMECUBE
		GXI_SwitchDisplay(1);
		return GDI_Cul_SRC_4over3;
#endif	
#ifdef PSX2_TARGET
		ps2_ScreenMode = 1;
		return GDI_Cul_SRC_4over3;
#endif
#ifdef _XBOX
		TVMode=2;
		Gx8_SwitchDisplay();
		return GDI_Cul_SRC_4over3;
#endif		
		pst_DisplayDatas->st_ScreenFormat.l_ScreenRatioConst = GDI_Cul_SRC_4over3;
		pst_DisplayDatas->st_ScreenFormat.ul_Flags &= ~GDI_Cul_SFF_169BlackBand;
		return pst_DisplayDatas->st_ScreenFormat.l_ScreenRatioConst;
	}
	
	if (i_Value == 3)
	{
#ifdef _GAMECUBE
		GXI_SwitchDisplay(2);
		return GDI_Cul_SRC_16over9;
#endif	
#ifdef PSX2_TARGET
		ps2_ScreenMode = 2;
		return GDI_Cul_SRC_16over9;
#endif		

#ifdef _XBOX
		{
			u_int32 STVMode=TVMode;
			TVMode=3;
			Gx8_SwitchDisplay();
			if ( TVMode==3 )
			return GDI_Cul_SRC_16over9;
			else
			{
				if (STVMode==2 ) TVMode=1;//STVMode;
				else if (STVMode==1 ) TVMode=2;
				return GDI_Cul_SRC_4over3;
			}
		}
#endif	

		pst_DisplayDatas->st_ScreenFormat.l_ScreenRatioConst = GDI_Cul_SRC_16over9;
		pst_DisplayDatas->st_ScreenFormat.ul_Flags &= ~GDI_Cul_SFF_169BlackBand;
		
	

		return pst_DisplayDatas->st_ScreenFormat.l_ScreenRatioConst;
	}
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIEWSetMode( AI_tdst_Node *_pst_Node )
{
    AI_PushInt( AI_EvalFunc_VIEWSetMode_C( AI_PopInt() ) );
    return ++_pst_Node;
}


/*
 =======================================================================================================================
	Play a BINK HERE
 =======================================================================================================================
 */
void VIDEO_PLAY(ULONG ulVideoNumber)
{
	/*
		0 -> NEWGAME.BIK
		1 -> BONUS_0.BIK
		2 -> BONUS_1.BIK
		...
	*/ 
#ifdef PSX2_TARGET
	{
		extern void Bink_IA(u32 Mode);
		Bink_IA(ulVideoNumber);
	}
#endif	
#ifdef _GAMECUBE
	GC_VideoFullScreenCreate(ulVideoNumber);
#endif // _GAMECUBE

#ifdef _XBOX
		AI_VideoLauncher(ulVideoNumber);
#endif
#ifdef _XENON
		char* szVideoName = NULL;

		switch( ulVideoNumber )
		{
			case 1:
				szVideoName = "GAME:\\Video\\IntPJ.wmv";
				break;
			case 3:
				szVideoName = "GAME:\\Video\\IntPB.wmv";
				break;
			case 4:
				szVideoName = "GAME:\\Video\\Trailer.wmv";
				break;
		}

		if( szVideoName )
		{
			//g_pVideoManager->Play( szVideoName, FALSE, 3000 );
			g_pVideoManager->PlayExclusive(szVideoName);
		}
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_VIDEO_PLAY( AI_tdst_Node *_pst_Node )
{
	VIDEO_PLAY(AI_PopInt());
    return ++_pst_Node;
}

/*
 =======================================================================================================================
	Set key of video to play for next loading
	( 0xFFFFFFFF => no video for next loading ) 
 =======================================================================================================================
 */
ULONG	AI_gul_AlternativeVideoKey = 0;
void VIDEO_SETKEY( ULONG ul_VideoKey )
{
	AI_gul_AlternativeVideoKey = ul_VideoKey;
}
/**/
AI_tdst_Node *AI_EvalFunc_VIDEO_SETKEY( AI_tdst_Node *_pst_Node )
{
	VIDEO_SETKEY(AI_PopInt());
    return ++_pst_Node;
}

#if (defined(PSX2_TARGET) || defined(_GAMECUBE)) && defined(__cplusplus)
}
#endif
