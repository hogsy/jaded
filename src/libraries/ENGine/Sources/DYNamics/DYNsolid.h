/*$T DYNsolid.h GC!1.52 12/03/99 15:11:16 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Initialisation and close of the module
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#include "BASe/BASsys.h"/* Pour la definition du _inline_ */
#include "MATHs/MATH.h"
#include "ENGine/Sources/DYNamics/DYNstruct.h"
#include "ENGine/Sources/DYNamics/DYNconst.h"
#include "BASe/MEMory/MEM.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$3
 ===================================================================================================
    Init functions
 ===================================================================================================
 */

DYN_tdst_Solid  *DYN_pst_SolidCreate(void);
void            DYN_SolidInit
                (
                    DYN_tdst_Solid      *_pst_Solid,
                    MATH_tdst_Vector    *_pst_w,
                    MATH_tdst_Vector    *_pst_T,
                    MATH_tdst_Vector    *_pst_X,
                    float               _f_wLimit
                );
void            DYN_SolidReinit(DYN_tdst_Solid *_pst_Solid);
void            DYN_SolidClose(DYN_tdst_Solid *_pst_Solid);

/*$3
 ===================================================================================================
    Access functions
 ===================================================================================================
 */

_inline_ MATH_tdst_Vector *DYN_pst_WGet(DYN_tdst_Solid *_pst_Solid)
{
    if(MEM_b_CheckPointer(_pst_Solid))
        return(&_pst_Solid->st_w);
    else
        return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ MATH_tdst_Vector *DYN_pst_TGet(DYN_tdst_Solid *_pst_Solid)
{
    if(MEM_b_CheckPointer(_pst_Solid))
        return(&_pst_Solid->st_T);
    else
        return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ float DYN_f_wLimitGet(DYN_tdst_Solid *_pst_Solid)
{
    if(MEM_b_CheckPointer(_pst_Solid))
        return(_pst_Solid->f_wLimit);
    else
        return 0;
}

/*
 ===================================================================================================
 ===================================================================================================
 */
_inline_ void DYN_wLimitSet(DYN_tdst_Solid *_pst_Solid, float _f_wLimit)
{
    if(MEM_b_CheckPointer(_pst_Solid))
        _pst_Solid->f_wLimit=_f_wLimit;
}
void    DYN_CalcRotationSpeed(DYN_tdst_Dyna *_pst_Dyna, float _f_dt);
void    DYN_ApplyRotationSpeedLimit(DYN_tdst_Dyna *_pst_Dyna);
void    DYN_ApplyRotationSpeed(DYN_tdst_Dyna *_pst_Dyna, float _f_dt);
void    DYN_ApplyAngularVelocity(DYN_tdst_Dyna *_pst_Dyna, float _f_dt);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
