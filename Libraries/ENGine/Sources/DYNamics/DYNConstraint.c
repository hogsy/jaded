/*$T DYNConstraint.c GC!1.52 01/21/00 15:29:07 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    This file countains most of the functions called directly by DYNMain.c
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BASsys.h"/* Pour la definition du _inline_ */
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"

#ifdef PSX2_TARGET
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C" {
#endif
#else
#include "TIMer/TIMdefs.h"
#endif

/*
 ===================================================================================================
 ===================================================================================================
 */
void DYN_ConstraintToSegment
(
    DYN_tdst_Dyna       *_pst_Dyna,
    MATH_tdst_Vector    *_pst_A,
    MATH_tdst_Vector    *_pst_B
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_X1;
    DYN_tdst_Constraint *pst_Constraint;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Constraint = DYN_pst_ConstraintGet(_pst_Dyna);
    MATH_PointSegmentProjection(&st_X1, DYN_pst_GetPosition(_pst_Dyna), _pst_A, _pst_B);
    MATH_CopyVector(DYN_pst_GetPosition(_pst_Dyna), &st_X1);
    MATH_SubVector(&pst_Constraint->st_Tangent, _pst_B, _pst_A);
	MATH_NormalizeEqualVector(&pst_Constraint->st_Tangent);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void DYN_ConstraintToParabol
(
    DYN_tdst_Dyna       *_pst_Dyna,
    MATH_tdst_Vector    *_pst_A,
    MATH_tdst_Vector    *_pst_B,
    MATH_tdst_Vector    *_pst_O,
    float               _f_r
)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_X1;  /* Projection position */
    DYN_tdst_Constraint *pst_Constraint;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Constraint = DYN_pst_ConstraintGet(_pst_Dyna);

    MATH_PointParabolProjection3D
    (
        &st_X1,
        &pst_Constraint->st_Tangent,
        DYN_pst_GetPosition(_pst_Dyna),
        _pst_A,
        _pst_B,
        _pst_O,
        _f_r
    );
    MATH_CopyVector(DYN_pst_GetPosition(_pst_Dyna), &st_X1);
	MATH_NormalizeEqualVector(&pst_Constraint->st_Tangent);
}

/*
 ===================================================================================================
    Init functions
 ===================================================================================================
 */
DYN_tdst_Constraint *DYN_pst_ConstraintCreate(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DYN_tdst_Constraint *pst_Constraint;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Constraint = (DYN_tdst_Constraint *) MEM_p_Alloc(sizeof(DYN_tdst_Constraint));

    L_memset(pst_Constraint, 0, sizeof(DYN_tdst_Constraint)); 
	MATH_CopyVector(&pst_Constraint->st_Tangent,&MATH_gst_NulVector);
    return(pst_Constraint);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void DYN_ConstraintClose(DYN_tdst_Constraint *_pst_Constraint)
{
    MEM_Free(_pst_Constraint);
}

/*
 ===================================================================================================
 Aim:	initializes the parameters of a constraint
 Note:	the tangent is not initialized on purpose !
 ===================================================================================================
 */
void DYN_ConstraintInit
(
    DYN_tdst_Constraint *_pst_Constraint,
    char                _c_ConstraintType,
    MATH_tdst_Vector    *_pst_A,
    MATH_tdst_Vector    *_pst_B,
    MATH_tdst_Vector    *_pst_O,
    float               _f_r
)
{
    _pst_Constraint->c_ConstraintType = _c_ConstraintType;
    _pst_Constraint->f_r = _f_r;
    MATH_CopyVector(&_pst_Constraint->st_A, _pst_A);
    MATH_CopyVector(&_pst_Constraint->st_B, _pst_B);
    MATH_CopyVector(&_pst_Constraint->st_O, _pst_O);
	/*
    if(_c_ConstraintType == DYN_C_ConstraintParabol)
        MATH_SubVector(&_pst_Constraint->st_Tangent, _pst_A, _pst_O);
    else if(_c_ConstraintType == DYN_C_ConstraintSegment)
        MATH_SubVector(&_pst_Constraint->st_Tangent, _pst_B, _pst_A);
	if (_c_ConstraintType == DYN_C_NoConstraint)
		MATH_CopyVector(&_pst_Constraint->st_Tangent,&MATH_gst_BaseVectorI);
	else
		MATH_NormalizeEqualVector(&_pst_Constraint->st_Tangent);
	*/
	
}

/*
 ===================================================================================================
    Aim:    Reinit the constraint structure (with no constraint)
 ===================================================================================================
 */
void DYN_ConstraintReinit(DYN_tdst_Constraint *_pst_Constraint)
{
    _pst_Constraint->c_ConstraintType = DYN_C_NoConstraint;
	MATH_CopyVector(&_pst_Constraint->st_Tangent,&MATH_gst_NulVector);
}

/*
 ===================================================================================================
    Aim:    Apply the current constraint to the object
 ===================================================================================================
 */
void DYN_ApplyConstraint(DYN_tdst_Dyna *_pst_Dyna, float _f_dt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DYN_tdst_Constraint *pst_Constraint;
    MATH_tdst_Vector    st_Speed;
	MATH_tdst_Vector    st_NoConstraintSpeed;
	float f;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Constraint = DYN_pst_ConstraintGet(_pst_Dyna);
    if(pst_Constraint->c_ConstraintType == DYN_C_ConstraintSegment)
    {
        DYN_ConstraintToSegment(_pst_Dyna, &pst_Constraint->st_A, &pst_Constraint->st_B);
    }
    else if(pst_Constraint->c_ConstraintType == DYN_C_ConstraintParabol)
    {
        DYN_ConstraintToParabol
        (
            _pst_Dyna,
            &pst_Constraint->st_A,
            &pst_Constraint->st_B,
            &pst_Constraint->st_O,
            pst_Constraint->f_r
        );
    }

    if(pst_Constraint->c_ConstraintType != DYN_C_NoConstraint)
    {
		/* project the speed vector on the tangent */
		
		
		DYN_GetSpeedVector(_pst_Dyna,&st_NoConstraintSpeed);
		if (!MATH_b_NulVectorWithEpsilon(&st_NoConstraintSpeed,Cf_EpsilonBig))
		{
			f=MATH_f_DotProduct(&st_NoConstraintSpeed,&pst_Constraint->st_Tangent);
			MATH_ScaleVector(&st_Speed,&pst_Constraint->st_Tangent,f);
			DYN_SetSpeedVector(_pst_Dyna,&st_Speed);
		}
    }
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
