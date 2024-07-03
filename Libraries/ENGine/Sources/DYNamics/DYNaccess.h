/*$T DYNaccess.h GC!1.52 12/03/99 15:14:34 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Initialisation and close of the module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __DYNACCES_H__
#define __DYNACCES_H__


#include "BASe/BAStypes.h"

#include "BASe/BASsys.h"/* Pour la definition du _inline_ */
#include "MATHs/MATH.h"
#include "ENGine/Sources/DYNamics/DYNstruct.h"
#include "ENGine/Sources/DYNamics/DYNconst.h"
/*
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
*/


/* #include "ENGine/Sources/DYNamics/DYNinit.h" #include "ENGine/Sources/DYNamics/DYNconst.h" */
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Access functions
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Mass
 ===================================================================================================
 */
_inline_ float DYN_f_GetMass(DYN_tdst_Dyna *_pst_Dyna)
{
    return(fInv(_pst_Dyna->f_InvMass));
}

/**/
_inline_ void DYN_SetMass(DYN_tdst_Dyna *_pst_Dyna, float _f_Mass)
{
    _pst_Dyna->f_InvMass = fInv(_f_Mass);
#ifdef ACTIVE_EDITORS
    _pst_Dyna->f_Mass = _f_Mass;
#endif
}

/**/
_inline_ float DYN_f_GetInvMass(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->f_InvMass);
}

/*
 ===================================================================================================
    Max Step Size
 ===================================================================================================
 */
_inline_ float DYN_f_GetMaxStepSize(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->f_MaxStepSize);
}
/**/
_inline_ void DYN_SetMaxStepSize(DYN_tdst_Dyna *_pst_Dyna, float _f_MaxStepSize)
{
    _pst_Dyna->f_MaxStepSize = _f_MaxStepSize;
}


/*
 ===================================================================================================
    Speed Limit
 ===================================================================================================
 */
_inline_ float DYN_f_GetSpeedLimitHoriz(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->f_SpeedLimitHoriz);
}

/**/
_inline_ void DYN_SetSpeedLimitHoriz(DYN_tdst_Dyna *_pst_Dyna, float _f_SpeedLimitHoriz)
{
    _pst_Dyna->f_SpeedLimitHoriz = _f_SpeedLimitHoriz;
}

/**/
_inline_ float DYN_f_GetSpeedLimitVert(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->f_SpeedLimitVert);
}

/**/
_inline_ void DYN_SetSpeedLimitVert(DYN_tdst_Dyna *_pst_Dyna, float _f_SpeedLimitVert)
{
    _pst_Dyna->f_SpeedLimitVert = _f_SpeedLimitVert;
}

/*$3
 ===================================================================================================
    Speed and Linear Momentum Access
 ===================================================================================================
 */

_inline_ void DYN_GetSpeedVector(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_Speed)
{
    MATH_MulVector(_pst_Speed, &_pst_Dyna->st_P, _pst_Dyna->f_InvMass);
}

/**/
_inline_ void DYN_SetSpeedVector(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_Speed)
{
    /* P=m.v */
    MATH_MulVector(&_pst_Dyna->st_P, _pst_Speed, DYN_f_GetMass(_pst_Dyna));
#if defined(_DEBUG) && defined(_GAMECUBE)
		if(
			((*(int *) &_pst_Dyna->st_P.x) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_Dyna->st_P.y) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_Dyna->st_P.z) == (int) 0xFFC00000)
		||	((*(int *) &_pst_Dyna->st_P.x) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_Dyna->st_P.y) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_Dyna->st_P.z) == (int) 0x7FC00000)
		
		)
		OSReport("Collision recalage error \n");
#endif			
    
    
#ifdef ACTIVE_EDITORS

    /* In editor mode, we store not only the linear momentum but also the speed vector */
	MATH_CheckVector(_pst_Speed, "Speed given by AI");
    MATH_CopyVector(&_pst_Dyna->st_SpeedVector, _pst_Speed);
#endif
}

void DYN_SetSpeedVector_WithoutChangingGravity(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_Speed);
void DYN_SetHorizontalSpeedComponent(DYN_tdst_Dyna *, MATH_tdst_Vector *);

/**/
_inline_ MATH_tdst_Vector *DYN_pst_GetP(DYN_tdst_Dyna *_pst_Dyna)
{
    return &(_pst_Dyna->st_P);
}

