/*$T GEOload.h GC!1.38 06/08/99 16:31:51 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GEOLOAD_H__
#define __GEOLOAD_H__


#include "BIGfiles/BIGfat.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

ULONG           GEO_ul_Load_ObjectCallback(ULONG _ul_PosFile);
GRO_tdst_Struct *GEO_pst_Object_Load(BIG_INDEX, WOR_tdst_World * );

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif

#endif /* __GEOLOAD_H__ */