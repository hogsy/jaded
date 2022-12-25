/*$T OGLdebugfct.h GC!1.40 08/31/99 09:54:49 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    OpenGL initialization / close / flip / restore
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GSPDEBUGFCT_H__
#define __GSPDEBUGFCT_H__

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

void    GSP_DrawEllipse(GDI_tdst_DisplayData *, SOFT_tdst_Ellipse *);
void    GSP_DrawArrow(GDI_tdst_DisplayData *, SOFT_tdst_Arrow *);
void    GSP_DrawSquare(GDI_tdst_DisplayData *, SOFT_tdst_Square *);

#ifdef __cplusplus
}
#endif

#endif /*__OGLDEBUGFCT_H__*/