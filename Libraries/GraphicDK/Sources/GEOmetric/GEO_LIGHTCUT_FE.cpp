/* GEO_LIGHTCUT_FE.c */

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
extern ULONG ulPolylineCounter;

#ifdef JADEFUSION
extern ULONG GLV_RT_GetIsHitChannel(tdst_GLV *p_stGLV , MATHD_tdst_Vector *p_P1, MATHD_tdst_Vector *p_P2 , tdst_GLV_Face *p_F , GLV_OCtreeNode *p_stOctreeROOT , GLV_Cluster *p_ClusterToFill , ULONG Channel);
#endif

_inline_ void MATH_NormalizeVector2(MATH_tdst_Vector *Dest, MATH_tdst_Vector *A)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float   fInvNorm;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    fInvNorm = MATH_f_InvNormVector(A);
    Dest->x = fMul(A->x, fInvNorm);
    Dest->y = fMul(A->y, fInvNorm);
    Dest->z = fMul(A->z, fInvNorm);
}

void GLV_ULto4Scalar(ULONG Col, GLV_Scalar *p4)
{
	p4[0] = (GLV_Scalar)((Col >> 24L) & 0xff);
	p4[1] = (GLV_Scalar)((Col >> 16L) & 0xff);
	p4[2] = (GLV_Scalar)((Col >> 8L) & 0xff);
	p4[3] = (GLV_Scalar)((Col >> 0L) & 0xff);
}

ULONG GLV_4ScalarToUL( GLV_Scalar *p4)
{
	ULONG Counter;
	ULONG Col;
	for (Counter = 0 ; Counter < 4 ; Counter ++)
	{
		if (p4[Counter] <= 0.0f) p4[Counter] = 0.0f;
		if (p4[Counter] >= 255.0f) p4[Counter] = 255.0f;
	}
	Col  = ((ULONG)p4[0]) << 24L;
	Col |= ((ULONG)p4[1]) << 16L;
	Col |= ((ULONG)p4[2]) << 8L;
	Col |= ((ULONG)p4[3]) << 0L;
	return Col;
}
void GLV_TurnFace(tdst_GLV *p_stGLV , ULONG FaceNum , ULONG K)
{
	tdst_GLV_Face OldFace; 
	ULONG Counter , I2;
	OldFace = p_stGLV ->p_stFaces[FaceNum];
	for (Counter = 0 ; Counter < 3 ; Counter ++)
	{
		I2 = (Counter + K) % 3;
		p_stGLV ->p_stFaces[FaceNum].Index[Counter] = OldFace.Index[I2];
		p_stGLV ->p_stFaces[FaceNum].Nghbr[Counter] = OldFace.Nghbr[I2];
		p_stGLV ->p_stFaces[FaceNum].Colours[Counter] = OldFace.Colours[I2];
		p_stGLV ->p_stFaces[FaceNum].falpha[Counter] = OldFace.falpha[I2];
		p_stGLV ->p_stFaces[FaceNum].OriginalPointsNormales[Counter] = OldFace.OriginalPointsNormales[I2];
		p_stGLV ->p_stFaces[FaceNum].UV[(Counter << 1) + 0] = OldFace.UV[(I2 << 1) + 0];
		p_stGLV ->p_stFaces[FaceNum].UV[(Counter << 1) + 1] = OldFace.UV[(I2 << 1) + 1];
	}
}
ULONG GLV_GetLightAngle(tdst_GLV *p_stGLV , ULONG I1 , ULONG I2 , ULONG LightNum)
{
	MATHD_tdst_Vector St1 , St2 , StCP; 
	GLV_Scalar Angle;
	MATHD_SubVector(&St1 , &p_stGLV->p_stPoints[I1].P3D , &p_stGLV->p_Lights[LightNum].stLightCenterOS);
	MATHD_SubVector(&St2 , &p_stGLV->p_stPoints[I2].P3D , &p_stGLV->p_Lights[LightNum].stLightCenterOS);
	Angle = MATHD_f_DotProduct(&St1 , &St2);
	if (Angle < 0.0f) return 1;
	MATHD_NormalizeVector(&St1 , &St1);
	MATHD_NormalizeVector(&St2 , &St2);
	MATHD_CrossProduct( &StCP , &St1 , &St2);
	Angle = MATHD_f_NormVector(&StCP);
	if (Angle > 0.3f)
		return 1;
	else
		return 0;
}
ULONG GLV_GetSquareFlag(tdst_GLV *p_stGLV , ULONG FaceNum )
{
	MATHD_tdst_Vector St1 ; 
	GLV_Scalar fLenghts[3];
	ULONG Max,Min;
	MATHD_SubVector(&St1  , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[FaceNum].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[FaceNum].Index[1]].P3D);
	fLenghts[0] = MATHD_f_NormVector(&St1);
	MATHD_SubVector(&St1  , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[FaceNum].Index[1]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[FaceNum].Index[2]].P3D);
	fLenghts[1] = MATHD_f_NormVector(&St1);
	MATHD_SubVector(&St1  , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[FaceNum].Index[2]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[FaceNum].Index[0]].P3D);
	fLenghts[2] = MATHD_f_NormVector(&St1);
	if (fLenghts[0] > fLenghts[1]) 
	{
		Max = 0;
		Min = 1;
	} else
	{
		Max = 1;
		Min = 0;
	}
	if (fLenghts[2] > fLenghts[Max]) 
	{
		Max = 2;
	}
	if (fLenghts[2] < fLenghts[Min]) 
	{
		Min = 2;
	}
	if ((fLenghts[Min] / fLenghts[Max]) < 0.2f) 
	{
		return ((1<<Min) ^ 7);
	}
	return 0;
}
ULONG GLV_RefineGetFlags(tdst_GLV *p_stGLV , ULONG FaceNum , ULONG LightNum)
{
	ULONG Flags;
	Flags = 0;
	if (LightNum != -1)
	{
		if (GLV_GetLightAngle(p_stGLV , p_stGLV->p_stFaces[FaceNum].Index[0] , p_stGLV->p_stFaces[FaceNum].Index[1] , LightNum)) Flags |= 1;
		if (GLV_GetLightAngle(p_stGLV , p_stGLV->p_stFaces[FaceNum].Index[1] , p_stGLV->p_stFaces[FaceNum].Index[2] , LightNum)) Flags |= 2;
		if (GLV_GetLightAngle(p_stGLV , p_stGLV->p_stFaces[FaceNum].Index[2] , p_stGLV->p_stFaces[FaceNum].Index[0] , LightNum)) Flags |= 4;
	} else
	{ /* "Squarify" mesh */
		return GLV_GetSquareFlag(p_stGLV , FaceNum);
	}
/*	if (Flags) return 7;
	return 0;//*/
	return 	(Flags);
}

void GLV_CopyScalar(tdst_GLV_Face *pFaceD , tdst_GLV_Face *pFaceS , ULONG ID ,  ULONG IS)
{
	pFaceD->Index[ID]			= pFaceS->Index[IS];
	pFaceD->UV[(ID << 1) + 0]	= pFaceS->UV[(IS << 1) + 0];
	pFaceD->UV[(ID << 1) + 1]	= pFaceS->UV[(IS << 1) + 1];
	pFaceD->OriginalPointsNormales[ID]	= pFaceS->OriginalPointsNormales[IS];
	pFaceD->Colours[ID]	= pFaceS->Colours[IS];
	pFaceD->falpha[ID]	= pFaceS->falpha[IS];
}

void GLV_ComputeMiddleFace(tdst_GLV *p_stGLV , ULONG FaceNum , tdst_GLV_Face *pstMiddleFace , ULONG bNormalFace)
{
	tdst_GLV_Face *pFace; 
	ULONG Counter;
	pFace = p_stGLV -> p_stFaces + FaceNum ; 
	GLV_SetNumbers(p_stGLV , p_stGLV ->ulNumberOfPoints + 3 , 0 , 0 , 1);
	for (Counter = 0 ; Counter < 3 ; Counter ++)
	{
		pstMiddleFace->Colours[Counter] = ((pFace->Colours[Counter] >> 1) & 0x7f7f7f7f) + ((pFace->Colours[(Counter + 1) % 3] >> 1) & 0x7f7f7f7f);
		pstMiddleFace->falpha[Counter] = (pFace->falpha[Counter] + pFace->falpha[(Counter + 1) % 3]) * 0.5f;
		pstMiddleFace->UV[Counter << 1] = (pFace->UV[Counter << 1] + pFace->UV[(( Counter + 1 ) % 3 ) << 1]) * 0.5;
		pstMiddleFace->UV[(Counter << 1) + 1] = (pFace->UV[(Counter << 1) + 1] + pFace->UV[((( Counter + 1 ) % 3 ) << 1) + 1]) * 0.5;
		if (bNormalFace)
		{
			MATHD_To_MATH(&pstMiddleFace->OriginalPointsNormales[Counter] , &pFace->Plane.Normale);
		} else
		{
			MATH_BlendVector ( &pstMiddleFace->OriginalPointsNormales[Counter] , &pFace->OriginalPointsNormales[Counter] , &pFace->OriginalPointsNormales[(Counter + 1 )% 3] , 0.5f ) ;
			MATH_NormalizeVector2(&pstMiddleFace->OriginalPointsNormales[Counter] , &pstMiddleFace->OriginalPointsNormales[Counter] );
		}
		pstMiddleFace->Index[Counter] = p_stGLV->ulNumberOfPoints - Counter - 1;
		//p_stGLV ->p_stPoints[p_stGLV->ulNumberOfPoints - Counter - 1] = p_stGLV ->p_stPoints[pFace->Index[Counter]];
		MATHD_BlendVector(&p_stGLV ->p_stPoints[p_stGLV->ulNumberOfPoints - Counter - 1].P3D , 
						  &p_stGLV ->p_stPoints[pFace->Index[Counter]].P3D , 
						  &p_stGLV ->p_stPoints[pFace->Index[(Counter + 1 ) % 3 ]].P3D ,
						  0.5 );
		p_stGLV ->p_stPoints[p_stGLV->ulNumberOfPoints - Counter - 1].ulColor =  
						  ((p_stGLV ->p_stPoints[pFace->Index[Counter]].ulColor >> 1) & 0x7f7f7f7f) +
						  ((p_stGLV ->p_stPoints[pFace->Index[(Counter + 1 ) % 3 ]].ulColor >> 1) & 0x7f7f7f7f) ;
		
	}
}
void GLV_UnifySubdivisionFlags(tdst_GLV *p_stGLV)
{
	ULONG Counter , OtherFace , C2;
	for (Counter = 0 ;Counter < p_stGLV -> ulNumberOfFaces ; Counter ++)
	{
		for (C2 = 0 ;C2 < 3 ; C2++)
		{
			if (p_stGLV->p_stFaces[Counter].ulSubdivideFlags & (1<<C2)) 
			{
				if (p_stGLV->p_stFaces[Counter].Nghbr[C2] < 0xffff)
				{
					OtherFace = p_stGLV->p_stFaces[Counter].Nghbr[C2];
					if (p_stGLV->p_stFaces[OtherFace].Nghbr[0] == Counter) p_stGLV->p_stFaces[OtherFace].ulSubdivideFlags |= 1;
					if (p_stGLV->p_stFaces[OtherFace].Nghbr[1] == Counter) p_stGLV->p_stFaces[OtherFace].ulSubdivideFlags |= 2;
					if (p_stGLV->p_stFaces[OtherFace].Nghbr[2] == Counter) p_stGLV->p_stFaces[OtherFace].ulSubdivideFlags |= 4;
				}
			}
		}
	}
}

