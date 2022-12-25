/* GEO_LIGHTCUT_RT.c */

/*
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Light Cut Ray-tracing module 
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
#define GLV_LOOP_OPT 10
static GLV_Scalar Topotolerance = 0.005f;
static GLV_Scalar Colortolerance = 1.0f;
static ULONG gs_UseScalars = 0;
#define GLV_OPT_GLF_PrecMetric (Topotolerance) /* 0.5% */

//#define GLV_FAST_OPTIMIZE

typedef struct GLV_OPT_ScalarPoint_
{
	ULONG ulNewIndex;
	ULONG NewColor ;
	GLV_Scalar NewU;
	GLV_Scalar NewV;
	MATH_tdst_Vector NewNormale;
	float	NewAlpha;
	ULONG						NewulSkinNumber;
	tdst_GLV_PointPonderation	NewSkin[4]; /* 4 Matrixes Max */

}
GLV_OPT_ScalarPoint;

ULONG GLV_OPT_EqualScalar(GLV_Scalar A , GLV_Scalar B )
{
	GLV_Scalar D;
	D = A - B;
	if (D < 0.0f) D = -D;
	if (D < 2.0f) return 1;
	return 0;
}
ULONG GLV_OPT_EqualScalarPercnet(GLV_Scalar A , GLV_Scalar B , GLV_Scalar Percent)
{
	GLV_Scalar D;
	D = A - B;
	if (D < 0.0f) D = -D;
	if (D <= Percent + 0.01) return 1;
	return 0;
}

ULONG GLV_OPT_EqualColor_FLT(GLV_Scalar *Color1, GLV_Scalar *Color2 )
{
	GLV_Scalar Res;
	Color1[0] -= Color2[0];
	Color1[1] -= Color2[1];
	Color1[2] -= Color2[2];
	Color1[3] -= Color2[3];
	Res = Color1[0] * Color1[0] + Color1[1] * Color1[1] + Color1[2] * Color1[2] + Color1[3] * Color1[3];
	if (Res <= Colortolerance)
		return 1;
	else
		return 0;
}
GLV_Scalar GLV_LastColorError;
ULONG GLV_OPT_EqualColor_FLT_AMP(GLV_Scalar *Color1, GLV_Scalar *Color2 , GLV_Scalar Amplification)
{
	GLV_Scalar Res;
	Color1[0] -= Color2[0];
	Color1[1] -= Color2[1];
	Color1[2] -= Color2[2];
	Color1[3] -= Color2[3];
	Res = Color1[0] * Color1[0] + Color1[1] * Color1[1] + Color1[2] * Color1[2] + Color1[3] * Color1[3];
	GLV_LastColorError = Res * Amplification ;
	if (GLV_LastColorError <= Colortolerance)
		return 1;
	else
		return 0;
}
ULONG GLV_OPT_EqualColor(ULONG A , ULONG B )
{
	GLV_Scalar Color1[4],Color2[4];
	GLV_ULto4Scalar(A, Color1);
	GLV_ULto4Scalar(B, Color2);
	return GLV_OPT_EqualColor_FLT(Color1, Color2);
}

void GLV_OPT_GetScalarPlane ( tdst_GLV *p_stGLV , ULONG Face , GLV_Scalar *p_3SCL , tdst_GLV_Plane *p_P)
{
#if 1
	GLV_Scalar K0 , K1 ,Local;
	MATHD_tdst_Vector A , B , C ;
	MATHD_tdst_Vector L1 , L2 ;
	K0 = p_3SCL[1] - p_3SCL[0];
	K1 = p_3SCL[2] - p_3SCL[0];
	MATHD_SubVector(&A , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[1]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[0]].P3D);
	MATHD_SubVector(&B , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[2]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[0]].P3D);
	MATHD_CrossProduct(&C , &A, &B);
	MATHD_CrossProduct(&L1 , &B, &C);
	MATHD_CrossProduct(&L2 , &A, &C);
	MATHD_ScaleVector(&L1 , &L1 , 1.0 / MATHD_f_DotProduct(&L1,&A));
	MATHD_ScaleVector(&L2 , &L2 , 1.0 / MATHD_f_DotProduct(&L2,&B));
	p_P->Normale . x = L1.x * K0 + L2.x * K1;
	p_P->Normale . y = L1.y * K0 + L2.y * K1;
	p_P->Normale . z = L1.z * K0 + L2.z * K1;
	p_P->fDistanceTo0 =  p_3SCL[0] - MATHD_f_DotProduct(&p_P->Normale , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[0]].P3D);
	p_P->fDistanceTo0 += p_3SCL[1] - MATHD_f_DotProduct(&p_P->Normale , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[1]].P3D);
	p_P->fDistanceTo0 += p_3SCL[2] - MATHD_f_DotProduct(&p_P->Normale , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[2]].P3D);
	p_P->fDistanceTo0 /= 3.0f;
