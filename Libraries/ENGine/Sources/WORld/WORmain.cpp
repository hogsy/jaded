/*$T WORmain.c GC 1.139 03/26/04 13:48:18 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include <algorithm>

/* Aim: Main functions of the world module */
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/WORld/WORsecto.h"
#include "ENGine/Sources/OBJects/OBJ.h"
#include "ENGine/Sources/OBJects/OBJculling.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "MATHs/MATH.h"
#include "LINks/LINKmsg.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/EOT/EOT.h"
#include "AIinterp/Sources/AIengine.h"

#ifdef ACTIVE_EDITORS
#include "SOFT/SOFTHelper.h"
#include "BIGfiles/BIGread.h"
#include "EDIpaths.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#include "TIMer/TIMdefs.h"
#include "TIMer/PROfiler/PROdefs.h"

extern BOOL ENG_gb_ActiveSectorization;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_ActivateObjectIfFlagsAllow(OBJ_tdst_GameObject *_pst_Object, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~*/
	BOOL	res;
	/*~~~~~~~~*/

	if(OBJ_b_TestStatusFlag(_pst_Object, OBJ_C_StatusFlag_Active)) return;
	res = WOR_SectoObjActive(_pst_World, _pst_Object);

	if
	(
		(!OBJ_b_TestControlFlag(_pst_Object, OBJ_C_ControlFlag_ForceInactive))
	&&	(!OBJ_b_TestControlFlag(_pst_Object, OBJ_C_ControlFlag_InPause))
	&&	res
	)
	{
		/* Doit on réinitialiser l'objet car il était désactivé par la secto ? */
		if
		(
			(_pst_Object->ul_StatusAndControlFlags & OBJ_C_ControlFlag_SectoInactive)
		&&	(_pst_Object->ul_StatusAndControlFlags & OBJ_C_ControlFlag_SectoReinit)
		)
		{
			ENG_ReinitOneObject(_pst_Object, 0);
		}

		if(_pst_Object->ul_StatusAndControlFlags & OBJ_C_ControlFlag_SectoInactive)
		{
			AI_ExecCallback(_pst_Object, AI_C_Callback_SectoActOff);
		}

		_pst_Object->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_SectoInactive;
		TAB_PFtable_AddElemWithData(&_pst_World->st_ActivObjects, (void *) _pst_Object, _pst_Object->ul_IdentityFlags);
		OBJ_SetStatusFlag(_pst_Object, OBJ_C_StatusFlag_Active);
	}

	/* Secto */
	if(!res) 
	{
		extern void SND_RegisterInactiveGAO(OBJ_tdst_GameObject*);
		if(!(_pst_Object->ul_StatusAndControlFlags & OBJ_C_ControlFlag_SectoInactive)) 
		{
			OBJ_SetInactive(_pst_Object);
			AI_ExecCallback(_pst_Object, AI_C_Callback_SectoActOn);
		}

		_pst_Object->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_SectoInactive;
		
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_MakeObjectVisibleIfFlagsAllow(OBJ_tdst_GameObject *_pst_Object, WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL	b_DisplayInvisible;
	BOOL	res;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(OBJ_b_TestStatusFlag(_pst_Object, OBJ_C_StatusFlag_Visible)) return;

	b_DisplayInvisible = FALSE;
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GDI_tdst_DisplayData	*pst_DD;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_DD = (GDI_tdst_DisplayData*)_pst_World->pst_View->st_DisplayInfo.pst_DisplayDatas;
		b_DisplayInvisible = ( pst_DD && (pst_DD->ul_DisplayFlags & GDI_Cul_DF_DisplayInvisible) );
	}
#endif

	res = WOR_SectoObjVisible(_pst_World, _pst_Object);
	if
	(
		(b_DisplayInvisible || (!OBJ_b_TestControlFlag(_pst_Object, OBJ_C_ControlFlag_ForceInvisible)))
	&&	res
	)
	{
		if(_pst_Object->ul_StatusAndControlFlags & OBJ_C_ControlFlag_SectoInvisible)
			AI_ExecCallback(_pst_Object, AI_C_Callback_SectoVisOff);

		_pst_Object->ul_StatusAndControlFlags &= ~OBJ_C_ControlFlag_SectoInvisible;

		assert( _pst_World->st_VisibleObjects_ActiveIterators == 0 );

		WOR_World_VisibleObjectsVector *w_visible_objects = ( WOR_World_VisibleObjectsVector * ) ( _pst_World->st_VisibleObjects );
		w_visible_objects->push_back( _pst_Object );

		OBJ_SetStatusFlag(_pst_Object, OBJ_C_StatusFlag_Visible);
		if(_pst_Object->ul_IdentityFlags & OBJ_C_IdentityFlag_Lights)
		{
			WOR_World_LightsVector *world_lights = ( WOR_World_LightsVector * ) ( _pst_World->st_Lights );
			world_lights->push_back(_pst_Object);
		}
	}
	else
	{
		if(!res) 
		{
			if(!(_pst_Object->ul_StatusAndControlFlags & OBJ_C_ControlFlag_SectoInvisible))
				AI_ExecCallback(_pst_Object, AI_C_Callback_SectoVisOn);
			_pst_Object->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_SectoInvisible;
		}

		_pst_Object->uc_LOD_Vis = 0;
		_pst_Object->ul_StatusAndControlFlags |= OBJ_C_StatusFlag_Culled;
	}
}

/*
 =======================================================================================================================
    Aim: Refresh World Active Objects Table.
 =======================================================================================================================
 */
void WOR_World_ActivateObjects(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	TAB_PFtable_Clear(&_pst_World->st_ActivObjects);

	/* Recreate the Active Objects table from the AllWorldObjects table. */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_CurrentGO)) continue;

		if(pst_CurrentGO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Active)
			OBJ_ClearStatusFlag(pst_CurrentGO, OBJ_C_StatusFlag_Active);

		if(pst_CurrentGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone) continue;

		WOR_ActivateObjectIfFlagsAllow(pst_CurrentGO, _pst_World);
	}
}

