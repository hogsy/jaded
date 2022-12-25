/*$T WORinit.c GC! 1.081 05/17/01 14:28:37 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "stdio.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINks/LINKmsg.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKtoed.h"

#include "MATHs/MATH.h"
#include "TIMer/PROfiler/PROdefs.h"

#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIengine.h"
#include "TIMer/PROfiler/PROdefs.h"
#include "TIMer/TIM.h"


#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/EOT/EOTinit.h"
#include "ENGine/Sources/OBJects/OBJ.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/ANImation/ANIinit.h"

#include "ENGine/Sources/INTersection/INTinit.h"
#include "INTersection/INTSnP.h"

#include "AIinterp/Sources/AIdebug.h"

#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "GFX/GFX.h"

#if defined(_XBOX)
#include "GX8/AfterFX/Gx8AfterFX_Def.h"
#endif

#ifdef _XENON
#include "XenonGraphics/XeRenderer.h"
#endif

#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "SELection/SELection.h"
#endif

#include "INOut/INOsaving.h"



extern BOOL WOR_gb_FinalWorld;
extern BOOL WOR_gb_RealAllocWorld;

extern OBJ_tdst_GameObject **AI_gppst_GeneratedObject_Destroyed;
extern int AI_gi_GeneratedObject_Destroyed;

extern BOOL		GDI_gb_IsSplitScreen;
extern int		GDI_gi_SplitViewIdx;


/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/* Rasters for a world. */
#ifdef RASTERS_ON
typedef struct	WOR_tdst_WorldRasters_
{
	PRO_tdst_TrameRaster	st_ActivObjects;
	PRO_tdst_TrameRaster	st_Anims;
	PRO_tdst_TrameRaster	st_Dyna;
	PRO_tdst_TrameRaster	st_AI;
	PRO_tdst_TrameRaster	st_ColMap;
	PRO_tdst_TrameRaster	st_ZDM;
	PRO_tdst_TrameRaster	st_ZDE;
	PRO_tdst_TrameRaster	st_Events;
	PRO_tdst_TrameRaster	st_Hierarchy;
} WOR_tdst_WorldRasters;
#endif
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    First initialisation of universe:: empty universe
 =======================================================================================================================
 */
void WOR_Universe_Init(void)
{
	L_memset(&WOR_gst_Universe, 0, sizeof(WOR_tdst_Universe));
	TAB_PFtable_Init(&WOR_gst_Universe.st_WorldsTable, WOR_C_MaxNbWorlds, TAB_Cf_IgnoreHoles);
}

/*
 =======================================================================================================================
    Aim:    Last desinit of universe
 =======================================================================================================================
 */
void WOR_Universe_Destroy(void)
{
	/*
	 * j'ai changé le 0 en 3 dans WOR_Universe_Close sinon en sortie d'éditeur des
	 * mondes pouvaient ne pas être libéré (par exemple 2 monde chargé à la suite dans
	 * la même vue 3D)
     * 3 signifie : destruction totale (pas de test de reference)
	 */
	WOR_Universe_Close(3);
	TAB_PFtable_Close(&WOR_gst_Universe.st_WorldsTable);
}

/*
 =======================================================================================================================
    Aim:    Initialisation of the universe

    In:     universe AI key
 =======================================================================================================================
 */
void WOR_Universe_Open(BIG_KEY _ul_AIKey)
{
	/* Close before if already opened */
	WOR_Universe_Close(0);

	/* Read zi ai */
	if(_ul_AIKey == BIG_C_InvalidKey)
		WOR_gst_Universe.pst_AI = NULL;
	else
		/* Load AI instance */
		WOR_gst_Universe.pst_AI = AI_pst_LoadInstance(_ul_AIKey);
}

/*
 =======================================================================================================================
    Aim:    Destruction of the universe
 =======================================================================================================================
 */
