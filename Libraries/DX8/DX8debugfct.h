/*$T DX8debugfct.h GC!1.40 08/31/99 09:54:49 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __DX8DEBUGFCT_H__
#define __DX8DEBUGFCT_H__

#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Function
 ***************************************************************************************************
 */

void    DX8_DrawEllipse(GDI_tdst_DisplayData *, SOFT_tdst_Ellipse *);
void    DX8_DrawArrow(GDI_tdst_DisplayData *, SOFT_tdst_Arrow *);
void    DX8_DrawSquare(GDI_tdst_DisplayData *, SOFT_tdst_Square *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /*__DX8DEBUGFCT_H__*/