ULONG GLV_CutAFaceWithFlags(tdst_GLV *p_stGLV , ULONG Counter , ULONG Flag , ULONG bNormalFace)
{
	ULONG FLG;
	tdst_GLV_Face stMiddleFace;
	switch (Flag)
	{
		case 0:return 0;
		case 1:
			Flag = 1;
			break;
		case 2:
			GLV_TurnFace(p_stGLV , Counter , 1);
			Flag = 1;
			break;
		case 3:
			Flag = 2;
			break;
		case 4:
			GLV_TurnFace(p_stGLV , Counter , 2);
			Flag = 1;
			break;
		case 5:
			GLV_TurnFace(p_stGLV , Counter , 2);
			Flag = 2;
			break;
		case 6:
			GLV_TurnFace(p_stGLV , Counter , 1);
			Flag = 2;
			break;
		case 7:
			Flag = 3;
			break;
	}
	FLG = 0;
	
	switch (Flag)
	{
	case 0:break; /* do nothing */
	case 1:/* Cut edge 0 */
		FLG = 1;
		GLV_ComputeMiddleFace ( p_stGLV , Counter , &stMiddleFace , bNormalFace);
		GLV_SetNumbers(p_stGLV , 0 , 0 , p_stGLV ->ulNumberOfFaces + 1 , 4);
		p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 1 ] = p_stGLV->p_stFaces[Counter];
		GLV_CopyScalar( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 1 ] , &stMiddleFace , 0 ,  0);
		GLV_CopyScalar( &p_stGLV -> p_stFaces [ Counter						 ] , &stMiddleFace , 1 ,  0);
		break; 
	case 2:/* Cut edge 0 & 1 */
		FLG = 1;
		GLV_ComputeMiddleFace ( p_stGLV , Counter , &stMiddleFace , bNormalFace);
		GLV_SetNumbers(p_stGLV , 0 , 0 , p_stGLV ->ulNumberOfFaces + 2 , 4);
		p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 1 ] = p_stGLV->p_stFaces[Counter];
		p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 2 ] = p_stGLV->p_stFaces[Counter];
		GLV_CopyScalar( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 2 ] , &stMiddleFace , 0 ,  0);
		GLV_CopyScalar( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 2 ] , &stMiddleFace , 2 ,  1);
		GLV_CopyScalar( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 1 ] , &stMiddleFace , 1 ,  0);
		GLV_CopyScalar( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 1 ] , &stMiddleFace , 2 ,  1);
		GLV_CopyScalar( &p_stGLV -> p_stFaces [ Counter						  ] , &stMiddleFace , 1 ,  1);
		break; 
	case 3:/* Cut edge 1,2 & 3 */
		FLG = 1;
		GLV_ComputeMiddleFace ( p_stGLV , Counter , &stMiddleFace , bNormalFace);
		GLV_SetNumbers(p_stGLV , 0 , 0 , p_stGLV ->ulNumberOfFaces + 3 , 4);
		p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 1 ] = p_stGLV->p_stFaces[Counter];
		p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 2 ] = p_stGLV->p_stFaces[Counter];
		p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 3 ] = p_stGLV->p_stFaces[Counter];
		GLV_CopyScalar ( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 3 ] , &stMiddleFace , 1 ,  0);
		GLV_CopyScalar ( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 3 ] , &stMiddleFace , 2 ,  2);
		GLV_CopyScalar ( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 2 ] , &stMiddleFace , 0 ,  0);
		GLV_CopyScalar ( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 2 ] , &stMiddleFace , 2 ,  1);
		GLV_CopyScalar ( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 1 ] , &stMiddleFace , 1 ,  1);
		GLV_CopyScalar ( &p_stGLV -> p_stFaces [ p_stGLV ->ulNumberOfFaces - 1 ] , &stMiddleFace , 0 ,  2);
		GLV_CopyScalar ( &p_stGLV -> p_stFaces [ Counter						  ] , &stMiddleFace , 0 ,  0);
		GLV_CopyScalar ( &p_stGLV -> p_stFaces [ Counter						  ] , &stMiddleFace , 1 ,  1);
		GLV_CopyScalar ( &p_stGLV -> p_stFaces [ Counter						  ] , &stMiddleFace , 2 ,  2);
		break; 
	}
	return FLG;
}

void GLV_Subdivide(tdst_GLV *p_stGLV , ULONG KeepItSquare)//
{
	ULONG Counter , NF;
	NF = p_stGLV -> ulNumberOfFaces;
	if (KeepItSquare)
	{
		GLV_UnifySubdivisionFlags(p_stGLV);
		for (Counter = 0 ;Counter < NF ; Counter ++)
		{
			if (p_stGLV->p_stFaces[Counter].ulSubdivideFlags == 7)
			{
				GLV_Scalar L[3],MaxLenght;
				MATHD_tdst_Vector St1 ; 
				MATHD_SubVector( &St1  , &(p_stGLV->p_stPoints + p_stGLV->p_stFaces[Counter].Index[1])->P3D , &(p_stGLV->p_stPoints + p_stGLV->p_stFaces[Counter].Index[0])->P3D );
				L[0] = MATHD_f_NormVector(&St1  );
				MATHD_SubVector( &St1  , &(p_stGLV->p_stPoints + p_stGLV->p_stFaces[Counter].Index[2])->P3D , &(p_stGLV->p_stPoints + p_stGLV->p_stFaces[Counter].Index[1])->P3D );
				L[1] = MATHD_f_NormVector(&St1  );
				MATHD_SubVector( &St1  , &(p_stGLV->p_stPoints + p_stGLV->p_stFaces[Counter].Index[0])->P3D , &(p_stGLV->p_stPoints + p_stGLV->p_stFaces[Counter].Index[2])->P3D );
				L[2] = MATHD_f_NormVector(&St1  );
				if (L[1] > L[0])
					MaxLenght = L[1];
				else
					MaxLenght = L[0];
				if (L[2] > MaxLenght )
					MaxLenght = L[2];
				if (MaxLenght > 1.33 * L[0]) p_stGLV->p_stFaces[Counter].ulSubdivideFlags &= ~1;
				if (MaxLenght > 1.33 * L[1]) p_stGLV->p_stFaces[Counter].ulSubdivideFlags &= ~2;
				if (MaxLenght > 1.33 * L[2]) p_stGLV->p_stFaces[Counter].ulSubdivideFlags &= ~4;
				if (MaxLenght > 8.00 * L[0]) p_stGLV->p_stFaces[Counter].ulSubdivideFlags |= 2 + 4;
				if (MaxLenght > 8.00 * L[1]) p_stGLV->p_stFaces[Counter].ulSubdivideFlags |= 1 + 4;
				if (MaxLenght > 8.00 * L[2]) p_stGLV->p_stFaces[Counter].ulSubdivideFlags |= 1 + 2;
			}
		}
	}

	GLV_UnifySubdivisionFlags(p_stGLV);
	for (Counter = 0 ;Counter < NF ; Counter ++)
	{
		GLV_CutAFaceWithFlags(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].ulSubdivideFlags & 0x7, 0);
	}
	for (Counter = 0 ;Counter < p_stGLV -> ulNumberOfFaces ; Counter ++)
	{
		p_stGLV->p_stFaces[Counter].ulSubdivideFlags = 0;
	}
}