void WOR_Universe_Close(LONG _l_Destroy)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem *pst_CurrentElem;
	TAB_tdst_PFelem *pst_EndElem;
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	AI_Reset();
#endif
	/* Read the number of worlds in the universe */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	if(!pst_CurrentElem) return;
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_World = (WOR_tdst_World *) pst_CurrentElem->p_Pointer;
		if(!TAB_b_IsAHole(pst_World))
		{
			WOR_Universe_DeleteWorld(pst_World, (char) _l_Destroy);
#ifdef ACTIVE_EDITORS
			if(!pst_World->c_EditorReferences) pst_CurrentElem->ul_Flags = 0;
#endif
		}
	}

	/* Remove IA */
	if(WOR_gst_Universe.pst_AI)
	{
		AI_FreeInstance(WOR_gst_Universe.pst_AI);
		WOR_gst_Universe.pst_AI = NULL;
	}

	/* Global save buffer */
	L_memset(INO_gst_SavManager.ac_CurrSlotBuffer, 0, INO_Cte_SavUniverseMaxSize);

#if !defined(XML_CONV_TOOL)
	/* No more activated world */
	MAI_gst_MainHandles.pst_World = NULL;
#endif

#ifdef ACTIVE_EDITORS
	LINK_UpdatePointers();
#endif
	/* Reset engine status */
	UNI_Status() = UNI_Cuc_Reset;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    This function makes the interface between the position of the father and the position of the activator.
 =======================================================================================================================
 */
void WOR_View_FollowFather(struct WOR_tdst_View_ *pst_View)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix		st_Matrix;
#ifdef ACTIVE_EDITORS
	GDI_tdst_DisplayData	*pst_DD;

#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	pst_DD = (GDI_tdst_DisplayData *) pst_View->st_DisplayInfo.pst_DisplayDatas;
	if(pst_DD && ((!(pst_DD->uc_EngineCamera)) && (!(pst_DD->uc_EditorCamera)) || !(pst_View->pst_Father)))
	{
		/* A camera matrix cannot be directly put in a view matrix due to the Y,Z problem */
		CAM_SetObjectMatrixFromCam(&st_Matrix, &pst_DD->st_Camera.st_Matrix);
		WOR_View_SetViewPoint(pst_View, &st_Matrix, &MATH_gst_NulVector, TRUE, FALSE);
	}
	else
	{
#endif
		if(pst_View->pst_Father)
		{
			/* We make a matrix using the position and (if exist) orientation of the father */
			OBJ_MakeAbsoluteMatrix(pst_View->pst_Father, &st_Matrix);

			/* Set the position, and calculate the speed of the world view */
			WOR_View_SetViewPoint(pst_View, &st_Matrix, &MATH_gst_NulVector, TRUE, FALSE);
		}

#ifdef ACTIVE_EDITORS
	}

#endif
}

/*
 =======================================================================================================================
    Aim:    Initialises a world view
 =======================================================================================================================
 */
