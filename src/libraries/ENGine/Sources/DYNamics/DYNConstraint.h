/*$T DYNConstraint.h GC!1.52 01/21/00 14:38:39 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Main functions of the dynamics module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _DYNCONSTRAINT_H_
#define _DYNCONSTRAINT_H_

/* #include "MATHs/MATH.h" */
#include "ENGine/Sources/DYNamics/DYNstruct.h"
#include "MATHs/MATH.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$3
 ===================================================================================================
    Init functions
 ===================================================================================================
 */

DYN_tdst_Constraint *DYN_pst_ConstraintCreate(void);
void                DYN_ConstraintClose(DYN_tdst_Constraint *_pst_Constraint);
void                DYN_ConstraintReinit(DYN_tdst_Constraint *_pst_Constraint);
void                DYN_ConstraintInit
                    (
                        DYN_tdst_Constraint *_pst_Constraint,
                        char                _c_ConstraintType,
                        MATH_tdst_Vector    *_pst_A,
                        MATH_tdst_Vector    *_pst_B,
                        MATH_tdst_Vector    *_pst_O,
                        float               _f_r
                    );
void DYN_ApplyConstraint(DYN_tdst_Dyna *_pst_Dyna, float _f_dt);

/*
 ===================================================================================================
    Constraint to Segment
 ===================================================================================================
 */
void                DYN_ConstraintToSegment
                    (
                        DYN_tdst_Dyna       *_pst_Dyna,
                        MATH_tdst_Vector    *_pst_A,
                        MATH_tdst_Vector    *_pst_B
                    );

/*
 ===================================================================================================
    Constraint a position to a parabol
 ===================================================================================================
 */
void                DYN_ConstraintToParabol
                    (
                        DYN_tdst_Dyna       *_pst_Dyna,
                        MATH_tdst_Vector    *_pst_A,
                        MATH_tdst_Vector    *_pst_B,
                        MATH_tdst_Vector    *_pst_O,
                        float               _f_r
                    );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