void GLV_SubdivideLenght(tdst_GLV *p_stGLV , float fLenghtMax)//
{
	ULONG Counter , NF , FLG;
	MATHD_tdst_Vector St1 ; 
	fLenghtMax *= fLenghtMax;
	FLG = 1;
	while (FLG)
	{
		FLG = 0;
//		GLD_Weld( p_stGLV , GLV_WELD_FCT , 0 );
//		GLV_ComputeNghbr(p_stGLV);
		NF = p_stGLV -> ulNumberOfFaces;
		for (Counter = 0 ;Counter < NF ; Counter ++)
		{
			p_stGLV->p_stFaces[Counter].ulSubdivideFlags = 0;
//			if ((p_stGLV->p_stFaces[Counter].ulFlags & GLV_FLGS_Original) && (p_stGLV->p_stFaces[Counter].ulMARK & 0x40000000))
			{
				{
//					if (!(p_stGLV->p_stFaces[Counter].ulFlags  &  GLV_FLGS_DoNotCut))
					{
//						GLV_CutAFaceWithFlags(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].ulSubdivideFlags , 1);
						MATHD_SubVector(&St1, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[0]].P3D, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[1]].P3D);
						if (MATHD_f_SqrNormVector(&St1) > fLenghtMax) 
							p_stGLV->p_stFaces[Counter].ulSubdivideFlags |= 1;
						MATHD_SubVector(&St1, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[1]].P3D, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[2]].P3D);
						if (MATHD_f_SqrNormVector(&St1) > fLenghtMax) 
							p_stGLV->p_stFaces[Counter].ulSubdivideFlags |= 2;
						MATHD_SubVector(&St1, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[2]].P3D, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[Counter].Index[0]].P3D);
						if (MATHD_f_SqrNormVector(&St1) > fLenghtMax) 
							p_stGLV->p_stFaces[Counter].ulSubdivideFlags |= 4;
					}
				}
			}
		}
//		GLV_UnifySubdivisionFlags(p_stGLV);
		for (Counter = 0 ;Counter < NF ; Counter ++)
		{
			FLG |= GLV_CutAFaceWithFlags(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].ulSubdivideFlags , 1);
		}
	}
//	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0);
//	GLV_ComputeNghbr(p_stGLV);
}

void GLV_RefineWithAngleSpherical(tdst_GLV *p_stGLV , ULONG LightNum)
{
	ULONG FLG, Counter , Local , NF;
	FLG = 1;
	if ((p_stGLV->p_Lights[LightNum].ulLightFlags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct) return;
	while (FLG)
	{
		NF = p_stGLV -> ulNumberOfFaces;
		FLG = 0;
		for (Counter = 0 ;Counter < NF ; Counter ++)
		{
			if (p_stGLV->p_stFaces[Counter].ulChannel & p_stGLV->p_Lights[LightNum].ulChannel)
			{
				if ((p_stGLV->p_stFaces[Counter].ulFlags & GLV_FLGS_Original) && (p_stGLV->p_stFaces[Counter].ulMARK & 0x40000000))
				{
					if (p_stGLV->p_stFaces[Counter].ulFlags & GLV_FLGS_WhiteFront)
					{
						if (!(p_stGLV->p_stFaces[Counter].ulFlags  &  GLV_FLGS_DoNotCut))
						{
							Local = GLV_RefineGetFlags(p_stGLV , Counter , LightNum  ) ;
							FLG |= GLV_CutAFaceWithFlags(p_stGLV , Counter , Local , 0);
						}
					}
				}
			}
		}
	}
}

void GLV_FirstRefine(tdst_GLV *p_stGLV , ULONG LightNum) /* if lightnum == -1 this function will "squarify" the mesh*/
{
	ULONG FLG, Counter , Counter2 , NF;
	FLG = 1;
	if (LightNum != -1)
	{
		if ((p_stGLV->p_Lights[LightNum].ulLightFlags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct) return;
	}
	Counter2 = 0;
	while (FLG)
	{
		NF = p_stGLV -> ulNumberOfFaces;
		FLG = 0;
		for (Counter = 0 ;Counter < NF ; Counter ++)
		{
			p_stGLV->p_stFaces[Counter].ulSubdivideFlags = 0;
		}
		GLD_Weld( p_stGLV , GLV_WELD_FCT , 0 );
		GLV_ComputeNghbr(p_stGLV);
		for (Counter = 0 ;Counter < NF ; Counter ++)
		{
			if ((p_stGLV->p_stFaces[Counter].ulFlags & GLV_FLGS_Original) && (p_stGLV->p_stFaces[Counter].ulMARK & 0x40000000))
			{
				p_stGLV->p_stFaces[Counter].ulSubdivideFlags = GLV_RefineGetFlags(p_stGLV , Counter , LightNum  ) ;
			}
		}
		GLV_UnifySubdivisionFlags(p_stGLV);
		for (Counter = 0 ; Counter < NF ; Counter ++)
		{
			if ((p_stGLV->p_stFaces[Counter].ulFlags & GLV_FLGS_Original) && (p_stGLV->p_stFaces[Counter].ulMARK & 0x40000000))
			{
				if (p_stGLV->p_stFaces[Counter].ulSubdivideFlags)
					FLG |= GLV_CutAFaceWithFlags(p_stGLV , Counter , p_stGLV->p_stFaces[Counter].ulSubdivideFlags , 0);
			}
		}
		Counter2 ++;
		if (Counter2 >= 5) return;
	}
}


#define GLF_FE_PrecMetric10 GLF_PrecMetric
ULONG GLV_f_IsPointOnEdge( MATHD_tdst_Vector *p_P1 , MATHD_tdst_Vector *p_P2 , MATHD_tdst_Vector *p_P)
{
	MATHD_tdst_Vector stP12 , stP1P , stPP;
	MATHD_tdst_Vector stP1 , stP2 , stP;
	GLV_Scalar Dist,fLen;
	MATHD_InitVector(&stP1 , 0.0f , 0.0f , 0.0f );
	MATHD_SubVector(&stP2 , p_P2 , p_P1);
	MATHD_SubVector(&stP , p_P , p_P1);
	/* */
	MATHD_SubVector(&stP12 , &stP2 , &stP1);
	MATHD_SubVector(&stP1P , &stP , &stP1);
	fLen  = MATHD_f_NormVector(&stP12);
	Dist = MATHD_f_DotProduct(&stP12 , &stP1P) / fLen;
	if (Dist < GLF_FE_PrecMetric10) return 0;
	if (Dist > fLen - GLF_FE_PrecMetric10) return 0;
	MATHD_AddScaleVector(&stPP , &stP1 , &stP12 , Dist / fLen);
	MATHD_SubVector(&stPP , &stP , &stPP );
	Dist = MATHD_f_NormVector(&stPP);
	if (Dist < GLF_FE_PrecMetric10)
		return 1;
	return 0;
}


ULONG GLV_IsEdgeOnFace(tdst_GLV *p_stGLV , ULONG F,ULONG I1,ULONG I2)
{
	GLV_Scalar DP,Prec;
	ULONG Count;
	MATHD_tdst_Vector *P_1,*P_2;
	MATHD_tdst_Vector P1,P2;
	Count = 0;
	if (p_stGLV->p_stFaces[F].Index[0] == I1) Count++;
	if (p_stGLV->p_stFaces[F].Index[1] == I1) Count++;
	if (p_stGLV->p_stFaces[F].Index[2] == I1) Count++;
	if (p_stGLV->p_stFaces[F].Index[0] == I2) Count++;
	if (p_stGLV->p_stFaces[F].Index[1] == I2) Count++;
	if (p_stGLV->p_stFaces[F].Index[2] == I2) Count++;
	if (Count >= 2) return 0;
	P1 = p_stGLV->p_stPoints[I1].P3D;
	P2 = p_stGLV->p_stPoints[I2].P3D;
	MATHD_SubVector(&P1 , &P1 , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[0]].P3D);
	MATHD_SubVector(&P2 , &P2 , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[0]].P3D);
	P_1 = &P1;
	P_2 = &P2;
	Prec = GLF_FE_PrecMetric10;
	{
		DP = MATHD_f_DotProduct(P_1 , &p_stGLV->p_stFaces[F].Plane.Normale) /*- p_stGLV->p_stFaces[F].Plane.fDistanceTo0*/;
		if ((DP < Prec) && (DP > -Prec))
		{
			DP = MATHD_f_DotProduct(P_2 , &p_stGLV->p_stFaces[F].Plane.Normale) /*- p_stGLV->p_stFaces[F].Plane.fDistanceTo0*/;
			if ((DP < Prec) && (DP > -Prec))
				return 1;
		}
	}
	return 0;
}
/*
	Retreive final Edges
	GLV Must be cut, culled and "niced"
*/
void GLV_GenerateFinalEdges(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG C1,C2,FLG;
	GLV_Cluster *p_stCluster;
	GLV_OCtreeNode stOctreeROOT;

	L_memset(&stOctreeROOT , 0 , sizeof (GLV_OCtreeNode));
	stOctreeROOT.p_OctreeFather = NULL;
	stOctreeROOT.p_stThisCLT = GLV_NewCluster();
	C1 = p_stGLV->ulNumberOfFaces;
	while (C1--) 
	{
		if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original)
			GLV_Clst_ADDV(stOctreeROOT.p_stThisCLT , C1); /* Add all the original faces */
	}
	GLV_ComputeGLVBox(p_stGLV , &stOctreeROOT.stAxisMin, &stOctreeROOT.stAxisMax , GLV_FLGS_Original);
	SeprogressPos (0.0f, "Build OK3..");
	GLV_Octree_DiveAndCompute( p_stGLV , &stOctreeROOT , GLV_OC3_MAX_DEPTH , GLV_OC3_MAX_FPO, SeprogressPos);
	p_stCluster = GLV_NewCluster();
	/* retreive Edges KILT		-	Original */
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
	{
		if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Kilt)
		{
			for (C2 = 0 ; C2 < 3 ; C2 ++)
			{
				if (p_stGLV->p_stFaces[C1].Nghbr[C2] > 0xffff)
				{
					GLV_SetNumbers(p_stGLV , 0 , p_stGLV->ulNumberOfEdges + 1 , 0 , 2);
					p_stGLV->p_stEdges[p_stGLV->ulNumberOfEdges - 1].Index[0] = p_stGLV->p_stFaces[C1].Index[C2];
					p_stGLV->p_stEdges[p_stGLV->ulNumberOfEdges - 1].Index[1] = p_stGLV->p_stFaces[C1].Index[(C2 + 1) % 3];
					p_stGLV->p_stEdges[p_stGLV->ulNumberOfEdges - 1].ulFlags = p_stGLV->p_stFaces[C1].ulFlags & (GLV_FLGS_WhiteFront|GLV_FLGS_BlackFront);
					GLV_ResetCluster(p_stCluster);
					GLV_Octree_DiveAndIntesectLine( &stOctreeROOT , 
						&p_stGLV->p_stPoints[p_stGLV->p_stEdges[p_stGLV->ulNumberOfEdges - 1].Index[0]].P3D , 
						&p_stGLV->p_stPoints[p_stGLV->p_stEdges[p_stGLV->ulNumberOfEdges - 1].Index[1]].P3D , p_stCluster);
					FLG = 0;
					GLV_Enum_Cluster_Value(p_stCluster)
						if (GLV_IsEdgeOnFace(p_stGLV , 
											 MCL_Value,
											 p_stGLV->p_stEdges[p_stGLV->ulNumberOfEdges - 1].Index[0], 
											 p_stGLV->p_stEdges[p_stGLV->ulNumberOfEdges - 1].Index[1]))
											 FLG = 1;
					GLV_Enum_Cluster_Value_End()
					if (!FLG)
						GLV_SetNumbers(p_stGLV , 0 , p_stGLV->ulNumberOfEdges - 1 , 0 , 2);
				}
			}
		}//*/
	}

	GLV_DelCluster(p_stCluster);
	GLV_Octree_Destroy( &stOctreeROOT );
}

