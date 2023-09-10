/*$T MDFmodifier_GEO.h GC! 1.081 11/14/00 12:12:36 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef __MDFMODIFIER_WATER3D_H__
#define __MDFMODIFIER_WATER3D_H__

#include "BASe/BAStypes.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "GDInterface/GDInterface.h"

#ifdef _XENON_RENDER
#include "XenonGraphics/XeMesh.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Modifier const
 ***********************************************************************************************************************
 */

/*$4
 ***********************************************************************************************************************
    Modifier desc
 ***********************************************************************************************************************
 */

#define MAX_DISTURBANCE 20

#if defined(_XENON_RENDER)
# define MAX_PATCH_SIZE  128
#else
# define MAX_PATCH_SIZE  64
#endif

#define FLOAT_SCALE (0.00006047777f) //1/8192.0f : 0.00006047777f

#define WATER3D_Culled			0x00000001
#define WATER3D_Static			0x00000002

typedef struct WATER3D_tdst_Patches_
{
	FLOAT				fMinX; 
	FLOAT				fMaxX;
	FLOAT				fMinY;
	FLOAT				fMaxY;
	FLOAT				fZMinMin;	
	FLOAT				fZMinMax;	
	FLOAT				fZMaxMin;	
	FLOAT				fZMaxMax;	

	FLOAT				fMinXGlobal; 
	FLOAT				fMaxXGlobal;
	FLOAT				fMinYGlobal;
	FLOAT				fMaxYGlobal;

	CHAR				TurbulanceGuide;
	MATH_tdst_Vector	vDisturbances[MAX_DISTURBANCE]; 
	CHAR				nCurrentDisturbance;
	FLOAT				fDisturbanceMax;
	FLOAT				fLastTurbulance;

	UCHAR				GridWidth;
	UCHAR				GridHeight;
	void 			    *VZ;
	void				*SZ;

	ULONG				ulFlag;

	GEO_tdst_Object		GEO_Object;

	GEO_Vertex			*pGEO_Object_Points;
	GEO_Vertex			*pGEO_Object_Normals;

#ifndef _GAMECUBE	
	GEO_tdst_UV			*pGEO_Object_UVs;
#endif

	GEO_tdst_ElementIndexedTriangles	GEO_Element;

#if defined(_XENON_RENDER) && !defined(XML_CONV_TOOL)
    ULONG               m_ulPatchInfoId;
    XeMesh              *m_poWaterMesh;
    XeMesh              *m_poStaticWaterMesh;
#endif

} WATER3D_tdst_Patches;
		
		
typedef struct	WATER3D_tdst_Modifier_
{
#ifdef ACTIVE_EDITORS
	ULONG				ulCodeKey;
#endif

	BOOL								bActive;
	struct  OBJ_tdst_GameObject_  	 	*p_GO;
	MDF_tdst_Modifier					*pst_Mod;
	struct	WATER3D_tdst_Modifier_ 		*p_NextActiveWater3D;
	
	// water specs
	FLOAT				fDamping;
	FLOAT				fPropagationSpeed;
	FLOAT				fPerturbanceAmplitudeModifier;
	FLOAT				fImpactForceAttenuation;
	FLOAT				fTurbulanceAmplitude;
	LONG				lTurbulanceFactor;

	// radius bias
	FLOAT				fRadius;
	FLOAT				fDampingOutsideRadius;
	BOOL				bTurbulanceOffIfOutsideRadius;
	FLOAT				fRadiusCut;

	// interconnected patch definition
	WATER3D_tdst_Patches **PatchGrid;
	INT					PatchSizeX;
	INT					PatchSizeY;

	// water precision
	INT					Density;
	
	// water height (used for disturbances checks)
	FLOAT				fHeight; 

	// update
	BOOL				bUpdated;
	BOOL				bDrawn;
	
    // reflection/refraction
    unsigned int		bWaterChrome;	// For refraction-like effect
    FLOAT               fRefractionIntensity;
    FLOAT               fReflectionIntensity;
    FLOAT               fWaterDensity;

    // texture params
    FLOAT               fBaseMapOpacity;
    FLOAT               fMossMapOpacity;
    FLOAT               fMinX, fMaxX;
    FLOAT               fMinY, fMaxY;

    // fog
    FLOAT               fFogIntensity;

	// stats
#ifdef ACTIVE_EDITORS
	INT					iNbPoly;
#endif

} WATER3D_tdst_Modifier;

extern WATER3D_tdst_Modifier *p_FirstActiveWater3D;

/*$4
 ***********************************************************************************************************************
    Modifier functions
 ***********************************************************************************************************************
 */

extern void		WATER3D_Modifier_Create(struct OBJ_tdst_GameObject_ *, MDF_tdst_Modifier *,	void *);
extern void		WATER3D_Modifier_Destroy(MDF_tdst_Modifier *);
extern void 	WATER3D_Modifier_Reset( WATER3D_tdst_Modifier *);
extern void		WATER3D_Modifier_Apply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern void		WATER3D_Modifier_Unapply(MDF_tdst_Modifier *, struct GEO_tdst_Object_ *);
extern ULONG	WATER3D_Modifier_Load(MDF_tdst_Modifier *, char *);
extern void		WATER3D_Modifier_Reinit(MDF_tdst_Modifier *);
extern void		WATER3D_Modifier_Save(MDF_tdst_Modifier *);
extern void		WATER3D_Modifier_Disturb(OBJ_tdst_GameObject *pGO, MATH_tdst_Vector *pDisturberArray, FLOAT *pDisturberValues, INT nbDisturber, FLOAT fZOffset, BOOL bFloatOnWater, MATH_tdst_Vector *, MATH_tdst_Vector *);
extern WATER3D_tdst_Modifier* WATER3D_ModifierFind(OBJ_tdst_GameObject *_pst_GO);
extern void 	WATER3D_Modifier_Display(OBJ_tdst_GameObject *_pst_GO);

#ifdef __cplusplus
}
#endif

#endif 