/*
 =======================================================================================================================
    Aim: Build the tables of visible objects for all the world views.
 =======================================================================================================================
 */
void WOR_World_MakeObjectsVisible(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef RASTERS_ON
	PRO_StartTrameRaster(&ENG_gpst_RasterEng_Visibility);
#endif
	assert( _pst_World->st_VisibleObjects_ActiveIterators == 0 );
	WOR_World_VisibleObjectsVector *w_visible_objects = ( WOR_World_VisibleObjectsVector * ) ( _pst_World->st_VisibleObjects );
	w_visible_objects->clear();

	WOR_World_LightsVector *world_lights = ( WOR_World_LightsVector * ) ( _pst_World->st_Lights );
	world_lights->clear();

	/* Recreate the Active Objects table from the AllWorldObjects table. */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_AllWorldObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_AllWorldObjects);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_CurrentGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_CurrentGO)) continue;

		if(pst_CurrentGO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Visible)
			OBJ_ClearStatusFlag(pst_CurrentGO, OBJ_C_StatusFlag_Visible);

		if(pst_CurrentGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone) continue;

		WOR_MakeObjectVisibleIfFlagsAllow(pst_CurrentGO, _pst_World);
	}

#ifdef RASTERS_ON
	PRO_StopTrameRaster(&ENG_gpst_RasterEng_Visibility);
#endif
}

/*
 =======================================================================================================================
    Aim: Add a loaded object (its memory pointer, and its 32 bit "on the disk" reference) to the table of all the
    objects of a world Note: Use AddUnloadedObject to add an object which is not already loaded.
 =======================================================================================================================
 */
void WOR_World_AddLoadedObject(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_Object, ULONG _ul_DiskReference)
{
	/* We add the object in the references table */
	if(TAB_ul_PFtable_GetElemIndexWithPointer(&_pst_World->st_AllWorldObjects, _pst_Object) == TAB_Cul_BadIndex)
	{
		TAB_PFtable_AddElemWithDataAndResize(&_pst_World->st_AllWorldObjects, _pst_Object, _ul_DiskReference);
	}

	/* Add Object in World tables , except if the object is a bone */
	if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_Bone))
	{
		WOR_ActivateObjectIfFlagsAllow(_pst_Object, _pst_World);
		WOR_MakeObjectVisibleIfFlagsAllow(_pst_Object, _pst_World);
	}