int GLV_FAST_FE_PointInTriangle(tdst_GLV *p_stGLV , ULONG F , MATHD_tdst_Vector *p_P3D)
{
	MATHD_tdst_Vector	st_VectMA, st_VectMB, st_VectMC , *_pst_N;
	GLV_Scalar			_fVal;

	MATHD_SubVector(&st_VectMA, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[0]].P3D, p_P3D);
	MATHD_SubVector(&st_VectMB, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[1]].P3D, p_P3D);
	MATHD_SubVector(&st_VectMC, &p_stGLV->p_stPoints[p_stGLV->p_stFaces[F].Index[2]].P3D, p_P3D);
	_pst_N = &p_stGLV->p_stFaces[F].Plane.Normale;

	if (fabs(_pst_N->x) >= Cf_InvSqrt3)
	{
		_fVal = st_VectMA.y * st_VectMB.z - st_VectMA.z * st_VectMB.y;
		if(ulDifferentSign(_pst_N->x, _fVal))
			return 0;
		else
		{
			_fVal = st_VectMB.y * st_VectMC.z - st_VectMB.z * st_VectMC.y;
			if(ulDifferentSign(_pst_N->x, _fVal))
				return 0;
			else
			{
				_fVal = st_VectMC.y * st_VectMA.z - st_VectMC.z * st_VectMA.y;
				if (ulDifferentSign(_pst_N->x, _fVal))
					return 0;
				else
					return 1;
			}
		}
	}
	else
	{
		if(fabs(_pst_N->y) >= Cf_InvSqrt3)
		{
			_fVal = st_VectMA.z * st_VectMB.x - st_VectMA.x * st_VectMB.z;
			if(ulDifferentSign(_pst_N->y, _fVal))
				return 0;
			else
			{
				_fVal = st_VectMB.z * st_VectMC.x - st_VectMB.x * st_VectMC.z;
				if (ulDifferentSign(_pst_N->y, _fVal))
					return 0;
				else
				{
					_fVal = st_VectMC.z * st_VectMA.x - st_VectMC.x * st_VectMA.z;
					if (ulDifferentSign(_pst_N->y, _fVal))
						return 0;
					else
						return 1;
				}
			}
		}
		else
		{
			_fVal = st_VectMA.x * st_VectMB.y - st_VectMA.y * st_VectMB.x;
			if(ulDifferentSign(_pst_N->z, _fVal))
				return 0;
			else
			{
				_fVal = st_VectMB.x * st_VectMC.y - st_VectMB.y * st_VectMC.x;
				if(ulDifferentSign(_pst_N->z, _fVal))
					return 0;
				else
				{
					_fVal = st_VectMC.x * st_VectMA.y - st_VectMC.y * st_VectMA.x;
					if (ulDifferentSign(_pst_N->z, _fVal))
						return 0;
					else
						return 1;
				}
			}
		}
	}
}

/* 

  The mikado reconstruction : 
	   p_stGLV must have 1 triangle
	   and at least the 3 edges of the triangle.
	   Mikado will 
		- cut edges-points
		- cut edges-edges 
		- cut edges-triangle
		- remove externals edges
		- remove doubled edges

*/
void GLV_ResolveMikado(tdst_GLV *p_stGLV)
{
	ULONG C1,C2;
	MATHD_tdst_Vector stX,stY;
	GLV_Scalar Bx, By;
	if (!p_stGLV -> ulNumberOfEdges) return;
	if (!p_stGLV -> ulNumberOfFaces) return;
	/* 1: Clean */
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0 );
	GLD_RemoveBadEdges( p_stGLV );
	GLD_RemoveDoubleEdges( p_stGLV );
	GLD_RemoveUnusedIndexes( p_stGLV );
	/* Cut Edge - Edge */
	/* SQUARE! */
	MATHD_SubVector(&stX , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[0].Index[1]].P3D , &p_stGLV->p_stPoints[p_stGLV ->p_stFaces[0].Index[0]].P3D);
	MATHD_NormalizeVector(&stX , &stX );
	MATHD_CrossProduct(&stY , &stX , &p_stGLV ->p_stFaces[0].Plane.Normale);
	MATHD_NormalizeVector(&stY , &stY );
	Bx = MATHD_f_DotProduct(&stX , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[0].Index[0]].P3D);
	By = MATHD_f_DotProduct(&stY , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[0].Index[0]].P3D);
	for ( C1 = 0 ; C1 < p_stGLV -> ulNumberOfEdges - 1; C1 ++)
	{
		GLV_Scalar p2DCoords[8];
		for ( C2 = C1 + 1; C2 < p_stGLV -> ulNumberOfEdges ; C2 ++)
		{
			if (p_stGLV->p_stEdges[C1].Index[0] == p_stGLV->p_stEdges[C2].Index[0]) continue;
			if (p_stGLV->p_stEdges[C1].Index[0] == p_stGLV->p_stEdges[C2].Index[1]) continue;
			if (p_stGLV->p_stEdges[C1].Index[1] == p_stGLV->p_stEdges[C2].Index[0]) continue;
			if (p_stGLV->p_stEdges[C1].Index[1] == p_stGLV->p_stEdges[C2].Index[1]) continue;

			/* 0 */
			p2DCoords[0] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stEdges[C1].Index[0]].P3D, &stX) - Bx;
			p2DCoords[1] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stEdges[C1].Index[0]].P3D, &stY) - By;
			/* 1 */
			p2DCoords[2] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stEdges[C1].Index[1]].P3D, &stX) - Bx;
			p2DCoords[3] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stEdges[C1].Index[1]].P3D, &stY) - By;
			/* 2 */
			p2DCoords[4] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stEdges[C2].Index[0]].P3D, &stX) - Bx;
			p2DCoords[5] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stEdges[C2].Index[0]].P3D, &stY) - By;
			/* 3 */
			p2DCoords[6] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stEdges[C2].Index[1]].P3D, &stX) - Bx;
			p2DCoords[7] = MATHD_f_DotProduct(&p_stGLV->p_stPoints[p_stGLV->p_stEdges[C2].Index[1]].P3D, &stY) - By;

			if (GLV_ul_EdgeCutEdge2D(p2DCoords , 0 , 1 , 2 , 3 ))
			{
				GLV_Scalar  Inter1 , Inter2 , D4;
				MATHD_tdst_Vector st1,st2,stI;
				/* Compute intersection point (STI)*/
				Inter1 = GLV_2DDotProduct(0 , 1 , 2 , p2DCoords);
				Inter2 = GLV_2DDotProduct(0 , 1 , 3 , p2DCoords);
				D4 = -Inter1 / (Inter2 - Inter1);
				MATHD_BlendVector(&st1 , &p_stGLV->p_stPoints[p_stGLV->p_stEdges[C2].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stEdges[C2].Index[1]].P3D, D4);

				Inter1 = GLV_2DDotProduct(2 , 3 , 0 , p2DCoords);
				Inter2 = GLV_2DDotProduct(2 , 3 , 1 , p2DCoords);
				D4 = -Inter1 / (Inter2 - Inter1);
				MATHD_BlendVector(&st2 , &p_stGLV->p_stPoints[p_stGLV->p_stEdges[C1].Index[0]].P3D , &p_stGLV->p_stPoints[p_stGLV->p_stEdges[C1].Index[1]].P3D, D4);

				MATHD_AddVector(&stI , &st1 , &st2);
				MATHD_ScaleEqualVector(&stI , 0.5f);

				/* Create 2 new Edges and 1 point */
				GLV_SetNumbers(p_stGLV , p_stGLV ->ulNumberOfPoints + 1 , p_stGLV ->ulNumberOfEdges + 2 , 0 , 3);

				p_stGLV->p_stPoints[p_stGLV ->ulNumberOfPoints - 1] .P3D = stI;

				p_stGLV->p_stEdges[p_stGLV ->ulNumberOfEdges - 2] = p_stGLV->p_stEdges[C1];
				p_stGLV->p_stEdges[p_stGLV ->ulNumberOfEdges - 2].Index[0] = p_stGLV ->ulNumberOfPoints - 1;
				p_stGLV->p_stEdges[C1].Index[1] = p_stGLV ->ulNumberOfPoints - 1;

				p_stGLV->p_stEdges[p_stGLV ->ulNumberOfEdges - 1] = p_stGLV->p_stEdges[C2];
				p_stGLV->p_stEdges[p_stGLV ->ulNumberOfEdges - 1].Index[0] = p_stGLV ->ulNumberOfPoints - 1;
				p_stGLV->p_stEdges[C2].Index[1] = p_stGLV ->ulNumberOfPoints - 1;
			}
		}
	}
	/* cut edges-points */
	/* SQUARE! */ 
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfEdges ; C1 ++)
	{
		for (C2 = 0 ; C2 < p_stGLV->ulNumberOfPoints ; C2 ++)
		{
			if (C2 == p_stGLV->p_stEdges[C1].Index[0]) continue;
			if (C2 == p_stGLV->p_stEdges[C1].Index[1]) continue;
			if (GLV_f_IsPointOnEdge( &p_stGLV->p_stPoints[p_stGLV->p_stEdges[C1].Index[0]].P3D  , &p_stGLV->p_stPoints[p_stGLV->p_stEdges[C1].Index[1]].P3D  , &p_stGLV->p_stPoints[C2].P3D ))
			{
				GLV_SetNumbers(p_stGLV , 0 , p_stGLV ->ulNumberOfEdges + 1 , 0 , 2);
				p_stGLV->p_stEdges[p_stGLV ->ulNumberOfEdges - 1] = p_stGLV->p_stEdges[C1];
				p_stGLV->p_stEdges[p_stGLV ->ulNumberOfEdges - 1].Index[0] = C2;
				p_stGLV->p_stEdges[C1].Index[1] = C2;
			}
		}
	}//*/
	/* cut edges-triangle */
	/* Should be done just before */

	/* remove externals edges */
	/* just detect if middle point is out of the tri */