#else
	GLV_Scalar MAXD , MIND , Delta , Local;
	MATHD_tdst_Vector Save[3];
	tdst_GLV_Face SaveFace;
	MATHD_InitVector(&p_P -> Normale , 0.0f, 0.0f, 0.0f);
	p_P -> fDistanceTo0 = p_3SCL[0];
	MAXD = MIND = p_3SCL[0];
	if (MAXD < p_3SCL[1]) MAXD = p_3SCL[1];
	if (MIND > p_3SCL[1]) MIND = p_3SCL[1];
	if (MAXD < p_3SCL[2]) MAXD = p_3SCL[2];
	if (MIND > p_3SCL[2]) MIND = p_3SCL[2];
	Delta = MAXD - MIND;
	if ((Delta < GLF_PrecMetric) && (Delta > -GLF_PrecMetric)) goto The_Test;
	Save[0] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[0]].P3D;
	Save[1] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[1]].P3D;
	Save[2] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[2]].P3D;
	SaveFace = p_stGLV->p_stFaces[Face];

	Local = 10.0f / Delta;
//	Local = GLV_GetSurf(p_stGLV , p_stGLV->p_stFaces + Face) / Delta;
	MATHD_AddScaleVector(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[0]].P3D , &p_stGLV->p_stFaces[Face].Plane.Normale , (p_3SCL[0] - MIND) * Local);
	MATHD_AddScaleVector(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[1]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[1]].P3D , &p_stGLV->p_stFaces[Face].Plane.Normale , (p_3SCL[1] - MIND) * Local);
	MATHD_AddScaleVector(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[2]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[2]].P3D , &p_stGLV->p_stFaces[Face].Plane.Normale , (p_3SCL[2] - MIND) * Local);
	GLV_ComputeNormale(p_stGLV , p_stGLV->p_stFaces + Face);
	*p_P = p_stGLV->p_stFaces[Face].Plane;
	p_stGLV->p_stFaces[Face] = 	SaveFace;
	p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[0]].P3D = Save[0] ;
	p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[1]].P3D = Save[1] ;
	p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[2]].P3D = Save[2] ;

	Local = MATHD_f_DotProduct(&p_P->Normale , &SaveFace.Plane.Normale);
	MATHD_AddScaleVector(&p_P->Normale , &p_P->Normale , &SaveFace.Plane.Normale , - Local);
	MATHD_NormalizeVector(&p_P->Normale , &p_P->Normale );

	MIND = MAXD = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[0]].P3D , &p_P->Normale);
	Local = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[1]].P3D , &p_P->Normale);
	if (MIND > Local) MIND = Local;
	if (MAXD < Local) MAXD = Local;
	Local = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[2]].P3D , &p_P->Normale);
	if (MIND > Local) MIND = Local;
	if (MAXD < Local) MAXD = Local;
	Local = Delta / (MAXD - MIND);
	MATHD_ScaleEqualVector(&p_P->Normale , -Local);
	p_P->fDistanceTo0 =  p_3SCL[0] - MATHD_f_DotProduct(&p_P->Normale , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[0]].P3D);
	p_P->fDistanceTo0 += p_3SCL[1] - MATHD_f_DotProduct(&p_P->Normale , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[1]].P3D);
	p_P->fDistanceTo0 += p_3SCL[2] - MATHD_f_DotProduct(&p_P->Normale , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[2]].P3D);
	p_P->fDistanceTo0 /= 3.0f;