#ifdef ACTIVE_EDITORS
	_pst_Object->pst_World = _pst_World;
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_World_DetachObject(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_CurrentGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_HasChild)
	{
		pst_Elem = _pst_World->st_AllWorldObjects.p_Table;
		pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem;

		for(; pst_Elem < pst_LastElem; pst_Elem++)
		{
			pst_CurrentGO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
			if(TAB_b_IsAHole(pst_CurrentGO)) continue;

			if(pst_CurrentGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			{
				if(pst_CurrentGO && pst_CurrentGO->pst_Base && pst_CurrentGO->pst_Base->pst_Hierarchy)
				{
					if(pst_CurrentGO->pst_Base->pst_Hierarchy->pst_Father == _pst_GO)
					{
						pst_CurrentGO->pst_Base->pst_Hierarchy->pst_Father = NULL;
						pst_CurrentGO->pst_Base->pst_Hierarchy->pst_FatherInit = NULL;
						pst_CurrentGO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Hierarchy;
						MEM_SafeFree(pst_CurrentGO->pst_Base->pst_Hierarchy);
					}
				}
				else
				{
					ERR_X_Warning(0, "Bad GAO pointer, please call one programmer for debugging !!", NULL);
				}
			}
		}
	}

	/* Delete object in World list */
	TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_AllWorldObjects, _pst_GO);

	/* Delete object in Active list */
	TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_ActivObjects, _pst_GO);

	/* Delete object in Visible list */
	{
		assert( _pst_World->st_VisibleObjects_ActiveIterators == 0 );

		WOR_World_VisibleObjectsVector *w_visible_objects = ( WOR_World_VisibleObjectsVector * ) ( _pst_World->st_VisibleObjects );
		w_visible_objects->erase( std::remove( w_visible_objects->begin(), w_visible_objects->end(), _pst_GO ), w_visible_objects->end() );
	}

	/* Delete object in Light list */
	if ( OBJ_b_TestIdentityFlag( _pst_GO, OBJ_C_IdentityFlag_Lights ) )
	{
		WOR_World_LightsVector *world_lights = ( WOR_World_LightsVector * ) ( _pst_World->st_Lights );

		auto it = std::find_if( world_lights->begin(), world_lights->end(), [ & ]( OBJ_tdst_GameObject * &elem )
			                    { return elem == _pst_GO; } );

		assert( it != world_lights->end() );
		world_lights->erase( it );
	}

	/* Delete object from EOT tables. */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu))
		TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_EOT.st_Visu, _pst_GO);
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
		TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_EOT.st_Anims, _pst_GO);
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Dyna))
		TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_EOT.st_Dyna, _pst_GO);
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_AI))
		TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_EOT.st_AI, _pst_GO);
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
		TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_EOT.st_ColMap, _pst_GO);
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDM))
		TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_EOT.st_ZDM, _pst_GO);
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ZDE))
		TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_EOT.st_ZDE, _pst_GO);
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Events))
		TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_EOT.st_Events, _pst_GO);
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_EOT.st_Hierarchy, _pst_GO);
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ODE))
		TAB_PFtable_RemoveElemWithPointer(&_pst_World->st_EOT.st_ODE, _pst_GO);


#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		WOR_tdst_View	*pst_View, *pst_LastView;
		int				i;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		/* Delete object in helpers */
		pst_View = _pst_World->pst_View;
		pst_LastView = pst_View + _pst_World->ul_NbViews;
		for(; pst_View < pst_LastView; pst_View++)
		{
			if(!(pst_View->uc_Flags & WOR_Cuc_View_Activ)) continue;
			if(!pst_View->st_DisplayInfo.pst_DisplayDatas) continue;
			SOFT_Helpers_DetachObject
			(
				((GDI_tdst_DisplayData *) pst_View->st_DisplayInfo.pst_DisplayDatas)->pst_Helpers,
				_pst_GO
			);
		}

		/* delete show vector using object deleted */
		for(i = 0; i < _pst_World->uc_ShowVector; i++)
		{
			if(_pst_GO == _pst_World->ast_ShowVector[i].pst_Gao)
			{
				_pst_World->uc_ShowVector--;
				if(i < _pst_World->uc_ShowVector)
				{
					L_memcpy
					(
						_pst_World->ast_ShowVector + i,
						_pst_World->ast_ShowVector + _pst_World->uc_ShowVector,
						sizeof(WOR_tdst_ShowVector)
					);
				}
			}
		}
	}
#endif

	// hogsy: HACK - set the world to null, as after detatch this will be accessed again per GameObject_Remove after the fact which causes issues...
	_pst_GO->pst_World = NULL;
}

/*
 =======================================================================================================================
    Aim: Sets the absolute position and or orientation of a world biew, with possibility of directly setting the speed
    In: _pst_View::: The world view _pst_ViewPoint::: The wanted position/orientation of the activator _pst_Speed:::
    The wanted speed, (if _b_AutoCalculateSpeed ) Is false _b_AutoCalculateSpeed::: Indicates if the speed should be
    calculated or not. It set to true, the given speed is not taken into acount. _b_KeepOldOrientation::: Indicates if
    the old orientation should be kept or not (if not, the orientation given in the matrix is used)
 =======================================================================================================================
 */