//	GLV_ComputeNormales	(p_stGLV);
	C2 = 0;
	for ( C1 = 0 ; C1 < p_stGLV -> ulNumberOfEdges ; C1 ++)
	{
		MATHD_AddVector(&stX , &p_stGLV ->p_stPoints[p_stGLV ->p_stEdges[C1].Index[0]].P3D, &p_stGLV ->p_stPoints[p_stGLV ->p_stEdges[C1].Index[1]].P3D);
		MATHD_ScaleEqualVector(&stX , 0.5f);
		if ((p_stGLV ->p_stEdges[C1].ulFlags & GLV_FLGS_Original) || (GLV_FAST_FE_PointInTriangle(p_stGLV , 0 , &stX)))
		{
			p_stGLV ->p_stEdges[C2++] = p_stGLV ->p_stEdges[C1];
		} 
	}
	GLV_SetNumbers(p_stGLV , 0 , C2 , 0 , 2);//*/
	/* remove doubled edges */
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0);
	GLD_RemoveBadEdges( p_stGLV );
	GLD_RemoveDoubleEdges( p_stGLV );
//	GLD_OptimizeEdges( p_stGLV );
	GLD_RemoveUnusedIndexes( p_stGLV );
}
/*
	Cut With Coplanar Edges:
	Generate final mesh.
	Kilts must be removed.
		Only mesh to cut must stay.
*/
void GLV_CutWithCoplanrEdges(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG C1,FLG;
	GLV_Cluster *p_stCluster;
	tdst_GLV *p_stGLVToClip, *p_stGLVAccum ;
	GLV_OCtreeNode stOctreeROOT;
	
	ulPolylineCounter = 0;

	GLD_RemoveBadEdges( p_stGLV );
	GLD_RemoveDoubleEdges( p_stGLV );
/*	GLD_OptimizeEdges( p_stGLV );
	GLD_OptimizeEdges( p_stGLV );
	GLD_OptimizeEdges( p_stGLV );
	GLD_OptimizeEdges( p_stGLV );
	GLD_OptimizeEdges( p_stGLV );
	GLD_OptimizeEdges( p_stGLV );*/
	GLD_RemoveUnusedIndexes( p_stGLV );

	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
		p_stGLV->p_stFaces[C1].ulFlags &= ~(GLV_FLGS_ClipResultFF|GLV_FLGS_ClipResultBF|GLV_FLGS_Clipped|GLV_FLGS_DeleteIt);

	p_stGLVToClip = NewGLV();
	p_stGLVAccum  = NewGLV();
	L_memset(&stOctreeROOT , 0 , sizeof (GLV_OCtreeNode));
	stOctreeROOT.p_OctreeFather = NULL;
	stOctreeROOT.p_stThisCLT = GLV_NewCluster();
	C1 = p_stGLV->ulNumberOfEdges;
	while (C1--) GLV_Clst_ADDV(stOctreeROOT.p_stThisCLT , C1); 
	GLV_ComputeGLVBox(p_stGLV , &stOctreeROOT.stAxisMin, &stOctreeROOT.stAxisMax , GLV_FLGS_Original);
	SeprogressPos (0.0f, "Compute least OK3..");
	GLV_Octree_DiveAndCompute_for_edges( p_stGLV , &stOctreeROOT , GLV_OC3_MAX_DEPTH_E , GLV_OC3_MAX_EPO, SeprogressPos);
	p_stCluster = GLV_NewCluster();
	GLV_SetNumbers(p_stGLVAccum , 0 , 0 , 0 , 7);
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
	{
		if ((p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original) && (p_stGLV->p_stFaces[C1].ulMARK & 0x40000000))
		{
			SeprogressPos ((float)C1 / (float)p_stGLV->ulNumberOfFaces, "Cut face with coplanar edges ..");
			if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_DoNotCut) continue;
			/* for each face find coplanar edges */
			GLV_SetNumbers(p_stGLVToClip , 3 , 3 , 1 , 7);
			p_stGLVToClip -> p_stFaces [0] = p_stGLV->p_stFaces[C1];
			p_stGLVToClip -> p_stFaces [0].Index[0] = 0;
			p_stGLVToClip -> p_stFaces [0].Index[1] = 1;
			p_stGLVToClip -> p_stFaces [0].Index[2] = 2;
			p_stGLVToClip -> p_stFaces [0].Nghbr[0] = p_stGLVToClip->p_stFaces[0].Nghbr[1] = p_stGLVToClip->p_stFaces[0].Nghbr[2] = -1;
			p_stGLVToClip -> p_stPoints[0] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[0]];
			p_stGLVToClip -> p_stPoints[1] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[1]];
			p_stGLVToClip -> p_stPoints[2] = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[2]];
			p_stGLVToClip -> p_stPoints[0].ulFlags = GLV_FLGS_FromEdge;
			p_stGLVToClip -> p_stPoints[1].ulFlags = GLV_FLGS_FromEdge;
			p_stGLVToClip -> p_stPoints[2].ulFlags = GLV_FLGS_FromEdge;
			p_stGLVToClip -> p_stEdges [0].Index[0] = 0;
			p_stGLVToClip -> p_stEdges [0].Index[1] = 1;
			p_stGLVToClip -> p_stEdges [1].Index[0] = 1;
			p_stGLVToClip -> p_stEdges [1].Index[1] = 2;
			p_stGLVToClip -> p_stEdges [2].Index[0] = 2;
			p_stGLVToClip -> p_stEdges [2].Index[1] = 0;
			p_stGLVToClip -> p_stEdges [0].ulFlags = GLV_FLGS_Original;
			p_stGLVToClip -> p_stEdges [1].ulFlags = GLV_FLGS_Original;
			p_stGLVToClip -> p_stEdges [2].ulFlags = GLV_FLGS_Original;
			GLV_ResetCluster(p_stCluster);
			GLV_Octree_DiveAndIntesectTriangle_On_Line_OK3( &stOctreeROOT ,p_stGLV ,  C1 , p_stCluster );
			FLG = 0;
/*			for (C2 = 0 ; C2 < p_stGLV ->ulNumberOfEdges ; C2 ++)
			{
				if ((p_stGLV->p_stEdges[C2].FromFaces[0] == C1) || (p_stGLV->p_stEdges[C2].FromFaces[1] == C1))
				{
 					GLV_SetNumbers(p_stGLVToClip , p_stGLVToClip->ulNumberOfPoints + 2 , p_stGLVToClip->ulNumberOfEdges + 1 , 0 , 3);
					p_stGLV->p_stEdges[C2].ulFlags |= GLV_FLGS_DBG1;
					p_stGLVToClip->p_stEdges[p_stGLVToClip->ulNumberOfEdges - 1] = p_stGLV->p_stEdges[C2];
					p_stGLVToClip->p_stEdges[p_stGLVToClip->ulNumberOfEdges - 1].Index[0] = p_stGLVToClip->ulNumberOfPoints - 2;
					p_stGLVToClip->p_stEdges[p_stGLVToClip->ulNumberOfEdges - 1].Index[1] = p_stGLVToClip->ulNumberOfPoints - 1;
					p_stGLVToClip->p_stEdges[p_stGLVToClip->ulNumberOfEdges - 1].ulFlags = 0;
					p_stGLVToClip->p_stPoints[p_stGLVToClip->ulNumberOfPoints - 2] = p_stGLV->p_stPoints[p_stGLV->p_stEdges[C2].Index[0]];
					p_stGLVToClip->p_stPoints[p_stGLVToClip->ulNumberOfPoints - 1] = p_stGLV->p_stPoints[p_stGLV->p_stEdges[C2].Index[1]];
					p_stGLVToClip->p_stPoints[p_stGLVToClip->ulNumberOfPoints - 1].ulFlags = GLV_FLGS_FromFace;
					p_stGLVToClip->p_stPoints[p_stGLVToClip->ulNumberOfPoints - 2].ulFlags = GLV_FLGS_FromFace;
					FLG = 1;
				}
			}//*/
			GLV_Enum_Cluster_Value(p_stCluster)
#ifdef GLV_StayWithEdges
				if ((p_stGLV->p_stEdges[MCL_Value].FromFaces[0] == C1) || (p_stGLV->p_stEdges[MCL_Value].FromFaces[1] == C1))
