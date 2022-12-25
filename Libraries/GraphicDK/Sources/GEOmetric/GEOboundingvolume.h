/*$T GEOobject.h GC!1.5 10/05/99 11:24:58 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GEOBOUNDINGVOLUME_H__
#define __GEOBOUNDINGVOLUME_H__

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "GDInterface/GDInterface.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif


/*
 ===================================================================================================
    Functions
 ===================================================================================================
 */
void GEO_BoundingVolume_Display(GDI_tdst_DisplayData *, OBJ_tdst_GameObject *);

#ifdef _XENON_RENDER
void GEO_BoundingVolume_DisplayElementsBV(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO);
#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __GEOBOUNDINGVOLUME_H__ */ 
 