#endif
The_Test:
	p_P->fDistanceTo0 = -p_P->fDistanceTo0;
	Local = p_3SCL[0] - (MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[0]].P3D , &p_P->Normale) - p_P->fDistanceTo0);
	if ((Local < -GLF_PrecMetric ) || (Local > GLF_PrecMetric ))
		Local = Local;
	Local = p_3SCL[1] - (MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[1]].P3D , &p_P->Normale) - p_P->fDistanceTo0);
	if ((Local < -GLF_PrecMetric ) || (Local > GLF_PrecMetric ))
		Local = Local;
	Local = p_3SCL[2] - (MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[2]].P3D , &p_P->Normale) - p_P->fDistanceTo0);
	if ((Local < -GLF_PrecMetric ) || (Local > GLF_PrecMetric ))
		Local = Local;
}
ULONG GLV_OPT_ProjectScalar ( tdst_GLV *p_stGLV , ULONG Face , ULONG oldIndex , ULONG NewIndex , GLV_OPT_ScalarPoint *p_NS ,GLV_Scalar Amplification)
{
	/* new  fashion */
	GLV_Scalar T3V[3];
	GLV_Scalar T3VC[3];
	GLV_Scalar Color1[4];
	GLV_Scalar Color2[4];
	GLV_Scalar Color3[4];
	GLV_Scalar ColorDST[4];
	GLV_Scalar ColorR[4];

	ULONG Counter;
	tdst_GLV_Plane Plane;
	GLV_Scalar T;
	/* Compute T3V */
	T3VC[0] = 1.0f;
	T3VC[1] = 0.0f;
	T3VC[2] = 0.0f;
	GLV_OPT_GetScalarPlane ( p_stGLV , Face , T3VC , &Plane);
	T3V[0] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[NewIndex].P3D , &Plane.Normale) - Plane.fDistanceTo0;
	T3VC[0] = 0.0f;
	T3VC[1] = 1.0f;
	T3VC[2] = 0.0f;
	GLV_OPT_GetScalarPlane ( p_stGLV , Face , T3VC , &Plane);
	T3V[1] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[NewIndex].P3D , &Plane.Normale) - Plane.fDistanceTo0;
	T3VC[0] = 0.0f;
	T3VC[1] = 0.0f;
	T3VC[2] = 1.0f;
	GLV_OPT_GetScalarPlane ( p_stGLV , Face , T3VC , &Plane);
	T3V[2] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[NewIndex].P3D , &Plane.Normale) - Plane.fDistanceTo0;
	// UV ' S 
	{
		GLV_Scalar PercnetMin,PercnetMax;
 		T = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[0] * T3V[0] + 
			(GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[2] * T3V[1] + 
			(GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[4] * T3V[2] ;

		PercnetMax = PercnetMin = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[0];
		if (PercnetMin > (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[2]) PercnetMin = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[2];
		if (PercnetMin > (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[4]) PercnetMin = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[4];
		if (PercnetMax < (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[2]) PercnetMax = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[2];
		if (PercnetMax < (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[4]) PercnetMax = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[4];

		if (!GLV_OPT_EqualScalarPercnet(p_NS->NewU , T , (PercnetMax - PercnetMin) * 0.05)) return 0;

		T = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[1] * T3V[0] + 
			(GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[3] * T3V[1] + 
			(GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[5] * T3V[2] ;

		PercnetMax = PercnetMin = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[1];
		if (PercnetMin > (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[3]) PercnetMin = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[3];
		if (PercnetMin > (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[5]) PercnetMin = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[5];
		if (PercnetMax < (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[3]) PercnetMax = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[3];
		if (PercnetMax < (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[5]) PercnetMax = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].UV[5];

		if (!GLV_OPT_EqualScalarPercnet(p_NS->NewV , T , (PercnetMax - PercnetMin) * 0.05)) return 0;
	}//*/
	// Alpha 
	{
		T = (GLV_Scalar)p_stGLV ->p_stFaces[Face ].falpha[0] * T3V[0] + 
			(GLV_Scalar)p_stGLV ->p_stFaces[Face ].falpha[1] * T3V[1] + 
			(GLV_Scalar)p_stGLV ->p_stFaces[Face ].falpha[2] * T3V[2] ;
		if (!GLV_OPT_EqualScalar(p_NS->NewAlpha , T)) return 0;
	}//*/

	GLV_ULto4Scalar(p_NS->NewColor, ColorDST);

	/* 2 color are == ? OK */
/*	if (GLV_OPT_EqualColor_FLT(ColorDST, ColorR ))
		return 1;
	else//*/
	{
		GLV_ULto4Scalar(p_stGLV ->p_stFaces[Face ].Colours[0], Color1);
		GLV_ULto4Scalar(p_stGLV ->p_stFaces[Face ].Colours[1], Color2);
		GLV_ULto4Scalar(p_stGLV ->p_stFaces[Face ].Colours[2], Color3);

		for (Counter = 0 ; Counter < 4 ; Counter ++)
		{
			ColorR[Counter] =	T3V[0] * Color1[Counter] +
								T3V[1] * Color2[Counter] + 
								T3V[2] * Color3[Counter];
		}
		if (GLV_OPT_EqualColor_FLT_AMP(ColorDST, ColorR ,Amplification))
			return 1;
		else
		{
			return 0;
		}//*/
	}
//	return 1/*GLV_OPT_EqualColor_FLT(Color3, ColorR )*/;

}

ULONG GLV_IsColinear_OPT( MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_POI , MATHD_tdst_Vector *p_PNI , GLV_Scalar *Error)
{
	MATHD_tdst_Vector S1 , S2 ;
	GLV_Scalar D2;
	MATHD_SubVector(&S1 , p_POI , p_P1);
	MATHD_SubVector(&S2 , p_PNI , p_POI);
	MATHD_NormalizeVector(&S1 , &S1 );
	MATHD_NormalizeVector(&S2 , &S2 );
	D2 = MATHD_f_DotProduct(&S2 , &S1 );
	if (D2*D2 >= 1.0) return 1;
	D2 = (GLV_Scalar)sqrt(1.0 - D2*D2);
	if (D2 < Topotolerance )
		return 1;
	else
		return 0;
}//*/
#define GLV_MAX_NUMBER_OF_SCALAR_POINTS 8
typedef struct GLV_OPT_Operation_
{
	GLV_OPT_ScalarPoint stS[GLV_MAX_NUMBER_OF_SCALAR_POINTS];
	GLV_Scalar Error;
	ULONG Source;
	ULONG Dest;
	ULONG				ulNumberOfScalar;
} GLV_OPT_Operation;

ULONG GLV_AcceptMerge(tdst_GLV *p_stGLV , ULONG Face, ULONG OldIndex , ULONG NewIndex , GLV_OPT_ScalarPoint *p_NS , u32 ulNumberOfScalarPoint , GLV_Scalar *pError)
{
	tdst_GLV_Face SaveFace;
	GLV_Scalar SurfaceBefore,SurfaceAfter;
	ULONG IPos;

	if (p_stGLV ->p_stFaces[Face].ulFlags & GLV_FLGS_RSV1) return 0;

	if (p_stGLV ->p_stFaces[Face].Index[0] == OldIndex) IPos = 0;
	else
	if (p_stGLV ->p_stFaces[Face].Index[1] == OldIndex) IPos = 1;
	else
	if (p_stGLV ->p_stFaces[Face].Index[2] == OldIndex) IPos = 2;

	/* test colinearity */
	if (p_stGLV->p_stFaces[Face].Nghbr[IPos] & 0xff000000)
	{
		if (p_stGLV->p_stFaces [ Face ] . Index [ ( IPos + 1 ) % 3 ] != NewIndex)
		{
			if (!GLV_IsColinear_OPT( &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[(IPos + 1) % 3]].P3D  , &p_stGLV->p_stPoints[OldIndex].P3D  , &p_stGLV->p_stPoints[NewIndex].P3D , pError))
			{
				return 0;
			}
		}
	}

	if ( p_stGLV -> p_stFaces [ Face ] . Nghbr [ ( IPos + 2) % 3] & 0xff000000) 
	{
		if (p_stGLV->p_stFaces[Face].Index[(IPos + 2) % 3] != NewIndex)
		{
			if (!GLV_IsColinear_OPT( &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Face].Index[(IPos + 2) % 3]].P3D  , &p_stGLV->p_stPoints[OldIndex].P3D  , &p_stGLV->p_stPoints[NewIndex].P3D  , pError))
			{
				return 0;
			}
		}
	}//*/

	if ((p_stGLV ->p_stFaces[Face].Index[0] == NewIndex) ||
		(p_stGLV ->p_stFaces[Face].Index[1] == NewIndex) ||
		(p_stGLV ->p_stFaces[Face].Index[2] == NewIndex)) return 1;

	SurfaceBefore = GLV_GetSurf( p_stGLV , p_stGLV->p_stFaces+ Face);
	SaveFace = p_stGLV ->p_stFaces[Face];

	p_stGLV ->p_stFaces[Face].Index[IPos] = NewIndex;

	if (GLV_FlicTst(p_stGLV,Face)) goto Pas_de_merge;

	{
		GLV_Scalar Dist,Hd;
		MATHD_tdst_Vector	st_Vect1;
		Dist = MATHD_f_DotProduct(&SaveFace.Plane.Normale , &p_stGLV ->p_stPoints[NewIndex].P3D ) - SaveFace.Plane.fDistanceTo0;
		MATHD_SubVector(&st_Vect1 , &p_stGLV ->p_stPoints[NewIndex].P3D , &p_stGLV ->p_stPoints[OldIndex].P3D);
		Hd = MATHD_f_NormVector(&st_Vect1);
		if (Dist < 0.0f) Dist = -Dist;
		Hd = (Dist / Hd);
//		*pError += Hd;
		if (Hd >= GLV_OPT_GLF_PrecMetric) goto Pas_de_merge;
	}
//*/
	GLV_ComputeNormale(p_stGLV,p_stGLV ->p_stFaces + Face);

	SurfaceAfter = GLV_GetSurf( p_stGLV , p_stGLV->p_stFaces+ Face);
	if (SurfaceAfter < GLF_PrecMetric) goto Pas_de_merge;

	/* Scalar tests */	
/*	if ((GLV_OPT_EqualColor(p_stGLV ->p_stFaces[Face].Colours[0] , p_stGLV ->p_stFaces[Face].Colours[1] )) &&
		(GLV_OPT_EqualColor(p_stGLV ->p_stFaces[Face].Colours[0] , p_stGLV ->p_stFaces[Face].Colours[2] )))
	{
		if (!GLV_OPT_EqualColor(p_stGLV ->p_stFaces[Face].Colours[IPos] , p_NS->NewColor))
			goto Pas_de_merge;
	} else//*/
	
	p_stGLV ->p_stFaces[Face] = SaveFace;

	if (gs_UseScalars)
	{
		while (ulNumberOfScalarPoint--)
		{
			if (GLV_OPT_ProjectScalar( p_stGLV , Face , OldIndex , NewIndex , p_NS , 1.0+SurfaceAfter)) return 1;
			p_NS++;
		}
		*pError += GLV_LastColorError;
		return 0;
	}
//*/
	return 1;
Pas_de_merge:
	p_stGLV ->p_stFaces[Face] = SaveFace;
	return 0;
}


void GLV_Merge(tdst_GLV *p_stGLV , ULONG Face, ULONG OldIndex , ULONG NewIndex , GLV_OPT_ScalarPoint *p_NS )
{
	ULONG IPos;

	if (!GLV_OPT_ProjectScalar( p_stGLV , Face , OldIndex , NewIndex , p_NS , 1.0 ))
		return;

	p_stGLV ->p_stFaces[Face].ulFlags |= GLV_FLGS_RSV1;
	if (p_stGLV ->p_stFaces[Face].Index[0] == OldIndex) IPos = 0;
	else
	if (p_stGLV ->p_stFaces[Face].Index[1] == OldIndex) IPos = 1;
	else
	if (p_stGLV ->p_stFaces[Face].Index[2] == OldIndex) IPos = 2;
	else
		IPos = 0;


	p_stGLV ->p_stFaces[Face].Index[IPos] = NewIndex;

	p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[Face].Index[0]].ulFlags |= GLV_FLGS_RSV1;
	p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[Face].Index[1]].ulFlags |= GLV_FLGS_RSV1;
	p_stGLV ->p_stPoints[p_stGLV ->p_stFaces[Face].Index[2]].ulFlags |= GLV_FLGS_RSV1;
	p_stGLV ->p_stPoints[OldIndex].ulFlags |= GLV_FLGS_RSV1;

	if ((p_stGLV ->p_stFaces[Face].Index[0] == p_stGLV ->p_stFaces[Face].Index[1]) ||
		(p_stGLV ->p_stFaces[Face].Index[0] == p_stGLV ->p_stFaces[Face].Index[2]) ||
		(p_stGLV ->p_stFaces[Face].Index[1] == p_stGLV ->p_stFaces[Face].Index[2])) 
	{
		p_stGLV ->p_stFaces[Face].ulFlags |= GLV_FLGS_DeleteIt;
		return;
	}


	p_stGLV ->p_stFaces[Face].Colours[IPos] = p_NS->NewColor;
	p_stGLV ->p_stFaces[Face].OriginalPointsNormales[IPos] = p_NS->NewNormale;
	p_stGLV ->p_stFaces[Face].UV[(IPos << 1) + 0] = (float)p_NS->NewU;
	p_stGLV ->p_stFaces[Face].UV[(IPos << 1) + 1] = (float)p_NS->NewV;
	p_stGLV ->p_stFaces[Face].falpha[IPos] = p_NS->NewAlpha;
	p_stGLV ->p_stFaces[Face].ulFlags |= GLV_FLGS_RSV1;
}

int GLV_QSortOperationsClabk(const void *T1,const void *T2)
{
	if (((GLV_OPT_Operation *)T1)->Error == ((GLV_OPT_Operation *)T2)->Error) 
		return 0;
	if (((GLV_OPT_Operation *)T1)->Error <= ((GLV_OPT_Operation *)T2)->Error) 
		return -1;
	else 
		return 1;
}



void GLV_Optimize(tdst_GLV *p_stGLV , ULONG OptimizeLVL , ULONG bUseScalars , ULONG (*SeprogressPos) (float F01, char *))
{
	GLV_Scalar Error;
	ULONG Counter , Counter2 , Total , FLG , IterCounter , Iter2Counter;
	ULONG *p_PPTNum , *p_PPTBase , *p_PPT_ENum ;
	GLV_OPT_Operation *p_AllOperations;
	ULONG				ulNumberOfOperation;
	ULONG				ulMAXOperationNum;
	char Text[1024];
	GLV_Cluster *p_stCluster;
	gs_UseScalars = bUseScalars;
	GLV_BreakUncompatibleLinks_JadeElement_and_UV(p_stGLV);
	p_PPTNum = (ULONG*)GLV_ALLOC(p_stGLV->ulNumberOfPoints * 4L);
	p_PPTBase= (ULONG*)GLV_ALLOC(p_stGLV->ulNumberOfPoints * 4L);
	ulMAXOperationNum = 1024;
	p_AllOperations = (GLV_OPT_Operation*)GLV_ALLOC(ulMAXOperationNum * sizeof (GLV_OPT_Operation) );
	Total = p_stGLV->ulNumberOfFaces;
	p_PPT_ENum = NULL;
	p_stCluster = GLV_NewCluster();
	Iter2Counter = 0;
	Topotolerance = 0.01f;
	Colortolerance = 2.0f * (float)(1 << OptimizeLVL);
	
	for (IterCounter = 0 ; IterCounter < GLV_LOOP_OPT ; IterCounter++)
	{
		u32 ulNumberOfFacesBefore;
		sprintf(Text , "Optimize (%d faces / %d at start) => %d %%" , p_stGLV->ulNumberOfFaces , Total , (ULONG)(100.0f - 100.0f * (float)p_stGLV->ulNumberOfFaces / (float)Total));
		SeprogressPos((float)IterCounter / (float)GLV_LOOP_OPT , Text);
		FLG = 0;

		
		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter++)
		{
			p_stGLV -> p_stFaces [ Counter ].ulSurfaceNumber = 0;
		}

		ulNumberOfFacesBefore = p_stGLV->ulNumberOfFaces;
		GLV_BreakUncompatibleLinks_JadeElement_and_UV(p_stGLV);
		GLD_RemoveZeroSurfaces  ( p_stGLV );
		GLD_RemoveIllegalFaces  ( p_stGLV );
		GLD_RemoveUnusedIndexes ( p_stGLV );
		GLV_ComputeNghbr(p_stGLV);
		L_memset(p_PPTNum , 0 , p_stGLV->ulNumberOfPoints * 4L);
		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter++)
		{
			p_stGLV->p_stFaces [ Counter ] .Colours [ 0 ] = p_stGLV->p_stPoints[p_stGLV->p_stFaces [ Counter ] . Index [ 0 ] ] .ulColor;
			p_stGLV->p_stFaces [ Counter ] .Colours [ 1 ] = p_stGLV->p_stPoints[p_stGLV->p_stFaces [ Counter ] . Index [ 1 ] ] .ulColor;
			p_stGLV->p_stFaces [ Counter ] .Colours [ 2 ] = p_stGLV->p_stPoints[p_stGLV->p_stFaces [ Counter ] . Index [ 2 ] ] .ulColor;
		}
		/* Weld TOTAL !! */
		for (Counter = 0 , Counter2 = 0; Counter < p_stGLV->ulNumberOfPoints; Counter++)
		{
			p_stGLV->p_stPoints[Counter2].ulFlags &= ~GLV_FreeDomDegree;
		}
		GLV_ComputeNghbr(p_stGLV);
//		GLD_Weld( p_stGLV , GLV_WELD_FCT , 0); 

		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces; Counter++)
		{
			p_stGLV->p_stFaces[Counter].ulFlags &= ~(GLV_FLGS_RSV1|GLV_FLGS_DeleteIt);
			p_PPTNum[p_stGLV->p_stFaces[Counter].Index[0]]++;
			p_PPTNum[p_stGLV->p_stFaces[Counter].Index[1]]++;
			p_PPTNum[p_stGLV->p_stFaces[Counter].Index[2]]++;
		}

		for (Counter = 0 , Counter2 = 0; Counter < p_stGLV->ulNumberOfPoints; Counter++)
		{
			p_stGLV->p_stPoints[Counter].ulFlags &= ~GLV_FLGS_RSV1;
			p_PPTBase [ Counter ] = Counter2;
			Counter2 += p_PPTNum [ Counter ];
			p_PPTNum [ Counter ] = 0;
		}

		if (p_PPT_ENum)
			p_PPT_ENum = (ULONG*)GLV_REALLOC(p_PPT_ENum , Counter2 * 4L);
		else
			p_PPT_ENum = (ULONG*)GLV_ALLOC(Counter2 * 4L);

		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfFaces ; Counter++)
		{
			p_PPT_ENum [ p_PPTBase [ p_stGLV->p_stFaces [ Counter ] . Index [ 0 ] ] + p_PPTNum [ p_stGLV->p_stFaces [ Counter ] .Index [ 0 ] ] ++ ] = Counter;
			p_PPT_ENum [ p_PPTBase [ p_stGLV->p_stFaces [ Counter ] . Index [ 1 ] ] + p_PPTNum [ p_stGLV->p_stFaces [ Counter ] .Index [ 1 ] ] ++ ] = Counter;
			p_PPT_ENum [ p_PPTBase [ p_stGLV->p_stFaces [ Counter ] . Index [ 2 ] ] + p_PPTNum [ p_stGLV->p_stFaces [ Counter ] .Index [ 2 ] ] ++ ] = Counter;
		}
		
		ulNumberOfOperation = 0;
		/* detect Merge operation */
		for (Counter = 0 ; Counter < p_stGLV->ulNumberOfPoints; Counter++)
		{
//			if (!(p_stGLV->p_stPoints[Counter].ulFlags & 0X40000000)) goto Next_Source;
			GLV_ResetCluster(p_stCluster);
			for (Counter2 = 0 ; Counter2 < p_PPTNum [Counter] ; Counter2 ++)
			{
				if (p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].Index[0] != Counter) GLV_Clst_ADDV(p_stCluster , p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].Index[0]);
				if (p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].Index[1] != Counter) GLV_Clst_ADDV(p_stCluster , p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].Index[1]);
				if (p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].Index[2] != Counter) GLV_Clst_ADDV(p_stCluster , p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].Index[2]);
			}
			GLV_Enum_Cluster_Value(p_stCluster) /* ENUM TARGETS */
			{
				ULONG Counter3;
				GLV_OPT_ScalarPoint stS[8];
				ULONG ulNumberOFScalarPoints;
				ulNumberOFScalarPoints = 0;
				for (Counter2 = 0 ; Counter2 < p_PPTNum [Counter] ; Counter2 ++)
				{
					for (Counter3 = 0 ; Counter3 < 3 ; Counter3 ++ )
					{
						if (p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].Index[Counter3] == MCL_Value)
						{
							stS[ulNumberOFScalarPoints].ulNewIndex	= MCL_Value;
							stS[ulNumberOFScalarPoints].NewColor	= p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].Colours[Counter3];
							stS[ulNumberOFScalarPoints].NewNormale	= p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].OriginalPointsNormales[Counter3];
							stS[ulNumberOFScalarPoints].NewU		= p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].UV[(Counter3 << 1) + 0];
							stS[ulNumberOFScalarPoints].NewV		= p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].UV[(Counter3 << 1) + 1];
							stS[ulNumberOFScalarPoints].NewAlpha	= p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[Counter] + Counter2]].falpha[Counter3];
							if (ulNumberOFScalarPoints == GLV_MAX_NUMBER_OF_SCALAR_POINTS)
								// BUG!
								goto Next_Target;
							ulNumberOFScalarPoints++;
						}
					}
				}//*/
				Error = 0.0f;
 				for (Counter2 = 0 ; Counter2 < p_PPTNum [Counter] ; Counter2 ++)
				{
					if (!GLV_AcceptMerge(p_stGLV , p_PPT_ENum [p_PPTBase[Counter] + Counter2] , Counter , MCL_Value ,stS ,ulNumberOFScalarPoints, &Error))
						goto Next_Target;
				}
				/* Operation has been accepted by all the triangle -> Add the operation */
				if (ulMAXOperationNum == ulNumberOfOperation)
				{
					ulMAXOperationNum += 1024;
					p_AllOperations = (GLV_OPT_Operation*)GLV_REALLOC(p_AllOperations , ulMAXOperationNum * sizeof(GLV_OPT_Operation));
				}
				p_AllOperations[ulNumberOfOperation].Dest = MCL_Value;
				p_AllOperations[ulNumberOfOperation].Source = Counter;
				p_AllOperations[ulNumberOfOperation].ulNumberOfScalar = ulNumberOFScalarPoints;
				while (ulNumberOFScalarPoints--)
					p_AllOperations[ulNumberOfOperation].stS[ulNumberOFScalarPoints] = stS[ulNumberOFScalarPoints];
				p_AllOperations[ulNumberOfOperation].Error = Error;
				ulNumberOfOperation ++;

