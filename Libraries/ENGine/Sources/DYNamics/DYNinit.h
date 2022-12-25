/*$T DYNinit.h GC!1.41 09/15/99 09:51:10 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Initialisations
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __DYNINIT_H__
#define __DYNINIT_H__
#include "BASe/BAStypes.h"

//#include "ENGine/Sources/DYNamics/DYN.h"
#include "ENGine/Sources/DYNamics/DYNstruct.h"
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$3
 ===================================================================================================
    Init the DYN_tdst_Dyna structure
 ===================================================================================================
 */

DYN_tdst_Dyna   *DYN_pst_Dyna_Create(void);

void            DYN_Dyna_Init
                (
                    DYN_tdst_Dyna       *_pst_Dyna,
                    LONG                _ul_DynFlags,
                    float               _f_Mass,
                    float               _f_SpeedLimitHoriz,
					float               _f_SpeedLimitVert,
                    MATH_tdst_Vector    *_pst_InitSpeed,
                    MATH_tdst_Vector    *_pst_Position
                );
void            DYN_Dyna_Close(DYN_tdst_Dyna *_pst_Dyna);
#ifdef ACTIVE_EDITORS
void            DYN_Dyna_RegisterWithName(DYN_tdst_Dyna *_pst_Dyna, char *_sz_Name, int _b_SetName);
void            DYN_Dyna_SetName(DYN_tdst_Dyna *_pst_Dyna, char *_sz_Name);
#else
#define DYN_Dyna_RegisterWithName(a, b, c)
#define DYN_Dyna_SetName(a, b)

#endif

/*$3
 ===================================================================================================
    Init the DYN_tdst_Forces structure
 ===================================================================================================
 */

DYN_tdst_Forces *DYN_pst_Forces_Create(void);
void            DYN_Forces_Init
                (
                    DYN_tdst_Forces     *_pst_Forces,
                    MATH_tdst_Vector    *_pst_Gravity,
                    MATH_tdst_Vector    *_pst_Traction,
                    MATH_tdst_Vector    *_pst_Stream,
                    float               _f_K
                );
void            DYN_Forces_Close(DYN_tdst_Forces *_pst_Forces);
void            DYN_Reinit(DYN_tdst_Dyna *_pst_Dyna);
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __DYNINIT_H__ */