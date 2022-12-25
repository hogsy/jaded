/*$T Gx8debugfct.h GC!1.40 08/31/99 09:54:49 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GX8DEBUGFCT_H__
#define __GX8DEBUGFCT_H__

#include "GDInterface/GDInterface.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Function
 ***************************************************************************************************
 */

void    Gx8_DrawEllipse(GDI_tdst_DisplayData *, SOFT_tdst_Ellipse *);
void    Gx8_DrawArrow(GDI_tdst_DisplayData *, SOFT_tdst_Arrow *);
void    Gx8_DrawSquare(GDI_tdst_DisplayData *, SOFT_tdst_Square *);

#ifdef __cplusplus
}
#endif

#endif /*__DX8DEBUGFCT_H__*/