#else
				if (GLV_IsEdgeOnFace(p_stGLV , 
					C1,
					p_stGLV->p_stEdges[MCL_Value].Index[0] , 
					p_stGLV->p_stEdges[MCL_Value].Index[1]))
#endif
				{
 					GLV_SetNumbers(p_stGLVToClip , p_stGLVToClip->ulNumberOfPoints + 2 , p_stGLVToClip->ulNumberOfEdges + 1 , 0 , 3);
					p_stGLV->p_stEdges[MCL_Value].ulFlags |= GLV_FLGS_DBG1;
					p_stGLVToClip->p_stEdges[p_stGLVToClip->ulNumberOfEdges - 1] = p_stGLV->p_stEdges[MCL_Value];
					p_stGLVToClip->p_stEdges[p_stGLVToClip->ulNumberOfEdges - 1].Index[0] = p_stGLVToClip->ulNumberOfPoints - 2;
					p_stGLVToClip->p_stEdges[p_stGLVToClip->ulNumberOfEdges - 1].Index[1] = p_stGLVToClip->ulNumberOfPoints - 1;
					p_stGLVToClip->p_stEdges[p_stGLVToClip->ulNumberOfEdges - 1].ulFlags = 0;
					p_stGLVToClip->p_stPoints[p_stGLVToClip->ulNumberOfPoints - 2] = p_stGLV->p_stPoints[p_stGLV->p_stEdges[MCL_Value].Index[0]];
					p_stGLVToClip->p_stPoints[p_stGLVToClip->ulNumberOfPoints - 1] = p_stGLV->p_stPoints[p_stGLV->p_stEdges[MCL_Value].Index[1]];
					p_stGLVToClip->p_stPoints[p_stGLVToClip->ulNumberOfPoints - 1].ulFlags = GLV_FLGS_FromFace;
					p_stGLVToClip->p_stPoints[p_stGLVToClip->ulNumberOfPoints - 2].ulFlags = GLV_FLGS_FromFace;
					FLG = 1;
				}
			GLV_Enum_Cluster_Value_End()//*/
			if (FLG)
			{
				p_stGLV->p_stFaces[C1].ulFlags |= GLV_FLGS_DeleteIt ;
				/* And cut face with them */
				/* 1: Mikado */
				GLV_ResolveMikado(p_stGLVToClip);
				/* 2: Resolve */
				GLV_ResolvePolyline( p_stGLVToClip );
				/* Add result */
				p_stGLVToClip -> p_stFaces[0].ulFlags |= GLV_FLGS_DeleteIt;
				GLV_AddGLV(p_stGLVAccum,p_stGLVToClip);
			}
		}
	}
	for (C1 = 0 ; C1 < p_stGLVAccum->ulNumberOfFaces ; C1 ++)
	{
		p_stGLVAccum->p_stFaces[C1].ulFlags |= GLV_FLGS_DBG1;
	}

	GLD_Remove2BD(p_stGLVAccum);
	GLD_Remove2BD(p_stGLV);
	GLV_SetNumbers(p_stGLV, 0 , 0 , 0 , 2);
#ifdef GLV_DEBUG
	GLD_Weld( p_stGLVAccum , GLV_WELD_FCT , 0 );
	for (C1 = 0 ; C1 < p_stGLVAccum->ulNumberOfEdges; C1 ++)
	{
		p_stGLVAccum->p_stEdges[C1].ulFlags &= ~GLV_FLGS_DBG1;
	}
	GLD_RemoveDoubleEdges( p_stGLVAccum);
