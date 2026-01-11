/*$T COLaccess.h GC! 1.081 03/07/01 16:08:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __COL_ACCESS_H__
#define __COL_ACCESS_H__

#ifndef PSX2_TARGET
#pragma once
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif /* _cplusplcus */

/* GC Dependencies 11/22/99 */
#include "MATHs/MATH.h"
#include "COLstruct.h"
#include "COLconst.h"
#include "BASe/BASsys.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"

/*$F
 =======================================================================================================================
		
										ACCESS FROM GO

 =======================================================================================================================
 */

_inline_ COL_tdst_Instance *COL_pst_GetInstance(OBJ_tdst_GameObject *_pst_GO, BOOL _b_Check)
{
	if((_b_Check) && (!_pst_GO || !_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Col))
	{
		return NULL;
	}

	return ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ COL_tdst_ColMap *COL_pst_GetColMap(OBJ_tdst_GameObject *_pst_GO, BOOL _b_Check)
{
	if((_b_Check) && (!_pst_GO || !_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Col))
	{
		return NULL;
	}

	return ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ COL_tdst_DetectionList *COL_pst_GetDetectionList(OBJ_tdst_GameObject *_pst_GO, BOOL _b_Check)
{
	if((_b_Check) && (!_pst_GO || !_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Col))
	{
		return NULL;
	}

	return ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_List;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ COL_tdst_ColSet *COL_pst_GetColSet(OBJ_tdst_GameObject *_pst_GO, BOOL _b_Check)
{
	if
	(
		(_b_Check)
	&&	(!_pst_GO || !_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Col)
	||	!(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance)
	)
	{
		return NULL;
	}

	return ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_ColSet;
}

/*$F
 ===================================================================================================


										SHAPE FUNCTIONS


 ===================================================================================================
 */

_inline_ MATH_tdst_Vector *COL_pst_Shape_GetCenter(void *_pst_Shape)
{
	return(&((COL_tdst_Sphere *) _pst_Shape)->st_Center);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float COL_f_Shape_GetRadius(void *_pst_Shape)
{
	return(((COL_tdst_Sphere *) _pst_Shape)->f_Radius);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Shape_SetRadius(void *_pst_Shape, float _f_Radius)
{
	((COL_tdst_Sphere *) _pst_Shape)->f_Radius = _f_Radius;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Shape_SetHeight(void *_pst_Shape, float _f_Height)
{
	((COL_tdst_Cylinder *) _pst_Shape)->f_Height = _f_Height;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *COL_pst_Shape_GetMax(void *_pst_Shape)
{
	return(&((COL_tdst_Box *) _pst_Shape)->st_Max);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *COL_pst_Shape_GetMin(void *_pst_Shape)
{
	return(&((COL_tdst_Box *) _pst_Shape)->st_Min);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Vector *COL_pst_Shape_GetBase(void *_pst_Shape)
{
	return(&((COL_tdst_Cone *) _pst_Shape)->st_Base);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float COL_f_Shape_GetHeight(void *_pst_Shape)
{
	return(((COL_tdst_Cone *) _pst_Shape)->f_Height);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float COL_f_Shape_GetAngle(void *_pst_Shape)
{
	return(((COL_tdst_Cone *) _pst_Shape)->f_Angle);
}

/*$F
 ===================================================================================================


										COB FUNCTIONS


 ===================================================================================================
 */

_inline_ BOOL COL_b_Cob_TestFlag(COL_tdst_Cob *_pst_Cob, UCHAR _uc_Flag)
{
	return(_pst_Cob->uc_Flag & _uc_Flag);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Cob_SetFlag(COL_tdst_Cob *_pst_Cob, UCHAR _uc_Flag)
{
	_pst_Cob->uc_Flag |= _uc_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Cob_ResetFlag(COL_tdst_Cob *_pst_Cob, UCHAR _uc_Flag)
{
	_pst_Cob->uc_Flag &= ~_uc_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Cob_CopyFlag(COL_tdst_Cob *_pst_Cob, UCHAR _uc_Flag)
{
	_pst_Cob->uc_Flag = _uc_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ ULONG COL_Cob_GetType(COL_tdst_Cob *_pst_Cob)
{
	return _pst_Cob->uc_Type;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL COL_b_Cob_TestType(COL_tdst_Cob *_pst_Cob, UCHAR _uc_Type)
{
	return(_pst_Cob->uc_Type == _uc_Type);
}

/*$F
 ===================================================================================================


										COLMAP FUNCTIONS


 ===================================================================================================
 */

/*
 =======================================================================================================================
    Aim:    Looks in the ColMap's uc_Activation flag to determine if the (Index)ieme Cob is active or not.
 =======================================================================================================================
 */
_inline_ BOOL COL_b_ColMap_IsActive(COL_tdst_ColMap *_pst_ColMap, UCHAR _uc_CobIndex)
{
	return _pst_ColMap->uc_Activation & ((UCHAR) 1 << _uc_CobIndex);
}

/*
 =======================================================================================================================
    Aim:    Set the _uc_CobIndex's bit of the ColMap's uc_Activation Flag.
 =======================================================================================================================
 */
_inline_ void COL_ColMap_SetActivationFlag(COL_tdst_ColMap *_pst_ColMap, UCHAR _uc_CobIndex)
{
	_pst_ColMap->uc_Activation |= ((UCHAR) 1 << _uc_CobIndex);
}

/*
 =======================================================================================================================
    Aim:    Reset the _uc_CobIndex's bit of the ColMap's uc_Activation Flag.
 =======================================================================================================================
 */
_inline_ void COL_ColMap_ResetActivationFlag(COL_tdst_ColMap *_pst_ColMap, UCHAR _uc_CobIndex)
{
	_pst_ColMap->uc_Activation &= ~((UCHAR) 1 << _uc_CobIndex);
}

/*
 =======================================================================================================================
    Aim:    Returns the Engine Index of a Cob.
 =======================================================================================================================
 */
_inline_ UCHAR COL_uc_ColMap_GetEngineIndexWithCob(COL_tdst_ColMap *_pst_ColMap, COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob	**dpst_Cob, **dpst_LastCob;
	UCHAR			uc_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dpst_Cob = _pst_ColMap->dpst_Cob;
	dpst_LastCob = dpst_Cob + _pst_ColMap->uc_NbOfCob;
	for(uc_Index = 0; dpst_Cob < dpst_LastCob; dpst_Cob++, uc_Index++)
	{
		if(*dpst_Cob == _pst_Cob) return uc_Index;
	}

	return (UCHAR) -1;
}

/*$F
 ===================================================================================================


										ZONE FUNCTIONS


 ===================================================================================================
 */

_inline_ BOOL COL_b_Zone_TestFlag(COL_tdst_ZDx *_pst_Zone, UCHAR _uc_Flag)
{
	return(_pst_Zone->uc_Flag & _uc_Flag);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL COL_b_Zone_TestType(COL_tdst_ZDx *_pst_Zone, UCHAR _uc_Type)
{
	return(_pst_Zone->uc_Type == _uc_Type);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Zone_SetFlag(COL_tdst_ZDx *_pst_Zone, UCHAR _uc_Flag)
{
	_pst_Zone->uc_Flag |= _uc_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Zone_ResetFlag(COL_tdst_ZDx *_pst_Zone, UCHAR _uc_Flag)
{
	_pst_Zone->uc_Flag &= ~_uc_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Zone_CopyFlag(COL_tdst_ZDx *_pst_Zone, UCHAR _uc_Flag)
{
	_pst_Zone->uc_Flag = _uc_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ ULONG COL_Zone_GetType(COL_tdst_ZDx *_pst_Zone)
{
	return _pst_Zone->uc_Type;
}

/*$F
 ===================================================================================================


									INSTANCE FUNCTIONS
		

 ===================================================================================================
    Aim:    Looks if the _uc_ZoneIndex's bit of the instance's uw_Specific Flag is set. That will
            means that this zone is a specific one.
 ===================================================================================================
 */

_inline_ BOOL COL_b_Instance_IsSpecific(COL_tdst_Instance *_pst_Instance, UCHAR _uc_ZoneIndex)
{
	return _pst_Instance->uw_Specific & ((USHORT) 1 << _uc_ZoneIndex);
}

/*
 =======================================================================================================================
    Aim:    Set the _uc_ZoneIndex's bit of the instance's uw_Specific Flag.
 =======================================================================================================================
 */
_inline_ void COL_Instance_SetSpecificFlag(COL_tdst_Instance *_pst_Instance, UCHAR _uc_ZoneIndex)
{
	_pst_Instance->uw_Specific |= ((USHORT) 1 << _uc_ZoneIndex);
}

/*
 =======================================================================================================================
    Aim:    Reset the _uc_ZoneIndex's bit of the instance's uw_Specific Flag.
 =======================================================================================================================
 */
_inline_ void COL_Instance_ResetSpecificFlag(COL_tdst_Instance *_pst_Instance, UCHAR _uc_ZoneIndex)
{
	_pst_Instance->uw_Specific &= ~(((USHORT) 1 << _uc_ZoneIndex));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Instance_SetFlag(COL_tdst_Instance *_pst_Instance, USHORT _uw_Flags)
{
	if(_pst_Instance) _pst_Instance->uw_Flags |= _uw_Flags;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Instance_ResetFlag(COL_tdst_Instance *_pst_Instance, USHORT _uw_Flags)
{
	_pst_Instance->uw_Flags &= ~_uw_Flags;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ USHORT COL_Instance_TestFlag(COL_tdst_Instance *_pst_Instance, USHORT _uw_Flags)
{
	if(!_pst_Instance) return 0;

	return _pst_Instance->uw_Flags & _uw_Flags;
}

/*
 =======================================================================================================================
    Aim:    Looks in the Intance's uw_Activation flag to determine if the (Index)ieme zone is active or not.
 =======================================================================================================================
 */
_inline_ BOOL COL_b_Instance_IsActive(COL_tdst_Instance *_pst_Instance, UCHAR _uc_ENGIndex)
{
	/*~~~~~~*/
	UCHAR	i;
	/*~~~~~~*/

	for(i = 0; i < 16; i++)
	{
		if(_pst_Instance->pst_ColSet->pauc_AI_Indexes[i] == _uc_ENGIndex)
			return _pst_Instance->uw_Activation & ((USHORT) 1 << i);
	}

	return FALSE;
}

/*
 =======================================================================================================================
    Aim:    Set the _uc_ZoneIndex's bit of the instance's uw_Activation Flag.
 =======================================================================================================================
 */
_inline_ void COL_Instance_SetActivationFlag(COL_tdst_Instance *_pst_Instance, UCHAR _uc_ZoneIndex)
{
	_pst_Instance->uw_Activation |= ((USHORT) 1 << _uc_ZoneIndex);
}

/*
 =======================================================================================================================
    Aim:    Reset the _uc_ZoneIndex's bit of the instance's uw_Activation Flag.
 =======================================================================================================================
 */
_inline_ void COL_Instance_ResetActivationFlag(COL_tdst_Instance *_pst_Instance, UCHAR _uc_ZoneIndex)
{
	_pst_Instance->uw_Activation &= ~(((USHORT) 1 << _uc_ZoneIndex));
}

/*
 =======================================================================================================================
    Aim:    Returns the (Index)ieme zone of an instance, Shared or Specific.
 =======================================================================================================================
 */
_inline_ COL_tdst_ZDx *COL_pst_Instance_GetZoneWithIndex(COL_tdst_Instance *_pst_Instance, UCHAR _uc_Index)
{
	return *(_pst_Instance->dpst_ZDx + _uc_Index);
}

/*
 =======================================================================================================================
    Aim:    Returns the Engine Index of a specific zone.
 =======================================================================================================================
 */
_inline_ UCHAR COL_uc_Instance_GetEngineIndexWithZone(COL_tdst_Instance *_pst_Instance, COL_tdst_ZDx *_pst_ZDx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ZDx	**dpst_ZDx, **dpst_LastZDx;
	UCHAR			uc_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	dpst_ZDx = _pst_Instance->dpst_ZDx;
	dpst_LastZDx = dpst_ZDx + _pst_Instance->uc_NbOfZDx;
	for(uc_Index = 0; dpst_ZDx < dpst_LastZDx; dpst_ZDx++, uc_Index++)
	{
		if(*dpst_ZDx == _pst_ZDx) return uc_Index;
	}

	return (UCHAR) -1;
}

/*
 =======================================================================================================================
    Aim:    Returns the (Index)ieme specific zone of an instance.
 =======================================================================================================================
 */
_inline_ UCHAR COL_uc_Instance_GetSpecificIndexWithZone(COL_tdst_Instance *_pst_Instance, COL_tdst_ZDx *_pst_ZDx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ZDx	*pst_ZDx, *pst_LastZDx;
	UCHAR			uc_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ZDx = _pst_Instance->past_Specific;
	pst_LastZDx = pst_ZDx + _pst_Instance->uc_NbOfSpecific;
	for(uc_Index = 0; pst_ZDx < pst_LastZDx; pst_ZDx++, uc_Index++)
	{
		if(pst_ZDx == _pst_ZDx) return uc_Index;
	}

	return (UCHAR) -1;
}

/*
 =======================================================================================================================
    Aim:    We have changed one zone (Specific->Shared or Shared->Specific) or added a new one.So, we update the
            instance's array of pointers.
 =======================================================================================================================
 */
_inline_ void COL_Instance_UpdatePointerAtIndex
(
	COL_tdst_Instance	*_pst_Instance,
	COL_tdst_ZDx		*_pst_ZDx,
	UCHAR				_uc_Index
)
{
	*(_pst_Instance->dpst_ZDx + _uc_Index) = _pst_ZDx;
}

/*$F
 ===================================================================================================

								COLSET FUNCTIONS

 ===================================================================================================
 */

_inline_ void COL_ColSet_SetFlag(COL_tdst_ColSet *_pst_ColSet, UCHAR _uc_Flag)
{
	_pst_ColSet->uc_Flag |= _uc_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_ColSet_ResetFlag(COL_tdst_ColSet *_pst_ColSet, UCHAR _uc_Flag)
{
	_pst_ColSet->uc_Flag &= ~_uc_Flag;
}

/*
 =======================================================================================================================
    Aim:    Returns the (Index)ieme zone of a ColSet.
 =======================================================================================================================
 */
_inline_ COL_tdst_ZDx *COL_pst_ColSet_GetZoneWithIndex(COL_tdst_ColSet *_pst_ColSet, UCHAR _uc_Index)
{
	return _pst_ColSet->past_ZDx + _uc_Index;
}

/*
 =======================================================================================================================
    Aim:    Returns the index of a ColSet zone.
 =======================================================================================================================
 */
_inline_ UCHAR COL_uc_ColSet_GetIndexWithZone(COL_tdst_ColSet *_pst_ColSet, COL_tdst_ZDx *_pst_ZDx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ZDx	*pst_ZDx, *pst_LastZDx;
	UCHAR			uc_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ZDx = _pst_ColSet->past_ZDx;
	pst_LastZDx = pst_ZDx + _pst_ColSet->uc_NbOfZDx;
	for(uc_Index = 0; pst_ZDx < pst_LastZDx; pst_ZDx++, uc_Index++)
	{
		if(pst_ZDx == _pst_ZDx) return uc_Index;
	}

	return (UCHAR) -1;
}

/*$F
 ===================================================================================================

								REPORTS FUNCTIONS

 ===================================================================================================
 */

_inline_ void COL_Report_SetFlag(COL_tdst_Report *_pst_Report, ULONG _ul_Flag)
{
	_pst_Report->ul_Flag |= _ul_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL COL_b_Report_TestFlag(COL_tdst_Report *_pst_Report, ULONG _ul_Flag)
{
	return(_pst_Report->ul_Flag & _ul_Flag);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Report_ResetFlag(COL_tdst_Report *_pst_Report, ULONG _ul_Flag)
{
	_pst_Report->ul_Flag &= ~_ul_Flag;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_Report_CopyFlag(COL_tdst_Report *_pst_Report, ULONG _ul_Flag)
{
	_pst_Report->ul_Flag = _ul_Flag;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif /* _cplusplcus */

#endif /* __COL_ACCESS_H__ */