void WOR_View_SetViewPoint
(
	WOR_tdst_View		*_pst_View,
	MATH_tdst_Matrix	*_pst_ViewPoint,
	MATH_tdst_Vector	*_pst_Speed,
	BOOL				_b_AutoCalculateSpeed,
	BOOL				_b_KeepOldOrientation
)
{
	if(_b_KeepOldOrientation)
	{
		MATH_CopyMatrix(&_pst_View->st_OldViewPoint, &_pst_View->st_ViewPoint);
		MATH_SetTranslation(&_pst_View->st_ViewPoint, MATH_pst_GetTranslation(_pst_ViewPoint));
	}
	else
	{
		MATH_CopyMatrix(&_pst_View->st_OldViewPoint, &_pst_View->st_ViewPoint);
		MATH_CopyMatrix(&_pst_View->st_ViewPoint, _pst_ViewPoint);
	}
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim: Gets the BoneGO associated to a given Gizmo. The Algo is based on the Fact that the Hierarchy of the Gizmo and
    the Associated Bone is linked and is the same.
 =======================================================================================================================
 */
OBJ_tdst_GameObject *WOR_pst_GetBoneFromGizmo(OBJ_tdst_GameObject *_pst_Gizmo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_BoneGO;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(OBJ_b_TestIdentityFlag(_pst_Gizmo, OBJ_C_IdentityFlag_Anims)) return _pst_Gizmo;
	if(!(OBJ_b_TestIdentityFlag(_pst_Gizmo, OBJ_C_IdentityFlag_Hierarchy))) return NULL;

	pst_World = _pst_Gizmo->pst_World;
	pst_Elem = pst_World->st_AllWorldObjects.p_Table;
	pst_LastElem = pst_World->st_AllWorldObjects.p_NextElem;

	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
		if(TAB_b_IsAHole(pst_BoneGO)) continue;

		if
		(
			!(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Bone))
		||	!(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_Hierarchy))
		) continue;

		if(pst_BoneGO->pst_Base->pst_Hierarchy == _pst_Gizmo->pst_Base->pst_Hierarchy) return pst_BoneGO;
	}

	return NULL;
}

/*
 =======================================================================================================================
    Aim: Gets the GizmoObject associated to a given Bone. The Algo is based on the Fact that the Hierarchy of the Gizmo
    and the Associated Bone is linked and is the same.
 =======================================================================================================================
 */
