/*$T OBJaccess.h GC! 1.098 12/04/00 11:30:59 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*
 * Access to some fields of an object £
 * For accesses on the abolute matrix, see "objorient.h" £
 */
#ifndef __OBJ_ACCESS_H__
#define __OBJ_ACCESS_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/ENGvars.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined(_DEBUG)
static char c_SkipTest = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL OBJ_IsNullOrValidGAO(OBJ_tdst_GameObject *_pst_GAO)
{
#ifndef PSX2_TARGET
#ifndef _GAMECUBE
    if(!c_SkipTest)
	{
		if(!_pst_GAO) return TRUE;
		if(IsBadReadPtr(_pst_GAO, sizeof(OBJ_tdst_GameObject))) return FALSE;
		if(IsBadWritePtr(_pst_GAO, sizeof(OBJ_tdst_GameObject))) return FALSE;
		if(_pst_GAO->sz_Name && IsBadStringPtr(_pst_GAO->sz_Name, 128)) return FALSE;
	}	
#endif
#endif
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL OBJ_IsValidGAO(OBJ_tdst_GameObject *_pst_GAO)
{
#ifndef PSX2_TARGET
#ifndef _GAMECUBE
	if(!c_SkipTest)
	{
		if(IsBadReadPtr(_pst_GAO, sizeof(OBJ_tdst_GameObject))) return FALSE;
		if(IsBadWritePtr(_pst_GAO, sizeof(OBJ_tdst_GameObject))) return FALSE;
		if(_pst_GAO->sz_Name && IsBadStringPtr(_pst_GAO->sz_Name, 128)) return FALSE;
	}
#endif
#endif
	return TRUE;
}

#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    Access flags of the object £
    functions are: £
    OBJ_FlagsControlGet £
    OBJ_FlagsControlSet £
    OBJ_ul_FlagsStatusGet £
    OBJ_ClearStatusFlag £
    OBJ_SetStatusFlag £
    OBJ_ClearIdentityFlag £
    OBJ_SetIdentityFlag £
    OBJ_ClearControlFlag £
    OBJ_SetControlFlag £
    OBJ_b_TestStatusFlag £
    OBJ_b_TestControlFlag £
    OBJ_b_TestIdentityFlag £
    OBJ_ul_FlagsIdentityGet £
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim return the Design flags
 =======================================================================================================================
 */
_inline_ UCHAR OBJ_uc_FlagsDesignGet(OBJ_tdst_GameObject *_pst_Object)
{
	return(_pst_Object->uc_DesignFlags);	/* Clear status flags */
}

/*
 =======================================================================================================================
    Aim:    Set Design flags
 =======================================================================================================================
 */
_inline_ void OBJ_FlagsDesignSet(OBJ_tdst_GameObject *_pst_Object, UCHAR _uc_DesignFlag)
{
	(_pst_Object->uc_DesignFlags) = _uc_DesignFlag;
}

/*
 =======================================================================================================================
    Aim return the control flags
 =======================================================================================================================
 */
_inline_ ULONG OBJ_ul_FlagsControlGet(OBJ_tdst_GameObject *_pst_Object)
{
	return((_pst_Object->ul_StatusAndControlFlags) & 0xffff0000);	/* Clear status flags */
}

/*
 =======================================================================================================================
    Aim:    Set control flags
 =======================================================================================================================
 */
_inline_ void OBJ_FlagsControlSet(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	(_pst_Object->ul_StatusAndControlFlags) &= 0xffff;					/* Keep status flags */
	(_pst_Object->ul_StatusAndControlFlags) |= (0xffff0000 & _ul_Flag); /* Set control flags */
}

/*
 =======================================================================================================================
    Aim return the control flags
 =======================================================================================================================
 */
_inline_ ULONG OBJ_ul_FlagsStatusGet(OBJ_tdst_GameObject *_pst_Object)
{
	return((_pst_Object->ul_StatusAndControlFlags) & 0xffff);	/* Clear control flags */
}

/*
 =======================================================================================================================
    Aim:    Clear any status flag of a game object
 =======================================================================================================================
 */
_inline_ void OBJ_ClearStatusFlag(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	(_pst_Object->ul_StatusAndControlFlags) &= (~_ul_Flag);
}

/*
 =======================================================================================================================
    Aim:    Set any status flag of a game object
 =======================================================================================================================
 */
_inline_ void OBJ_SetStatusFlag(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	(_pst_Object->ul_StatusAndControlFlags) |= _ul_Flag;
}

/*
 =======================================================================================================================
    Aim:    Clear any identity flag of a game object
 =======================================================================================================================
 */
_inline_ void OBJ_ClearIdentityFlag(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	(_pst_Object->ul_IdentityFlags) &= (~_ul_Flag);
}

/*
 =======================================================================================================================
    Aim:    Set identity flag of a game object, without changing the others

    Note:   WARNING ! this function changes only the flag. Use OBJ_ChangeIdentityFlags to change the Identity flags and
            make the appropriate desallocations/allocations
 =======================================================================================================================
 */
_inline_ void OBJ_SetIdentityFlag(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	(_pst_Object->ul_IdentityFlags) |= _ul_Flag;
}

/*
 =======================================================================================================================
    Aim:    Sets all the identity flags of an object (1 or 0)
 =======================================================================================================================
 */
_inline_ void OBJ_SetIdentityFlags(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	_pst_Object->ul_IdentityFlags = _ul_Flag;
}

/*
 =======================================================================================================================
    Aim:    Clear any control flag of a game object
 =======================================================================================================================
 */
_inline_ void OBJ_ClearControlFlag(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	(_pst_Object->ul_StatusAndControlFlags) &= (~_ul_Flag);
}

/*
 =======================================================================================================================
    Aim:    Set any control flag of a game object
 =======================================================================================================================
 */
_inline_ void OBJ_SetControlFlag(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	(_pst_Object->ul_StatusAndControlFlags) |= _ul_Flag;
}

/*
 =======================================================================================================================
    Aim:    Return true if the given status flag is set
 =======================================================================================================================
 */
_inline_ ULONG OBJ_b_TestStatusFlag(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	return(_pst_Object->ul_StatusAndControlFlags & _ul_Flag);
}

/*
 =======================================================================================================================
    Aim:    Return true if the given control flag is set
 =======================================================================================================================
 */
_inline_ ULONG OBJ_b_TestControlFlag(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	return(_pst_Object->ul_StatusAndControlFlags & _ul_Flag);
}

/*
 =======================================================================================================================
    Aim:    Return true if the given identity flag is set
 =======================================================================================================================
 */
_inline_ ULONG OBJ_b_TestIdentityFlag(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag)
{
	return(_pst_Object->ul_IdentityFlags & _ul_Flag);
}

/*
 =======================================================================================================================
    #define OBJ_b_TestIdentityFlag(a, b) ((a)->ul_IdentityFlags & (b)) £

    Aim:    Return the identity flags of an object
 =======================================================================================================================
 */
_inline_ ULONG OBJ_ul_FlagsIdentityGet(OBJ_tdst_GameObject *_pst_Object)
{
	return(_pst_Object->ul_IdentityFlags);
}

/*
 =======================================================================================================================
    Aim:    Return the Extra Flags of an object
 =======================================================================================================================
 */
_inline_ USHORT OBJ_uw_ExtraFlagsGet(OBJ_tdst_GameObject *_pst_Object)
{
	if(_pst_Object->pst_Extended)
		return(_pst_Object->pst_Extended->uw_ExtraFlags);
	else
		return 0;
}

/*
 =======================================================================================================================
    Aim:    Sets the Extra Flags of an object
 =======================================================================================================================
 */
_inline_ void OBJ_ExtraFlagsSet(OBJ_tdst_GameObject *_pst_Object, USHORT _uw_ExtraFlags)
{
	if(_pst_Object->pst_Extended) _pst_Object->pst_Extended->uw_ExtraFlags = _uw_ExtraFlags;
}

/*
 =======================================================================================================================
    Aim:    Test the Extra Flags of an object
 =======================================================================================================================
 */
_inline_ USHORT OBJ_uw_ExtraFlagsTest(OBJ_tdst_GameObject *_pst_Object, USHORT _uw_ExtraFlags)
{
	if(_pst_Object->pst_Extended)
		return(_pst_Object->pst_Extended->uw_ExtraFlags & _uw_ExtraFlags);
	else
		return 0;
}

/*$4
 ***********************************************************************************************************************
    Functions related to real time loading: £
    OBJ_b_GameObject_isLoaded £
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Returns true if a game object is loaded

    Note:   An object with a last bit set to zero is loaded
 =======================================================================================================================
 */
_inline_ ULONG OBJ_b_GameObject_isLoaded(OBJ_tdst_GameObject *_pst_Object)
{
	return(!(((ULONG) _pst_Object) & 1));
}

/*$4
 ***********************************************************************************************************************
    Access functions to Object Sub-Structures
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Return a pointer to the Dynamic structure (if allocated)
 =======================================================================================================================
 */
_inline_ DYN_tdst_Dyna *OBJ_pst_GetDynaStruct(OBJ_tdst_GameObject *_pst_Object)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Base	*pst_Base;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Base = _pst_Object->pst_Base;
	if(pst_Base)
		return(pst_Base->pst_Dyna);
	else
		return NULL;
}