Next_Target:
				Counter2 = Counter2 ; /* Because salebugalacon*/
				/* Continue with next target */
			}
			GLV_Enum_Cluster_Value_End()
					/* All as been tested -> Merge */
		}
		/* Merge */
		for (Counter = 0 ; Counter < ulNumberOfOperation ; Counter ++)
		{
			//VERIFY
			for (Counter2 = 0 ; Counter2 < p_PPTNum [p_AllOperations[Counter].Source] ; Counter2 ++)
			{
				u32 ulNumberOFScalarPoints ;
				ulNumberOFScalarPoints = p_AllOperations[Counter].ulNumberOfScalar;
				while (ulNumberOFScalarPoints--)
				{
					if (GLV_OPT_ProjectScalar(p_stGLV , p_PPT_ENum [p_PPTBase[p_AllOperations[Counter].Source] + Counter2] , p_AllOperations[Counter].Source , p_AllOperations[Counter].Dest, &p_AllOperations[Counter].stS[ulNumberOFScalarPoints] , 1.0 ))
						goto ScalarIsAproved4;
				}
			}
			ulNumberOfOperation = ulNumberOfOperation; // Bug;
ScalarIsAproved4:
			ulNumberOfOperation = ulNumberOfOperation; // Bug;
		}

		/* quicksort operations */
