/*$T GEO_MRM.c GC!1.55 01/25/00 10:22:47 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "LINks/LINKtoed.h"
#include "LINks/LINKstruct.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJBoundingVolume.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"

#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrasters.h"
#include "SELection/SELection.h"
#include "GRObject/GROstruct.h"
#include "GRObject/GROedit.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOobjectaccess.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_MRM.h"
#include "GEOmetric/GEO_SKIN.h"

#ifdef ACTIVE_EDITORS
#include <math.h>
#include <STDLIB.H>
#include "../dlls/MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "../dlls/MAD_mem/Sources/MAD_mem.h"
#include "GEOmetric/GEO_LODCmpt.h"
#endif
#ifdef PSX2_TARGET
// mamagouille
#include "PSX2debug.h"
#endif

#include "BASe/BENch/BENch.h"

#define MAT_C_f_8Bits   (32768.0f + 16384.0f)
#define IFAL            lInterpretFloatAsLong
#define ILAF            fInterpretLongAsFloat

float GEO_gdf_MRMQualityCurve[16];
/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_MRM_GetRealNumberOfPoints(GEO_tdst_Object *pst_Object)
{
    if(GEO_MRM_ul_IsMrmObject(pst_Object))
        return pst_Object->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints;
    else
        return pst_Object->l_NbPoints;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_MRM_SetNumberOfPoints_Number(GEO_tdst_Object *pst_Object, ULONG NewNumber)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
	unsigned short						*ConvertionBuffer,*p_Current,*p_CurrentLast,*p_Absorber;
	ULONG                               LastPosition;
	unsigned short						Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    if(!GEO_MRM_ul_IsMrmObject(pst_Object)) return;

 	if (GDI_gpst_CurDD) PRO_StartTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_ComputeMRM);

	if (!GDI_gpst_CurDD)
		ConvertionBuffer = (unsigned short *) MEM_p_Alloc(pst_Object ->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints * (LONG)2);
	else
		ConvertionBuffer = (unsigned short *) GDI_gpst_CurDD->pst_ComputingBuffers->aul_Diffuse;

	if (NewNumber < 8)
		NewNumber = 8;
	if (NewNumber > pst_Object -> p_MRM_ObjectAdditionalInfo->RealNumberOfPoints)
		NewNumber = pst_Object -> p_MRM_ObjectAdditionalInfo->RealNumberOfPoints;

	LastPosition = pst_Object -> l_NbPoints;
	pst_Object -> l_NbPoints = NewNumber;

	if (GDI_gpst_CurDD)
		GDI_gpst_CurDD->pus_ReorderBuffer = (unsigned short *)pst_Object ->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer;

	p_Current = ConvertionBuffer ;
	p_CurrentLast = ConvertionBuffer + NewNumber;
	Counter = 0;
	while (p_Current < p_CurrentLast ) *(p_Current ++) = Counter++;
	

	p_CurrentLast = ConvertionBuffer + pst_Object -> p_MRM_ObjectAdditionalInfo->RealNumberOfPoints;
	p_Absorber = (unsigned short *)(pst_Object ->p_MRM_ObjectAdditionalInfo->Absorbers + Counter);
	while (p_Current < p_CurrentLast ) 
	{
		if (*p_Absorber >= NewNumber) 
			*(p_Current ++) = ConvertionBuffer[*(p_Absorber++)];
		else
			*(p_Current ++) = *(p_Absorber++);
	}

    pst_Element = pst_Object->dst_Element;
    pst_LastElement = pst_Element + pst_Object->l_NbElements;
    while(pst_Element < pst_LastElement)
    {
		if (pst_Element ->pus_ListOfUsedIndex)
		{
			p_Current = pst_Element -> pus_ListOfUsedIndex;
			pst_Element -> ul_NumberOfUsedIndex = 0;
			while (*(p_Current++) < NewNumber ) pst_Element -> ul_NumberOfUsedIndex++;
		}
        pst_Triangle = pst_Element->dst_Triangle;
        pst_LastTriangle = pst_Triangle + pst_Element->p_MrmElementAdditionalInfo->ul_RealNumberOfTriangle;
		pst_Element->l_NbTriangles = 0;
		if (pst_Element->p_MrmElementAdditionalInfo->ul_RealNumberOfTriangle)
        while (pst_Triangle -> ul_SmoothingGroup < NewNumber)
        {
			pst_Triangle ->auw_Index[0] = ConvertionBuffer[pst_Triangle -> auw_UV[0]];
			pst_Triangle ->auw_Index[1] = ConvertionBuffer[pst_Triangle -> auw_UV[1]];
			pst_Triangle ->auw_Index[2] = ConvertionBuffer[pst_Triangle -> auw_UV[2]];
			pst_Element->l_NbTriangles ++;
            pst_Triangle++;
        }

        pst_Element++;
    }

	if (GDI_gpst_CurDD) PRO_StopTrameRaster(&GDI_gpst_CurDD->pst_Raster->st_ComputeMRM);

	if (!GDI_gpst_CurDD)
		MEM_Free(ConvertionBuffer);
}

ULONG GEO_MRM_GetNumberOfPointsFromQuality(GEO_tdst_Object *pst_Object, float Quality)
{
    ULONG ulResult; 
    if (!GEO_MRM_ul_IsMrmObject(pst_Object)) return pst_Object->l_NbPoints;

	if (Quality >= 1.0f) return pst_Object->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints;

	ulResult = (ULONG)((float) pst_Object->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints * Quality);
    return ((ulResult > pst_Object->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints) ? 
        pst_Object->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints : (ulResult < 8 ? 8 : ulResult));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_MRM_GetNumberOfPoints_Curved(GEO_tdst_Object *pst_Object, float Quality)
{
	ULONG Index,L3;
	float RealQuality;
    if (!GEO_MRM_ul_IsMrmObject(pst_Object)) return pst_Object->l_NbPoints;

	if (Quality >= 1.0f) return pst_Object->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints;

	if (GEO_gdf_MRMQualityCurve[0] != 0.0f)
		Quality *= GEO_gdf_MRMQualityCurve[0];

	ILAF(Index) = 8.0f * Quality + MAT_C_f_8Bits;
	L3 = Index;
	L3 &= 0xfff000ff;
	RealQuality = Quality;
	Quality = ILAF(L3) - MAT_C_f_8Bits;

	Index &= 0x000fffff;
	Index >>= 8;

	if (Index == 0) 
		RealQuality = Quality * GEO_gdf_MRMQualityCurve[Index+1];
	else
		if (Index > 7)
			return pst_Object->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints;
		else
			if (Index == 7)
				RealQuality = (1.0f - Quality) * GEO_gdf_MRMQualityCurve[Index] + (Quality) * 1.0f;
			else
				RealQuality = (1.0f - Quality) * GEO_gdf_MRMQualityCurve[Index] + (Quality) * GEO_gdf_MRMQualityCurve[Index+1];

	Index = (ULONG)((float) pst_Object->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints * RealQuality);
	if (Index < 8) Index = 8;

	return Index;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
ULONG GEO_MRM_GetNumberOfPoints_CurvedLevels(GEO_tdst_Object *pst_Object, float Quality)
{
    ULONG ulLevelNb,i;
    float *afDistanceThreshold,*afPointNbRatio;

    if (!pst_Object->p_MRM_Levels || Quality >= 1.0f || pst_Object->p_MRM_Levels->ul_LevelNb < 2)  
        return pst_Object->l_NbPoints;

    ulLevelNb = pst_Object->p_MRM_Levels->ul_LevelNb;
    afDistanceThreshold = pst_Object->p_MRM_Levels->f_Thresholds;
    afPointNbRatio = pst_Object->p_MRM_Levels->f_MRMLevels;

    for (i=0; i<ulLevelNb; i++)
    {
        if (Quality > afDistanceThreshold[i])
        {
            if (i==0)
                return pst_Object->l_NbPoints;
            else
                return (ULONG)((float)pst_Object->l_NbPoints * afPointNbRatio[i-1]);
        }
    }

    return (ULONG)((float)pst_Object->l_NbPoints * afPointNbRatio[i-1]);
}
#endif //ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_MRM_SetNumberOfPoints_Percent(GEO_tdst_Object *pst_Object, float Quality)
{
    if(!GEO_MRM_ul_IsMrmObject(pst_Object)) return;

	_GSP_BeginRaster(22);

    GEO_MRM_SetNumberOfPoints_Number
    (
        pst_Object,
        GEO_MRM_GetNumberOfPoints_Curved(pst_Object, Quality)
    );

	_GSP_EndRaster(22);
}

void GEO_MRM_v_ApplyReorder(LONG *_pValues, unsigned short *_pIndirection, LONG _lElementNb)
{
    LONG i;
    LONG *pVectorSrc = (LONG* )MEM_p_AllocFromEnd(_lElementNb*sizeof(LONG));

    // Store values in array.
    for (i=0; i<_lElementNb; i++)
        pVectorSrc[i] = _pValues[i];

    // apply indirection 
    for (i=0; i<_lElementNb; i++)
        _pValues[i] = pVectorSrc[_pIndirection[i]];

    MEM_FreeFromEnd(pVectorSrc);
}

extern "C" WOR_tdst_World *WOR_gpst_CurrentWorld;
void GEO_MRM_DeleteLevels(GEO_tdst_Object *pst_Object)
{
    if (pst_Object->p_MRM_Levels)
    {
        TAB_tdst_PFelem     *pst_Elem, *pst_LastElem;
        WOR_tdst_World		*pst_World;
        int i;

        // Remove indirection to all RLI in world that use this geometry.
        if (pst_Object->p_MRM_Levels->dus_ReorderBuffer)
        {
            pst_World = WOR_gpst_CurrentWorld;
            pst_Elem = pst_World->st_AllWorldObjects.p_Table;
            pst_LastElem = pst_World->st_AllWorldObjects.p_NextElem;
            for(; pst_Elem < pst_LastElem; pst_Elem++)
            {
                OBJ_tdst_GameObject *GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
                if (TAB_b_IsAHole(GO)) continue;

                if (GO && GO->pst_Base && GO->pst_Base->pst_Visu && 
                    ((GEO_tdst_Object *)GO->pst_Base->pst_Visu->pst_Object == pst_Object) &&
                    GO->pst_Base->pst_Visu->dul_VertexColors)
                {
                    LONG uArraySize = GO->pst_Base->pst_Visu->dul_VertexColors[0];
                    if (uArraySize > pst_Object->l_NbPoints)
                    {
                        ERR_X_Warning(0,"Instance vertex color buffer size is bigger than vertex buffer size, reduce instance vertex color buffer", GO->sz_Name);
                        GO->pst_Base->pst_Visu->dul_VertexColors[0] = pst_Object->l_NbPoints;
                    }
                    else if (uArraySize < pst_Object->l_NbPoints)
                    {
                        ERR_X_Warning(0,"Instance vertex color buffer size is smaller than vertex buffer size, delete instance vertex color buffer ", GO->sz_Name);
                        MEM_Free(GO->pst_Base->pst_Visu->dul_VertexColors);
                        GO->pst_Base->pst_Visu->dul_VertexColors = NULL;
                        continue;
                    }

                    GEO_MRM_v_ApplyReverseReorder((LONG* )(GO->pst_Base->pst_Visu->dul_VertexColors+1),pst_Object->p_MRM_Levels->dus_ReorderBuffer,pst_Object->l_NbPoints);
                }
            }
        }

        pst_Object->l_NbElements = pst_Object->p_MRM_Levels->dl_ElementNb[0];

        // Free elements.
        for (i=pst_Object->p_MRM_Levels->dl_ElementNb[0] ; i<(int)pst_Object->p_MRM_Levels->l_TotalElementNb; i++)
        {
            GEO_tdst_ElementIndexedTriangles *pElements = pst_Object->p_MRM_Levels->a_ElementBase + i;
            GEO_FreeElementContent(pElements);
        }

        // Do no free pst_Object->p_MRM_Levels->a_ElementBase (it is used)
        MEM_Free(pst_Object->p_MRM_Levels->dus_ReorderBuffer);
        MEM_Free(pst_Object->p_MRM_Levels);
        pst_Object->p_MRM_Levels = NULL;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_MRM_SetCurve(GEO_tdst_Object *pst_Object, float fExp , float fMin)
{
	ULONG Counter;

    if (!pst_Object || !GEO_gdf_MRMQualityCurve)
        return;

	if (fExp <= 0.1f) fExp = 0.1f;
	if (fExp >= 6.0f) fExp = 6.0f;
	for (Counter = 0 ; Counter < 8 ; Counter ++)
        GEO_gdf_MRMQualityCurve[Counter] = powf(Counter * 0.125f, fExp);
	
    if (fMin < 0.1f) fMin = 0.1f;
	if (fMin > 1.0f) fMin = 1.0f;
	GEO_gdf_MRMQualityCurve[0] = 1.0f/ fMin;
}

// Put all levels in geometry (used outside of display)
void GEO_MRM_SetAllLevels(GEO_tdst_Object *pst_Object)
{
    int i;
    if (!pst_Object->p_MRM_Levels)
        return;

    ERR_X_Assert((pst_Object->p_MRM_Levels->ul_CurrentLevel >= 0) && (pst_Object->p_MRM_Levels->ul_CurrentLevel < GEO_MRM_MAX_LEVEL_NB));

    // Set material high bits to level index (pseudo materials used 
    // so that 2 identical materials in different levels do not have the same material).
    for (i=0; i<pst_Object->l_NbElements; i++)
    {
        pst_Object->dst_Element[i].l_MaterialId |= (pst_Object->p_MRM_Levels->ul_CurrentLevel << 8);
    }
    pst_Object->dst_Element = pst_Object->p_MRM_Levels->a_ElementBase;
    pst_Object->l_NbElements = pst_Object->p_MRM_Levels->l_TotalElementNb;
    pst_Object->l_NbPoints = pst_Object->p_MRM_Levels->l_TotalPointNb;
    pst_Object->l_NbUVs = pst_Object->p_MRM_Levels->l_TotalUVNb;
    pst_Object->p_MRM_Levels->ul_CurrentLevel = 0xFFFFFFFF;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_MRM_SetLevelFromQuality(GEO_tdst_Object *pst_Object,float _fQuality)
{
    if (pst_Object->p_MRM_Levels)
    {
        ULONG level = 0;
        _fQuality *= pst_Object->p_MRM_Levels->f_DistanceCoef;
        while ((_fQuality < pst_Object->p_MRM_Levels->f_Thresholds[level]) && 
            (level < pst_Object->p_MRM_Levels->ul_LevelNb - 1))
            level++;

        GEO_MRM_SetLevel(pst_Object,level);
    }
}

// Put chosen level in geometry.
void GEO_MRM_SetLevel(GEO_tdst_Object *pst_Object,ULONG _ulLevel)
{
    int i;
    if (!pst_Object->p_MRM_Levels)
        return;

    ERR_X_Assert(pst_Object->p_MRM_Levels->ul_CurrentLevel == 0xFFFFFFFF);
    ERR_X_Assert(_ulLevel>=0 && _ulLevel<pst_Object->p_MRM_Levels->ul_LevelNb);

    pst_Object->p_MRM_Levels->ul_CurrentLevel = _ulLevel;
    pst_Object->dst_Element = pst_Object->p_MRM_Levels->aa_Element[_ulLevel];
    pst_Object->l_NbElements = pst_Object->p_MRM_Levels->dl_ElementNb[_ulLevel];
    pst_Object->l_NbPoints = pst_Object->p_MRM_Levels->dl_PointNb[_ulLevel];
    pst_Object->l_NbUVs = pst_Object->p_MRM_Levels->dl_UVNb[_ulLevel];

    // Set material high bits to 0 (real material values).
    for (i=0; i<pst_Object->l_NbElements; i++)
    {
        pst_Object->dst_Element[i].l_MaterialId &= 0x000000FF;
    }
}

void GEO_MRM_v_ApplyReverseReorder(LONG *_pValues, unsigned short *_pIndirection, LONG _lElementNb)
{
    unsigned short i;
    LONG *pVectorSrc = (LONG* )MEM_p_AllocFromEnd(_lElementNb*sizeof(LONG));
    unsigned short *pReverseIndirection = (unsigned short* ) MEM_p_AllocFromEnd(_lElementNb*sizeof(unsigned short));

    // Build reverse indirection
    for (i=0; i<_lElementNb; i++)
        pReverseIndirection[_pIndirection[i]] = i;

    // Save values
    L_memcpy(pVectorSrc,_pValues,sizeof(LONG)*_lElementNb);

    // Apply reverse indirection 
    for (i=0; i<_lElementNb; i++)
        _pValues[i] = pVectorSrc[pReverseIndirection[i]];

    MEM_FreeFromEnd(pReverseIndirection);
    MEM_FreeFromEnd(pVectorSrc);
}

GEO_tdst_MRM_Levels *GEO_MRM_CreateLevels()
{
    GEO_tdst_MRM_Levels *pLevels = (GEO_tdst_MRM_Levels *)MEM_p_Alloc(sizeof(GEO_tdst_MRM_Levels));
    L_memset(pLevels,0,sizeof(GEO_tdst_MRM_Levels));
    pLevels->f_DistanceCoef = 1.f;
    return pLevels;
}


#ifdef ACTIVE_EDITORS
int GEO_MRM_CompTriangleCLBK(const void *T1,const void *T2)
{
	GEO_tdst_IndexedTriangle            *pst_T1,*pst_T2;
	pst_T1 = (GEO_tdst_IndexedTriangle  *)T1;
	pst_T2 = (GEO_tdst_IndexedTriangle  *)T2;
	if (pst_T1 -> ul_SmoothingGroup < pst_T2 -> ul_SmoothingGroup) return -1;
	if (pst_T1 -> ul_SmoothingGroup == pst_T2 -> ul_SmoothingGroup) return 0;
	return 1;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_MRM_Optimize_Reorder_Triangles(GEO_tdst_Object *pst_Object, ULONG (*SeprogressPos) (float F01, char *))
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle,st_Triangle;
	ULONG								Counter,Lastindex;
	ULONG								TNumFaces, CNumFace;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
    pst_Element = pst_Object->dst_Element;
    pst_LastElement = pst_Element + pst_Object->l_NbElements;

    for(TNumFaces = 0; pst_Element < pst_LastElement; pst_Element++)
			TNumFaces += pst_Element->l_NbTriangles;

    pst_Element = pst_Object->dst_Element;
    for(CNumFace = 0; pst_Element < pst_LastElement; pst_Element++)
    {
		SeprogressPos ((float)CNumFace / (float)TNumFaces , "Optimize triangles");
		pst_Triangle = pst_Element->dst_Triangle;
		pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;
		while (pst_Triangle < pst_LastTriangle)
		{
			st_Triangle = *pst_Triangle;
			while ((st_Triangle . auw_UV[0] != st_Triangle . auw_UV[1]) && 
					(st_Triangle . auw_UV[0] != st_Triangle . auw_UV[2]) &&
					(st_Triangle . auw_UV[1] != st_Triangle . auw_UV[2]))
			{
				Counter = 0;
				if (st_Triangle . auw_UV[0] < st_Triangle . auw_UV[1]) Counter = 1;
				if (st_Triangle . auw_UV[Counter] < st_Triangle . auw_UV[2]) Counter = 2;

				Lastindex = st_Triangle . auw_UV[Counter];

				st_Triangle . auw_UV[Counter] = pst_Object ->p_MRM_ObjectAdditionalInfo->Absorbers[st_Triangle . auw_UV[Counter]];
			}
			pst_Triangle -> ul_SmoothingGroup = Lastindex;
			pst_Triangle++;
		}
		qsort ((void *)pst_Element->dst_Triangle,pst_Element->l_NbTriangles,sizeof(GEO_tdst_IndexedTriangle),GEO_MRM_CompTriangleCLBK);
    }


}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void    GEO_MRM_GetCurve(GEO_tdst_Object *pst_Object, float *p_fExp , float *p_fMin)
{
	if(!pst_Object || !(GEO_gdf_MRMQualityCurve))
	{
		*p_fExp = 0.0f;
		*p_fMin = 0.0f;
		return;
	}
	*p_fMin = GEO_gdf_MRMQualityCurve[0];
	if ((*p_fMin) == 0.0f) 
		*p_fMin = 1.0f;
	*p_fMin = 1.0f / *p_fMin;
	*p_fExp = (float)(log((double)GEO_gdf_MRMQualityCurve[4]) / log(0.5));
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_MRM_RemapUV(GEO_tdst_Object *pst_Object)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
	unsigned short						*p_Redir,*p_FrstElm,*p_LstElm;
	ULONG								Counter;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Element = pst_Object->dst_Element;
    pst_LastElement = pst_Element + pst_Object->l_NbElements;
	p_Redir = (unsigned short *) MEM_p_Alloc( (LONG)2 * pst_Object -> l_NbPoints );

    for(; pst_Element < pst_LastElement; pst_Element++)
    {
		L_memset(p_Redir , 0 , pst_Object -> l_NbPoints * (LONG)2);
		p_FrstElm = pst_Element -> pus_ListOfUsedIndex ;
		p_LstElm  = p_FrstElm + pst_Element -> ul_NumberOfUsedIndex;
		Counter = 0;
		while (p_FrstElm < p_LstElm ) p_Redir[*(p_FrstElm++)] = (unsigned short)(Counter++ + pst_Element ->p_MrmElementAdditionalInfo->ul_One_UV_Per_Point_Per_Element_Base);
		pst_Triangle = pst_Element->dst_Triangle;
		pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;
		while (pst_Triangle < pst_LastTriangle)
		{
			pst_Triangle->auw_UV[0] = p_Redir[pst_Triangle->auw_Index[0]];
			pst_Triangle->auw_UV[1] = p_Redir[pst_Triangle->auw_Index[1]];
			pst_Triangle->auw_UV[2] = p_Redir[pst_Triangle->auw_Index[2]];
			pst_Triangle++;
		}
    }
    MEM_Free(p_Redir);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_MRM_Free(GEO_tdst_Object *pst_Object)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_Object) return;
    if(!GEO_MRM_ul_IsMrmObject(pst_Object)) return;

    MEM_Free(pst_Object->p_MRM_ObjectAdditionalInfo->Absorbers);
	if (pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer)
		MEM_Free(pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer);
		
	if (pst_Object->p_MRM_ObjectAdditionalInfo->df_Errors)
		MEM_Free(pst_Object->p_MRM_ObjectAdditionalInfo->df_Errors);

    MEM_Free(pst_Object->p_MRM_ObjectAdditionalInfo);
    pst_Object->p_MRM_ObjectAdditionalInfo = NULL;

    pst_Element = pst_Object->dst_Element;
    pst_LastElement = pst_Element + pst_Object->l_NbElements;

    for(; pst_Element < pst_LastElement; pst_Element++)
    {
		MEM_Free(pst_Element->p_MrmElementAdditionalInfo);
        pst_Element->p_MrmElementAdditionalInfo = NULL;
    }

}


void GEO_MRM_DestroyAll(GEO_tdst_Object *pst_Object)
{
    if (!pst_Object)
        return;

    GEO_MRM_DeleteLevels(pst_Object);
    GEO_MRM_Destroy(pst_Object);
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_MRM_Destroy(GEO_tdst_Object *pst_Object)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
	MATH_tdst_Vector                    *p_Src,*p_SrcNormals;
	ULONG								ulCounter,*p_Colors;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_Object) return;
    if(!GEO_MRM_ul_IsMrmObject(pst_Object)) return;

    GEO_MRM_RESET(pst_Object);

	GEO_MRM_ComputeUsedIndex(pst_Object);
	/* Here I Reorder vertices to find Originals position */
	if (pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer)
	{
		p_Src  = (MATH_tdst_Vector*)MEM_p_Alloc(sizeof (MATH_tdst_Vector) * pst_Object->l_NbPoints);
		p_SrcNormals = (MATH_tdst_Vector*)MEM_p_Alloc(sizeof (MATH_tdst_Vector) * pst_Object->l_NbPoints);
		L_memcpy(p_Src,pst_Object->dst_Point,sizeof (MATH_tdst_Vector) * pst_Object->l_NbPoints);
		GEO_UseNormals(pst_Object);
		L_memcpy(p_SrcNormals,pst_Object->dst_PointNormal,sizeof (MATH_tdst_Vector) * pst_Object->l_NbPoints);
		if (pst_Object->dul_PointColors)
		{
			p_Colors = (ULONG*)MEM_p_Alloc(sizeof (ULONG) * pst_Object->l_NbPoints);
			L_memcpy(p_Colors,pst_Object->dul_PointColors,sizeof (ULONG) * pst_Object->l_NbPoints);
		}
		ulCounter = pst_Object->l_NbPoints;
		while (ulCounter--) 
		{
			// Vertices 
			*(MATH_tdst_Vector *) &pst_Object->dst_Point[pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[ulCounter]] = p_Src[ulCounter];
			// Normales 
			pst_Object->dst_PointNormal[pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[ulCounter]] = p_SrcNormals[ulCounter];
			// Colors if exist
			if (pst_Object->dul_PointColors)
				pst_Object->dul_PointColors[pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[ulCounter]] = p_Colors[ulCounter];
		}
		if (GEO_SKN_IsSkinned(pst_Object))
		{
			ULONG ulCounter2;
			GEO_SKN_Compress(pst_Object);
			ulCounter = pst_Object->p_SKN_Objectponderation->NumberPdrtLists;
			// Skin ponderation
			while (ulCounter--) 
			{
				ulCounter2 = pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ulCounter]->us_NumberOfPonderatedVertices;
				while (ulCounter2--)
					pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ulCounter]->p_PdrtVrc_C[ulCounter2].Index = 
						pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[pst_Object->p_SKN_Objectponderation->pp_PdrtLst[ulCounter]->p_PdrtVrc_C[ulCounter2].Index];
			}
			// The two followinf functions force to reorder ponderations 
			GEO_SKN_Expand(pst_Object);
			GEO_SKN_Compress(pst_Object);
		}

		MEM_Free(p_Src);
		MEM_Free(p_SrcNormals);
		pst_Element = pst_Object->dst_Element;
		pst_LastElement = pst_Element + pst_Object->l_NbElements;
		while (pst_Element < pst_LastElement)
		{
			ulCounter = pst_Element->l_NbTriangles;
			// Reorder triangles indexes 
			while (ulCounter--)
			{
				pst_Element->dst_Triangle[ulCounter].auw_Index[0] = pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[pst_Element->dst_Triangle[ulCounter].auw_Index[0]];
				pst_Element->dst_Triangle[ulCounter].auw_Index[1] = pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[pst_Element->dst_Triangle[ulCounter].auw_Index[1]];
				pst_Element->dst_Triangle[ulCounter].auw_Index[2] = pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[pst_Element->dst_Triangle[ulCounter].auw_Index[2]];
				pst_Element->dst_Triangle[ulCounter].auw_UV[0] =	pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[pst_Element->dst_Triangle[ulCounter].auw_UV[0]];
				pst_Element->dst_Triangle[ulCounter].auw_UV[1] =	pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[pst_Element->dst_Triangle[ulCounter].auw_UV[1]];
				pst_Element->dst_Triangle[ulCounter].auw_UV[2] =	pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[pst_Element->dst_Triangle[ulCounter].auw_UV[2]];
			}
			// Reorder Used indexes 
			ulCounter = pst_Element->ul_NumberOfUsedIndex;
			while (ulCounter--)
			{
				pst_Element->pus_ListOfUsedIndex[ulCounter] = pst_Object->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[pst_Element->pus_ListOfUsedIndex[ulCounter]];
			}
			pst_Element++;
		}
	}
	/* Recompute UV */
	GEO_MRM_RemapUV(pst_Object);
	
    GEO_MRM_Free(pst_Object);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG SeprogressPos_BLANK(float F01, char *X)
{
    return 0;
}

#define SWAP(a,b) Swp = (ULONG)a;a = b;*(ULONG *)&b = Swp;
/*
 =======================================================================================================================
 =======================================================================================================================
 */

#define GEO_MRM_MinTotalPointNb 32
#define GEO_MRM_MinPointNb (GEO_MRM_MinTotalPointNb>>1)

BOOL GEO_MRM_Compute(OBJ_tdst_GameObject *pst_GAO,GEO_tdst_Object *pst_12Object, BOOL _bInteractive, ULONG (*SeprogressPos) (float F01, char *))
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MAD_GeometricObject *p_MadObject;
	GEO_tdst_Object		*pst_NewObject;
	GEO_tdst_Object		Swp;
	GRO_tdst_Struct     GRO_Keeper;
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
	MAD_GeometricObjectElement			**pst_MadElement;
	unsigned short						*p_usReorder; 
	float 								*df_Errors;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    // The min nb of points is arbitrary
    if (!pst_12Object || pst_12Object->l_NbElements <= 0 || pst_12Object -> l_NbPoints <= GEO_MRM_MinTotalPointNb)
    {
        return FALSE;
    }

    if(GEO_MRM_ul_IsMrmObject(pst_12Object))
        GEO_MRM_Destroy(pst_12Object);

    if(SeprogressPos == NULL)
        SeprogressPos = SeprogressPos_BLANK;

    Mad_meminit();

    SeprogressPos(0.0f, "Convert Object...");

    p_MadObject = (MAD_GeometricObject *) pst_12Object->st_Id.i->pfnp_ToMad(pst_12Object, NULL);
	if (!MAD_IsMagnifold(p_MadObject))
	{
        if (_bInteractive)
        {
    		char Str[1024];
	    	sprintf(Str , "The object is non-Magnifold. \n That mean that some edges touch more than two triangles. \n If you compute MRM on this, it may crash. \n Nevertheless do you want to try? ");
            if (IDNO == MessageBox ( NULL, Str, TEXT("Be carefull"), MB_YESNO | MB_ICONWARNING | MB_TASKMODAL | MB_SETFOREGROUND ))
	    	{
		    	Mad_free();
			    return FALSE;
	    	}
        }
        else
        {
            // MAD_MakeItMagnifold(p_MadObject);  plante en beauté
            ERR_X_Warning(0,"The object is non-Magnifold, can't compute LOD", pst_GAO ? pst_GAO->sz_Name : "Unknown GAO");
		    Mad_free();
			return FALSE;
        }
	}

	MAD_ReComputeElements(p_MadObject);

    if (p_MadObject->NumberOfElements <= 0 || p_MadObject -> NumberOfPoints <= 0)
    {
        Mad_free();
        return FALSE;
    }


	p_usReorder = (unsigned short *) MEM_p_Alloc(sizeof(unsigned short) * pst_12Object -> l_NbPoints);


	p_MadObject -> RealNumberOfPoints = pst_12Object->l_NbPoints;
    SeprogressPos(0.0f, "Compute one UV per vertex per Element...");
    MAD_ComputeOneIndexPerUVPerElementSolution(p_MadObject,1);
	MAD_OptimizeElements(p_MadObject);
	MAD_RestoreOneIndexPerUVPerElementSolution(p_MadObject);
    SeprogressPos(0.0f, "Compute LOD...");
	LOD_C_ComputeObjectLOD(p_MadObject, SeprogressPos , &p_usReorder, &df_Errors);
	MAD_RestoreOneIndexPerUVPerElementSolution(p_MadObject); /* I don't know why, but I call you */

	if (SeprogressPos(1.0f, "finish..."))
	{
		Mad_free();
		MEM_Free(p_usReorder);
		return FALSE;
	}


	pst_NewObject = (GEO_tdst_Object*)pst_12Object->st_Id.i->pfnp_CreateFromMad(p_MadObject);

	/* Swap ptr for avoid loosing used ptr in editors */
	Swp = *pst_12Object ;
	*pst_12Object = *pst_NewObject;
	*pst_NewObject = Swp ;
	GRO_Keeper = pst_12Object -> st_Id;
	pst_12Object->st_Id = pst_NewObject->st_Id;
	pst_NewObject -> st_Id = GRO_Keeper;
	pst_12Object -> p_SKN_Objectponderation = pst_NewObject->p_SKN_Objectponderation;

	/* Avoid destruction of ponderations */
	pst_NewObject -> p_SKN_Objectponderation = NULL;
	pst_NewObject -> st_Id.i->pfn_Destroy(pst_NewObject );

	/* Add MRM information in object */
	pst_12Object -> p_MRM_ObjectAdditionalInfo = (GEO_tdst_MRM_Object *) MEM_p_Alloc(sizeof(GEO_tdst_MRM_Object));
	pst_12Object -> p_MRM_ObjectAdditionalInfo -> RealNumberOfPoints  = pst_12Object -> l_NbPoints;
	pst_12Object -> p_MRM_ObjectAdditionalInfo -> MinimumNumberOfPoints= 8;
	pst_12Object -> p_MRM_ObjectAdditionalInfo -> df_Errors = df_Errors;

	pst_12Object -> p_MRM_ObjectAdditionalInfo -> Absorbers			= (short *) MEM_p_Alloc((LONG)2 * pst_12Object ->p_MRM_ObjectAdditionalInfo->RealNumberOfPoints );
	{
		ULONG Counter;
		Counter = pst_12Object -> p_MRM_ObjectAdditionalInfo -> RealNumberOfPoints ;
		while (Counter--)
			pst_12Object -> p_MRM_ObjectAdditionalInfo -> Absorbers[Counter] = (unsigned short)p_MadObject->Absorbers[Counter];//*/
	}
	pst_12Object -> p_MRM_ObjectAdditionalInfo ->p_us_ReorderBuffer	= (short*)p_usReorder;


	/* Reorder skin Ponderations */
	if (GEO_SKN_IsSkinned(pst_12Object))
	{
		ULONG Counter1,Counter2;
		GEO_tdst_ExpandedVertexPonderation *p_SrcPdrt;
		p_SrcPdrt = (GEO_tdst_ExpandedVertexPonderation *) MEM_p_Alloc(sizeof (GEO_tdst_ExpandedVertexPonderation) * pst_12Object->l_NbPoints);
		GEO_SKN_Expand(pst_12Object);
		Counter1 = pst_12Object->p_SKN_Objectponderation->NumberPdrtLists;
		while (Counter1--)
		{
			L_memcpy(p_SrcPdrt, pst_12Object->p_SKN_Objectponderation->pp_PdrtLst[Counter1]->p_PdrtVrc_E, sizeof (GEO_tdst_ExpandedVertexPonderation) * pst_12Object->l_NbPoints);
			Counter2 = pst_12Object->l_NbPoints;
			while (Counter2--)
			{
				pst_12Object->p_SKN_Objectponderation->pp_PdrtLst[Counter1]->p_PdrtVrc_E[Counter2] = p_SrcPdrt[p_usReorder[Counter2]];
			}
		}
		GEO_SKN_Compress(pst_12Object);
		MEM_Free(p_SrcPdrt);
	}//*/

	/* Add MRM information in elements */
    pst_Element = pst_12Object->dst_Element;
    pst_LastElement = pst_Element + pst_12Object->l_NbElements;
	pst_MadElement = p_MadObject-> Elements;
    while(pst_Element < pst_LastElement)
    {
		pst_Element -> p_MrmElementAdditionalInfo = (GEO_tdst_MRM_Element *) MEM_p_Alloc(sizeof(GEO_tdst_MRM_Element));
		pst_Element -> p_MrmElementAdditionalInfo -> ul_RealNumberOfTriangle = pst_Element -> l_NbTriangles;
		pst_Element -> p_MrmElementAdditionalInfo -> ul_One_UV_Per_Point_Per_Element_Base = (*pst_MadElement)->OneUVPerVertexPerElementBase;
		pst_Element -> p_MrmElementAdditionalInfo -> pul_Number_Of_Triangle_vs_Point_Equivalence = NULL;
		
        pst_Triangle = pst_Element->dst_Triangle;
        pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;
        while(pst_Triangle < pst_LastTriangle)
        {
			pst_Triangle -> auw_UV[0] = pst_Triangle -> auw_Index[0];
			pst_Triangle -> auw_UV[1] = pst_Triangle -> auw_Index[1];
			pst_Triangle -> auw_UV[2] = pst_Triangle -> auw_Index[2];
            pst_Triangle++;
        }
        pst_Element++;
		pst_MadElement++;
    }

	GEO_MRM_ComputeUsedIndex(pst_12Object);

	GEO_MRM_Optimize_Reorder_Triangles(pst_12Object,SeprogressPos);

    Mad_free();
    return TRUE;
}

// Max on screen error (in fraction of on screen pixels).
float GEO_MRM_fMaxError = 0.2f;

float GEO_MRM_fComputeQualityThreshold(GEO_tdst_Object *pst_Object,int _lLevel)
{
	int i;
	float fError = 0;
	
	for(i=pst_Object->p_MRM_Levels->dl_PointNb[_lLevel]; i<pst_Object->p_MRM_Levels->dl_PointNb[0]; i++)
	{
		fError += pst_Object->p_MRM_ObjectAdditionalInfo->df_Errors[i];
	}
	
	if (fError < GEO_MRM_fMaxError)
		return 0.999999f;
	else
		return GEO_MRM_fMaxError/fError;
}

BOOL GEO_MRM_bComputeLevelSettings(GEO_tdst_Object *pst_Object,int _lLevel,ULONG _lPrevPointNb,float _fPrevQuality)
{
    int i;
    int lMaxPointNb = _lPrevPointNb>>1;
    float fError = 0;
    float fQuality = 1.f;

    // Compute surface
    float fSurface;
    float fXmin = FLT_MAX;
    float fXmax = -FLT_MAX;
    float fYmin = FLT_MAX;
    float fYmax = -FLT_MAX;
    float fZmin = FLT_MAX;
    float fZmax = -FLT_MAX;
    float fDeltaX,fDeltaY,fDeltaZ;
    for (i=0; i<pst_Object->p_MRM_Levels->dl_PointNb[0]; i++)
    {
        if (pst_Object->dst_Point[i].x > fXmax) fXmax = pst_Object->dst_Point[i].x;
        if (pst_Object->dst_Point[i].x < fXmin) fXmin = pst_Object->dst_Point[i].x;
        if (pst_Object->dst_Point[i].y > fYmax) fYmax = pst_Object->dst_Point[i].y;
        if (pst_Object->dst_Point[i].y < fYmin) fYmin = pst_Object->dst_Point[i].y;
        if (pst_Object->dst_Point[i].z > fZmax) fZmax = pst_Object->dst_Point[i].z;
        if (pst_Object->dst_Point[i].z < fZmin) fZmin = pst_Object->dst_Point[i].z;
    }
    
    fDeltaX = fXmax - fXmin;
    fDeltaY = fYmax - fYmin;
    fDeltaZ = fZmax - fZmin;
    if (fDeltaX < fDeltaY && fDeltaX < fDeltaZ) fSurface = fDeltaY * fDeltaZ;
    else if (fDeltaY < fDeltaX && fDeltaY < fDeltaZ) fSurface = fDeltaX * fDeltaZ;
    else fSurface = fDeltaX * fDeltaY;

    // The new level must have at least twice less points and a lower quality than previous level.
    i = pst_Object->p_MRM_Levels->dl_PointNb[0]-1;
    while (i>=0 && (fQuality >= _fPrevQuality || i > lMaxPointNb ))
    {
		fError += pst_Object->p_MRM_ObjectAdditionalInfo->df_Errors[i];
        fQuality = (fError < fSurface * GEO_MRM_fMaxError ? 1.f : fSurface * GEO_MRM_fMaxError/fError);
        i--;
    }

    // Not enough points for this level : don't create it.
    if (i < GEO_MRM_MinPointNb)
        return FALSE;

    // Set number of points for this level.
    GEO_MRM_SetNumberOfPoints_Number(pst_Object,i);

    // Set quality threshold
    pst_Object->p_MRM_Levels->f_Thresholds[_lLevel-1] = fQuality;

    return TRUE;
}



// Compute discrete LOD levels based on the MRM data.
void GEO_MRM_ComputeLevels(OBJ_tdst_GameObject *pst_GAO,GEO_tdst_Object *pst_Object, BOOL _bInteractive,ULONG _ulLevelNb, float *_dfLevels, float *_dfThresholds , ULONG (*SeprogressPos) (float F01, char *))
{
    int i,j,k;
    TAB_tdst_PFelem     *pst_Elem, *pst_LastElem;
    WOR_tdst_World		*pst_World;
	unsigned short						*p_Redir,*p_FrstElm,*p_LstElm;
    LONG                                Counter;
    float dfLevels[GEO_MRM_MAX_LEVEL_NB-1];
    float dfThresholds[GEO_MRM_MAX_LEVEL_NB-1];

    if (!pst_Object)
        return;

    // If recompute levels, copy parameters first.
    if (_dfLevels == NULL || _dfThresholds == NULL)
    {
        if (_ulLevelNb == 0)
            _ulLevelNb = pst_Object->p_MRM_Levels->ul_LevelNb;

        _dfLevels = dfLevels;
        _dfThresholds = dfThresholds;
        for (i=0; i<GEO_MRM_MAX_LEVEL_NB-1; i++)
        {
            dfLevels[i] = pst_Object->p_MRM_Levels->f_MRMLevels[i];
            dfThresholds[i] = pst_Object->p_MRM_Levels->f_Thresholds[i];
        }
    }

    // Check parameters.
    if (_bInteractive)
    {
        // Check editors value, and exit computation if they are not correct.
        if ((_ulLevelNb< 2) || (_ulLevelNb> 6))
        {
            ERR_X_Warning(0,"Number of levels must be between 2 and 6, cannot compute", pst_GAO ? pst_GAO->sz_Name : "Unknown GAO");
            return;
        }

        for (i=0; i<(int)_ulLevelNb-1; i++)
        {
            if ((_dfLevels[i]<0.f) || (_dfLevels[i]>1.f))
            {
                ERR_X_Warning(0,"Levels must be between 0.0 and 1.0, cannot compute", pst_GAO ? pst_GAO->sz_Name : "Unknown GAO");
                return;
            }
            if ((_dfThresholds[i]<0.f) || (_dfThresholds[i]>1.f))
            {
                ERR_X_Warning(0,"Thresholds must be between 0.0 and 1.0, cannot compute", pst_GAO ? pst_GAO->sz_Name : "Unknown GAO");
                return;
            }
        }
    }

    if (pst_Object->l_NbElements <= 0)
    {
        ERR_X_Warning(0,"Must have at least one element", pst_GAO ? pst_GAO->sz_Name : "Unknown GAO");
        return;
    }
    
    // No MRM levels for objects with morphing
	if ((pst_GAO) && (pst_GAO->pst_Extended))
	{
		MDF_tdst_Modifier	*pst_Modifier;
		pst_Modifier = pst_GAO->pst_Extended->pst_Modifiers;
		while (pst_Modifier)
		{
			if (pst_Modifier->i->ul_Type == MDF_C_Modifier_Morphing) 
				return;
			pst_Modifier = pst_Modifier->pst_Next;
		}
	}    

    // If levels have been computed, delete them first (and restore MRM).
    GEO_MRM_DeleteLevels(pst_Object);

    // We need the geometry to be in level 0.
    GEO_MRM_RESET(pst_Object);

    // We need the MRM data in order to compute the MRM levels. 
    if (!GEO_MRM_ul_IsMrmObject(pst_Object))
    {
        // Set computing curve

        if (!GEO_MRM_Compute(pst_GAO,pst_Object,_bInteractive,SeprogressPos))
            return;
    }

    // We need at least 32 points (arbitrary)
    if (!_bInteractive && pst_Object->l_NbPoints <= GEO_MRM_MinTotalPointNb)
        return;

    // How many levels should be computed ?
   /* if (!_bInteractive)
    {
        double fLevelNb = log((double)pst_Object->l_NbPoints / (double)GEO_MRM_MinPointNb) / log(2);
        int iLevelNb= (int)fLevelNb;
        if (iLevelNb < 2)
            return;
        if (iLevelNb > 6)
            _ulLevelNb = 6;
        else 
            _ulLevelNb = iLevelNb;
    }*/

    SeprogressPos(0.0f, "Computing levels ...");

    pst_Object->p_MRM_Levels = GEO_MRM_CreateLevels();

    pst_Object->p_MRM_Levels->ul_LevelNb = _ulLevelNb;

    // Copy reorder vector from MRM data to MRM levels data.
    pst_Object->p_MRM_Levels->dus_ReorderBuffer = (unsigned short *) MEM_p_Alloc(sizeof(unsigned short) * pst_Object->l_NbPoints);
    for (i=0; i<pst_Object->l_NbPoints; i++)
        pst_Object->p_MRM_Levels->dus_ReorderBuffer[i] = pst_Object ->p_MRM_ObjectAdditionalInfo->p_us_ReorderBuffer[i];

    // Apply reorder to all RLI in world that use this geometry.
    pst_World = WOR_gpst_CurrentWorld;
    pst_Elem = pst_World->st_AllWorldObjects.p_Table;
    pst_LastElem = pst_World->st_AllWorldObjects.p_NextElem;
    for(; pst_Elem < pst_LastElem; pst_Elem++)
    {
        OBJ_tdst_GameObject *GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
        if (TAB_b_IsAHole(GO)) continue;

        if (GO && GO->pst_Base && GO->pst_Base->pst_Visu && 
            ((GEO_tdst_Object *)GO->pst_Base->pst_Visu->pst_Object == pst_Object) &&
            GO->pst_Base->pst_Visu->dul_VertexColors)
        {
            LONG uArraySize = GO->pst_Base->pst_Visu->dul_VertexColors[0];
            if (uArraySize > pst_Object->l_NbPoints)
            {
                ERR_X_Warning(0,"Instance vertex color buffer size is bigger than vertex buffer size, reduce instance vertex color buffer", GO->sz_Name);
                GO->pst_Base->pst_Visu->dul_VertexColors[0] = pst_Object->l_NbPoints;
            }
            else if (uArraySize < pst_Object->l_NbPoints)
            {
                ERR_X_Warning(0,"Instance vertex color buffer size is smaller than vertex buffer size, delete instance vertex color buffer ", GO->sz_Name);
                MEM_Free(GO->pst_Base->pst_Visu->dul_VertexColors);
                GO->pst_Base->pst_Visu->dul_VertexColors = NULL;
                continue;
            }

            GEO_MRM_v_ApplyReorder((LONG* )(GO->pst_Base->pst_Visu->dul_VertexColors+1),pst_Object->p_MRM_Levels->dus_ReorderBuffer,pst_Object->l_NbPoints);
        }
    }

    // Compute levels.
    pst_Object->p_MRM_Levels->a_ElementBase = (GEO_tdst_ElementIndexedTriangles *)MEM_p_Alloc(sizeof(GEO_tdst_ElementIndexedTriangles) * pst_Object->l_NbElements * _ulLevelNb);
    L_memset(pst_Object->p_MRM_Levels->a_ElementBase,0,sizeof(GEO_tdst_ElementIndexedTriangles) * pst_Object->l_NbElements * _ulLevelNb);
    pst_Object->p_MRM_Levels->l_TotalElementNb = 0;

	p_Redir = (unsigned short *) MEM_p_Alloc( (LONG)2 * pst_Object -> l_NbPoints );
	
    pst_Object->p_MRM_Levels->l_TotalPointNb = pst_Object->l_NbPoints;
    pst_Object->p_MRM_Levels->l_TotalUVNb = pst_Object->l_NbUVs;
        
    for (i=0; i<(int)_ulLevelNb; i++)
    {
        float fQuality;

        // Set threshold
        if (i>0)
        {
            pst_Object->p_MRM_Levels->f_MRMLevels[i-1] = _dfLevels[i-1];
            pst_Object->p_MRM_Levels->f_Thresholds[i-1] = _dfThresholds[i-1];
            fQuality = MATH_f_FloatLimit(_dfLevels[i-1],0.f,1.f);
        }
        else
            fQuality = 1.0f;

        // Use the MRM information to compute the various levels.
        GEO_MRM_SetNumberOfPoints_Number(pst_Object,GEO_MRM_GetNumberOfPointsFromQuality(pst_Object,fQuality));

        pst_Object->p_MRM_Levels->aa_Element[i] = pst_Object->p_MRM_Levels->a_ElementBase + pst_Object->p_MRM_Levels->l_TotalElementNb;
        pst_Object->p_MRM_Levels->dl_ElementNb[i] = pst_Object->l_NbElements;
        pst_Object->p_MRM_Levels->l_TotalElementNb += pst_Object->p_MRM_Levels->dl_ElementNb[i];
        pst_Object->p_MRM_Levels->dl_PointNb[i] = pst_Object->l_NbPoints;
        pst_Object->p_MRM_Levels->dl_UVNb[i] = pst_Object->l_NbUVs;
        
    	pst_Object->p_MRM_Levels->l_TotalPointNb = lMax(pst_Object->l_NbPoints,pst_Object->p_MRM_Levels->l_TotalPointNb);
	    pst_Object->p_MRM_Levels->l_TotalUVNb = lMax(pst_Object->l_NbUVs,pst_Object->p_MRM_Levels->l_TotalUVNb);
                
        // Copy current triangle data to level i (level 0 is full quality)
        for (j=0; j < pst_Object->l_NbElements; j++)
        {
            GEO_tdst_ElementIndexedTriangles *pSrcElement = pst_Object->dst_Element+j;
            GEO_tdst_ElementIndexedTriangles *pDstElement = (pst_Object->p_MRM_Levels->aa_Element[i])+j;

            // Copy element data
            // We put the level index in the high range of the material index 
            // so that we don't have the same materials for 2 different levels.
            pDstElement->l_MaterialId = pSrcElement->l_MaterialId | (i<<8);
            pDstElement->l_NbTriangles = pSrcElement->l_NbTriangles;

            // Copy triangle data.
            // The last triangle is a fake.
            pDstElement->dst_Triangle = (GEO_tdst_IndexedTriangle*)MEM_p_AllocAlign((pSrcElement->l_NbTriangles + 1 )* sizeof(GEO_tdst_IndexedTriangle),16);
     		pDstElement->dst_Triangle[pSrcElement->l_NbTriangles].ul_SmoothingGroup = 0xffffffff;

            // p_Redir transforms index to UV.
    		L_memset(p_Redir , 0 , pst_Object -> l_NbPoints * (LONG)2);
	    	p_FrstElm = pSrcElement -> pus_ListOfUsedIndex ;
		    p_LstElm  = p_FrstElm + pSrcElement -> ul_NumberOfUsedIndex;
		    Counter = 0;
		    while (p_FrstElm < p_LstElm ) p_Redir[*(p_FrstElm++)] = (unsigned short)(Counter++ + pSrcElement ->p_MrmElementAdditionalInfo->ul_One_UV_Per_Point_Per_Element_Base);

            for (k = 0; k<pSrcElement->l_NbTriangles; k++)
            {
                GEO_tdst_IndexedTriangle *pSrcTriangle = pSrcElement->dst_Triangle + k;
                GEO_tdst_IndexedTriangle *pDstTriangle = pDstElement->dst_Triangle + k;

                pDstTriangle->auw_Index[0] = pSrcTriangle->auw_Index[0];
                pDstTriangle->auw_Index[1] = pSrcTriangle->auw_Index[1];
                pDstTriangle->auw_Index[2] = pSrcTriangle->auw_Index[2];
                // Remap UVs.
                pDstTriangle->auw_UV[0] = p_Redir[pSrcTriangle->auw_Index[0]];
                pDstTriangle->auw_UV[1] = p_Redir[pSrcTriangle->auw_Index[1]];
                pDstTriangle->auw_UV[2] = p_Redir[pSrcTriangle->auw_Index[2]];
                pDstTriangle->ul_SmoothingGroup = pSrcTriangle->ul_SmoothingGroup;
#ifndef PSX2_TARGET
#ifndef _GAMECUBE
                pDstTriangle->ul_MaxFlags = pSrcTriangle->ul_MaxFlags;
#endif	
#endif
            }

            pDstElement->ul_NumberOfUsedIndex = 0;
            pDstElement->pus_ListOfUsedIndex = NULL;
            pDstElement->p_MrmElementAdditionalInfo = NULL;
            pDstElement->pst_StripData = NULL; // Will be computed later
	        pDstElement->pst_Gx8Add = NULL;
        }
    }

    // We need the geometry to be in level 0.
    GEO_MRM_RESET(pst_Object);

    pst_Object->p_MRM_Levels->ul_CurrentLevel = 0xFFFFFFFF;

    // Free MRM data
    GEO_MRM_Free(pst_Object);

    // Free previous elements 
    for (i=0; i<pst_Object->l_NbElements; i++)
    {
        GEO_FreeElementContent(pst_Object->dst_Element+i);
    }
    MEM_Free(pst_Object->dst_Element);

    // Put all the levels in geometry.
    pst_Object->dst_Element = pst_Object->p_MRM_Levels->a_ElementBase;
    pst_Object->l_NbElements = pst_Object->p_MRM_Levels->l_TotalElementNb;

    SeprogressPos(1.0f, "Finish ...");
}

#endif /* ACTIVE_EDITOR */
#define GEO_MRM_b_GetFlag(a)  (flags[(a) >> (LONG)5] & ((LONG)1 << ((a) & (LONG)31)))
#define GEO_MRM_SetFlag(a)    if(!GEO_MRM_b_GetFlag(a)) {FlagSetNum++; flags[(a)>>(LONG)5] |= (LONG)1<<((a)&(LONG)31); }
/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern void MEM_GEO_v_FreeAlign(void *ptr );
void GEO_MRM_ComputeUsedIndex(GEO_tdst_Object *pst_Object)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
    ULONG                               *flags, FlagSetNum;
    LONG                                Counter;
    unsigned short                      *pus_List;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GEO_MRM_RESET(pst_Object);

    pst_Element = pst_Object->dst_Element;
    pst_LastElement = pst_Element + pst_Object->l_NbElements;

    flags = (ULONG *) MEM_p_Alloc((pst_Object->l_NbPoints >> (LONG)3) + (LONG)8);
    while(pst_Element < pst_LastElement)
    {
		if (pst_Element->ul_NumberOfUsedIndex && pst_Element->pus_ListOfUsedIndex)
			MEM_GEO_v_FreeAlign(pst_Element->pus_ListOfUsedIndex);
        L_memset(flags, 0, (pst_Object->l_NbPoints >> (LONG)3) + (LONG)8);
        FlagSetNum = 0;
        pst_Triangle = pst_Element->dst_Triangle;
        pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;
        while(pst_Triangle < pst_LastTriangle)
        {
            GEO_MRM_SetFlag(pst_Triangle->auw_Index[0]);
            GEO_MRM_SetFlag(pst_Triangle->auw_Index[1]);
            GEO_MRM_SetFlag(pst_Triangle->auw_Index[2]);
            pst_Triangle++;
        }
        pst_Element->ul_NumberOfUsedIndex = FlagSetNum;
        if(FlagSetNum)
        {
#ifdef PSX2_TARGET
			FlagSetNum += 4;
#endif        
            pst_Element->pus_ListOfUsedIndex = (unsigned short *) MEM_GEO_p_AllocAlign(FlagSetNum * (LONG)2/* Ushort */ + (LONG)2,8);
            pus_List = pst_Element->pus_ListOfUsedIndex;
            for(Counter = 0; Counter < pst_Object->l_NbPoints; Counter++)
            {
                if(flags[Counter >> (LONG)5] == 0) continue;
                if(GEO_MRM_b_GetFlag(Counter))
                    *(pus_List++) = (unsigned short) Counter;
            }
#ifdef PSX2_TARGET
			{
				unsigned short usLastValue;
				usLastValue = *(pus_List-1);
				FlagSetNum = 4;
				while (FlagSetNum--)
					*(pus_List++) = usLastValue; // Align it
			}
#endif        
			*(pus_List) = 0xffff;
			
        }
        else
            pst_Element->pus_ListOfUsedIndex = NULL;
        pst_Element++;
    }

    MEM_Free(flags);
}