void WOR_View_Init
(
	WOR_tdst_View		*_pst_View,
	MATH_tdst_Matrix	*_pst_ViewPoint,
	OBJ_tdst_GameObject *_pst_Father
)
{
	L_memset(_pst_View, 0, sizeof(WOR_tdst_View));
	_pst_View->uc_Flags |= WOR_Cuc_View_Activ;

	MATH_CopyMatrix(&_pst_View->st_ViewPoint, _pst_ViewPoint);
	MATH_CopyMatrix(&_pst_View->st_OldViewPoint, _pst_ViewPoint);
	_pst_View->pst_Father = _pst_Father;

	/*
	 * Assign the default function that copies the position of the father to the
	 * position of the activator. If NULL, view is considered as deleted !!!
	 */
	_pst_View->pfnv_ViewPointModificator = WOR_View_FollowFather;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int WOR_View_Create(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	UINT						i_Num;
#ifndef ACTIVE_EDITORS
#if !defined _XBOX && !defined(_XENON)
	extern GDI_tdst_DisplayData *GDI_pst_GlobalDD;
#endif // _XBOX
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Search a hole */
#ifdef ACTIVE_EDITORS
	for(i_Num = 4; i_Num < _pst_World->ul_NbViews; i_Num++)
	{
		if(!_pst_World->pst_View[i_Num].pfnv_ViewPointModificator) break;
	}

#else
	for(i_Num = 0; i_Num < _pst_World->ul_NbViews; i_Num++)
	{
		if(!_pst_World->pst_View[i_Num].pfnv_ViewPointModificator) break;
	}

#endif
	/* No hole. Create a new view */
	if(i_Num >= _pst_World->ul_NbViews)
	{
		i_Num = _pst_World->ul_NbViews;
		if(!_pst_World->ul_NbViews)
		{
			_pst_World->pst_View = (WOR_tdst_View *) MEM_p_VMAlloc(sizeof(WOR_tdst_View));
		}
		else
		{
			_pst_World->pst_View = (WOR_tdst_View *) MEM_p_VMRealloc
				(
					_pst_World->pst_View,
					(_pst_World->ul_NbViews + 1) * sizeof(WOR_tdst_View)
				);
		}

		_pst_World->ul_NbViews++;
	}

	/* Init the view */
	WOR_View_Init
	(
		&_pst_World->pst_View[i_Num],
		&MATH_gst_IdentityMatrix,
		NULL
	);

	/* Set default value for focale and viewport of view */
	WOR_View_SetFocale(&_pst_World->pst_View[i_Num], 1.0f);
	WOR_View_SetViewport(&_pst_World->pst_View[i_Num], 0.0f, 0.0f, 1.0f, 1.0f);

#ifndef ACTIVE_EDITORS
//#ifndef _XBOX && !defined(_XENON)
#if !defined(_XBOX) && !defined(_XENON)
	_pst_World->pst_View[i_Num].st_DisplayInfo.pst_DisplayDatas = GDI_pst_GlobalDD;
#endif // _XBOX
#endif

	_pst_World->pst_View[i_Num].st_DisplayInfo.f_FieldOfVision = _pst_World->pst_View[0].st_DisplayInfo.f_FieldOfVision;

	return i_Num;
}

/*
 =======================================================================================================================
    Aim:    Removes a world view
 =======================================================================================================================
 */
void WOR_View_Close(WOR_tdst_View *_pst_View)
{
	/* check used view */
	if(_pst_View->pfnv_ViewPointModificator == NULL) return;
}

#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
    Aim:    Create another active view. The 2 active views are displayed side by side
			(the viewports parameters are modified to obtain this effect).
 =======================================================================================================================
 */
BOOL WOR_View_SplitView(WOR_tdst_World * _pst_World)
{
	struct WOR_tdst_View_	*pst_View, *pst_LastView;
	WOR_tdst_WorldDisplay	*pst_ViewDispInfo;

	if (_pst_World == NULL) return FALSE;
	
	if (_pst_World->b_IsSplitScreen) return TRUE;

	_pst_World->i_SplitViewIdx = WOR_View_Create(_pst_World);
	if (_pst_World->i_SplitViewIdx >= _pst_World->ul_NbViews || _pst_World->i_SplitViewIdx < 0) return FALSE;

	WOR_View_Init (&_pst_World->pst_View[_pst_World->i_SplitViewIdx], &MATH_gst_IdentityMatrix, NULL);

	pst_ViewDispInfo = &(_pst_World->pst_View[_pst_World->i_SplitViewIdx].st_DisplayInfo);

	pst_ViewDispInfo->pst_DisplayDatas = NULL;
	pst_View = _pst_World->pst_View;
	pst_LastView = pst_View + _pst_World->ul_NbViews;
	for(; pst_View < pst_LastView; pst_View++) {
		if (pst_View != &_pst_World->pst_View[_pst_World->i_SplitViewIdx]) {
			if ( (pst_View->uc_Flags & WOR_Cuc_View_Activ) && (pst_View->st_DisplayInfo.pst_DisplayDatas == GDI_gpst_CurDD)) {
				// Copy datas of the active view
				L_memcpy( pst_ViewDispInfo, &pst_View->st_DisplayInfo, sizeof (WOR_tdst_WorldDisplay));
				pst_ViewDispInfo->f_ViewportWidth /=  2;
				pst_ViewDispInfo->f_ViewportLeft = 0.5f;
			}
			// Resize
			pst_View->st_DisplayInfo.f_ViewportWidth /= 2;
			pst_View->st_DisplayInfo.f_ViewportLeft = 0.0f;
		}
	}
	_pst_World->pst_View[_pst_World->i_SplitViewIdx].uc_Flags |= WOR_Cuc_View_SplitView;

	_pst_World->b_IsSplitScreen = TRUE;

	if (pst_ViewDispInfo->pst_DisplayDatas == NULL) {
		WOR_View_UnsplitView(_pst_World);
		return FALSE;
	}

	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Create another active view. The 2 active views are displayed side by side
			(the viewports parameters are modified to obtain this effect).
 =======================================================================================================================
 */
void WOR_View_UnsplitView(WOR_tdst_World * _pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_View		*pst_View, *pst_LastView;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_World) return;
	if (!_pst_World->b_IsSplitScreen) return;
	_pst_World->b_IsSplitScreen = FALSE;

	pst_View = _pst_World->pst_View;
	pst_LastView = pst_View + _pst_World->ul_NbViews;
	for(; pst_View < pst_LastView; pst_View++) {     // REMARK : we should use saved f_ViewportWidth
		// Resize
		pst_View->st_DisplayInfo.f_ViewportWidth *= 2;
	}
	/*
	 * Disable the view. Set pfnv_ViewPointModificator to NULL to indicate that
	 * the view is destroyed.
	 */
	WOR_View_Close(&_pst_World->pst_View[_pst_World->i_SplitViewIdx]);
	_pst_World->pst_View[_pst_World->i_SplitViewIdx].uc_Flags &= ~WOR_Cuc_View_Activ;
	_pst_World->pst_View[_pst_World->i_SplitViewIdx].uc_Flags &= ~WOR_Cuc_View_SplitView;
	_pst_World->pst_View[_pst_World->i_SplitViewIdx].pfnv_ViewPointModificator = NULL;
}
#endif

/*
 =======================================================================================================================
    Aim:    Set focale of view
 =======================================================================================================================
 */
void WOR_View_SetFocale(WOR_tdst_View *_pst_View, float _f_FOV)
{
	_pst_View->st_DisplayInfo.f_FieldOfVision = _f_FOV;
}

/*
 =======================================================================================================================
    Aim:    Set viewport of view. Location (x,y) and size (w,h) have to be given in ratio of real size (a w set to .5
            indicate that viewport will have a half device size).
 =======================================================================================================================
 */
void WOR_View_SetViewport(WOR_tdst_View *_pst_View, float x, float y, float w, float h)
{
	_pst_View->st_DisplayInfo.f_ViewportLeft = x;
	_pst_View->st_DisplayInfo.f_ViewportTop = y;
	_pst_View->st_DisplayInfo.f_ViewportWidth = w;
	_pst_View->st_DisplayInfo.f_ViewportHeight = h;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Create an empty world, WOR_World_Init have to be called after
 =======================================================================================================================
 */
WOR_tdst_World *WOR_pst_World_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	int				iSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/


	iSize = sizeof(WOR_tdst_World);
#ifdef ACTIVE_EDITORS
#ifdef RASTERS_ON
	iSize += sizeof(WOR_tdst_WorldRasters);
#endif
#endif
	if(WOR_gb_RealAllocWorld)
		pst_World = (WOR_tdst_World *) MEM_p_VMAlloc(iSize);
	else
		pst_World = (WOR_tdst_World *) MEM_p_AllocTmp(iSize);
	L_memset(pst_World, 0, iSize);

//#ifdef ODE_INSIDE && !defined(XML_CONV_TOOL)
#if defined(ODE_INSIDE) && !defined(XML_CONV_TOOL)
	pst_World->ode_id_world = dWorldCreate();
	pst_World->ode_id_space = dSimpleSpaceCreate(0);
//	pst_World->ode_id_space = dHashSpaceCreate(0);
//	dHashSpaceSetLevels(pst_World->ode_id_space, 1, 5);
	dSpaceSetCleanup(pst_World->ode_id_space, 0);

	dWorldSetGravity (pst_World->ode_id_world, 0.0f, 0.0f, -20.0f);

//	dWorldSetERP (pst_World->ode_id_world, 0.2f);
//	dWorldSetCFM (pst_World->ode_id_world, 1E-5f);

	pst_World->ode_joint_dyna = dJointGroupCreate(100);
	pst_World->ode_joint_col_jade = dJointGroupCreate(100);
	pst_World->ode_joint_col_ode = dJointGroupCreate(100);
	pst_World->ode_joint_ode = dJointGroupCreate(25);

	dJointGroupEmpty(pst_World->ode_joint_dyna);
	dJointGroupEmpty(pst_World->ode_joint_col_jade);
	dJointGroupEmpty(pst_World->ode_joint_col_ode);
	dJointGroupEmpty(pst_World->ode_joint_ode);
	pst_World->ode_contacts_num_jade = 0;
	pst_World->ode_contacts_num_ode = 0;
#endif


	return pst_World;
}

/*
 =======================================================================================================================
    Aim:    Destroy a world -> empty data and delete world memory block
 =======================================================================================================================
 */
BOOL WOR_World_Destroy(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
#ifdef RASTERS_ON
	WOR_tdst_WorldRasters	*pst_Rasters;
#endif
#endif
	extern void STR_3DStringList_Clear( void );
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#if defined(_XBOX)
	extern Gx8_tdst_SpecificData   *p_gGx8SpecificData;
#endif

	if(!_pst_World) return FALSE;
	
	STR_3DStringList_Clear();

	{
		extern void WTR_BeginLoadWorldHook(void);
		WTR_BeginLoadWorldHook();
	}

//#ifdef ODE_INSIDE && !defined(XML_CONV_TOOL)
#if defined(ODE_INSIDE) && !defined(XML_CONV_TOOL)
	dJointGroupEmpty (_pst_World->ode_joint_col_jade);
	dJointGroupEmpty (_pst_World->ode_joint_col_ode);
	dJointGroupEmpty (_pst_World->ode_joint_ode);
	dJointGroupEmpty (_pst_World->ode_joint_dyna);

	dSpaceDestroy(_pst_World->ode_id_space);
#endif
	
	/* Close world. Exit if there's still something */
	if(WOR_World_Close(_pst_World)) return TRUE;

#ifdef ODE_INSIDE

	dJointGroupDestroy (_pst_World->ode_joint_col_jade);
	dJointGroupDestroy (_pst_World->ode_joint_col_ode);
	dJointGroupDestroy (_pst_World->ode_joint_ode);
	dJointGroupDestroy (_pst_World->ode_joint_dyna);

	/* dWorldDestroy must be called AFTER the Jade Closing World (Bodies destroyed by the function) */
	dWorldDestroy(_pst_World->ode_id_world);
#endif


#ifdef ACTIVE_EDITORS
#ifdef RASTERS_ON
	pst_Rasters = (WOR_tdst_WorldRasters *) (_pst_World + 1);
	PRO_KillRaster(&pst_Rasters->st_ActivObjects);
	PRO_KillRaster(&pst_Rasters->st_Anims);
	PRO_KillRaster(&pst_Rasters->st_Dyna);
	PRO_KillRaster(&pst_Rasters->st_AI);
	PRO_KillRaster(&pst_Rasters->st_ColMap);
	PRO_KillRaster(&pst_Rasters->st_ZDM);
	PRO_KillRaster(&pst_Rasters->st_ZDE);
	PRO_KillRaster(&pst_Rasters->st_Events);
	PRO_KillRaster(&pst_Rasters->st_Hierarchy);
#endif
#endif
	/* Delete pointer from loading tables */
	LOA_DeleteAddress(_pst_World);
	LINK_DelRegisterPointer(_pst_World);

	/* Close and destroy world */
	MEM_Free(_pst_World);

#if !defined(XML_CONV_TOOL)
	/* Is it current active world ? */
	if(MAI_gst_MainHandles.pst_World == _pst_World)
	{
		MAI_gst_MainHandles.pst_World = NULL;
		if(MAI_gst_MainHandles.pst_DisplayData) MAI_gst_MainHandles.pst_DisplayData->pst_World = NULL;
	}
#endif

#ifdef ACTIVE_EDITORS
	ERR_gpst_ContextGAO = NULL;
	ERR_gpsz_ContextString = NULL;
#endif

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Initialise a world£

    In:     _pst_World      :Pointer to a world (must be non nul pointer)
            _ul_NbObjects   :Number of objects to create 

    Note:   This function does: £
            Create the table of all the objects £
            Create the table of activ objects (one for the whole world) £
            Create the table of activators £
            Create the table of world views £
 =======================================================================================================================
 */
void WOR_World_Init(WOR_tdst_World *_pst_World, ULONG _ul_NbObjects)
{
	/*~~~~~~*/
#ifdef ACTIVE_EDITORS
	ULONG	i;

#endif
	/*~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Create the table of all the objects references
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* Create an empty table of _ul_NbObjects references */
	_pst_World->ul_NbTotalGameObjects = _ul_NbObjects;
	TAB_PFtable_Init(&_pst_World->st_AllWorldObjects, _ul_NbObjects, TAB_Cf_IgnoreHoles);

	/* Create an empty table with 0 object for graphic objects references */
	TAB_Ptable_Init(&_pst_World->st_GraphicObjectsTable, 0, 0.1f);

	/* Create an empty table with 0 object for graphic materials references */
	TAB_Ptable_Init(&_pst_World->st_GraphicMaterialsTable, 0, 0.1f);

	TAB_PFtable_Init(&_pst_World->st_ActivObjects, WOR_C_MaxNbActObjects, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&_pst_World->st_VisibleObjects, WOR_C_MaxNbVisObjects, TAB_Cf_IgnoreHoles);
	TAB_PFtable_Init(&_pst_World->st_Lights, WOR_C_MaxNbLights, TAB_Cf_IgnoreHoles);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Create the table of world views
	 -------------------------------------------------------------------------------------------------------------------
	 */

	_pst_World->ul_NbViews = 0;
	_pst_World->pst_View = NULL;
#ifdef ACTIVE_EDITORS
	for(i = 0; i < 4; i++) WOR_View_Create(_pst_World);
#else
	WOR_View_Create(_pst_World);
#endif
	/* Create all the Engine Objects Tables of the World */
	EOT_SetOfEOT_Init(&_pst_World->st_EOT);

	/* Default refresh time of the activation and visibility */
	_pst_World->b_ForceActivationRefresh = TRUE;
	_pst_World->b_ForceVisibilityRefresh = TRUE;
	_pst_World->b_ForceBVRefresh = TRUE;

	/* No network */
	_pst_World->pst_AllNetworks = NULL;

	/* SnP Allocation */
	if(WOR_gb_FinalWorld) INT_SnP_Alloc(_pst_World);

#ifdef ACTIVE_EDITORS

	/* Init selection list */
	_pst_World->pst_Selection = (SEL_tdst_Selection *) MEM_p_Alloc(sizeof(SEL_tdst_Selection));
	SEL_Init(_pst_World->pst_Selection);

	_pst_World->b_IsSplitScreen = FALSE;
	_pst_World->i_SplitViewIdx = -1;
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Rasters for that world
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
#ifdef RASTERS_ON
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_WorldRasters	*pst_Rasters;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Rasters = (WOR_tdst_WorldRasters *) (_pst_World + 1);
		PRO_FirstInitTrameRaster
		(
			&pst_Rasters->st_ActivObjects,
			"Tables",
			_pst_World->sz_Name,
			"Active Objects",
			PRO_E_Table,
			(ULONG) & _pst_World->st_ActivObjects
		);
		PRO_FirstInitTrameRaster
		(
			&pst_Rasters->st_Anims,
			"Tables",
			_pst_World->sz_Name,
			"Anims",
			PRO_E_Table,
			(ULONG) & _pst_World->st_EOT.st_Anims
		);
		PRO_FirstInitTrameRaster
		(
			&pst_Rasters->st_Dyna,
			"Tables",
			_pst_World->sz_Name,
			"Dyna",
			PRO_E_Table,
			(ULONG) & _pst_World->st_EOT.st_Dyna
		);
		PRO_FirstInitTrameRaster
		(
			&pst_Rasters->st_AI,
			"Tables",
			_pst_World->sz_Name,
			"AI",
			PRO_E_Table,
			(ULONG) & _pst_World->st_EOT.st_AI
		);
		PRO_FirstInitTrameRaster
		(
			&pst_Rasters->st_ColMap,
			"Tables",
			_pst_World->sz_Name,
			"ColMap",
			PRO_E_Table,
			(ULONG) & _pst_World->st_EOT.st_ColMap
		);
		PRO_FirstInitTrameRaster
		(
			&pst_Rasters->st_ZDM,
			"Tables",
			_pst_World->sz_Name,
			"ZDM",
			PRO_E_Table,
			(ULONG) & _pst_World->st_EOT.st_ZDM
		);
		PRO_FirstInitTrameRaster
		(
			&pst_Rasters->st_ZDE,
			"Tables",
			_pst_World->sz_Name,
			"ZDE",
			PRO_E_Table,
			(ULONG) & _pst_World->st_EOT.st_ZDE
		);
		PRO_FirstInitTrameRaster
		(
			&pst_Rasters->st_Events,
			"Tables",
			_pst_World->sz_Name,
			"Events",
			PRO_E_Table,
			(ULONG) & _pst_World->st_EOT.st_Events
		);
		PRO_FirstInitTrameRaster
		(
			&pst_Rasters->st_Hierarchy,
			"Tables",
			_pst_World->sz_Name,
			"Hierarchy",
			PRO_E_Table,
			(ULONG) & _pst_World->st_EOT.st_Hierarchy
		);
	}

#endif
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_World_Attached( WOR_tdst_World *_pst_World )
{
    if (_pst_World)
    {
        GFX_Attached( _pst_World->pst_GFX );
        GFX_Attached( _pst_World->pst_GFXInterface );
    }
}

#ifdef ACTIVE_EDITORS
#ifdef RASTERS_ON

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_World_KillRasters(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_WorldRasters	*pst_Rasters;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Rasters = (WOR_tdst_WorldRasters *) (_pst_World + 1);
	PRO_KillRaster(&pst_Rasters->st_ActivObjects);
	PRO_KillRaster(&pst_Rasters->st_Anims);
	PRO_KillRaster(&pst_Rasters->st_Dyna);
	PRO_KillRaster(&pst_Rasters->st_AI);
	PRO_KillRaster(&pst_Rasters->st_ColMap);
	PRO_KillRaster(&pst_Rasters->st_ZDM);
	PRO_KillRaster(&pst_Rasters->st_ZDE);
	PRO_KillRaster(&pst_Rasters->st_Events);
	PRO_KillRaster(&pst_Rasters->st_Hierarchy);
}

#endif
#endif

extern void AI_EvalFunc_WORPreLoadTex_C(OBJ_tdst_GameObject *_pt_Ref);

/*
 =======================================================================================================================
    Aim:    Removes a whole world from the memory
 =======================================================================================================================
 */
BOOL WOR_World_Close(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem;		/* Pointer to the element of the objects pointers table */
	TAB_tdst_PFelem		*pst_LastElem;	/* For the end of the loop */
	OBJ_tdst_GameObject *pst_Object;
	UINT				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_World) return FALSE;

#ifdef _XENON // make sure our rendering thread has finished with all the objects before destroying them

    g_oXeRenderer.WaitForEventComplete(eXeThreadEvent_Flip);

#endif

    /* Close GFX */
	GFX_Free(&_pst_World->pst_GFX, 0);
	GFX_Free(&_pst_World->pst_GFXInterface, 0);

	/* Loop thru the objects of the world to remove them, (if loaded into memory) */
	pst_Elem = &_pst_World->st_AllWorldObjects.p_Table[0];
	pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem;
	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		pst_Object = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
		if(!TAB_b_IsAHole(pst_Object))
		{
		    AI_EvalFunc_WORPreLoadTex_C(NULL);
		    
			if(OBJ_b_GameObject_isLoaded(pst_Object))
			{
				if(!(pst_Object->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone)) 
				{
					WOR_World_DetachObject(_pst_World, pst_Object);
					OBJ_GameObject_Remove(pst_Object, 1);
				}
			}
		}
	}

	/* Objects générés détruit par le TTL */
	if(AI_gppst_GeneratedObject_Destroyed)
	{
		int i;

		for(i = 0; i < AI_gi_GeneratedObject_Destroyed; i++)
		{
			MEM_Free(AI_gppst_GeneratedObject_Destroyed[i]);
		}
		AI_gi_GeneratedObject_Destroyed = 0;

		MEM_Free(AI_gppst_GeneratedObject_Destroyed);
		AI_gppst_GeneratedObject_Destroyed = NULL;
	}

	GRO_Struct_DestroyTable(&_pst_World->st_GraphicObjectsTable, 1);
	GRO_Struct_DestroyTable(&_pst_World->st_GraphicMaterialsTable, 2);

	/* Loop thru the views and remove them */
	for(i = 0; i < _pst_World->ul_NbViews; i++)
	{
		WOR_View_Close(&_pst_World->pst_View[i]);
	}

	/* Remove all the world views table */
	if(_pst_World->pst_View) MEM_Free(_pst_World->pst_View);

	/* Remove all the references */
	TAB_PFtable_Close(&_pst_World->st_AllWorldObjects);

	/* Remove all the activ objects */
	TAB_PFtable_Close(&_pst_World->st_ActivObjects);

	/* Remove all the activ objects */
	TAB_PFtable_Close(&_pst_World->st_VisibleObjects);

	/* Remove all the activ objects */
	TAB_PFtable_Close(&_pst_World->st_Lights);

	/* Remove all the Engine Objects Tables of the World */
	EOT_SetOfEOT_Close(&_pst_World->st_EOT);

	/* Remove networks */
	if(_pst_World->pst_AllNetworks)
	{
		WAY_RemoveAllNetworks(_pst_World->pst_AllNetworks);
		_pst_World->pst_AllNetworks = NULL;
	}

	/* Remove grid */
	if(_pst_World->pst_Grid) GRI_Destroy(_pst_World->pst_Grid);
	if(_pst_World->pst_Grid1) GRI_Destroy(_pst_World->pst_Grid1);

	INT_SnP_DesAlloc(_pst_World);

#if !defined(XML_CONV_TOOL)
	/* Is it the activated world */
	if(MAI_gst_MainHandles.pst_World == _pst_World) MAI_gst_MainHandles.pst_World = NULL;
#endif

#ifdef ACTIVE_EDITORS
	SEL_Close(_pst_World->pst_Selection);
	MEM_SafeFree(_pst_World->pst_Selection);
	LINK_UpdatePointers();
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Delete rasters
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
#ifdef RASTERS_ON
	WOR_World_KillRasters(_pst_World);
#endif
#endif

	return FALSE;
}