/**/
_inline_ void DYN_SetP(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_P)
{
    MATH_CopyVector(&_pst_Dyna->st_P, _pst_P);
#if defined(_DEBUG) && defined(_GAMECUBE)
		if(
			((*(int *) &_pst_Dyna->st_P.x) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_Dyna->st_P.y) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_Dyna->st_P.z) == (int) 0xFFC00000)
		||	((*(int *) &_pst_Dyna->st_P.x) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_Dyna->st_P.y) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_Dyna->st_P.z) == (int) 0x7FC00000)
		
		)
		OSReport("Collision recalage error \n");
#endif			
    
#ifdef ACTIVE_EDITORS

    /* In editor mode, we store not only the linear momentum but also the speed vector */
    MATH_MulVector(&_pst_Dyna->st_SpeedVector, _pst_P, DYN_f_GetInvMass(_pst_Dyna));
#endif
}

/**/
_inline_ float DYN_f_GetNormSpeed(DYN_tdst_Dyna *_pst_Dyna)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_Speed;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    DYN_GetSpeedVector(_pst_Dyna, &st_Speed);
    return(MATH_f_NormVector(&st_Speed));
}

/**/
_inline_ void DYN_SetNormSpeed(DYN_tdst_Dyna *_pst_Dyna, float _f_NormSpeed)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   f_Tmp;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    f_Tmp = MATH_f_NormVector(&(_pst_Dyna->st_P));
    if(f_Tmp != 0)
    {
        MATH_MulEqualVector(&_pst_Dyna->st_P, fDiv(_f_NormSpeed * DYN_f_GetMass(_pst_Dyna), f_Tmp));
#if defined(_DEBUG) && defined(_GAMECUBE)
		if(
			((*(int *) &_pst_Dyna->st_P.x) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_Dyna->st_P.y) == (int) 0xFFC00000)
		||  ((*(int *) &_pst_Dyna->st_P.z) == (int) 0xFFC00000)
		||	((*(int *) &_pst_Dyna->st_P.x) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_Dyna->st_P.y) == (int) 0x7FC00000)
		||  ((*(int *) &_pst_Dyna->st_P.z) == (int) 0x7FC00000)
		
		)
		OSReport("Collision recalage error \n");
#endif			
        
#ifdef ACTIVE_EDITORS

        /* In editor mode, we store not only the linear momentum but also the speed vector */
        MATH_MulVector(&_pst_Dyna->st_SpeedVector, &(_pst_Dyna->st_P), DYN_f_GetInvMass(_pst_Dyna));
#endif
    }
}

/*$3
 ===================================================================================================
    Position pointer and rotation matrix
 ===================================================================================================
 */

_inline_ void DYN_SetPositionPointer(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_Position)
{
    _pst_Dyna->pst_X = _pst_Position;
}

/**/
_inline_ MATH_tdst_Vector *DYN_pst_GetPosition(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->pst_X);
}

/**/
_inline_ MATH_tdst_Matrix *DYN_pst_GetGlobalMatrix(DYN_tdst_Dyna *_pst_Dyna)
{
    return((MATH_tdst_Matrix *) (((char *) (_pst_Dyna->pst_X)) - 48));
}

/*$3
 ===================================================================================================
    Dynamic flags
 ===================================================================================================
 */

extern void DYN_ChangeDynFlags
            (
                DYN_tdst_Dyna   *_pst_Dyna,
                ULONG   _ul_NewDynFlags,
                ULONG   _ul_OldDynFlags
            );

/**/
_inline_ void DYN_SetDynFlag(DYN_tdst_Dyna *_pst_Dyna, ULONG _ul_DynFlag)
{
    _pst_Dyna->ul_DynFlags |= _ul_DynFlag;
}

/**/
_inline_ void DYN_SetDynFlags(DYN_tdst_Dyna *_pst_Dyna, ULONG _ul_DynFlags)
{
    _pst_Dyna->ul_DynFlags = _ul_DynFlags;
}

/**/
_inline_ ULONG DYN_ul_GetDynFlags(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->ul_DynFlags);
}

/**/
_inline_ ULONG DYN_ul_TestDynFlags(DYN_tdst_Dyna *_pst_Dyna, ULONG _ul_DynFlags)
{
    return(_pst_Dyna->ul_DynFlags & _ul_DynFlags);
}

/**/
_inline_ void DYN_ClearFlags(DYN_tdst_Dyna *_pst_Dyna, ULONG _ul_DynFlags)
{
    (_pst_Dyna->ul_DynFlags) &= (0xFFFFFFFF - _ul_DynFlags);
}

/*$3
 ===================================================================================================
    One constant Force (or sum of forces, depending on the mode)
 ===================================================================================================
 */

