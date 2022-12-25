/*$T DYNBasic.h GC!1.52 10/20/99 14:16:47 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Main functions of the dynamics module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef _DYNBASIC_H_
#define _DYNBASIC_H_

/* #include "MATHs/MATH.h" */
#include "ENGine/Sources/DYNamics/DYNstruct.h"
#include "MATHs/MATH.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
void    DYN_ApplySpeedLimit(DYN_tdst_Dyna *_pst_Dyna);
float   DYN_f_FindFrictionForMaxSpeed(DYN_tdst_Dyna *_pst_Dyna, float _f_MaxSpeed);
int     DYN_b_SpeedReachInTime
        (
            DYN_tdst_Dyna       *_pst_Dyna,
            float               *_pf_K,
            MATH_tdst_Vector    *_pst_T,
            MATH_tdst_Vector    *_pst_V2,
            float               _f_dt
        );
void    DYN_SpeedAfterNseconds
        (
            DYN_tdst_Dyna       *_pst_Dyna,
            MATH_tdst_Vector    *_pst_FinalSpeed,
            float               _f_dt
        );
void    DYN_PositionAfterNseconds
        (
            DYN_tdst_Dyna       *_pst_Dyna,
            MATH_tdst_Vector    *_pst_FinalPos,
            float               _f_dt
        );
void    DYN_Position_ReachInTime
        (
            DYN_tdst_Dyna       *_pst_Dyna,
            MATH_tdst_Vector    *_pst_FinalPos,
            float               _f_dt
        );
void    DYN_CalculateSpeedWhenFrictionVector(DYN_tdst_Dyna *_pst_Dyna, float _f_Dt);



#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif
