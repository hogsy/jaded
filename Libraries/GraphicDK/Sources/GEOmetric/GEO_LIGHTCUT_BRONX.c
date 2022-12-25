/* GEO_LIGHTCUT_BRONX.c */

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/

#include "Precomp.h"
#ifdef ACTIVE_EDITORS

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
#include "GEOmetric/GEO_LIGHTCUT.h"

#include <math.h>
#include <STDLIB.H>
#include "../dlls/MAD_loadsave/Sources/MAD_STRUCT_V0.h"
#include "../dlls/MAD_mem/Sources/MAD_mem.h"
#include "GEOmetric/GEO_LODCmpt.h"

ULONG ulPolylineCounter = 0;
#ifdef GLV_DEBUG
extern tdst_GLV *BugReport;
#endif
/*
MATHD_SubVector(&stX , &p_stGLVToClip->p_stPoints[p_stGLVToClip->p_stFaces[0].Index[1]].P3D , &p_stGLVToClip->p_stPoints[p_stGLVToClip ->p_stFaces[0].Index[0]].P3D);
MATHD_NormalizeVector(&stX , &stX );
MATHD_CrossProduct(&stY , &stX , &p_stGLVToClip ->p_stFaces[0].Plane.Normale);
MATHD_NormalizeVector(&stY , &stY );
*/
void GLV_ulColor2Vector(ULONG ulColor , MATH_tdst_Vector *pCol3d)
{
	pCol3d->x = (float)((ulColor>>16) & 0xff);
	pCol3d->y = (float)((ulColor>> 8) & 0xff);
	pCol3d->z = (float)((ulColor>> 0) & 0xff);
}
void GLV_Vector2ulColor(MATH_tdst_Vector *pCol3d , ULONG *pulColor)
{
	*pulColor  = ((ULONG)pCol3d->x) << 16;
	*pulColor |= ((ULONG)pCol3d->y) <<  8;
	*pulColor |= ((ULONG)pCol3d->z) <<  0;

}

ULONG GLV_Share2Indexes(tdst_GLV *p_stGLV , ULONG F1 , ULONG F2)
{
	ULONG REs;
	REs  = 0;
	if (p_stGLV->p_stFaces[F1].Index[0] == p_stGLV->p_stFaces[F2].Index[0]) REs ++;
	if (p_stGLV->p_stFaces[F1].Index[0] == p_stGLV->p_stFaces[F2].Index[1]) REs ++;
	if (p_stGLV->p_stFaces[F1].Index[0] == p_stGLV->p_stFaces[F2].Index[2]) REs ++;
	if (p_stGLV->p_stFaces[F1].Index[1] == p_stGLV->p_stFaces[F2].Index[0]) REs ++;
	if (p_stGLV->p_stFaces[F1].Index[1] == p_stGLV->p_stFaces[F2].Index[1]) REs ++;
	if (REs >= 2) return 1;

	if (p_stGLV->p_stFaces[F1].Index[1] == p_stGLV->p_stFaces[F2].Index[2]) REs ++;
	if (p_stGLV->p_stFaces[F1].Index[2] == p_stGLV->p_stFaces[F2].Index[0]) REs ++;
	if (p_stGLV->p_stFaces[F1].Index[2] == p_stGLV->p_stFaces[F2].Index[1]) REs ++;
	if (p_stGLV->p_stFaces[F1].Index[2] == p_stGLV->p_stFaces[F2].Index[2]) REs ++;
	if (REs >= 2) return 1;
	return 0;
}
void GLV_InterpolOriginalsScalars(tdst_GLV *p_stGLV)
{
	ULONG Counter , Counter2;
	MATH_tdst_Vector Col3d;
	MATH_tdst_Vector OrigCol3d[3];
	GLV_Scalar Weith[3];
	MATHD_tdst_Vector	V1[3];
	GLV_Scalar			Distance[3];
	/* Compute V1 & distance to 0 & originals colors */
	for (Counter2 = 0 ; Counter2 < 3 ; Counter2 ++)
	{
		MATHD_tdst_Vector	stX  ;
		GLV_Scalar D;
		GLV_ulColor2Vector(p_stGLV->p_stFaces[0].Colours[Counter2] , &OrigCol3d[Counter2]);
		MATHD_SubVector(&stX , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[0].Index[(Counter2 + 1)  % 3]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[0].Index[(Counter2 + 2)  % 3]].P3D);
		MATHD_NormalizeVector(&stX , &stX );
		MATHD_CrossProduct(&V1[Counter2] , &stX , &p_stGLV ->p_stFaces[0].Plane.Normale);
		MATHD_NormalizeVector(&V1[Counter2] , &V1[Counter2] );
		Distance[Counter2] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[0].Index[(Counter2 + 1)  % 3]].P3D , &V1[Counter2]);
		D = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[0].Index[Counter2]].P3D , &V1[Counter2]) - Distance[Counter2];
		D = 1.0f / D;
		Distance[Counter2] *= D;
		MATHD_ScaleEqualVector(&V1[Counter2] , D);
	}
	/* interpole originals scalars */
	for (Counter = 1 ; Counter < p_stGLV -> ulNumberOfFaces ; Counter ++)
	{
		for (Counter2 = 0 ; Counter2 < 3 ; Counter2 ++ )
		{
			/* Compute Weights */
			Weith[0] = MATHD_f_DotProduct(&V1[0] , &p_stGLV->p_stPoints [p_stGLV->p_stFaces [Counter] . Index [Counter2] ]. P3D) - Distance[0];
			Weith[1] = MATHD_f_DotProduct(&V1[1] , &p_stGLV->p_stPoints [p_stGLV->p_stFaces [Counter] . Index [Counter2] ]. P3D) - Distance[1];
			Weith[2] = MATHD_f_DotProduct(&V1[2] , &p_stGLV->p_stPoints [p_stGLV->p_stFaces [Counter] . Index [Counter2] ]. P3D) - Distance[2];
			MATHD_AbsoluteVector((MATHD_tdst_Vector *)Weith);
			/* Interpol */
			/* Normal */
			MATH_InitVectorToZero(&p_stGLV ->p_stFaces[Counter].OriginalPointsNormales[Counter2]);
			MATH_AddScaleVector(&p_stGLV -> p_stFaces[Counter].OriginalPointsNormales[Counter2] , &p_stGLV -> p_stFaces[Counter].OriginalPointsNormales[Counter2] , &p_stGLV ->p_stFaces[0].OriginalPointsNormales[0] , (float)Weith[0] );
			MATH_AddScaleVector(&p_stGLV -> p_stFaces[Counter].OriginalPointsNormales[Counter2] , &p_stGLV -> p_stFaces[Counter].OriginalPointsNormales[Counter2] , &p_stGLV ->p_stFaces[0].OriginalPointsNormales[1] , (float)Weith[1] );
			MATH_AddScaleVector(&p_stGLV -> p_stFaces[Counter].OriginalPointsNormales[Counter2] , &p_stGLV -> p_stFaces[Counter].OriginalPointsNormales[Counter2] , &p_stGLV ->p_stFaces[0].OriginalPointsNormales[2] , (float)Weith[2] );
			MATH_NormalizeEqualVector(&p_stGLV -> p_stFaces[Counter].OriginalPointsNormales[Counter2]);
			/* UV */
			/* U : */
			p_stGLV -> p_stFaces[Counter].UV[(Counter2 << 1L) + 0]  = p_stGLV -> p_stFaces[0].UV[0] * Weith[0];
			p_stGLV -> p_stFaces[Counter].UV[(Counter2 << 1L) + 0] += p_stGLV -> p_stFaces[0].UV[2] * Weith[1];
			p_stGLV -> p_stFaces[Counter].UV[(Counter2 << 1L) + 0] += p_stGLV -> p_stFaces[0].UV[4] * Weith[2];
			/* V : */
			p_stGLV -> p_stFaces[Counter].UV[(Counter2 << 1L) + 1]  = p_stGLV -> p_stFaces[0].UV[1] * Weith[0];
			p_stGLV -> p_stFaces[Counter].UV[(Counter2 << 1L) + 1] += p_stGLV -> p_stFaces[0].UV[3] * Weith[1];
			p_stGLV -> p_stFaces[Counter].UV[(Counter2 << 1L) + 1] += p_stGLV -> p_stFaces[0].UV[5] * Weith[2];
			/* Alphas */
			p_stGLV -> p_stFaces[Counter].falpha[Counter2]  = p_stGLV -> p_stFaces[0].falpha[0] * (float)Weith[0];
			p_stGLV -> p_stFaces[Counter].falpha[Counter2] += p_stGLV -> p_stFaces[0].falpha[1] * (float)Weith[1];
			p_stGLV -> p_stFaces[Counter].falpha[Counter2] += p_stGLV -> p_stFaces[0].falpha[2] * (float)Weith[2];
			/* Color */
			MATH_InitVectorToZero(&Col3d);
			MATH_AddScaleVector(&Col3d , &Col3d , &OrigCol3d[0] , (float)Weith[0] );
			MATH_AddScaleVector(&Col3d , &Col3d , &OrigCol3d[1] , (float)Weith[1] );
			MATH_AddScaleVector(&Col3d , &Col3d , &OrigCol3d[2] , (float)Weith[2] );
			GLV_Vector2ulColor(&Col3d , &p_stGLV -> p_stFaces[Counter].Colours[Counter2]);
		}
	}
}
/* This function will convert non-convex polyline in triangles */
/* Triangle 0 is the original */
/* Polyline isn't cut herself */
/* !!!! modify p_Indexes */
GLV_Scalar GetPolySurf(tdst_GLV *p_stGLV , ULONG *p_Indexes , ULONG ulNum , GLV_Scalar *p2DCoords)
{
	ULONG C1 , C1P1;
	GLV_Scalar Sum;
	Sum = 0;
	for (C1 = 0; C1 < ulNum ; C1++)
	{
		C1P1 = C1 + 1;
		if (C1P1 == ulNum) C1P1 = 0;
		Sum += (p2DCoords[p_Indexes[C1] << 1L] - p2DCoords[p_Indexes[C1P1] << 1L]) * 
			(p2DCoords[(p_Indexes[C1] << 1L) + 1L] + p2DCoords[(p_Indexes[C1P1] << 1L) + 1L]);
	}
	return Sum * 0.5f;
}
GLV_Scalar  GLV_2DDotProduct(ULONG I1 , ULONG I2 , ULONG IT , GLV_Scalar *p2DCoords)
{
	GLV_Scalar DX1 , DY1 , DX2 , DY2 , L;
	DX1 = p2DCoords[I2 << 1L] - p2DCoords[I1 << 1L];
	DY1 = p2DCoords[(I2 << 1L) + 1] - p2DCoords[(I1 << 1L) + 1];
	DX2 = p2DCoords[IT << 1L] - p2DCoords[I1 << 1L];
	DY2 = p2DCoords[(IT << 1L) + 1] - p2DCoords[(I1 << 1L) + 1];
	/* normalize D1 */
	L = DX1 * DX1 + DY1 * DY1 ;
	if (L)
	{
		L = 1.0f / (GLV_Scalar)sqrt(L);
		DX1 *= L;
		DY1 *= L;
	}
	return (-DY1 * DX2 + DX1 * DY2);
}
ULONG IsPolyConvex(tdst_GLV *p_stGLV , ULONG *p_Indexes , ULONG ulNum , GLV_Scalar *p2DCoords)
{
	ULONG C1 , C1P1 , C1M1 , Def;
	GLV_Scalar Sum;
	Sum = 0;
	C1M1 = ulNum - 1;
	Def = 0;
	for (C1 = 0; C1 < ulNum ; C1++)
	{
		C1P1 = C1 + 1;
		if (C1P1 == ulNum) C1P1 = 0;
		if (Def == 0)
		{
			Sum = GLV_2DDotProduct( C1M1 , C1P1 , C1 , p2DCoords);
			if (Sum >  GLF_PrecMetric) Def = 1;
			if (Sum < -GLF_PrecMetric) Def = 2;
		}
		else
		{
			Sum = GLV_2DDotProduct( C1M1 , C1P1 , C1 , p2DCoords);
			if ((Sum >  GLF_PrecMetric) && (Def == 2)) return 0;
			if ((Sum < -GLF_PrecMetric) && (Def == 1)) return 0;
		}

		C1M1 = C1;
	}
	return 1;
}

ULONG GLV_2DBetween(ULONG I1 , ULONG I2 , ULONG IT , GLV_Scalar *p2DCoords)
{
	GLV_Scalar DX1 , DY1 , DX2 , DY2 ;
	DX1 = p2DCoords[I2 << 1L] - p2DCoords[I1 << 1L];
	DY1 = p2DCoords[(I2 << 1L) + 1] - p2DCoords[(I1 << 1L) + 1];
	DX2 = p2DCoords[IT << 1L] - p2DCoords[I1 << 1L];
	DY2 = p2DCoords[(IT << 1L) + 1] - p2DCoords[(I1 << 1L) + 1];
	if ((DX1 * DX2 + DY1 * DY2) < 0) return 0;
	DX1 = p2DCoords[I1 << 1L] - p2DCoords[I2 << 1L];
	DY1 = p2DCoords[(I1 << 1L) + 1] - p2DCoords[(I2 << 1L) + 1];
	DX2 = p2DCoords[IT << 1L] - p2DCoords[I2 << 1L];
	DY2 = p2DCoords[(IT << 1L) + 1] - p2DCoords[(I2 << 1L) + 1];
	if ((DX1 * DX2 + DY1 * DY2) < 0) return 0;
	return 1;
}

ULONG GLV_IsGoodChoice(tdst_GLV *p_stGLV , ULONG *p_Indexes , ULONG ulNum , ULONG ulNum2Test , GLV_Scalar *p2DCoords)
{
	ULONG p3Index[3],Counter;
	GLV_Scalar fSurfaceRest;
	GLV_Scalar fTest;
	/* Compute triangle surface */
	if (ulNum2Test == 0)
		p3Index[0] = p_Indexes [ulNum - 1];
	else
		p3Index[0] = p_Indexes [ulNum2Test - 1];
	p3Index[1] = p_Indexes [ulNum2Test];
	if (ulNum2Test >= ulNum-1)
		p3Index[2] = p_Indexes [0];
	else
		p3Index[2] = p_Indexes [ulNum2Test + 1];
	
	fTest = GLV_2DDotProduct(p3Index[0] , p3Index[2] , p3Index[1] , p2DCoords);
#define CHOICE_PREC GLF_PrecMetric
	if (fTest <= CHOICE_PREC) 
		return 0; // Mean the triangle is flipping 
	
	/* else test if no points are in the triangle */
	for (Counter = 0 ; Counter < ulNum ; Counter ++)
	{
		if (p_Indexes[Counter] == p3Index[0]) continue;
		if (p_Indexes[Counter] == p3Index[1]) continue;
		if (p_Indexes[Counter] == p3Index[2]) continue;
		fSurfaceRest = GLV_2DDotProduct(p3Index[0] , p3Index[1] , p_Indexes[Counter] , p2DCoords);
		if (fSurfaceRest >= -CHOICE_PREC)
		{
			if (GLV_2DDotProduct(p3Index[1] , p3Index[2] , p_Indexes[Counter] , p2DCoords) >= -CHOICE_PREC)
				if (GLV_2DDotProduct(p3Index[2] , p3Index[0] , p_Indexes[Counter] , p2DCoords) >= -CHOICE_PREC)
					return 0; /* mean is inside */
		} else
			if (fSurfaceRest <= CHOICE_PREC)
			{
				if (GLV_2DDotProduct(p3Index[1] , p3Index[2] , p_Indexes[Counter] , p2DCoords) <= CHOICE_PREC)
					if (GLV_2DDotProduct(p3Index[2] , p3Index[0] , p_Indexes[Counter] , p2DCoords) <= CHOICE_PREC)
						return 0; /* mean is inside */
			} 
	}
	return 1;
}
ULONG GLV_PolyRemoveColinear(tdst_GLV *p_stGLV , ULONG *p_Indexes , ULONG ulNum , GLV_Scalar *p2DCoords)
{
	ULONG Counter , CP1 , CPM1 , DETECT;
	GLV_Scalar Test;
	//	return ulNum;
	DETECT = 1;
	ulNum --;
	while (DETECT)
	{
		DETECT = 0;
		CPM1 = ulNum - 1;
		for (Counter = 0 ; Counter < ulNum ; Counter ++)
		{
			CP1 = Counter + 1;
			if (CP1 == ulNum) CP1 = 0;
			if (GLV_2DBetween(p_Indexes[CPM1] , p_Indexes[CP1] , p_Indexes[Counter] , p2DCoords))
			{
				Test = GLV_2DDotProduct(p_Indexes[CPM1] , p_Indexes[CP1] , p_Indexes[Counter] , p2DCoords);
				if ((Test < GLF_PrecMetric) && (Test > -GLF_PrecMetric))
				{
					DETECT = 1;	
					while (Counter < ulNum)
					{
						p_Indexes[Counter] = p_Indexes[Counter + 1];
						Counter++;
					}
					ulNum --;
				}
			}
			CPM1 = Counter;
		}
	}
	return ulNum + 1;
}

void GLV_CreateTrianglesFromPolyline(tdst_GLV *p_stGLV , ULONG *p_Indexes , ULONG ulNum , GLV_Scalar *p2DCoords , ULONG PolylineNum)
{
	ULONG Counter , k /*, COnvex*/;
	ULONG DetectInfinite;
//	GLV_Scalar Sum;	
	DetectInfinite = 0;
	ulNum --;
//	COnvex = IsPolyConvex(p_stGLV , p_Indexes , ulNum , p2DCoords);
	while (ulNum > 3)
	{
		DetectInfinite ++;
		if (DetectInfinite == 2)
		{
			GLV_BUG(4, "Createtriangle from polyline bugs (infinite)...");
			return;
		}
		for (Counter = 0 ; Counter < ulNum ; Counter ++ )
		{
/*			Sum = GetPolySurf( p_stGLV , p_Indexes , ulNum , p2DCoords);
			if ((Sum < GLF_PrecMetric) && (Sum > -GLF_PrecMetric))
				ulNum = 0;
			else*/
			{
				if (/*COnvex || */GLV_IsGoodChoice(p_stGLV , p_Indexes , ulNum , Counter , p2DCoords))
				{
					DetectInfinite = 0;
					GLV_SetNumbers(p_stGLV , 0 , 0 , p_stGLV ->ulNumberOfFaces + 1 , 4);
					p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1]  = p_stGLV ->p_stFaces[0];
					p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1].ulPolylineNum = PolylineNum;

					p_stGLV->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .Index [0] = p_Indexes[Counter];
					if ((Counter + 1) == ulNum)
						p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .Index [1] = p_Indexes[0];
					else
						p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .Index [1] = p_Indexes[Counter + 1];
					
					if (Counter == 0)
						p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .Index [2] = p_Indexes[ulNum - 1];
					else
						p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .Index [2] = p_Indexes[Counter - 1];
					
					/* and Remove index from polyline */
					k = Counter;
					while (k < ulNum)
					{
						p_Indexes[k] = p_Indexes[k + 1];
						k++;
					}
					ulNum --;
				} 
			}
		}
	}
	if (ulNum == 0)
		return;
	if (ulNum != 3)
	{
		return;
	}


	GLV_SetNumbers(p_stGLV , 0 , 0 , p_stGLV ->ulNumberOfFaces + 1 , 4);
	p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] = p_stGLV ->p_stFaces[0];
	p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .ulPolylineNum = PolylineNum;
	p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .Index [0] = p_Indexes[0];
	p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .Index [1] = p_Indexes[1];
	p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .Index [2] = p_Indexes[2];
	if (GLV_FlicTst(p_stGLV , p_stGLV ->ulNumberOfFaces - 1))
	{
		p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .Index [1] = p_Indexes[2];
		p_stGLV ->p_stFaces[p_stGLV ->ulNumberOfFaces - 1] .Index [2] = p_Indexes[1];
	}
}
void GLV_RecursiveDetectIsolatedpolylines(tdst_GLV *p_stGLV , ULONG *pBase , ULONG *pNumber , ULONG *pEnum , ULONG CurTest , ULONG ulFlag)
{
	ULONG BaseCounter;
	BaseCounter = pNumber[CurTest];
	if (p_stGLV->p_stPoints[CurTest].ulFlags & ulFlag) return;
	p_stGLV->p_stPoints[CurTest].ulFlags |= ulFlag;
	while (BaseCounter--)
	{
		if (p_stGLV->p_stPoints[pEnum[pBase[CurTest] + BaseCounter]].ulFlags & ulFlag) continue;
		GLV_RecursiveDetectIsolatedpolylines( p_stGLV , pBase , pNumber , pEnum , pEnum[pBase[CurTest] + BaseCounter] , ulFlag);
	}
}
ULONG GLV_ul_GetQuarter(GLV_Scalar X, GLV_Scalar Y)	
{
	if (fabs(X) > fabs(Y)) 
	{
		if (X > 0.0f)
			return 0;
		else
			return 2;
	} else
	{
		if (Y > 0.0f)
			return 1;
		else
			return 3;
	}
}
GLV_Scalar GLV_f_GetAnAngle(GLV_Scalar X, GLV_Scalar Y)
{
	ULONG Quarter;
	Quarter = GLV_ul_GetQuarter(X, Y);
	if (Quarter & 1) /* Y */
		if (Quarter & 2) /* X */
			return ((GLV_Scalar)Quarter + 0.5f + 0.5f * X / (GLV_Scalar)fabs(Y));
		else
			return ((GLV_Scalar)Quarter + 0.5f - 0.5f * X / (GLV_Scalar)fabs(Y));
		else
			if (Quarter & 2) /* X */
				return ((GLV_Scalar)Quarter + 0.5f - 0.5f * Y / (GLV_Scalar)fabs(X));
			else
				return ((GLV_Scalar)Quarter + 0.5f + 0.5f * Y / (GLV_Scalar)fabs(X));
}

ULONG GLV_ul_EdgeCutEdge2D(GLV_Scalar *p2DCoords , ULONG E1I1 , ULONG E1I2 , ULONG E2I1 , ULONG E2I2 )
{
	GLV_Scalar tst1,tst2;
	if (E1I1 == E2I1) return 0;
	if (E1I1 == E2I2) return 0;
	if (E1I2 == E2I1) return 0;
	if (E1I2 == E2I2) return 0;
	tst1 = GLV_2DDotProduct(E1I1 , E1I2 , E2I1 , p2DCoords);
	tst2 = GLV_2DDotProduct(E1I1 , E1I2 , E2I2 , p2DCoords);
	if ((tst1 > -GLF_PrecMetric) && (tst2 > -GLF_PrecMetric))
		return 0;
	if ((tst1 < GLF_PrecMetric) && (tst2 < GLF_PrecMetric))
		return 0;
	tst1 = GLV_2DDotProduct(E2I1 , E2I2 , E1I1 , p2DCoords);
	tst2 = GLV_2DDotProduct(E2I1 , E2I2 , E1I2 , p2DCoords);
	if ((tst1 > -GLF_PrecMetric) && (tst2 > -GLF_PrecMetric))
		return 0;
	if ((tst1 < GLF_PrecMetric) && (tst2 < GLF_PrecMetric))
		return 0;
	return 1;
}

ULONG GLV_ul_EdgeTouchEdge2D(GLV_Scalar *p2DCoords , ULONG E1I1 , ULONG E1I2 , ULONG E2I1 , ULONG E2I2 )
{
	GLV_Scalar tst1,tst2;
	if (E1I1 == E2I1) return 0;
	if (E1I1 == E2I2) return 0;
	if (E1I2 == E2I1) return 0;
	if (E1I2 == E2I2) return 0;
	tst1 = GLV_2DDotProduct(E1I1 , E1I2 , E2I1 , p2DCoords);
	tst2 = GLV_2DDotProduct(E1I1 , E1I2 , E2I2 , p2DCoords);
	if ((tst1 > GLF_PrecMetric) && (tst2 > GLF_PrecMetric))
		return 0;
	if ((tst1 < -GLF_PrecMetric) && (tst2 < -GLF_PrecMetric))
		return 0;
	tst1 = GLV_2DDotProduct(E2I1 , E2I2 , E1I1 , p2DCoords);
	tst2 = GLV_2DDotProduct(E2I1 , E2I2 , E1I2 , p2DCoords);
	if ((tst1 > GLF_PrecMetric) && (tst2 > GLF_PrecMetric))
		return 0;
	if ((tst1 < -GLF_PrecMetric) && (tst2 < -GLF_PrecMetric))
		return 0;
	return 1;
}

/* This will link an isolated polyline to the rest of the grid */
/* This will create an edge, and detect if it cut the others edges */
ULONG GLV_LinkRV2( tdst_GLV *p_stGLV , GLV_Scalar *p2DCoords)
{
	ULONG C1,C2,C3,Valid;
	/* Square computation ! */
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfPoints ; C1 ++)
	{
		if ((p_stGLV ->p_stPoints[C1].ulFlags & (GLV_FLGS_RSV1 | GLV_FLGS_RSV2)) == GLV_FLGS_RSV1)
		{
			for (C2 = 0 ; C2 < p_stGLV->ulNumberOfPoints ; C2 ++)
			{
				if ((p_stGLV ->p_stPoints[C2].ulFlags & (GLV_FLGS_RSV1 | GLV_FLGS_RSV2)) == (GLV_FLGS_RSV1 | GLV_FLGS_RSV2))
				{
					Valid = 0;
					/* Verify C1 - C2 Is vadid */
					for (C3 = 0 ; C3 < p_stGLV -> ulNumberOfEdges ; C3 ++)
					{
						if (GLV_ul_EdgeTouchEdge2D(p2DCoords , C1 , C2 , p_stGLV ->p_stEdges[C3].Index[0] , p_stGLV ->p_stEdges[C3].Index[1] ))
						{
							/* invalidate if cut at leas an edge */
							Valid ++;
						}
					}
					if (!Valid)
					{
						/* Create the new Edge C1 - C2 */
						GLV_SetNumbers(p_stGLV , 0 , p_stGLV->ulNumberOfEdges + 1 , 0 , 2);
						p_stGLV ->p_stEdges [p_stGLV->ulNumberOfEdges - 1 ] .Index [0] = C1;
						p_stGLV ->p_stEdges [p_stGLV->ulNumberOfEdges - 1 ] .Index [1] = C2;
						p_stGLV ->p_stEdges [p_stGLV->ulNumberOfEdges - 1 ] .FromFaces[0] = 0xffffffff;
						p_stGLV ->p_stEdges [p_stGLV->ulNumberOfEdges - 1 ] .FromFaces[1] = 0xffffffff;
						/* And return */
						return 1;
					}
				}
			}
		}
	}
	GLV_BUG(4, "Couldn't link isolated polyline!!!");
	return 0;
}

void GLV_ResolvePolyline(tdst_GLV *p_stGLVToClip )
{
	ULONG Counter,Counter2,Counter3,Flag,ulNumberOfPoints;
	ULONG ulNumberofIsolatedPolylines, ulNumberofPolylines;
	ULONG *pCountPP,*pFPPBase;
	ULONG *pEdgesPerP , /**pDownCount,*/*pPolyline;
	GLV_Scalar *pEPPAngles;
	GLV_Scalar *p2DCoords;
	ULONG ulMaxNumOfPoyPoints;
	MATHD_tdst_Vector stX,stY;
	GLV_Scalar fSumSurf ;
#ifdef GLV_DEBUG
	GLV_Scalar fFirstSurf , fCalcSurf;
	ULONG ISADDED;
	GLV_Cluster *p_stCluster;
	/* Compute initial surface (DEBUG)*/
	ISADDED = 0;
	fFirstSurf = GLV_GetSurf(p_stGLVToClip , &p_stGLVToClip->p_stFaces[0]);
	p_stCluster = GLV_NewCluster();
#endif
	ulNumberofIsolatedPolylines = 1;
	while (ulNumberofIsolatedPolylines--)
	{
		GLD_Weld( p_stGLVToClip , GLV_WELD_FCT , 0);
		GLD_RemoveBadEdges( p_stGLVToClip );
		GLD_RemoveDoubleEdges(p_stGLVToClip);
		GLD_RemoveUnusedIndexes( p_stGLVToClip );
		pCountPP = (ULONG*)GLV_ALLOC(4L * p_stGLVToClip->ulNumberOfPoints);
		pFPPBase = (ULONG*)GLV_ALLOC(4L * p_stGLVToClip->ulNumberOfPoints);
		p2DCoords = (double*)GLV_ALLOC(sizeof(GLV_Scalar) * p_stGLVToClip->ulNumberOfPoints * 2L);
		L_memset(pCountPP , 0 , 4L * p_stGLVToClip->ulNumberOfPoints);
		/* Find Isolated polylines */
		
		/* Compute intersections */
		/* FUTUR */
		/* Count number of EDges per points*/
		Flag = 0;
		for (Counter=0;Counter<p_stGLVToClip->ulNumberOfEdges;Counter++)
		{
			pCountPP[p_stGLVToClip->p_stEdges[Counter].Index[0]] ++;
			pCountPP[p_stGLVToClip->p_stEdges[Counter].Index[1]] ++;
			Flag += 2;
		}
/*		for (Counter=0;Counter<p_stGLVToClip->ulNumberOfPoints;Counter++)
		{
			pDownCount[Counter] = pCountPP[Counter];
		}*/
		/* Kiil Aligned points */
		/* FUTUR */
		/* DEBUG */
#ifdef GLV_DEBUG
		for (Counter=0;Counter<p_stGLVToClip->ulNumberOfPoints;Counter++)
		{
			if ((p_stGLVToClip->p_stPoints[Counter].ulFlags == GLV_FLGS_Original) && (pCountPP[Counter] != 2))
				Flag = Flag ;//BUG!
			if ((p_stGLVToClip->p_stPoints[Counter].ulFlags == GLV_FLGS_FromEdge) && (pCountPP[Counter] != 3))
				Flag = Flag ;//BUG!
			if (pCountPP[Counter] == 1)
				Flag = Flag ;//BUG!
		}
#endif
		
		/* Compute 2D coordinates */ 
		MATHD_SubVector(&stX , &p_stGLVToClip->p_stPoints[p_stGLVToClip->p_stFaces[0].Index[1]].P3D , &p_stGLVToClip->p_stPoints[p_stGLVToClip ->p_stFaces[0].Index[0]].P3D);
		MATHD_NormalizeVector(&stX , &stX );
		MATHD_CrossProduct(&stY , &stX , &p_stGLVToClip ->p_stFaces[0].Plane.Normale);
		MATHD_NormalizeVector(&stY , &stY );
		for (Counter=0;Counter<p_stGLVToClip->ulNumberOfPoints;Counter++)
		{
			p2DCoords[Counter<<1L]		= MATHD_f_DotProduct(&p_stGLVToClip->p_stPoints[Counter].P3D, &stX);
			p2DCoords[(Counter<<1L) + 1]= MATHD_f_DotProduct(&p_stGLVToClip->p_stPoints[Counter].P3D, &stY);
		}
		/* Compute Edges Enum Base */ 
		Flag = 0;
		for (Counter=0;Counter<p_stGLVToClip->ulNumberOfPoints;Counter++)
		{
			pFPPBase[Counter] = Flag + 1;
			Flag += pCountPP[Counter] + 2;
			pCountPP[Counter] = 0;
		}
		ulMaxNumOfPoyPoints = Flag;
		pPolyline = (ULONG*)GLV_ALLOC(4L * ulMaxNumOfPoyPoints);
		pEdgesPerP = (ULONG*)GLV_ALLOC(4L * Flag);
		pEPPAngles = (double*)GLV_ALLOC(sizeof(GLV_Scalar) * Flag);
		/* Enum edges per points */
		for (Counter=0;Counter<p_stGLVToClip->ulNumberOfEdges;Counter++)
		{
			Counter2 =  p_stGLVToClip->p_stEdges[Counter].Index[0];
			pEdgesPerP	[pFPPBase[Counter2] + pCountPP[Counter2]++] = p_stGLVToClip->p_stEdges[Counter].Index[1];
			Counter2 =  p_stGLVToClip->p_stEdges[Counter].Index[1];
			pEdgesPerP	[pFPPBase[Counter2] + pCountPP[Counter2]++] = p_stGLVToClip->p_stEdges[Counter].Index[0];
		}
		/* Comnpute angles */
		for (Counter=0;Counter<p_stGLVToClip->ulNumberOfPoints;Counter++)
		{
			for (Counter2 = 0; Counter2 < pCountPP[Counter]; Counter2++)
			{
				pEPPAngles[pFPPBase[Counter] + Counter2] = GLV_f_GetAnAngle(p2DCoords[pEdgesPerP[pFPPBase[Counter] + Counter2]<<1L] - p2DCoords[Counter<<1L], p2DCoords[(pEdgesPerP[pFPPBase[Counter] + Counter2]<<1L) + 1] - p2DCoords[(Counter<<1L) +1]);
			}
		}
		/* Reorder points, edge & alphas */
		for (Counter=0;Counter<p_stGLVToClip->ulNumberOfPoints;Counter++)
		{
			if (pCountPP[Counter])
			{
				for (Counter2 = 0; Counter2 < pCountPP[Counter] - 1; Counter2++)
				{
					for (Counter3 = Counter2 + 1; Counter3 < pCountPP[Counter] ; Counter3++)
					{
						if (pEPPAngles[pFPPBase[Counter] + Counter2] > pEPPAngles[pFPPBase[Counter] + Counter3])
						{
							GLV_Scalar fSwp;
							ULONG ESwp;
							ESwp = pEdgesPerP[pFPPBase[Counter] + Counter2];
							pEdgesPerP[pFPPBase[Counter] + Counter2] = pEdgesPerP[pFPPBase[Counter] + Counter3];
							pEdgesPerP[pFPPBase[Counter] + Counter3] = ESwp ;
							fSwp = pEPPAngles[pFPPBase[Counter] + Counter2];
							pEPPAngles[pFPPBase[Counter] + Counter2] = pEPPAngles[pFPPBase[Counter] + Counter3];
							pEPPAngles[pFPPBase[Counter] + Counter3] = fSwp ;
						}
#ifdef GLV_DEBUG
						if (pEPPAngles[pFPPBase[Counter] + Counter2] == pEPPAngles[pFPPBase[Counter] + Counter3])
						{
							Counter = Counter;
						}
#endif
						if (pEdgesPerP[pFPPBase[Counter] + Counter2] == pEdgesPerP[pFPPBase[Counter] + Counter3])
						{
							Counter = Counter;
						}
					}
				}
			}
		}
		/* Close Angle connectivity */
		for (Counter=0;Counter<p_stGLVToClip->ulNumberOfPoints;Counter++)
		{
			pEdgesPerP[pFPPBase[Counter] + pCountPP[Counter]] = pEdgesPerP[pFPPBase[Counter]];
			pEdgesPerP[pFPPBase[Counter] - 1 ] = pEdgesPerP[pFPPBase[Counter] + pCountPP[Counter] - 1];
		}
		/* Isolated poyline detection */
		/* erase GLV_FLGS_RSV1 & GLV_FLGS_RSV2 */
		ulNumberofIsolatedPolylines = 0;
		for (Counter=0;Counter<p_stGLVToClip->ulNumberOfPoints;Counter++)
		{
			p_stGLVToClip->p_stPoints[Counter].ulFlags &= ~(GLV_FLGS_RSV1|GLV_FLGS_RSV2);
		}
		GLV_RecursiveDetectIsolatedpolylines ( p_stGLVToClip , pFPPBase , pCountPP , pEdgesPerP , p_stGLVToClip->p_stFaces[0].Index[0] , GLV_FLGS_RSV1 );
		GLV_RecursiveDetectIsolatedpolylines ( p_stGLVToClip , pFPPBase , pCountPP , pEdgesPerP , p_stGLVToClip->p_stFaces[0].Index[1] , GLV_FLGS_RSV1 );
		GLV_RecursiveDetectIsolatedpolylines ( p_stGLVToClip , pFPPBase , pCountPP , pEdgesPerP , p_stGLVToClip->p_stFaces[0].Index[2] , GLV_FLGS_RSV1 );
		ulNumberofIsolatedPolylines = 0;
		for ( Counter = 0 ; Counter < p_stGLVToClip -> ulNumberOfPoints ; Counter ++ )
		{
			if (!(p_stGLVToClip->p_stPoints[Counter].ulFlags & GLV_FLGS_RSV1))
			{
				ulNumberofIsolatedPolylines++;
				GLV_RecursiveDetectIsolatedpolylines ( p_stGLVToClip , pFPPBase , pCountPP , pEdgesPerP , Counter , GLV_FLGS_RSV2|GLV_FLGS_RSV1 );
				// Create a Link Edge 
				if (!GLV_LinkRV2( p_stGLVToClip , p2DCoords)) 
					ulNumberofIsolatedPolylines = 0xffffffff;
				// Leave... Resolve one by one ...
				Counter = p_stGLVToClip->ulNumberOfPoints; 
			}
		}
		if (ulNumberofIsolatedPolylines)
		{
			GLV_FREE(pCountPP);
			GLV_FREE(pFPPBase);
			GLV_FREE(p2DCoords);
			GLV_FREE(pPolyline);
			GLV_FREE(pEdgesPerP);
			GLV_FREE(pEPPAngles);
			if (ulNumberofIsolatedPolylines == 0xffffffff) return;
		}//*/
	} // While ulNumberofIsolatedPolylines
	
	/* Find ngones And triangularize */
	fSumSurf = 0.0f;
	ulNumberOfPoints = 0;
	ulNumberofPolylines = 0;
	for (Counter=0;Counter<p_stGLVToClip->ulNumberOfPoints;Counter++)
	{
		for (Counter2 = 0; Counter2 < pCountPP[Counter] ;Counter2++ )
			if (!(pEdgesPerP[pFPPBase[Counter] + Counter2] & 0x80000000))
			{
				ULONG ulLocal,ActulP,NextP;
				//					pDownCount[Counter]--;
				ActulP = Counter;
				NextP = pEdgesPerP[pFPPBase[Counter] + Counter2];
				ulNumberOfPoints = 0;
				pPolyline[ulNumberOfPoints++] = ActulP;
				while (!(NextP & 0x80000000))
				{
					ulLocal = 0;
					while ((pEdgesPerP[pFPPBase[ActulP] + ulLocal] & 0x7fffffff) != NextP) ulLocal++;
					pEdgesPerP[pFPPBase[ActulP] + ulLocal] |= 0x80000000; /* Could not be a next point anymore */
					if (ulLocal == 0) 
						pEdgesPerP[pFPPBase[ActulP] + pCountPP[ActulP]] |= 0x80000000; /* Could not be a next point anymore */
					if (ulLocal == pCountPP[ActulP] - 1) 
						pEdgesPerP[pFPPBase[ActulP] - 1] |= 0x80000000; /* Could not be a next point anymore */
					ulLocal = 0;
					while ((pEdgesPerP[pFPPBase[NextP] + ulLocal] & 0x7fffffff) != ActulP) ulLocal++;
					ActulP = NextP;
					NextP = pEdgesPerP[pFPPBase[ActulP] + ulLocal + 1];
					pPolyline[ulNumberOfPoints++]= ActulP;
				}
				ulLocal = 0;
				/*					while ((pEdgesPerP[pFPPBase[ActulP] + ulLocal] & 0x3fffffff) != NextP) ulLocal++;
				pEdgesPerP[pFPPBase[ActulP] + ulLocal] |= 0x80000000; 
				pPolyline[ulNumberOfPoints] = Counter; /* Close poly */
#ifdef GLV_DEBUG
				if (pPolyline[ulNumberOfPoints - 1] != pPolyline[0])
					GLV_BUG(4, "polylines not closed");
				if (ulNumberOfPoints > ulMaxNumOfPoyPoints)
					GLV_BUG(4, "Generate too long polylines");
				{
					GLV_Scalar Sum;	
					Sum = GetPolySurf( p_stGLVToClip , pPolyline , ulNumberOfPoints , p2DCoords);
					if (Sum < 0.0f) Sum = -Sum;
					fSumSurf += Sum * 0.5f;
				}

#endif
				/* Detect if this poly is VAlid (not The poly around the triangle)*/
				/* This is the only one with a positive surface */
				ulNumberofPolylines++;
				if (GetPolySurf( p_stGLVToClip , pPolyline , ulNumberOfPoints , p2DCoords) > 0.0f) 
				{
					/* Ersase flag of Could not be a next point anymore */
					/* and continue */
					continue;//*/
				}
				
#ifdef GLV_DEBUG
				/* DETECT MULTIPASS ON A POINT */
				{
					ULONG Counter4;
					GLV_ResetCluster(p_stCluster);
					for (Counter4 = 0 ; Counter4 < ulNumberOfPoints - 1; Counter4 ++)
					{
						if (GLV_Clst_IsExist(p_stCluster , pPolyline[Counter4]))
						{
							ISADDED |= 4;
						}
						GLV_Clst_ADDV(p_stCluster , pPolyline[Counter4]);
					}

				}

#endif
		//		ulNumberOfPoints = GLV_PolyRemoveColinear(p_stGLVToClip , pPolyline , ulNumberOfPoints , p2DCoords);
				/* compute surface of polyline (DEBUG) */
				GLV_CreateTrianglesFromPolyline(p_stGLVToClip , pPolyline , ulNumberOfPoints , p2DCoords , ulPolylineCounter++);
			}
	}
#ifdef GLV_DEBUG
	{
		float Err1 , Err2 ;
		/* Verify with surface & flictst (DEBUG) */
		if (fSumSurf > fFirstSurf)
			fCalcSurf = fSumSurf / fFirstSurf;
		else
			fCalcSurf = fFirstSurf / fSumSurf;
		fCalcSurf *= 100.0f;
		fCalcSurf -= 100.0f;
		if (fCalcSurf >= 0.01f) /* 0.0001f % of error max */
		{
			ISADDED |= 1; 
		}
		Err1 = (float)fCalcSurf;
		fSumSurf = 0.0f;
		Flag = 0;
		for (Counter=1;Counter<p_stGLVToClip->ulNumberOfFaces;Counter++)
		{
			fSumSurf += GLV_GetSurf(p_stGLVToClip , &p_stGLVToClip->p_stFaces[Counter]);
			if (GLV_FlicTst(p_stGLVToClip , Counter)) 
			{
				/* Flip it */
				Counter = Counter;
			}
		}
		if (fSumSurf > fFirstSurf)
			fCalcSurf = fSumSurf / fFirstSurf;
		else
			fCalcSurf = fFirstSurf / fSumSurf;
		fCalcSurf *= 100.0f;
		fCalcSurf -= 100.0f;
		if (fCalcSurf >= 0.01f) /* 0.0001f % of error max */
		{
			ISADDED |= 2;
		}
		Err2 = (float)fCalcSurf;
		{
			char ErrorString[256];
			switch (ISADDED)
			{
			case 0 : break;
			case 1 : 
				GLV_BUG(4,"bug in polyline generation");
				break;
			case 2 : 
				GLV_BUG(4,"bug in triangle generation");
				{
					ULONG EC;
					EC = p_stGLVToClip->ulNumberOfEdges;
					while (EC--) p_stGLVToClip->p_stEdges[EC].ulFlags = GLV_FLGS_RSV2;
				}
				GLV_AddGLV(BugReport , p_stGLVToClip);
				break;
			case 3 : 
				sprintf(ErrorString , "bug in polyline & triangle generation (%f %% of error for poly & %f %% of error for tris)" , Err1 , Err2);
				GLV_BUG(4,ErrorString);
				{
					ULONG EC;
					EC = p_stGLVToClip->ulNumberOfEdges;
					while (EC--) p_stGLVToClip->p_stEdges[EC].ulFlags = GLV_FLGS_RSV1;
				}
				GLV_AddGLV(BugReport , p_stGLVToClip);
				break;
			case 4 : 
				break;
			case 5 : 
			case 6 : 
			case 7 : 
				sprintf(ErrorString , "bug in polyline & triangle generation (WHIT MP Detected) (%f %% of error for poly & %f %% of error for tris)" , Err1 , Err2);
				GLV_BUG(4,ErrorString);
				{
					ULONG EC;
					EC = p_stGLVToClip->ulNumberOfEdges;
					while (EC--) p_stGLVToClip->p_stEdges[EC].ulFlags = GLV_FLGS_RSV1|GLV_FLGS_RSV2;
				}
				GLV_AddGLV(BugReport , p_stGLVToClip);
				break;
			}
		}
	}
	GLV_DelCluster(p_stCluster);
#endif

	GLV_FREE(pPolyline);
	GLV_FREE(p2DCoords);
	GLV_FREE(pCountPP);
	GLV_FREE(pEdgesPerP);
	GLV_FREE(pFPPBase);
	GLV_FREE(pEPPAngles);
	
	if (p_stGLVToClip->p_stFaces[0].ulFlags & GLV_FLGS_Original)
	{
		/* Detect NUll surface */
		GLV_InterpolOriginalsScalars(p_stGLVToClip);
		GLV_ComputeNghbr(p_stGLVToClip);
		GLD_MakeItShorter( p_stGLVToClip , 3);
		GLD_MakeItShorter( p_stGLVToClip , 3);
		GLD_MakeItShorter( p_stGLVToClip , 3);
		GLD_MakeItShorter( p_stGLVToClip , 3);
	}
}

void GLV_CutPolylineWithAFace2(tdst_GLV *p_GLVDst,tdst_GLV *p_GLVSrc, ULONG I1 , ULONG IToCut )
{
	ULONG Counter;
	tdst_GLV_Point stProjPointResult;
	/* Cut all Existanrts edges if cut the face */
	stProjPointResult.ulFlags = GLV_FLGS_FromFace|GLV_FLGS_Kilt;
	for (Counter = 0 ; Counter < p_GLVDst->ulNumberOfEdges ; Counter ++)
	{
		if (GLV_IsPointInTriangle(p_GLVSrc , I1 , &p_GLVDst->p_stPoints[p_GLVDst->p_stEdges[Counter].Index[0]].P3D)) continue;
		if (GLV_IsPointInTriangle(p_GLVSrc , I1 , &p_GLVDst->p_stPoints[p_GLVDst->p_stEdges[Counter].Index[1]].P3D)) continue;
		if (GLV_PointFromEdgeTouchFace(&p_GLVSrc->p_stFaces[I1] , &p_GLVSrc->p_stFaces[IToCut] , &p_GLVDst->p_stPoints[p_GLVDst->p_stEdges[Counter].Index[0]].P3D, &p_GLVDst->p_stPoints[p_GLVDst->p_stEdges[Counter].Index[1]].P3D , &stProjPointResult.P3D))
			if (GLV_IsPointInTriangle(p_GLVSrc , I1 , &stProjPointResult.P3D))
			{
				GLV_SetNumbers(p_GLVDst , p_GLVDst->ulNumberOfPoints + 1 , p_GLVDst->ulNumberOfEdges + 1 , 0 , 3);
				p_GLVDst->p_stPoints[p_GLVDst->ulNumberOfPoints - 1] = stProjPointResult;
				p_GLVDst->p_stEdges[p_GLVDst->ulNumberOfEdges - 1] = p_GLVDst->p_stEdges[Counter];
				p_GLVDst->p_stEdges[p_GLVDst->ulNumberOfEdges - 1].Index[1] = p_GLVDst->p_stEdges[Counter].Index[1];
				p_GLVDst->p_stEdges[Counter].Index[1] = p_GLVDst->ulNumberOfPoints - 1;
				p_GLVDst->p_stEdges[p_GLVDst->ulNumberOfEdges - 1].Index[0] = p_GLVDst->ulNumberOfPoints - 1;
				p_GLVDst->p_stEdges[p_GLVDst->ulNumberOfEdges - 1].ulFlags = 0;// GLV_FLGS_IsShadowCuter
			}
	}
}
void GLV_CutPolylineWithAFace(tdst_GLV *p_GLVDst,tdst_GLV *p_GLVSrc, ULONG I1 , ULONG IToCut )
{
	ULONG Counter,Counter2,ulNumEdges,ulNumEdgesResult;
//	tdst_GLV_Face *p_F1, *p_FToCut;
	tdst_GLV_Point stProjPoint[6];
	tdst_GLV_Point stProjPointResult;
	ULONG ulValidity[6];
	ulNumEdges = 0;

	/* Edges src poke in edge Dest */
	for (Counter = 0 ; Counter < 3 ; Counter ++)
	{
		if (GLV_PointFromEdgeTouchFace(&p_GLVSrc->p_stFaces[I1] , &p_GLVSrc->p_stFaces[IToCut] , &p_GLVSrc->p_stPoints[p_GLVSrc->p_stFaces[IToCut].Index[Counter]].P3D, &p_GLVSrc->p_stPoints[p_GLVSrc->p_stFaces[IToCut].Index[(Counter + 1 ) % 3]].P3D , &stProjPoint[ulNumEdges].P3D))
			if (GLV_IsPointInTriangle(p_GLVSrc , I1 , &stProjPoint[ulNumEdges].P3D))
			{
				stProjPoint[ulNumEdges].ulFlags		= GLV_FLGS_FromEdge|GLV_FLGS_Kilt;
				stProjPoint[ulNumEdges].DownIndex	= (Counter + 0) % 3;
				ulNumEdges++;
			}
	}
	/* Edges Dest poke in edge src */
	for (Counter = 0 ; Counter < 3 ; Counter ++)
	{
		if (GLV_PointFromEdgeTouchFace(&p_GLVSrc->p_stFaces[IToCut] , &p_GLVSrc->p_stFaces[IToCut] , &p_GLVSrc->p_stPoints[p_GLVSrc->p_stFaces[I1].Index[Counter]].P3D, &p_GLVSrc->p_stPoints[p_GLVSrc->p_stFaces[I1].Index[(Counter + 1 ) % 3]].P3D , &stProjPoint[ulNumEdges].P3D))
			if (GLV_IsPointInTriangle(p_GLVSrc , IToCut  , &stProjPoint[ulNumEdges].P3D))
			{
				stProjPoint[ulNumEdges].ulFlags = GLV_FLGS_FromFace|GLV_FLGS_Kilt;
				ulNumEdges++;
			}
	}
	/* WELD Result */
	if (ulNumEdges == 0) 
		return;
	if (ulNumEdges == 1) 
	{
		ULONG Res;
		/* Mean two triangle must share 1 index */ 
		Res = 0xffffffff;
		for (Counter = 0 ; Counter < 3 ; Counter ++)
		{
			for (Counter2 = 0 ; Counter2 < 3 ; Counter2 ++)
			{
				if (p_GLVSrc->p_stFaces[I1].Index[Counter2] == p_GLVSrc->p_stFaces[IToCut].Index[Counter])
					Res = p_GLVSrc->p_stFaces[I1].Index[Counter2];
			}
		}//*/
		if (Res == 0xffffffff) // Mean one point is on the surface of the other ?
		{
			for (Counter = 0 ; Counter < 3 ; Counter ++)
			{
				/* p_F1 -> p_FToCut */
				if (GLV_IsPointInTriangle(p_GLVSrc , IToCut , &p_GLVSrc->p_stPoints[p_GLVSrc->p_stFaces[I1].Index[Counter]].P3D))
				{
					Res = p_GLVSrc->p_stFaces[I1].Index[Counter];
					Counter = 4;
				} else
					/* p_FToCut -> p_F1 */
					if (GLV_IsPointInTriangle(p_GLVSrc , I1 , &p_GLVSrc->p_stPoints[p_GLVSrc->p_stFaces[IToCut].Index[Counter]].P3D))
					{
						Res = p_GLVSrc->p_stFaces[IToCut].Index[Counter];
						Counter = 4;
					}
			}
		} 
		
		if (Res == 0xffffffff)
		{
			GLV_BUG(4, "couldn't find 2 point from 2 triangles (only 1!!!???)");//BUG! ()
			return; // BUG! Strange... Mean one edge cut the other? 
		}
		
		stProjPoint[ulNumEdges++] = p_GLVSrc->p_stPoints[Res];
		//stProjPoint[ulNumEdges++].ulFlags = GLV_FLGS_Original|GLV_FLGS_Kilt;
	}
	if (ulNumEdges != 2)
	{
		ulNumEdgesResult = 0;
		for (Counter = 0 ; Counter < ulNumEdges ; Counter ++)
			ulValidity[Counter] = 1;
		for (Counter = 0 ; Counter < ulNumEdges - 1 ; Counter ++)
			for (Counter2 = Counter + 1 ; Counter2 < ulNumEdges ; Counter2 ++)
			{
				MATHD_SubVector(&stProjPointResult.P3D, &stProjPoint[Counter].P3D, &stProjPoint[Counter2].P3D);
				if (MATHD_f_SqrNormVector(&stProjPointResult.P3D) < GLF_PrecMetric)
					ulValidity[Counter2] = 0;
			}
			Counter2 = ulNumEdges ;
			ulNumEdges = 0;
			for (Counter = 0 ; Counter < Counter2 ; Counter ++)
			{
				if (ulValidity[Counter])
					stProjPoint[ulNumEdges++] = stProjPoint[Counter];
			}
	}
	if (ulNumEdges != 2) 
		return;
	/* If weld is ok create an edge */
	GLV_SetNumbers(p_GLVDst , p_GLVDst ->ulNumberOfPoints + 2 , p_GLVDst ->ulNumberOfEdges + 1  , 0 , 3);
	p_GLVDst -> p_stEdges  [p_GLVDst->ulNumberOfEdges - 1].ulFlags = p_GLVSrc->p_stFaces[I1].ulFlags | p_GLVSrc->p_stFaces[IToCut].ulFlags;
	p_GLVDst -> p_stPoints [p_GLVDst -> ulNumberOfPoints - 1] = stProjPoint[0];
	p_GLVDst -> p_stPoints [p_GLVDst -> ulNumberOfPoints - 2] = stProjPoint[1];
	p_GLVDst -> p_stEdges  [p_GLVDst->ulNumberOfEdges - 1].Index[0] = p_GLVDst -> ulNumberOfPoints - 1;
	p_GLVDst -> p_stEdges  [p_GLVDst->ulNumberOfEdges - 1].Index[1] = p_GLVDst -> ulNumberOfPoints - 2;

	p_GLVDst -> p_stEdges  [p_GLVDst->ulNumberOfEdges - 1].FromFaces[0] = I1;
	p_GLVDst -> p_stEdges  [p_GLVDst->ulNumberOfEdges - 1].FromFaces[1] = IToCut;
#ifdef GLV_DEBUG
	GLV_Verify(p_GLVDst);
#endif
	
}


int GLV_IsFaceTouchFace(tdst_GLV *p_stGLV , ULONG F1 , ULONG F2)
{
	ULONG Counter;
	ULONG Ret;
	MATHD_tdst_Vector stMAX1 , stMIN1 , stMAX2 , stMIN2;
	//	return 1;
	GLV_MinMaxFace(p_stGLV , F1 , &stMIN1 , &stMAX1 );
	GLV_MinMaxFace(p_stGLV , F2 , &stMIN2 , &stMAX2 );
	if (!GLV_IsBoxTouchBox(&stMIN1 , &stMAX1 ,&stMIN2 , &stMAX2 )) return 0;
	//	return 1;
	Ret = 0;
	for (Counter = 0 ; Counter < 3 ; Counter++)
	{
		Ret |= GLV_IsEdgeTouchFace(p_stGLV , F1 , NULL , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F2].Index[Counter]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F2].Index[(Counter + 1)% 3]].P3D,NULL);
		Ret |= GLV_IsEdgeTouchFace(p_stGLV , F2 , NULL , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F1].Index[Counter]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F1].Index[(Counter + 1)% 3]].P3D,NULL);
	}
	return Ret;
}

ULONG GLV_FaceToFace(tdst_GLV *p_stGLV , ULONG FToCut , ULONG FWhichTouch)
{
	ULONG K;
	K = 0;

	if (!(p_stGLV->p_stFaces[FToCut].ulChannel & p_stGLV->p_stFaces[FWhichTouch].ulChannel)) 
		return 0;

	if (p_stGLV->p_stFaces[FToCut].ulFlags & GLV_FLGS_Original)
	{
		/* Everything cut an original face */
		/* Without if the face is not Selected */
		if (p_stGLV->p_stFaces[FWhichTouch].ulFlags & GLV_FLGS_Original)
		{
			if (!((p_stGLV->p_stFaces[FToCut].ulMARK | p_stGLV->p_stFaces[FWhichTouch].ulMARK) & 0x40000000)) 
				return 0;
		}
		goto CutIt;
	}

	if (p_stGLV ->p_stFaces[FToCut].ulFlags & GLV_FLGS_Kilt)
	{
		/* If kilts are from different faces , return*/
		if (p_stGLV ->p_stFaces[FWhichTouch].ulFlags & GLV_FLGS_Kilt) 
		{
			if (p_stGLV ->p_stFaces[FToCut].ulMARK != p_stGLV ->p_stFaces[FWhichTouch].ulMARK) return 0;
			if ((((p_stGLV ->p_stFaces[FToCut].ulFlags ^ p_stGLV ->p_stFaces[FWhichTouch].ulFlags)) & (GLV_FLGS_WhiteFront|GLV_FLGS_BlackFront)) == (GLV_FLGS_WhiteFront|GLV_FLGS_BlackFront)) return 0;
		}
		goto CutIt;
	}
	return 0;
CutIt:
	if (GLV_IsEqualPlanesNoBackface(&p_stGLV->p_stFaces[FToCut].Plane, &p_stGLV->p_stFaces[FWhichTouch].Plane)) 
		return 0;
	if (GLV_Share2Indexes(p_stGLV, FToCut, FWhichTouch)) return 0; /* 2 Common Indexes : Not good */
	if (GLV_IsFaceTouchFace(p_stGLV , FToCut , FWhichTouch)) return 1;
	return 0;
}

ULONG GLV_CutAll(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *) , GLV_OCtreeNode *p_stOctreeROOT)
{
	ULONG C1,C2,ulNumFaces;
	tdst_GLV *p_stGLVToClip,*p_stGLVToAccum ;
	
	GLD_RemoveIllegalFaces( p_stGLV );

	ulPolylineCounter = 0;
	
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces; C1 ++)
	{
		p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[0]].ulFlags = GLV_FLGS_Kilt|GLV_FLGS_Original;
		p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[1]].ulFlags = GLV_FLGS_Kilt|GLV_FLGS_Original;
		p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[2]].ulFlags = GLV_FLGS_Kilt|GLV_FLGS_Original;
	}
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces; C1 ++)
	{
		if (!(p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Kilt))
		{
			p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[0]].ulFlags = GLV_FLGS_Original;
			p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[1]].ulFlags = GLV_FLGS_Original;
			p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[2]].ulFlags = GLV_FLGS_Original;
		}
	}
	/* Clip */
	p_stGLVToClip = NewGLV();
	p_stGLVToAccum= NewGLV();
	GLV_COPY_VARS	(p_stGLVToClip , p_stGLV);
	GLV_COPY_VARS	(p_stGLVToAccum, p_stGLV);

	ulNumFaces = p_stGLV->ulNumberOfFaces;
	for (C1 = 0 ; C1 < ulNumFaces ; C1 ++)
	{
		p_stGLV->p_stFaces[C1].ulFlags &= ~(GLV_FLGS_ClipResultFF|GLV_FLGS_ClipResultBF|GLV_FLGS_Clipped|GLV_FLGS_DeleteIt);
	}
	/* Detect and Cut */
	{
		GLV_Cluster *p_stCluster1;
		GLV_Cluster *p_stCluster2;
		p_stCluster1 = GLV_NewCluster();
		p_stCluster2 = GLV_NewCluster();
		for (C1 = 0 ; C1 < ulNumFaces ; C1 ++)
		{
			if (SeprogressPos ((float)C1 / (float)ulNumFaces, "Detect Cut & Culling")) return 1;
			p_stGLV->p_stFaces[C1].ulFlags &= ~GLV_FLGS_DeleteIt;
/*			if (!(p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Kilt)) continue;
			GLV_ResetCluster(p_stCluster1);
			GLV_ResetCluster(p_stCluster2);
			GLV_Octree_DiveAndIntesectTriangle( p_stOctreeROOT , C1 , p_stCluster1 );
			C2 = 0;
			GLV_Enum_Cluster_Value(p_stCluster1)
				if (C1 != MCL_Value)
				{
					if ((p_stGLV->p_stFaces[C1].ulMARK == p_stGLV->p_stFaces[MCL_Value].ulMARK) || (p_stGLV->p_stFaces[C1].ulMARK & p_stGLV->p_stFaces[MCL_Value].ulMARK & 0x80000000) || (p_stGLV->p_stFaces[MCL_Value].ulFlags & GLV_FLGS_Original))
						if (GLV_FaceToFace(p_stGLV , C1 , MCL_Value))
						{
							GLV_Clst_ADDV(p_stCluster2 , MCL_Value);
							C2 = 1;
						}
				}
			GLV_Enum_Cluster_Value_End();//*/
			if (!(p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original)) continue;
			GLV_ResetCluster(p_stCluster1);
			GLV_ResetCluster(p_stCluster2);
			GLV_Octree_DiveAndIntesectTriangle( p_stOctreeROOT , C1 , p_stCluster1 );
			C2 = 0;
			GLV_Enum_Cluster_Value(p_stCluster1)
				if (C1 != MCL_Value)
				{
//					if (p_stGLV->p_stFaces[MCL_Value].ulFlags & GLV_FLGS_Kilt)
						if (GLV_FaceToFace(p_stGLV , C1 , MCL_Value))
						{
							GLV_Clst_ADDV(p_stCluster2 , MCL_Value);
							C2 = 1;
						}
				}
			GLV_Enum_Cluster_Value_End();//*/

			if (C2)
			{
#ifndef GLV_StayWithEdges					
				GLV_SetNumbers(p_stGLVToClip , 3 , 3 , 1 , 7);
#else
				GLV_SetNumbers(p_stGLVToClip , 3 , 0 , 1 , 7);
#endif
				p_stGLVToClip->p_stFaces[0] = p_stGLV->p_stFaces[C1];
				p_stGLVToClip->p_stFaces[0].Index[0] = 0;
				p_stGLVToClip->p_stFaces[0].Index[1] = 1;
				p_stGLVToClip->p_stFaces[0].Index[2] = 2;
				p_stGLVToClip->p_stFaces[0].Nghbr[0] = p_stGLVToClip->p_stFaces[0].Nghbr[1] = p_stGLVToClip->p_stFaces[0].Nghbr[2] = -1;
				p_stGLVToClip->p_stPoints[0] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[0]];
				p_stGLVToClip->p_stPoints[1] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[1]];
				p_stGLVToClip->p_stPoints[2] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[2]];
#ifndef GLV_StayWithEdges					
				p_stGLVToClip->p_stEdges[0].Index[0] = 0;
				p_stGLVToClip->p_stEdges[0].Index[1] = 1;
				p_stGLVToClip->p_stEdges[1].Index[0] = 1;
				p_stGLVToClip->p_stEdges[1].Index[1] = 2;
				p_stGLVToClip->p_stEdges[2].Index[0] = 2;
				p_stGLVToClip->p_stEdges[2].Index[1] = 0;
				p_stGLVToClip->p_stEdges[2].ulFlags = p_stGLV->p_stFaces[C1].ulFlags;
				p_stGLV->p_stFaces[C1].ulFlags |= GLV_FLGS_DeleteIt;
#endif
				//			if (C2)
				//			{
				GLV_Enum_Cluster_Value(p_stCluster2)
					GLV_CutPolylineWithAFace(p_stGLVToClip , p_stGLV , MCL_Value , C1);
				GLV_Enum_Cluster_Value_End()
					
				GLV_Enum_Cluster_Value(p_stCluster2)
					GLV_CutPolylineWithAFace2(p_stGLVToClip , p_stGLV , MCL_Value , C1);
				GLV_Enum_Cluster_Value_End()

#ifndef GLV_StayWithEdges
				GLV_ResolvePolyline(p_stGLVToClip );
#endif

				p_stGLVToClip->p_stFaces[0].ulFlags |= GLV_FLGS_DeleteIt;
				
				GLV_ResetCluster(p_stCluster2);
#ifndef GLV_StayWithEdges
				GLV_RemoveOverLap_LOCAL(p_stGLV , p_stGLVToClip , p_stOctreeROOT , p_stCluster2 );
#endif
				
				GLD_Remove2BD(p_stGLVToClip);
#ifndef GLV_StayWithEdges					
				GLV_SetNumbers(p_stGLVToClip , 0 , 0 , 0 , 2); /* erase all edges */
#endif
			}//*/

			GLV_AddGLV(p_stGLVToAccum,p_stGLVToClip);
			
		}
		GLV_DelCluster(p_stCluster1);
		GLV_DelCluster(p_stCluster2);
	}

	for (C1 = 0 ; C1 < p_stGLVToAccum->ulNumberOfFaces ; C1 ++)
	{
		p_stGLVToAccum->p_stFaces[C1].Nghbr[0] = p_stGLVToAccum->p_stFaces[C1].Nghbr[1] = p_stGLVToAccum->p_stFaces[C1].Nghbr[2] = -1;
	}

	GLV_Destroy(p_stGLVToClip);

	GLD_Remove2BD(p_stGLVToAccum);
	GLD_Remove2BD(p_stGLV);
	GLD_Weld( p_stGLVToAccum , GLV_WELD_FCT , 0 );
	GLD_RemoveIllegalFaces( p_stGLVToAccum );
	GLD_RemoveUnusedIndexes( p_stGLVToAccum );
	GLV_AddGLV(p_stGLV , p_stGLVToAccum);
#ifndef GLV_StayWithEdges					
	GLV_SetNumbers(p_stGLV , 0 , 0 , 0 , 2); /* erase edges */
#endif

	GLV_Destroy(p_stGLVToAccum);
	return 0;
}


ULONG GLV_DetectOO(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG C1,C2,ulNumFaces;
	tdst_GLV *p_stGLVToClip, *p_stGLVOfEdges;
	
	GLD_RemoveIllegalFaces( p_stGLV );
	
	/* Clip */
	p_stGLVToClip = NewGLV();
	p_stGLVOfEdges= NewGLV();
	ulNumFaces = p_stGLV->ulNumberOfFaces;
	/* Detect and Cut */
	{
		GLV_Cluster *p_stCluster1;
		GLV_Cluster *p_stCluster2;
		GLV_OCtreeNode stOctreeROOT;
		L_memset(&stOctreeROOT , 0 , sizeof (GLV_OCtreeNode));
		stOctreeROOT.p_OctreeFather = NULL;
		stOctreeROOT.p_stThisCLT = GLV_NewCluster();
		C1 = p_stGLV->ulNumberOfFaces;
		while (C1--) if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original) GLV_Clst_ADDV(stOctreeROOT.p_stThisCLT , C1); /* Add all the faces */
		GLV_ComputeGLVBox(p_stGLV , &stOctreeROOT.stAxisMin, &stOctreeROOT.stAxisMax , GLV_FLGS_Original);
		SeprogressPos (0.0f, "Build OK3..");
		GLV_Octree_DiveAndCompute( p_stGLV , &stOctreeROOT , GLV_OC3_MAX_DEPTH , GLV_OC3_MAX_FPO, SeprogressPos);
		p_stCluster1 = GLV_NewCluster();
		p_stCluster2 = GLV_NewCluster();
		/* Find Egde Original - Original */
		for (C1 = 0 ; C1 < ulNumFaces ; C1 ++)
			if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original)
			{
				if (SeprogressPos ((float)C1 / (float)ulNumFaces, "Original<->Original")) return 1;
				GLV_ResetCluster(p_stCluster1);
				GLV_ResetCluster(p_stCluster2);
				GLV_Octree_DiveAndIntesectTriangle( &stOctreeROOT , C1 , p_stCluster1 );
				C2 = 0;
				GLV_Enum_Cluster_Value(p_stCluster1)
					if (C1 != MCL_Value) 
					{
						if (GLV_FaceToFace(p_stGLV , C1 , MCL_Value))							
						{
							GLV_Clst_ADDV(p_stCluster2 , MCL_Value);
							C2 = 1;
						}
					}
				GLV_Enum_Cluster_Value_End();
				
				if (C2) // their is at least one triangle 
				{
					GLV_SetNumbers(p_stGLVToClip , 3 , 0 , 1 , 7);
					p_stGLVToClip->p_stFaces[0] = p_stGLV->p_stFaces[C1];
					p_stGLVToClip->p_stFaces[0].Index[0] = 0;
					p_stGLVToClip->p_stFaces[0].Index[1] = 1;
					p_stGLVToClip->p_stFaces[0].Index[2] = 2;
					p_stGLVToClip->p_stFaces[0].Nghbr[0] = p_stGLVToClip->p_stFaces[0].Nghbr[1] = p_stGLVToClip->p_stFaces[0].Nghbr[2] = -1;
					p_stGLVToClip->p_stPoints[0] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[0]];
					p_stGLVToClip->p_stPoints[1] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[1]];
					p_stGLVToClip->p_stPoints[2] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[2]];

					GLV_Enum_Cluster_Value(p_stCluster2)
						GLV_CutPolylineWithAFace(p_stGLVToClip , p_stGLV , MCL_Value , C1);
					GLV_Enum_Cluster_Value_End()

					GLV_Enum_Cluster_Value(p_stCluster2)
						GLV_CutPolylineWithAFace2(p_stGLVToClip , p_stGLV , MCL_Value , C1);
					GLV_Enum_Cluster_Value_End()

					GLV_AddGLV(p_stGLVOfEdges,p_stGLVToClip);
				};
			}//*/

		GLV_DelCluster(p_stCluster1);
		GLV_DelCluster(p_stCluster2);
		GLV_Octree_Destroy( &stOctreeROOT );
	}

	GLV_Destroy(p_stGLVToClip);	

	GLV_SetNumbers(p_stGLVOfEdges , 0 , 0 , 0 , 4); /* erase triangles */
	GLD_Weld( p_stGLVOfEdges , GLV_WELD_FCT , 0 );
	GLD_RemoveBadEdges( p_stGLVOfEdges );
	GLD_RemoveDoubleEdges(p_stGLVOfEdges);
	GLD_RemoveUnusedIndexes( p_stGLVOfEdges );
	GLV_AddGLV(p_stGLV , p_stGLVOfEdges);
	GLV_Destroy(p_stGLVOfEdges);
	return 0;
}

#endif