_inline_ void DYN_SetSumOfForces(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_SumOfForces)
{
    MATH_CopyVector(&_pst_Dyna->st_F, _pst_SumOfForces);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ MATH_tdst_Vector *DYN_pst_GetSumOfForces(DYN_tdst_Dyna *_pst_Dyna)
{
    return(&_pst_Dyna->st_F);
}

/*$3
 ===================================================================================================
    Basic Forces
 ===================================================================================================
 */

_inline_ void DYN_SetGravity(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_Gravity)
{
    MATH_CopyVector(&_pst_Dyna->pst_Forces->st_Gravity, _pst_Gravity);
}

/**/
_inline_ MATH_tdst_Vector *DYN_pst_GetGravity(DYN_tdst_Dyna *_pst_Dyna)
{
    return(&_pst_Dyna->pst_Forces->st_Gravity);
}

/**/
_inline_ void DYN_SetTraction(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_Traction)
{
    MATH_CopyVector(&_pst_Dyna->pst_Forces->st_Traction, _pst_Traction);
}

/**/
_inline_ MATH_tdst_Vector *DYN_pst_GetTraction(DYN_tdst_Dyna *_pst_Dyna)
{
    return(&_pst_Dyna->pst_Forces->st_Traction);
}

/**/
_inline_ void DYN_SetStream(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_Stream)
{
    MATH_CopyVector(&_pst_Dyna->pst_Forces->st_Stream, _pst_Stream);
}

/**/
_inline_ MATH_tdst_Vector *DYN_pst_GetStream(DYN_tdst_Dyna *_pst_Dyna)
{
    return(&_pst_Dyna->pst_Forces->st_Stream);
}

/*
 ===================================================================================================
    Set/Get the friction coeff (k from the formula F=-k.V)
 ===================================================================================================
 */
_inline_ void DYN_FrictionSet(DYN_tdst_Dyna *_pst_Dyna, float _f_K)
{
    _pst_Dyna->pst_Forces->st_K.x = _f_K;
}

/**/
_inline_ float DYN_f_FrictionGet(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->pst_Forces->st_K.x);
}

/*
 ===================================================================================================
    Set/Get the friction vector
 ===================================================================================================
 */
_inline_ void DYN_FrictionVectorSet(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *pst_FrictionVector)
{
    MATH_CopyVector(&_pst_Dyna->pst_Forces->st_K, pst_FrictionVector);
}

/**/
_inline_ MATH_tdst_Vector *DYN_pst_FrictionVectorGet(DYN_tdst_Dyna *_pst_Dyna)
{
    return((MATH_tdst_Vector *) &(_pst_Dyna->pst_Forces->st_K.x));
}





/*
 ===================================================================================================
    Aim:    Set The AutoOrientation Inertia Level

    Note:   The possible constants are DYN_C_NoAutoOrientInertia £
            DYN_C_LowAutoOrientInertia £
            DYN_C_MediumAutoOrientInertia £
            DYN_C_HiAutoOrientInertia
 ===================================================================================================
 */
_inline_ void DYN_AutoOrientInertiaSet(DYN_tdst_Dyna *_pst_Dyna, LONG _c_InertiaLevel)
{
    _pst_Dyna->ul_DynFlags &= ~(DYN_C_AutoOrientInertia1 + DYN_C_AutoOrientInertia2);
    _pst_Dyna->ul_DynFlags +=
        (_c_InertiaLevel & (DYN_C_AutoOrientInertia1 + DYN_C_AutoOrientInertia2));
}

/*
 ===================================================================================================
    Aim:    Get the auto orient inertia level
 ===================================================================================================
 */
_inline_ LONG DYN_l_AutoOrientInertiaGet(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->ul_DynFlags & (DYN_C_AutoOrientInertia1 + DYN_C_AutoOrientInertia2));
}

/*$4
 ***************************************************************************************************
    Substructures
 ***************************************************************************************************
 */

/*
 ===================================================================================================
    Aim:    Returns the pointer to the solid structure
 ===================================================================================================
 */
_inline_ DYN_tdst_Solid *DYN_pst_SolidGet(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->pst_Solid);
}

/*
 ===================================================================================================
    Aim:    Returns the pointer to the col structure
 ===================================================================================================
 */
_inline_ DYN_tdst_Col *DYN_pst_ColGet(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->pst_Col);
}

/*
 ===================================================================================================
    Aim:    Returns the pointer to the constraint structure
 ===================================================================================================
 */
_inline_ DYN_tdst_Constraint *DYN_pst_ConstraintGet(DYN_tdst_Dyna *_pst_Dyna)
{
    return(_pst_Dyna->pst_Constraint);
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __DYNACCES_H__ */