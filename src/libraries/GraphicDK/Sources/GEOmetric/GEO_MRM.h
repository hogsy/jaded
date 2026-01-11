/*$T GEO_MRM.h GC! 1.081 05/18/00 16:44:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifndef __GEOMRM_H__
#define __GEOMRM_H__
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

#define GEO_MRM_None                0
#define GEO_MRM_Curve               1
#define GEO_MRM_LOD                 2


extern float GEO_gdf_MRMQualityCurve[16];


/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ ULONG GEO_MRM_ul_IsMrmObject(GEO_tdst_Object *pst_Object)
{
#ifndef PSX2_TARGET
	if(!pst_Object) return 0;
    if(pst_Object->st_Id.i->ul_Type != GRO_Geometric) return 0;
#endif    
	if(pst_Object->p_MRM_ObjectAdditionalInfo) return 1;
	return 0;
}

_inline_ ULONG GEO_ul_GetMRMState(GEO_tdst_Object *pst_Object)
{
    if (!pst_Object || (pst_Object->st_Id.i->ul_Type != GRO_Geometric))
        return GEO_MRM_None;
    else if (pst_Object->p_MRM_Levels)
        return GEO_MRM_LOD;
    else if (GEO_MRM_ul_IsMrmObject(pst_Object))
        return GEO_MRM_Curve;
    else
        return GEO_MRM_None;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ ULONG GEO_MRM_ul_IsAnyMrmObject(GEO_tdst_Object *pst_Object)
{
#ifndef PSX2_TARGET
	if(!pst_Object) return 0;
    if(pst_Object->st_Id.i->ul_Type != GRO_Geometric) return 0;
#endif    
    if (pst_Object->p_MRM_ObjectAdditionalInfo || pst_Object->p_MRM_Levels) return 1;
	return 0;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ float GEO_MRM_GetFloatFromUC(ULONG ul_LOD_QUal)
{
	/*~~~~~~~~~*/
	float	Coef;
	/*~~~~~~~~~*/

	Coef = (float) ul_LOD_QUal;
	Coef *= 1.0f / 256.0f;
	Coef = 1.0f - Coef;
	Coef = Coef * Coef * Coef;
	Coef *= Coef;
	Coef = 1.0f - Coef;
	return Coef;
}

ULONG	GEO_MRM_GetRealNumberOfPoints(struct GEO_tdst_Object_ *);

void    GEO_MRM_DestroyAll(GEO_tdst_Object *pst_Object);

void	GEO_MRM_Destroy(struct GEO_tdst_Object_ *);
void	GEO_MRM_SetNumberOfPoints_Number(struct GEO_tdst_Object_ *, ULONG NewNumber);
void	GEO_MRM_SetNumberOfPoints_Percent(struct GEO_tdst_Object_ *, float Quality);
void	GEO_MRM_ComputeUsedIndex(struct GEO_tdst_Object_ *);
#define GEO_MRM_RESET(Object)	{GEO_MRM_SetNumberOfPoints_Percent(Object, 2.0f); }
ULONG	GEO_MRM_GetNumberOfPoints_Curved(struct GEO_tdst_Object_ *, float Quality);
#ifdef ACTIVE_EDITORS
ULONG   GEO_MRM_GetNumberOfPoints_CurvedLevels(GEO_tdst_Object *pst_Object, float Quality);
#endif //ACTIVE_EDITORS

void    GEO_MRM_SetLevelFromQuality(GEO_tdst_Object *pst_Object,float _fQuality);
void    GEO_MRM_SetLevel(GEO_tdst_Object *pst_Object,ULONG _ulLevel);
void    GEO_MRM_SetAllLevels(GEO_tdst_Object *pst_Object);
void    GEO_MRM_v_ApplyReorder(LONG *_pValues, unsigned short *_pIndirection, LONG _lElementNb);
void    GEO_MRM_v_ApplyReverseReorder(LONG *_pValues, unsigned short *_pIndirection, LONG _lElementNb);
void    GEO_MRM_DeleteLevels(GEO_tdst_Object *pst_Object);
void	GEO_MRM_GetCurve(struct GEO_tdst_Object_ *, float *, float *);
void	GEO_MRM_SetCurve(struct GEO_tdst_Object_ *, float fExp, float fMin);

GEO_tdst_MRM_Levels *GEO_MRM_CreateLevels();
#ifdef ACTIVE_EDITORS
void	GEO_MRM_RemapUV(struct GEO_tdst_Object_ *);
BOOL    GEO_MRM_Compute(OBJ_tdst_GameObject *pst_GAO,GEO_tdst_Object *pst_12Object, BOOL _bInteractive, ULONG (*SeprogressPos) (float F01, char *));
void    GEO_MRM_ComputeLevels(OBJ_tdst_GameObject *pst_GAO,GEO_tdst_Object *pst_Object, BOOL _bInteractive, ULONG _ulLevelNb, float *_dfLevels ,float *_dfThresholds, ULONG (*SeprogressPos) (float F01, char *));
void	GEO_MRM_Optimize_Reorder_Triangles(struct GEO_tdst_Object_ *, ULONG (*) (float F01, char *));
#endif /* ACTIVE_EDITOR */

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif 
 