#ifndef GLV_FAST_OPTIMIZE
		qsort ( (void *) p_AllOperations , ulNumberOfOperation , sizeof ( GLV_OPT_Operation ) , GLV_QSortOperationsClabk );
#endif
		/* Merge */
		for (Counter = 0 ; Counter < ulNumberOfOperation ; Counter ++)
		{
			FLG |= 1;
			{
				/* Already Merged ? */
				if (p_stGLV->p_stPoints[p_AllOperations[Counter].Dest].ulFlags & GLV_FLGS_RSV1) goto Next_Source2;
				if (p_stGLV->p_stPoints[p_AllOperations[Counter].Source].ulFlags & GLV_FLGS_RSV1) goto Next_Source2;//*/
			}
			for (Counter2 = 0 ; Counter2 < p_PPTNum [p_AllOperations[Counter].Source] ; Counter2 ++)
			{
				if (p_stGLV->p_stFaces[p_PPT_ENum [p_PPTBase[p_AllOperations[Counter].Source] + Counter2]].ulFlags & GLV_FLGS_RSV1) goto Next_Source2;//*/
			}
			
			for (Counter2 = 0 ; Counter2 < p_PPTNum [p_AllOperations[Counter].Source] ; Counter2 ++)
			{
				u32 ulNumberOFScalarPoints ,Pass;
				GLV_Scalar GLV_BigestColorError;
				ulNumberOFScalarPoints = p_AllOperations[Counter].ulNumberOfScalar;
				Pass = 0xffffffff;
				GLV_BigestColorError = 100000000000000000.0;
				while (ulNumberOFScalarPoints--)
				{
					if (GLV_OPT_ProjectScalar(p_stGLV , p_PPT_ENum [p_PPTBase[p_AllOperations[Counter].Source] + Counter2] , p_AllOperations[Counter].Source , p_AllOperations[Counter].Dest, &p_AllOperations[Counter].stS[ulNumberOFScalarPoints] , 1.0 ))
					{
						if (GLV_LastColorError < GLV_BigestColorError)
						{
							GLV_BigestColorError = GLV_LastColorError;
							Pass  = ulNumberOFScalarPoints;
						}
					}
				}
				if (Pass == 0xffffffff)
					Pass = Pass; // BUG
				else
					GLV_Merge(p_stGLV , p_PPT_ENum [p_PPTBase[p_AllOperations[Counter].Source] + Counter2] , p_AllOperations[Counter].Source , p_AllOperations[Counter].Dest , &p_AllOperations[Counter].stS[Pass]);

			}
			/* Continue with next target */
Next_Source2:
			Counter2 = Counter2 ; /* Because salebugalacon*/
		}

		
		GLD_Remove2BD(p_stGLV);	

		if ((!ulNumberOfOperation) /*|| (ulNumberOfFacesBefore != p_stGLV->ulNumberOfFaces)*/)
		{
			IterCounter = GLV_LOOP_OPT;
		}
	}
	GLV_BreakUncompatibleLinks_JadeElement_and_UV(p_stGLV);

	GLD_RemoveZeroSurfaces  ( p_stGLV );
	GLD_RemoveIllegalFaces  ( p_stGLV );
	GLD_RemoveUnusedIndexes ( p_stGLV );
	
	{
		char TimeSpent[1024];
		sprintf(TimeSpent , "%d faces optimized (%d %%)" , Total - p_stGLV->ulNumberOfFaces , (ULONG)(100.0f * (float)(Total - p_stGLV->ulNumberOfFaces) / (float)Total));
		GLV_BUG(0 , TimeSpent);
	}
	GLV_FREE(p_AllOperations);
	GLV_FREE(p_PPTNum );
	GLV_FREE(p_PPT_ENum );
	GLV_FREE(p_PPTBase);
	GLV_DelCluster(p_stCluster);
}	

#endif