/*$4
 ***********************************************************************************************************************
    Various access functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim:    Returns the Visibility coeff (converted to float)

    Note:   This coeff is always between 0.0625 and 15.9375
 =======================================================================================================================
 */
_inline_ float OBJ_f_GetVisiCoeff(OBJ_tdst_GameObject *_pst_Object)
{
	return(fLongToFloat((LONG) _pst_Object->uc_VisiCoeff) * Cf_Inv16);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ MATH_tdst_Matrix *OBJ_pst_GetGizmo(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_GizmoID, BOOL _b_Pointers)
{
	if(_b_Pointers)
		return (_pst_Object->pst_Base->pst_AddMatrix->dst_GizmoPtr +_ul_GizmoID)->pst_Matrix;
	else
		return &(_pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo + _ul_GizmoID)->st_Matrix;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ ULONG OBJ_ul_GetGizmoID(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Gizmo)
{
	return (_pst_Object->pst_Base->pst_AddMatrix->dst_GizmoPtr +_ul_Gizmo)->l_MatrixId;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ OBJ_tdst_GameObject *OBJ_pst_GetFather(OBJ_tdst_GameObject *_pst_Object)
{
	if(!_pst_Object || !_pst_Object->pst_Base || !_pst_Object->pst_Base->pst_Hierarchy) return NULL;
	return _pst_Object->pst_Base->pst_Hierarchy->pst_Father;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ OBJ_tdst_GameObject *OBJ_pst_GetFatherInit(OBJ_tdst_GameObject *_pst_Object)
{
	if(!_pst_Object || !_pst_Object->pst_Base || !_pst_Object->pst_Base->pst_Hierarchy) return NULL;
	return _pst_Object->pst_Base->pst_Hierarchy->pst_FatherInit;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ BOOL OBJ_b_TestFlag(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_OnFlags, ULONG _ul_OffFlags, ULONG _ul_FlagID)
{

	switch(_ul_FlagID)
	{
	case OBJ_C_OR_OR_IdentityFlags:
		return
			(
				(OBJ_b_TestIdentityFlag(_pst_Object, _ul_OnFlags))
			&&	!(OBJ_b_TestIdentityFlag(_pst_Object, _ul_OffFlags))
			);

	case OBJ_C_OR_OR_TypeFlags:
		if((!_pst_Object->uc_DesignFlags) && (_ul_OnFlags == (ULONG) - 1)) return TRUE;
		return((_pst_Object->uc_DesignFlags & _ul_OnFlags) && !(_pst_Object->uc_DesignFlags & _ul_OffFlags));
		break;

	case OBJ_C_OR_OR_DesignFlags:
		if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_ExtendedObject)) return FALSE;
		if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_DesignStruct)) return FALSE;
		if(!_pst_Object->pst_Extended->pst_Design->flags && (_ul_OnFlags == (ULONG) - 1)) return TRUE;
		return
			(
				(_pst_Object->pst_Extended->pst_Design->flags & _ul_OnFlags)
			&&	!(_pst_Object->pst_Extended->pst_Design->flags & _ul_OffFlags)
			);
		break;

	case OBJ_C_OR_OR_Capacities:
		if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_ExtendedObject)) return FALSE;
		if(!_pst_Object->pst_Extended->uw_Capacities && (_ul_OnFlags == (ULONG) - 1)) return TRUE;
		return
			(
				(_pst_Object->pst_Extended->uw_Capacities & _ul_OnFlags)
			&&	!(_pst_Object->pst_Extended->uw_Capacities & _ul_OffFlags)
			);
		break;

	case OBJ_C_OR_OR_DesignFlags2:
		if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_ExtendedObject)) return FALSE;
		if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_DesignStruct)) return FALSE;
		if(!_pst_Object->pst_Extended->pst_Design->ui_DesignFlags && (_ul_OnFlags == (ULONG) - 1)) return TRUE;
		return
			(
				(_pst_Object->pst_Extended->pst_Design->ui_DesignFlags & _ul_OnFlags)
			&&	!(_pst_Object->pst_Extended->pst_Design->ui_DesignFlags & _ul_OffFlags)
			);
		break;

	case OBJ_C_OR_OR_Custom:
		return
			(
				(_pst_Object->ul_StatusAndControlFlags & _ul_OnFlags)
			&&	!(_pst_Object->ul_StatusAndControlFlags & _ul_OffFlags)
			);
		break;

	case OBJ_C_AND_OR_IdentityFlags:
		return
			(
				((_pst_Object->ul_IdentityFlags & _ul_OnFlags) == _ul_OnFlags)
			&&	!(OBJ_b_TestIdentityFlag(_pst_Object, _ul_OffFlags))
			);
		break;

	case OBJ_C_AND_OR_TypeFlags:
		if((!_pst_Object->uc_DesignFlags) && (_ul_OnFlags == (ULONG) - 1)) return TRUE;
		return
			(
				((_pst_Object->uc_DesignFlags & _ul_OnFlags) == _ul_OnFlags)
			&&	!(_pst_Object->uc_DesignFlags & _ul_OffFlags)
			);
		break;

	case OBJ_C_AND_OR_DesignFlags:
		if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_ExtendedObject)) return FALSE;
		if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_DesignStruct)) return FALSE;
		if((!_pst_Object->pst_Extended->pst_Design->flags) && (_ul_OnFlags == (ULONG) - 1)) return TRUE;
		return
			(
				((_pst_Object->pst_Extended->pst_Design->flags & _ul_OnFlags) == _ul_OnFlags)
			&&	!(_pst_Object->pst_Extended->pst_Design->flags & _ul_OffFlags)
			);
		break;

	case OBJ_C_AND_OR_Capacities:
		if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_ExtendedObject)) return FALSE;
		if((!_pst_Object->pst_Extended->uw_Capacities) && (_ul_OnFlags == (ULONG) - 1)) return TRUE;
		return
			(
				((_pst_Object->pst_Extended->uw_Capacities & _ul_OnFlags) == _ul_OnFlags)
			&&	!(_pst_Object->pst_Extended->uw_Capacities & _ul_OffFlags)
			);
		break;

	case OBJ_C_AND_OR_DesignFlags2:
		if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_ExtendedObject)) return FALSE;
		if(!OBJ_b_TestIdentityFlag(_pst_Object, OBJ_C_IdentityFlag_DesignStruct)) return FALSE;
		if((!_pst_Object->pst_Extended->pst_Design->ui_DesignFlags) && (_ul_OnFlags == (ULONG) - 1)) return TRUE;
		return
			(
				((_pst_Object->pst_Extended->pst_Design->ui_DesignFlags & _ul_OnFlags) == _ul_OnFlags)
			&&	!(_pst_Object->pst_Extended->pst_Design->ui_DesignFlags & _ul_OffFlags)
			);
		break;

	case OBJ_C_AND_OR_Custom:
		return
			(
				((_pst_Object->ul_StatusAndControlFlags & _ul_OnFlags) == _ul_OnFlags)
			&&	!(_pst_Object->ul_StatusAndControlFlags & _ul_OffFlags)
			);
		break;
		
	default:
	    return TRUE;
	}

	return FALSE;

}
#ifdef JADEFUSION
// =======================================================================================================================
//    Return a pointer to the CCurrentWind (if allocated)
// =======================================================================================================================
_inline_ CCurrentWind * OBJ_pst_GetCurrentWind(OBJ_tdst_GameObject *_pst_Object)
{
    if(_pst_Object->pst_Extended)
        return(_pst_Object->pst_Extended->po_CurrentWind);
    else
        return NULL;
}
#endif
#endif