OBJ_tdst_GameObject *WOR_pst_GetGizmoFromBone(OBJ_tdst_GameObject *_pst_Bone)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_GizmoGO;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(OBJ_b_TestIdentityFlag(_pst_Bone, OBJ_C_IdentityFlag_Anims)) return _pst_Bone;
	if(!(OBJ_b_TestIdentityFlag(_pst_Bone, OBJ_C_IdentityFlag_Bone))) return NULL;
	if(!(OBJ_b_TestIdentityFlag(_pst_Bone, OBJ_C_IdentityFlag_Hierarchy))) return NULL;

	pst_World = _pst_Bone->pst_World;
	pst_Elem = pst_World->st_AllWorldObjects.p_Table;
	pst_LastElem = pst_World->st_AllWorldObjects.p_NextElem;

	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		pst_GizmoGO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
		if(TAB_b_IsAHole(pst_GizmoGO)) continue;

		if
		(
			!(pst_GizmoGO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
		||	!(OBJ_b_TestIdentityFlag(pst_GizmoGO, OBJ_C_IdentityFlag_Hierarchy))
		) continue;

		if(pst_GizmoGO->pst_Base->pst_Hierarchy == _pst_Bone->pst_Base->pst_Hierarchy) return pst_GizmoGO;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *WOR_pst_GetGizmoSymetric(OBJ_tdst_GameObject *_pst_GizmoGO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_BoneGO, *pst_BoneSym;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BoneGO = WOR_pst_GetBoneFromGizmo(_pst_GizmoGO);
	pst_BoneSym = WOR_pst_GetSymetric(pst_BoneGO);
	if(!pst_BoneSym) return NULL;
	return WOR_pst_GetGizmoFromBone(pst_BoneSym);
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *WOR_pst_GetSymetric(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_World;
	OBJ_tdst_GameObject *pst_Father, *pst_Child, *pst_GO;
	OBJ_tdst_GameObject *apst_Child[10];
	int					i_Num, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO) return NULL;

#ifdef ACTIVE_EDITORS
	if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject) return WOR_pst_GetGizmoSymetric(_pst_GO);
#endif
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	if(!pst_World) return NULL;

	pst_Father = OBJ_pst_GetFather(_pst_GO);
	if(!pst_Father) return NULL;

	pst_Child = _pst_GO;

	i_Num = 0;
	while(WOR_i_GetAllChildsOf(pst_World, pst_Father, apst_Child, FALSE) == 1)
	{
		pst_Child = pst_Father;
		pst_Father = OBJ_pst_GetFather(pst_Father);
		if(!pst_Father) return NULL;
		i_Num++;
	}

	if((apst_Child[0] != pst_Child) && (apst_Child[1] != pst_Child)) return NULL;

	pst_GO = (apst_Child[0] == pst_Child) ? apst_Child[1] : apst_Child[0];

	for(i = 0; i < i_Num; i++)
	{
		if(WOR_i_GetAllChildsOf(pst_World, pst_GO, apst_Child, FALSE) != 1) return NULL;
		pst_GO = apst_Child[0];
	}

	return pst_GO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int WOR_i_GetAllChildsOf
(
	WOR_tdst_World		*_pst_World,
	OBJ_tdst_GameObject *_pst_Father,
	OBJ_tdst_GameObject **_ppst_Childs,
	BOOL				b_KeepGizmo
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_Father;
	int					iNum;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	if(_pst_Father->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
		pst_Father = WOR_pst_GetBoneFromGizmo(_pst_Father);
	else
		pst_Father = _pst_Father;
#else
	pst_Father = _pst_Father;
#endif
	if(!pst_Father) return 0;

	iNum = 0;
	pst_Elem = _pst_World->st_AllWorldObjects.p_Table;
	pst_LastElem = _pst_World->st_AllWorldObjects.p_NextElem;
	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
		if(TAB_b_IsAHole(pst_GO)) continue;

#ifdef ACTIVE_EDITORS
		if(!b_KeepGizmo && (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)) continue;
#endif
		if(!OBJ_b_TestStatusFlag(pst_GO, OBJ_C_StatusFlag_Active)) continue;
		if(OBJ_b_TestControlFlag(pst_GO, OBJ_C_ControlFlag_ForceInactive)) continue;

		if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		{
			/* Normal case */
			if(pst_GO->pst_Base->pst_Hierarchy->pst_Father == pst_Father)
			{
				if(_ppst_Childs) _ppst_Childs[iNum++] = pst_GO;
				continue;
			}
		}
	}

	return iNum;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_RecomputeAllChilds(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	WOR_tdst_World		*pst_World;
	OBJ_tdst_GameObject *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Father = pst_GO;
	pst_World = WOR_World_GetWorldOfObject(pst_Father);

	pst_Elem = pst_World->st_AllWorldObjects.p_Table;
	pst_LastElem = pst_World->st_AllWorldObjects.p_NextElem;

	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		if(TAB_b_IsAHole(pst_Elem->p_Pointer)) continue;
		pst_GO = (OBJ_tdst_GameObject *) (pst_Elem->p_Pointer);
		if(pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) continue;
		if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)) continue;
		if(pst_GO->pst_Base->pst_Hierarchy->pst_Father != pst_Father) continue;
		OBJ_ComputeLocalWhenHie(pst_GO);
	}
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_AddDelObj(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GO, void *_p_Data)
{
	/*~~*/
	int i;
	/*~~*/

	/* Deja la ? */
	for(i = 0; i < _pst_World->i_NumDelObj; i++)
	{
		if(_pst_World->ast_DelObj[i].pst_GAO == _pst_GO && _pst_World->ast_DelObj[i].p_Data == _p_Data)
			return;
	}

	for(i = 0; i < _pst_World->i_NumDelObj; i++)
	{
		if(!_pst_World->ast_DelObj[i].pst_GAO) break;
	}

	if(i == _pst_World->i_NumDelObj) _pst_World->i_NumDelObj++;
	ERR_X_Assert(_pst_World->i_NumDelObj != MAX_DEL_OBJ);
	_pst_World->ast_DelObj[i].pst_GAO = _pst_GO;
	_pst_World->ast_DelObj[i].p_Data = _p_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_DelObjData(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GO, void *_p_Data)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < _pst_World->i_NumDelObj; i++)
	{
		if(_pst_World->ast_DelObj[i].pst_GAO == _pst_GO && _pst_World->ast_DelObj[i].p_Data == _p_Data)
		{
			_pst_World->ast_DelObj[i].pst_GAO = NULL;
			*(int *) _pst_World->ast_DelObj[i].p_Data = 0;
			return;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_DelObj(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < _pst_World->i_NumDelObj; i++)
	{
		if(_pst_World->ast_DelObj[i].pst_GAO == _pst_GO)
		{
			_pst_World->ast_DelObj[i].pst_GAO = NULL;
			if(*(int *) _pst_World->ast_DelObj[i].p_Data == (int) _pst_GO)
				*(int *) _pst_World->ast_DelObj[i].p_Data = 0;
		}
	}
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