#endif
	GLV_AddGLV(p_stGLV , p_stGLVAccum);
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0 );
//	GLD_RemoveIllegalFaces( p_stGLV );
//	GLD_RemoveUnusedIndexes( p_stGLV );
	

	GLV_Destroy(p_stGLVAccum);
	GLV_Destroy(p_stGLVToClip);

	GLV_DelCluster(p_stCluster);
	GLV_Octree_Destroy( &stOctreeROOT );

}
GLV_Scalar GLV_f_GetLightIlluminationCoef(tdst_GLV *p_stGLV , MATHD_tdst_Vector *p_stPoint , MATH_tdst_Vector *p_stPointNormale , MATHD_tdst_Vector *p_stLightCenter , ULONG LightCounter)
{
	MATHD_tdst_Vector stLocal , St2;
	GLV_Scalar D, C;

	if ((p_stGLV->p_Lights[LightCounter].ulLightFlags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct)
	{
		MATH_To_MATHD(&St2 , p_stPointNormale);
		C = MATHD_f_DotProduct(&St2, &p_stGLV->p_Lights[LightCounter].LDir);
	} else
	{
		MATHD_SubVector(&stLocal  , p_stLightCenter, p_stPoint);
		D = MATHD_f_NormVector(&stLocal);
		if (!GLV_RT_LightGeomRemove(p_stGLV , &stLocal , LightCounter))
		{
			if (((p_stGLV->p_Lights[LightCounter].ulLightFlags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Omni) ||
				((p_stGLV->p_Lights[LightCounter].ulLightFlags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Spot))
			{
				if (D > p_stGLV->p_Lights[LightCounter].fLightNear)
				{
					D = (D - p_stGLV->p_Lights[LightCounter].fLightFar) / (p_stGLV->p_Lights[LightCounter].fLightNear - p_stGLV->p_Lights[LightCounter].fLightFar);
				} else D = 1.0f;
				if (D < 0.1f) D = 0.0f;
				if ((p_stGLV->p_Lights[LightCounter].ulLightFlags & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Spot)
				{
					GLV_Scalar CC;
					CC = MATHD_f_DotProduct(&stLocal , &p_stGLV->p_Lights[LightCounter].LDir) / MATHD_f_NormVector(&stLocal);
					if (CC < 0.0f) CC = -CC;
					CC = (CC - p_stGLV->p_Lights[LightCounter].fCosBigAlpha) / (p_stGLV->p_Lights[LightCounter].fCosLittleAlpha - p_stGLV->p_Lights[LightCounter].fCosBigAlpha);
					if ((CC < 1.0f) && (CC > 0.0f))
						D *= CC;
				}
			} else
				/* Is a direct light */
			{
				D = 1.0f;
			}
		} else D = 0.0f;
		if (p_stGLV ->p_Lights[LightCounter ].ulLightFlags & LIGHT_Cul_LF_Paint)
			C = D;
		else
		{
			MATHD_NormalizeVector(&stLocal,&stLocal);
			MATH_To_MATHD(&St2 , p_stPointNormale);
			C = MATHD_f_DotProduct(&stLocal , &St2) * D;
		}
	}
	return C;
}


void GLV_ComputeColors(tdst_GLV *p_stGLV , ULONG SmoothFronteer , ULONG (*SeprogressPos) (float F01, char *))
{
	ULONG C1,C2,LightCounter;
	tdst_GLV *p_stGLVREF;
	char Text[256];
	GLV_Scalar LightCol[4];
	GLV_OCtreeNode stOctreeROOT;
	GLV_OCtreeNode stOctreeROOT2;
	GLV_Cluster *p_ClusterToFill,*p_ClusterResult;
	MATHD_tdst_Vector stCenter;
	MATHD_tdst_Vector St2 , stLightCenter;
	ULONG SF;
	MATHD_tdst_Vector stDistance;
	GLV_Scalar C,VF;

	GLD_RemoveUnusedIndexes ( p_stGLV );

	p_stGLVREF = GLV_Duplicate(p_stGLV);

	L_memset(&stOctreeROOT , 0 , sizeof (GLV_OCtreeNode));
	L_memset(&stOctreeROOT2 , 0 , sizeof (GLV_OCtreeNode));
	stOctreeROOT.p_OctreeFather = NULL;
	stOctreeROOT.p_stThisCLT = GLV_NewCluster();
	stOctreeROOT2.p_OctreeFather = NULL;
	stOctreeROOT2.p_stThisCLT = GLV_NewCluster();
	C1 = p_stGLVREF->ulNumberOfFaces;
	while (C1--) 
	{
		if (!(p_stGLVREF->p_stFaces[C1].ulFlags & GLV_FLGS_WhiteFront))
			GLV_Clst_ADDV(stOctreeROOT.p_stThisCLT , C1); /* Add whitefront kilt face */
		else
		if (p_stGLVREF->p_stFaces[C1].ulFlags & GLV_FLGS_Original)
			GLV_Clst_ADDV(stOctreeROOT.p_stThisCLT , C1); /* Add all the original faces */
	}
	C1 = p_stGLVREF->ulNumberOfFaces;
	while (C1--) 
	{
		if (p_stGLVREF->p_stFaces[C1].ulFlags & (GLV_FLGS_WhiteFront|GLV_FLGS_Original))
			GLV_Clst_ADDV(stOctreeROOT2.p_stThisCLT , C1); /* Add whitefront kilt face */
	}
	GLV_ComputeGLVBox(p_stGLV , &stOctreeROOT.stAxisMin, &stOctreeROOT.stAxisMax , GLV_FLGS_Original);
	GLV_ComputeGLVBox(p_stGLV , &stOctreeROOT2.stAxisMin, &stOctreeROOT2.stAxisMax , GLV_FLGS_Original);
	GLV_Octree_DiveAndCompute( p_stGLVREF , &stOctreeROOT , GLV_OC3_MAX_DEPTH  - 1, GLV_OC3_MAX_FPO << 1, SeprogressPos);
	GLV_Octree_DiveAndCompute( p_stGLVREF , &stOctreeROOT2 , GLV_OC3_MAX_DEPTH - 1, GLV_OC3_MAX_FPO << 1, SeprogressPos);
	p_ClusterToFill = GLV_NewCluster();
	p_ClusterResult = GLV_NewCluster();
	
	sprintf (Text , "Compute colors on %d faces", p_stGLV->ulNumberOfFaces);
	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
	{
//		p_stGLV->p_stFaces[C1].Colours[0] = p_stGLV->p_stFaces[C1].Colours[1] = p_stGLV->p_stFaces[C1].Colours[2] = 0;
		p_stGLV->p_stFaces[C1].ulFlags &= ~(GLV_FLGS_RSV2);
		MATH_NormalizeVector2(&p_stGLV->p_stFaces[C1].OriginalPointsNormales[0] , &p_stGLV->p_stFaces[C1].OriginalPointsNormales[0]);
		MATH_NormalizeVector2(&p_stGLV->p_stFaces[C1].OriginalPointsNormales[1] , &p_stGLV->p_stFaces[C1].OriginalPointsNormales[1]);
		MATH_NormalizeVector2(&p_stGLV->p_stFaces[C1].OriginalPointsNormales[2] , &p_stGLV->p_stFaces[C1].OriginalPointsNormales[2]);
	}

	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfPoints ; C1 ++)
	{
		p_stGLV->p_stPoints[C1].ulFlags = GLV_FLGS_Original;
	}
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0);

	for (LightCounter = 0 ; LightCounter < p_stGLV->ulNumberOfLight; LightCounter ++) 
	{
		for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
		{
			if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original)
			{
				p_stGLV->p_stFaces[C1].ulFlags &= ~(GLV_FLGS_WhiteFront|GLV_FLGS_BlackFront|GLV_FLGS_RSV1|GLV_FLGS_RSV3|GLV_FLGS_RSV4);
			} 
		}
		for (C1 = 0 ; C1 < p_stGLVREF->ulNumberOfFaces ; C1 ++)
		{
			p_stGLVREF->p_stFaces[C1].ulFlags &= ~(GLV_FLGS_Transparent2);
			if (p_stGLVREF->p_stFaces[C1].ulFlags & GLV_FLGS_Kilt)
			{
				if (p_stGLVREF->p_stFaces[C1].ulMARK != LightCounter)
				{
					p_stGLVREF->p_stFaces[C1].ulFlags |= GLV_FLGS_Transparent2;
				}
			}
		}
		/* Compute visibility */
		for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
		{
			ULONG ulChannel;
			ulChannel = p_stGLV->p_stFaces[C1].ulChannel & p_stGLV ->p_Lights[LightCounter ].ulChannel;
			sprintf (Text , "Compute colors on %d faces", p_stGLV->ulNumberOfFaces);
			if (SeprogressPos(((float)C1 + (float)LightCounter * (float)p_stGLV->ulNumberOfFaces) / ((float)p_stGLV->ulNumberOfFaces * (float)p_stGLV->ulNumberOfLight), Text)) 
				return;
			if (ulChannel)
			{
				if ((p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original) && (p_stGLV->p_stFaces[C1].ulMARK & 0x40000000))
				{
  					SF = p_stGLV->p_stFaces[C1].ulFlags;
					p_stGLV->p_stFaces[C1].ulFlags |= GLV_FLGS_Transparent | GLV_FLGS_Transparent2;
					GLV_GetTriRayPoint(p_stGLV , C1, &stCenter);
					GLV_RT_ComputeLightCenter(p_stGLV, LightCounter , &stCenter , &stLightCenter);
					MATHD_SubVector(&stDistance , &stLightCenter, &stCenter );
					{
						if ((MATHD_f_DotProduct(&stDistance , &p_stGLV->p_stFaces[C1].Plane.Normale) < 0.0f) && (!((p_stGLV ->p_Lights[LightCounter ].ulLightFlags & LIGHT_Cul_LF_Paint))))
						{
							SF |= GLV_FLGS_BlackFront;
						} else
						{
							if (GLV_RT_LightGeomRemove(p_stGLV , &stDistance , LightCounter))
								SF |= GLV_FLGS_BlackFront;
							else
							{
								if (p_stGLV ->p_Lights[LightCounter ].ulLightFlags & LIGHT_Cul_LF_CastShadows)
								{
									if (GLV_RT_GetIsHitChannel(p_stGLVREF , &stCenter , &stLightCenter , p_stGLV->p_stFaces+C1 , &stOctreeROOT , p_ClusterToFill ,ulChannel))
									{
										SF |= GLV_FLGS_BlackFront;
									}
									else
									{
										if ((p_stGLV -> p_Lights[LightCounter ].bLightPenombriaOn) && GLV_RT_GetIsHitChannel(p_stGLVREF , &stCenter , &stLightCenter , p_stGLV->p_stFaces+C1 , &stOctreeROOT2 , p_ClusterToFill , ulChannel))
											SF |= GLV_FLGS_WhiteFront|GLV_FLGS_BlackFront;
										else//*/
											SF |= GLV_FLGS_WhiteFront;
									}
								} else
									SF |= GLV_FLGS_WhiteFront;
							}
						}
					}
					p_stGLV->p_stFaces[C1].ulFlags = SF;
				} 
			}
		}
		/* refine angle for current light */
		GLV_ComputeNghbr(p_stGLV );
		/* Resolve Smooth Fronteer Problem */
//		GLV_SubdividePnbr2Times(p_stGLV );
		GLV_RefineWithAngleSpherical(p_stGLV , LightCounter);
		GLD_Weld( p_stGLV , GLV_WELD_FCT , 0 );
		for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
		{
			if ((p_stGLV->p_stFaces[C1].ulFlags & (GLV_FLGS_BlackFront|GLV_FLGS_WhiteFront)) == GLV_FLGS_BlackFront)
			{
				p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[0]].ulFlags = GLV_FLGS_BlackFront;
				p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[1]].ulFlags = GLV_FLGS_BlackFront;
				p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[2]].ulFlags = GLV_FLGS_BlackFront;
			}
		}
		for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
		{
			if ((p_stGLV->p_stFaces[C1].ulFlags & (GLV_FLGS_BlackFront|GLV_FLGS_WhiteFront)) == GLV_FLGS_WhiteFront)
			{
				p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[0]].ulFlags = GLV_FLGS_WhiteFront;
				p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[1]].ulFlags = GLV_FLGS_WhiteFront;
				p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[2]].ulFlags = GLV_FLGS_WhiteFront;
			}
		}
		/* Compute Colours second pass */
		{
			GLV_ULto4Scalar(p_stGLV->p_Lights[LightCounter].ulColor, LightCol);
			for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
			{
				ULONG ulChannel;
				ulChannel = p_stGLV->p_stFaces[C1].ulChannel & p_stGLV ->p_Lights[LightCounter ].ulChannel;
				if ((p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original) && (p_stGLV->p_stFaces[C1].ulMARK & 0x40000000))
				{
					if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_WhiteFront)
					{
						for (C2 = 0 ; C2 < 3 ; C2 ++)
						{
							stCenter = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].P3D ;
							GLV_RT_ComputeLightCenter(p_stGLV, LightCounter , &stCenter , &stLightCenter);
							VF = 1.0f;
							if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_BlackFront) // Penumbria triangle 
							{
								MATHD_SubVector(&St2 , &stLightCenter , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].P3D );
								MATHD_SetNormVector(&St2 , &St2 , GLF_FaceExtraction );
								MATHD_AddVector(&stCenter , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].P3D , &St2 );
								if (GLV_RT_GetIsHitAll(p_stGLVREF , &stCenter , &stLightCenter , p_stGLV->p_stFaces+C1 , &stOctreeROOT2 , p_ClusterToFill ,  p_ClusterResult ))
								{
									GLV_Enum_Cluster_Value(p_ClusterResult)
									if (ulChannel & p_stGLVREF->p_stFaces[MCL_Value].ulChannel)
									{
										/* Compute visibility factor */
										/* normaly , only one triangle should be touch (a whitefront kilt) */
										/* Deduct the visibility from this hit */
										if (p_stGLVREF->p_stFaces[MCL_Value].ulFlags & GLV_FLGS_Original)
										{
											VF = 0.0F;
										}
										if ( ( p_stGLVREF->p_stFaces[MCL_Value].ulFlags & (GLV_FLGS_WhiteFront|GLV_FLGS_Kilt) ) == (GLV_FLGS_WhiteFront|GLV_FLGS_Kilt))
										{
											GLV_Scalar Dist1,Dist2;
											MATHD_tdst_Vector stProjPoint;
											Dist1 = MATHD_f_DotProduct(&p_stGLVREF->p_stFaces[MCL_Value].PenombriaPlane.Normale , &stCenter) - p_stGLVREF->p_stFaces[MCL_Value].PenombriaPlane.fDistanceTo0;
											if (Dist1 > GLF_PrecMetric)
											{
												GLV_Scalar VFX;
												GLV_PointFromEdgeTouchFace(p_stGLVREF->p_stFaces + MCL_Value , NULL , &stCenter , &stLightCenter , &stProjPoint);
												Dist2 = MATHD_f_DotProduct(&p_stGLVREF->p_stFaces[MCL_Value].PenombriaPlane.Normale , &stProjPoint) - p_stGLVREF->p_stFaces[MCL_Value].PenombriaPlane.fDistanceTo0;
												VFX = Dist2 / Dist1;
												if (VFX > 1.0f) 
													VF = 1.0f;
												if (VFX < 0.0f) 
													VF = 0.0f;
												VF *= VFX;
											}
										} 
									} 
									GLV_Enum_Cluster_Value_End();
								} else 
								{
									if (p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].ulFlags == GLV_FLGS_WhiteFront) VF = 1.0f ; 
									if (p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].ulFlags == GLV_FLGS_BlackFront) VF = 0.0f ; 
								}
							}
							C = VF * GLV_f_GetLightIlluminationCoef(p_stGLV , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].P3D , &p_stGLV->p_stFaces[C1].OriginalPointsNormales[C2] , &stLightCenter , LightCounter);
							if (C > 0.0f)
							{
								GLV_Scalar Col[4] ;
								if (p_stGLV ->p_Lights[LightCounter ].ulLightFlags & LIGHT_Cul_LF_Absorb) C = -C;
								GLV_ULto4Scalar(p_stGLV->p_stFaces[C1].Colours[C2], Col);
 								Col[0] += LightCol[0] * C;
								Col[1] += LightCol[1] * C;
								Col[2] += LightCol[2] * C;
								Col[3] += LightCol[3] * C;
								p_stGLV->p_stFaces[C1].Colours[C2] = GLV_4ScalarToUL( Col );
							}
						}
					} 
				}
			}
		}
		if (SmoothFronteer)
		{
			GLV_ComputeNghbr(p_stGLV );
			for (C1 = 0 ; C1 < p_stGLV->ulNumberOfPoints ; C1 ++) p_stGLV->p_stPoints[C1].ulFlags &= ~(GLV_FLGS_RSV1|GLV_FLGS_RSV3|GLV_FLGS_RSV4);
			/* Detect suspect Points and mark them with RSV4 */
			for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
			{
				if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original)
				{
					if ((p_stGLV->p_stFaces[C1].ulFlags & (GLV_FLGS_BlackFront | GLV_FLGS_WhiteFront)) == GLV_FLGS_BlackFront)
					{
						for (C2 = 0 ; C2 < 3 ; C2 ++)
						{
							if (((p_stGLV->p_stFaces[p_stGLV->p_stFaces[C1].Nghbr[(C2 + 2) % 3]].ulFlags & GLV_FLGS_WhiteFront) && (p_stGLV->p_stFaces[p_stGLV->p_stFaces[C1].Nghbr[(C2 + 2) % 3]].ulKey != p_stGLV->p_stFaces[C1].ulKey)) ||
								((p_stGLV->p_stFaces[p_stGLV->p_stFaces[C1].Nghbr[C2]].ulFlags & GLV_FLGS_WhiteFront) && (p_stGLV->p_stFaces[p_stGLV->p_stFaces[C1].Nghbr[C2]].ulKey != p_stGLV->p_stFaces[C1].ulKey)))
							{
								stCenter = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].P3D ;
								GLV_RT_ComputeLightCenter(p_stGLV, LightCounter , &stCenter , &stLightCenter);
								C = GLV_f_GetLightIlluminationCoef(p_stGLV , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].P3D , &p_stGLV->p_stFaces[C1].OriginalPointsNormales[C2] , &stLightCenter , LightCounter);
								if (C > 0.0f)
								{
									GLV_Scalar Col[4] ;
									if (p_stGLV ->p_Lights[LightCounter ].ulLightFlags & LIGHT_Cul_LF_Absorb) C = -C;
									GLV_ULto4Scalar(p_stGLV->p_stFaces[C1].Colours[C2], Col);
									p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].ulFlags |= GLV_FLGS_RSV4;
									Col[0] += LightCol[0] * C;
									Col[1] += LightCol[1] * C;
									Col[2] += LightCol[2] * C;
									Col[3] += LightCol[3] * C;
									p_stGLV->p_stFaces[C1].Colours[C2] = GLV_4ScalarToUL( Col );
								}
							}
						}
					}
				}
			}
			for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
			{
				if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_Original)
				{
					if ((p_stGLV->p_stFaces[C1].ulFlags & (GLV_FLGS_BlackFront | GLV_FLGS_WhiteFront)) == GLV_FLGS_BlackFront)
					{
						for (C2 = 0 ; C2 < 3 ; C2 ++)
						{
							if (((p_stGLV->p_stFaces[p_stGLV->p_stFaces[C1].Nghbr[(C2 + 2) % 3]].ulFlags & GLV_FLGS_WhiteFront) && (p_stGLV->p_stFaces[p_stGLV->p_stFaces[C1].Nghbr[(C2 + 2) % 3]].ulKey != p_stGLV->p_stFaces[C1].ulKey)) ||
								((p_stGLV->p_stFaces[p_stGLV->p_stFaces[C1].Nghbr[C2]].ulFlags & GLV_FLGS_WhiteFront) && (p_stGLV->p_stFaces[p_stGLV->p_stFaces[C1].Nghbr[C2]].ulKey != p_stGLV->p_stFaces[C1].ulKey)))
								continue;
							if (p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].ulFlags & GLV_FLGS_RSV4)
							{
								stCenter = p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].P3D ;
								GLV_RT_ComputeLightCenter(p_stGLV, LightCounter , &stCenter , &stLightCenter);
								C = GLV_f_GetLightIlluminationCoef(p_stGLV , &p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].P3D , &p_stGLV->p_stFaces[C1].OriginalPointsNormales[C2] , &stLightCenter , LightCounter);
								if (C > 0.0f)
								{
									GLV_Scalar Col[4] ;
									if (p_stGLV ->p_Lights[LightCounter ].ulLightFlags & LIGHT_Cul_LF_Absorb) C = -C;
									GLV_ULto4Scalar(p_stGLV->p_stFaces[C1].Colours[C2], Col);
									Col[0] += LightCol[0] * C;
									Col[1] += LightCol[1] * C;
									Col[2] += LightCol[2] * C;
									Col[3] += LightCol[3] * C;
									p_stGLV->p_stFaces[C1].Colours[C2] = GLV_4ScalarToUL( Col );
								}
							}
						}
					}
				}
			}//*/
		}
	}
	GLV_DelCluster(p_ClusterToFill);
	GLV_DelCluster(p_ClusterResult);
	GLV_Octree_Destroy( &stOctreeROOT );
	GLV_Octree_Destroy( &stOctreeROOT2 );
	GLD_Remove2BD(p_stGLV);//*/
	GLV_RemoveKILT(p_stGLV);
	/* Resolve uncutted faces */
	{
		GLV_Scalar		*pColors;
		ULONG			*p_Counter;
		GLV_Scalar	Col[4] ;
		pColors = (double*)GLV_ALLOC(sizeof(GLV_Scalar) * p_stGLV->ulNumberOfPoints * 4L);
		p_Counter = (ULONG*)GLV_ALLOC(p_stGLV->ulNumberOfPoints * 4L);
		for (C1 = 0 ; C1 < p_stGLV->ulNumberOfPoints ; C1 ++)
		{
			pColors[(C1 << 2) + 0] = 
			pColors[(C1 << 2) + 1] = 
			pColors[(C1 << 2) + 2] = 
			pColors[(C1 << 2) + 3] = 0.0f;
			p_Counter[C1] = 0;
		}
		for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
		{
			if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_DoNotCut)
			{
				for (C2 = 0 ; C2 < 3 ; C2 ++)
				{
				GLV_ULto4Scalar(p_stGLV->p_stFaces[C1].Colours[C2], Col);
				pColors[(p_stGLV->p_stFaces[C1].Index[C2] << 2) + 0] += Col[0];
				pColors[(p_stGLV->p_stFaces[C1].Index[C2] << 2) + 1] += Col[1]; 
				pColors[(p_stGLV->p_stFaces[C1].Index[C2] << 2) + 2] += Col[2];
				pColors[(p_stGLV->p_stFaces[C1].Index[C2] << 2) + 3] += Col[3];
				p_Counter[p_stGLV->p_stFaces[C1].Index[C2]]++;
				}
			}
		}
		for (C1 = 0 ; C1 < p_stGLV->ulNumberOfPoints ; C1 ++)
		{
			if (p_Counter[C1])
			{
				GLV_Scalar Invers;
				Invers = 1.0f / (GLV_Scalar)p_Counter[C1];
				pColors[(C1 << 2) + 0] *= Invers;
				pColors[(C1 << 2) + 1] *= Invers;
				pColors[(C1 << 2) + 2] *= Invers;
				pColors[(C1 << 2) + 3] *= Invers;
				p_Counter[C1] = GLV_4ScalarToUL( &pColors[(C1 << 2) + 0] );
			}
		}
		for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
		{
			if (p_stGLV->p_stFaces[C1].ulFlags & GLV_FLGS_DoNotCut)
			{
				for (C2 = 0 ; C2 < 3 ; C2 ++)
				{
					p_stGLV->p_stFaces[C1].Colours[C2] = p_Counter[p_stGLV->p_stFaces[C1].Index[C2]];
				}
			}
		}
		GLV_FREE(p_Counter);
		GLV_FREE(pColors);
	}

	GLD_BlowUpIndexion( p_stGLV , 0 );

	for (C1 = 0 ; C1 < p_stGLV->ulNumberOfFaces ; C1 ++)
	{
		for (C2 = 0 ; C2 < 3 ; C2 ++)
		{
			p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].ulFlags = p_stGLV->p_stFaces[C1].ulMARK;
			p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].ulColor = p_stGLV->p_stFaces[C1].Colours[C2];
			MATH_To_MATHD(&p_stGLV->p_stPoints[p_stGLV->p_stFaces[C1].Index[C2]].UpPoint , &p_stGLV->p_stFaces[C1].OriginalPointsNormales[C2]);
		}
	} 
	GLD_Weld( p_stGLV , GLV_WELD_FCT , 1 );
	GLD_RemoveIllegalFaces( p_stGLV );
	GLD_RemoveUnusedIndexes( p_stGLV );
	GLV_ComputeNghbr(p_stGLV);
	GLV_Destroy(p_stGLVREF);
}

void GLV_FirstLighSubdivide(tdst_GLV *p_stGLV , ULONG (*SeprogressPos) (float F01, char *))
{
//	ULONG LightCounter;

	GLD_RemoveUnusedIndexes ( p_stGLV );
	GLV_FirstRefine(p_stGLV , -1);
/*	for (LightCounter = 0 ; LightCounter < p_stGLV->ulNumberOfLight; LightCounter ++) 
	{
		GLV_FirstRefine(p_stGLV , LightCounter);
	}*/

	GLD_Weld( p_stGLV , GLV_WELD_FCT , 0 );
	GLD_RemoveUnusedIndexes( p_stGLV );
	GLV_ComputeNghbr(p_stGLV);
}

#endif
