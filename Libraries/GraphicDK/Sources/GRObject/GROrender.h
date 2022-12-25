/*$T GROrender.h GC!1.52 11/18/99 11:31:02 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#include "BASe/BAStypes.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***************************************************************************************************
    Functions
 ***************************************************************************************************
 */

void    GRO_Render(OBJ_tdst_GameObject *);
void    GRO_RenderGro(OBJ_tdst_GameObject *, GRO_tdst_Struct *);
void    GRO_RenderPickableObject
        (
            OBJ_tdst_GameObject *,
            GEO_tdst_Object *,
            ULONG,
            ULONG
        );
#ifdef JADEFUSION
void    GAO_Render(OBJ_tdst_GameObject *);
#endif

#ifdef ACTIVE_EDITORS
void    GRO_RenderTrace(OBJ_tdst_GameObject *);
#else
#define GRO_RenderTrace(a)
#endif


#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
