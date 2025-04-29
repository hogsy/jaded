/*$T GEOobjectaccess.h GC!1.38 06/14/99 20:08:55 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#	pragma once
#endif

#ifndef __GEOOBJECTACCESS_H__
#	define __GEOOBJECTACCESS_H__


#	include "BASe/BAStypes.h"
#	include "GEOmetric/GEOobject.h"

/*$4
 ===================================================================================================
    Functions
 ===================================================================================================
 */

void GEO_SetPoint( GEO_tdst_Object *, LONG, float, float, float, float, float, float );
void GEO_AddPoint( GEO_tdst_Object *, float, float, float, float, float, float );
void GEO_AddSeveralPoint( GEO_tdst_Object *, int );
void GEO_DelPoint( GEO_tdst_Object *, int );

void GEO_SetUV( GEO_tdst_Object *, LONG, float, float );
void GEO_AddUV( GEO_tdst_Object *, float, float );
void GEO_DelUV( GEO_tdst_Object *, int );

void GEO_SetElement( GEO_tdst_Object *, LONG, LONG, LONG );
void GEO_AddElement( GEO_tdst_Object *, LONG, LONG );

void GEO_SetTriangle(
        GEO_tdst_ElementIndexedTriangles *,
        LONG,
        GEO_tdst_IndexedTriangle * );
void GEO_AddTriangle( GEO_tdst_ElementIndexedTriangles *, GEO_tdst_IndexedTriangle * );
void GEO_DelTriangle( GEO_tdst_ElementIndexedTriangles *, int );

#endif /* __GEOOBJECTACCESS_H__ */