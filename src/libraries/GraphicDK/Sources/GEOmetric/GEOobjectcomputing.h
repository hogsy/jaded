 /*$T GEOobjectcomputing.h GC!1.38 06/14/99 20:06:43 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __GEOOBJECTCOMPUTING_H__
#define __GEOOBJECTCOMPUTING_H__



#include "BASe/BAStypes.h"
#include "GEOmetric/GEOobject.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 ===================================================================================================
    $4 functions
 ===================================================================================================
 */
void    GEO_ComputeNormals(GEO_tdst_Object *);
LONG    GEO_l_GetNumberOfTriangles( GEO_tdst_Object * );
#if defined(_XENON_RENDER)

void    GEO_CalculateTangentSpaceBasis(GEO_tdst_Object* poGeoObj,
                                       ULONG ulElement,
                                       GEO_tdst_TextureSpaceBasis *pBasis,
                                       GEO_Vertex* _pVertexList );
#endif // _XENON || ACTIVE_EDITORS

#ifdef ACTIVE_EDITORS

void    GEO_ComputeNormalsOfSelected(GEO_tdst_Object *, char * );
void    GEO_ComputeNormalsOfPlanetAura( GEO_tdst_Object * );
void    GEO_PickRLI( GEO_tdst_Object *, ULONG **, GEO_tdst_Object *, ULONG *, char);


void	GEO_Clean(OBJ_tdst_GameObject *pst_GO , GEO_tdst_Object *__pst_Object , ULONG *p_AdditionalVertexColor);
void	GEO_OptimizeNumberOfElement(GEO_tdst_Object *__pst_Object);
void	GEO_Weld_XYZ_UV(GEO_tdst_Object *__pst_Object , BOOL CleanXYZ , BOOL CleanUV);
void	GEO_Clean_Faces(GEO_tdst_Object *__pst_Object );
void	GEO_Clean_XYZ_UV(OBJ_tdst_GameObject *pst_GO , GEO_tdst_Object *__pst_Object ,ULONG *p_OtherColor , BOOL CleanXYZ , BOOL CleanUV);

#else

#define GEO_ComputeNormalsOfSelected(a,b)
#define GEO_ComputeNormalsOfPlanetAura( a )
#define GEO_PickRLI(a,b,c,d,e)

#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif


#endif /* __GEOOBJECTCOMPUTING_H__ */