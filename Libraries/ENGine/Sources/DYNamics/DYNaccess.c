/*$T DYNaccess.c GC! 1.081 05/15/00 14:16:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Non inline access functions of the dynamics module */
#include "Precomp.h"
#include "ENGine/Sources/DYNamics/DYNinit.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "ENGine/Sources/DYNamics/DYNsolid.h"
#include "ENGine/Sources/DYNamics/DYNcol.h"
#include "ENGine/Sources/DYNamics/DYNConstraint.h"
#include "BASe/BAStypes.h"
#include "MATHs/Mathdebug.h"
#ifdef ACTIVE_EDITORS
#include <float.h>
#endif

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
    Aim:    Change the dynamic flags (update the DYN_C_NoForces flag)
 =======================================================================================================================
 */
void DYN_ChangeDynFlags(DYN_tdst_Dyna *_pst_Dyna, ULONG _ul_NewDynFlags, ULONG _ul_OldDynFlags)
{
	/* We update the DYN_C_NoForces flag */
	if
	(
		(_ul_NewDynFlags & DYN_C_OneConstantForce) + (_ul_NewDynFlags & DYN_C_BasicForces) +
			(_ul_NewDynFlags & DYN_C_ComplexForces)
	)
		/* Clear the NoForces Flag */
		_ul_NewDynFlags &= (0xffffffff - DYN_C_NoForces);
	else
		_ul_NewDynFlags |= DYN_C_NoForces;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Handle the DYN_C_BasicForces Flag
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_NewDynFlags & DYN_C_BasicForces)
	{
		if(!(_ul_OldDynFlags & DYN_C_BasicForces))
		{
			/* If not already allocated, we allocate the structure pst_Forces */
			_pst_Dyna->pst_Forces = DYN_pst_Forces_Create();
			DYN_Forces_Init
			(
				_pst_Dyna->pst_Forces,
				&MATH_gst_NulVector,
				&MATH_gst_NulVector,
				&MATH_gst_NulVector,
				Cf_One
			);

			/* Clear the OneConstantForce Flag */
			_ul_NewDynFlags &= (0xffffffff - DYN_C_OneConstantForce);
		}
	}
	else
	{
		if(_ul_OldDynFlags & DYN_C_BasicForces)
		{
			/* We desallocate the basic forces */
			DYN_Forces_Close(_pst_Dyna->pst_Forces);
			_pst_Dyna->pst_Forces = NULL;
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Handle the DYN_C_Solid Flag
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_NewDynFlags & DYN_C_Solid)
	{
		if(!(_ul_OldDynFlags & DYN_C_Solid))
		{
			/* If not already allocated, we allocate the structure solid */
			_pst_Dyna->pst_Solid = DYN_pst_SolidCreate();

			/*
			 * Initialize without rotation speed, without torque and with infinite rotation
			 * speed limit
			 */
			DYN_SolidInit(_pst_Dyna->pst_Solid, &MATH_gst_NulVector, &MATH_gst_NulVector, _pst_Dyna->pst_X, Cf_Infinit);
		}
	}
	else
	{
		if(_ul_OldDynFlags & DYN_C_Solid)
		{
			/* We desallocate the Solid info */
			DYN_SolidClose(_pst_Dyna->pst_Solid);
			_pst_Dyna->pst_Solid = NULL;
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Handle the DYN_C_Col Flag
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_NewDynFlags & DYN_C_Col)
	{
		if(!(_ul_OldDynFlags & DYN_C_Col))
		{
			/* If not already allocated, we allocate the structure col */
			_pst_Dyna->pst_Col = DYN_pst_ColCreate();
			DYN_ColReinit(_pst_Dyna->pst_Col);
		}
	}
	else
	{
		if(_ul_OldDynFlags & DYN_C_Col)
		{
			/* We desallocate the Col info */
			DYN_ColClose(_pst_Dyna->pst_Col);
			_pst_Dyna->pst_Col = NULL;
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Handle the DYN_C_Constraint Flag
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_NewDynFlags & DYN_C_Constraint)
	{
		if(!(_ul_OldDynFlags & DYN_C_Constraint))
		{
			/* If not already allocated, we allocate the structure constraint */
			_pst_Dyna->pst_Constraint = DYN_pst_ConstraintCreate();
			DYN_ConstraintReinit(_pst_Dyna->pst_Constraint);
		}
	}
	else
	{
		if(_ul_OldDynFlags & DYN_C_Constraint)
		{
			/* We desallocate the constraint info */
			DYN_ConstraintClose(_pst_Dyna->pst_Constraint);
			_pst_Dyna->pst_Constraint = NULL;
		}
	}

	/* Finally, sets the flags */
	DYN_SetDynFlags(_pst_Dyna, _ul_NewDynFlags);
}

/*
 =======================================================================================================================
    Aim:    Set the speed vector after removing the component parallel to the gravity vector

    Note:   If no gravity vector, the speed vector is not changed
 =======================================================================================================================
 */
void DYN_SetSpeedVector_WithoutChangingGravity(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_Speed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_SpeedHoriz;
	MATH_tdst_Vector	st_SpeedVert;
	float				f_SpeedVert;
	MATH_tdst_Vector	st_UnitG;	/* Gravity direction */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(DYN_ul_TestDynFlags(_pst_Dyna, DYN_C_BasicForces + DYN_C_IgnoreGravity) == DYN_C_BasicForces)
	{
		if(!MATH_b_NulVectorWithEpsilon(DYN_pst_GetGravity(_pst_Dyna), Cf_EpsilonBig))
		{
			/* Calculate gravity direction */
			MATH_CopyVector(&st_UnitG, DYN_pst_GetGravity(_pst_Dyna));
			MATH_NormalizeEqualVector(&st_UnitG);

			/* Calculate the vertical component */
			f_SpeedVert = MATH_f_DotProduct(&st_UnitG, _pst_Speed);
			MATH_ScaleVector(&st_SpeedVert, &st_UnitG, f_SpeedVert);

			/* Calculate the horizontal component */
			MATH_SubVector(&st_SpeedHoriz, _pst_Speed, &st_SpeedVert);
		}
		else
			MATH_CopyVector(&st_SpeedHoriz, _pst_Speed);
	}
	else
		MATH_CopyVector(&st_SpeedHoriz, _pst_Speed);

	/* P=m.v */
	MATH_MulVector(&_pst_Dyna->st_P, &st_SpeedHoriz, DYN_f_GetMass(_pst_Dyna));

#ifdef ACTIVE_EDITORS

	/* In editor mode, we store not only the linear momentum but also the speed vector */
	MATH_CopyVector(&_pst_Dyna->st_SpeedVector, &st_SpeedHoriz);
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void DYN_SetHorizontalSpeedComponent(DYN_tdst_Dyna *_pst_Dyna, MATH_tdst_Vector *_pst_SpeedHor)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_OldSpeed, st_SpeedToApply;
	MATH_tdst_Vector	st_SpeedVert;
	float				f_SpeedVert;
	MATH_tdst_Vector	st_UnitG;	/* Gravity direction */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(DYN_ul_TestDynFlags(_pst_Dyna, DYN_C_BasicForces + DYN_C_IgnoreGravity) == DYN_C_BasicForces)
	{
#ifdef ACTIVE_EDITORS
		MATH_CheckVector(_pst_SpeedHor, "Animation speed set");
#endif
		/* P=m.v */
		MATH_MulVector(&st_OldSpeed, &_pst_Dyna->st_P, fInv(DYN_f_GetMass(_pst_Dyna)));

		MATH_CopyVector(&st_UnitG, DYN_pst_GetGravity(_pst_Dyna));

		/* An object can have the Dyn structure allocated but a gravity of (0, 0, 0) */
		if((st_UnitG.x != 0.0f) || (st_UnitG.y != 0.0f) || (st_UnitG.z != 0.0f))
		{
			MATH_NormalizeEqualVector(&st_UnitG);
			f_SpeedVert = MATH_f_DotProduct(&st_UnitG, &st_OldSpeed);
			MATH_ScaleVector(&st_SpeedVert, &st_UnitG, f_SpeedVert);
			MATH_AddVector(&st_SpeedToApply, _pst_SpeedHor, &st_SpeedVert);
		}
		else
			MATH_CopyVector(&st_SpeedToApply, _pst_SpeedHor);
	}
	else
		MATH_CopyVector(&st_SpeedToApply, _pst_SpeedHor);

		/* P=m.v */
		MATH_MulVector(&_pst_Dyna->st_P, &st_SpeedToApply, DYN_f_GetMass(_pst_Dyna));

#ifdef ACTIVE_EDITORS
		MATH_CopyVector(&_pst_Dyna->st_SpeedVector, &st_SpeedToApply);
		MATH_CheckVector(&st_SpeedToApply, "Animation speed set");
#endif

}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
