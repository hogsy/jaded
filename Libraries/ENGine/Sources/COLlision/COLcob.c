/*$T COLcob.c GC! 1.081 01/28/03 17:29:05 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#include "BASe/MEMory/MEM.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGdefs.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"

#include "MATHs/MATH.h"

#include "COLstruct.h"
#include "COLconst.h"
#include "COLaccess.h"
#include "COLvars.h"

#include "OBJects/OBJstruct.h"
#include "OBJects/OBJconst.h"
#include "OBJects/OBJaccess.h"
#include "OBJects/OBJorient.h"

#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "GDInterface/GDInterface.h"
#include "GraphicDK/Sources/GEOmetric/GEOobject.h"
#include "GraphicDK/Sources/GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEO_MRM.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/INTersection/INTmain.h"

#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"

#define f_Max3(a, b, c) (a) > (b) ? (a) > (c) ? (a) : (c) : (b) > (c) ? (b) : (c)
#define f_Min3(a, b, c) (a) < (b) ? (a) < (c) ? (a) : (c) : (b) < (c) ? (b) : (c)

extern COL_tdst_GlobalVars COL_gst_GlobalVars;

#ifdef JADEFUSION
extern void COL_OK3_Build(void *_p_Data, BOOL _b_ColMap, BOOL _b_ReBuild);
#else
extern void	COL_OK3_Build(void *, BOOL, BOOL);
#endif

#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "LINKs/LINKstruct_reg.h"
#include "LINKs/LINKstruct.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "GraphicDK/Sources/GEOmetric/GEOload.h"
#include "GraphicDK/Sources/GEOmetric/GEOsubobject.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "SDK/Sources/LINks/LINKtoed.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include <float.h>


/*
 =======================================================================================================================
    Aim:    Transforms a Sphere Cob into a Box one.

    Note:   ACTIVE_EDITORS
 =======================================================================================================================
 */
void COL_Cob_SphereToBox(COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Min, st_Max;
	float				f_Radius, f_InsideRadius;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector(&st_Min, COL_pst_Shape_GetCenter(_pst_Cob->pst_MathCob->p_Shape));
	MATH_CopyVector(&st_Max, &st_Min);
	f_Radius = COL_f_Shape_GetRadius(_pst_Cob->pst_MathCob->p_Shape);

	f_InsideRadius = f_Radius * Cf_InvSqrt3;

	/*$F------- Min ------------ */
	st_Min.x = st_Min.x - f_InsideRadius;
	st_Min.y = st_Min.y - f_InsideRadius;
	st_Min.z = st_Min.z - f_InsideRadius;

	/*$F------- Max ------------ */
	st_Max.x = st_Max.x + f_InsideRadius;
	st_Max.y = st_Max.y + f_InsideRadius;
	st_Max.z = st_Max.z + f_InsideRadius;

	_pst_Cob->pst_MathCob->p_Shape = MEM_p_Realloc(_pst_Cob->pst_MathCob->p_Shape, sizeof(COL_tdst_Box));

	MATH_CopyVector(COL_pst_Shape_GetMin(_pst_Cob->pst_MathCob->p_Shape), &st_Min);
	MATH_CopyVector(COL_pst_Shape_GetMax(_pst_Cob->pst_MathCob->p_Shape), &st_Max);
}

/*
 =======================================================================================================================
    Aim:    Transforms a Cylinder Cob into a Sphere one.

    Note:   ACTIVE_EDITORS
 =======================================================================================================================
 */
void COL_Cob_CylinderToSphere(COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Radius, f_Height;
	MATH_tdst_Vector	st_Center;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_CopyVector(&st_Center, COL_pst_Shape_GetCenter(_pst_Cob->pst_MathCob->p_Shape));

	f_Radius = COL_f_Shape_GetRadius(_pst_Cob->pst_MathCob->p_Shape);
	f_Height = COL_f_Shape_GetHeight(_pst_Cob->pst_MathCob->p_Shape);

	_pst_Cob->pst_MathCob->p_Shape = MEM_p_Realloc(_pst_Cob->pst_MathCob->p_Shape, sizeof(COL_tdst_Sphere));

	MATH_CopyVector(COL_pst_Shape_GetCenter(_pst_Cob->pst_MathCob->p_Shape), &st_Center);
	((COL_tdst_Sphere *) _pst_Cob->pst_MathCob->p_Shape)->f_Radius = fMax(f_Radius, f_Height);
}

/*
 =======================================================================================================================
    Aim:    Transforms a Box Cob into a Cylinder one.

    Note:   ACTIVE_EDITORS
 =======================================================================================================================
 */
void COL_Cob_BoxToCylinder(COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Center;
	float				f_Radius;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector
	(
		&st_Center,
		COL_pst_Shape_GetMax(_pst_Cob->pst_MathCob->p_Shape),
		COL_pst_Shape_GetMin(_pst_Cob->pst_MathCob->p_Shape)
	);

	MATH_MulEqualVector(&st_Center, 0.5f);
	f_Radius = MATH_f_NormVector(&st_Center);

	MATH_AddEqualVector(&st_Center, COL_pst_Shape_GetMin(_pst_Cob->pst_MathCob->p_Shape));

	_pst_Cob->pst_MathCob->p_Shape = MEM_p_Realloc(_pst_Cob->pst_MathCob->p_Shape, sizeof(COL_tdst_Cylinder));

	MATH_CopyVector(COL_pst_Shape_GetCenter(_pst_Cob->pst_MathCob->p_Shape), &st_Center);
	COL_Shape_SetRadius(_pst_Cob->pst_MathCob->p_Shape, f_Radius);
	COL_Shape_SetHeight(_pst_Cob->pst_MathCob->p_Shape, f_Radius);
}

#endif

/*
 =======================================================================================================================
    Aim:    Add a Sphere Mathematical Cob to an object.
 =======================================================================================================================
 */
void COL_ColMap_AddSphere(COL_tdst_Cob *_pst_Cob, COL_tdst_Cob *_pst_Orig, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Sphere *pst_Sphere;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Cob->pst_MathCob->p_Shape = (COL_tdst_Sphere *) MEM_p_Alloc(sizeof(COL_tdst_Sphere));
	pst_Sphere = (COL_tdst_Sphere *) _pst_Cob->pst_MathCob->p_Shape;
	if(_pst_Orig)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Sphere *pst_SphOrig;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_SphOrig = (COL_tdst_Sphere *) _pst_Orig->pst_MathCob->p_Shape;
		pst_Sphere->f_Radius = pst_SphOrig->f_Radius;
		if(_pst_Pos)
			MATH_CopyVector(&pst_Sphere->st_Center, _pst_Pos);
		else
			MATH_CopyVector(&pst_Sphere->st_Center, &pst_SphOrig->st_Center);
		_pst_Cob->uc_Flag = _pst_Orig->uc_Flag;
		_pst_Cob->uc_Type = COL_C_Zone_Sphere;
	}
	else
	{
		pst_Sphere->f_Radius = 1.0f;
		if(_pst_Pos)
			MATH_CopyVector(&pst_Sphere->st_Center, _pst_Pos);
		else
			MATH_InitVector(&pst_Sphere->st_Center, 0.0f, 0.0f, 0.0f);
		_pst_Cob->uc_Type = COL_C_Zone_Sphere;
	}

#ifdef ACTIVE_EDITORS
	_pst_Cob->pst_MathCob->pst_Cob = _pst_Cob;

	/*$F--------- Dummy Name -------------- */
	_pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(5);
	_pst_Cob->p_GeoCob = NULL;
	strcpy(_pst_Cob->sz_GMatName, "None");

#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_GeoColMap_PosSet(OBJ_tdst_GameObject *_pst_GO, UCHAR _uc_Cob, MATH_tdst_Vector *_pst_Center)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_ColMap *pst_ColMap;
	COL_tdst_Cob	*pst_Cob;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO || !(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ColMap)) return;

	pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;

	if(pst_ColMap->uc_NbOfCob <= _uc_Cob) return;

	pst_Cob = pst_ColMap->dpst_Cob[_uc_Cob];

	switch(pst_Cob->uc_Type)
	{
	case COL_C_Zone_Sphere:
		MATH_CopyVector(&((COL_tdst_Sphere *) pst_Cob->pst_MathCob->p_Shape)->st_Center, _pst_Center);
		break;

	case COL_C_Zone_Cylinder:
		MATH_CopyVector(&((COL_tdst_Cylinder *) pst_Cob->pst_MathCob->p_Shape)->st_Center, _pst_Center);
		break;

	}
}

/*
 =======================================================================================================================
    Aim:    Add a Cylinder Mathematical Cob to an object.
 =======================================================================================================================
 */
void COL_ColMap_AddCylinder(COL_tdst_Cob *_pst_Cob, COL_tdst_Cob *_pst_Orig, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cylinder	*pst_Cyl;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Cob->pst_MathCob->p_Shape = (COL_tdst_Cylinder *) MEM_p_Alloc(sizeof(COL_tdst_Cylinder));
	pst_Cyl = (COL_tdst_Cylinder *) _pst_Cob->pst_MathCob->p_Shape;
	if(_pst_Orig)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_Cylinder	*pst_CylOrig;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_CylOrig = (COL_tdst_Cylinder *) _pst_Orig->pst_MathCob->p_Shape;
		pst_Cyl->f_Radius = pst_CylOrig->f_Radius;
		if(_pst_Pos)
			MATH_CopyVector(&pst_Cyl->st_Center, _pst_Pos);
		else
			MATH_CopyVector(&pst_Cyl->st_Center, &pst_CylOrig->st_Center);
		pst_Cyl->f_Height = pst_CylOrig->f_Height;
		_pst_Cob->uc_Flag = _pst_Orig->uc_Flag;
		_pst_Cob->uc_Type = COL_C_Zone_Cylinder;
	}
	else
	{
		pst_Cyl->f_Radius = 1.0f;
		if(_pst_Pos)
			MATH_CopyVector(&pst_Cyl->st_Center, _pst_Pos);
		else
			MATH_InitVector(&pst_Cyl->st_Center, 0.0f, 0.0f, 0.0f);
		pst_Cyl->f_Height = 1.0f;
		_pst_Cob->uc_Type = COL_C_Zone_Cylinder;
	}

#ifdef ACTIVE_EDITORS
	_pst_Cob->pst_MathCob->pst_Cob = _pst_Cob;

	/*$F--------- Dummy Name -------------- */
	_pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(5);
	_pst_Cob->p_GeoCob = NULL;
	strcpy(_pst_Cob->sz_GMatName, "None");

#endif
}

/*
 =======================================================================================================================
    Aim:    Add a Cone Mathematical Cob to an object.
 =======================================================================================================================
 */
void COL_ColMap_AddCone(COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cone	*pst_Cone;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	_pst_Cob->pst_MathCob->p_Shape = (COL_tdst_Cylinder *) MEM_p_Alloc(sizeof(COL_tdst_Sphere));
	pst_Cone = (COL_tdst_Cone *) _pst_Cob->pst_MathCob->p_Shape;
	pst_Cone->f_Angle = 1.0f;
	MATH_InitVector(&pst_Cone->st_Base, 0.0f, 0.0f, 1.0f);

	COL_Cob_CopyFlag(_pst_Cob, COL_C_Zone_ZDM);
	_pst_Cob->uc_Type = COL_C_Zone_Cone;

#ifdef ACTIVE_EDITORS
	_pst_Cob->pst_MathCob->pst_Cob = _pst_Cob;

	/*$F--------- Dummy Name -------------- */
	_pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(5);
	_pst_Cob->p_GeoCob = NULL;
	strcpy(_pst_Cob->sz_GMatName, "None");

#endif
}

/*
 =======================================================================================================================
    Aim:    Add a Triangle based Geometric Cob to an object.
 =======================================================================================================================
 */
void COL_ColMap_AddGeometric(COL_tdst_Cob *_pst_Cob, GEO_tdst_Object *_pst_Object, MATH_tdst_Vector *_pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject					*pst_GO;
	GEO_tdst_Object						*pst_Object;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
	GEO_tdst_IndexedTriangle			*pst_Triangle;
	GEO_tdst_StaticLOD					*pst_LOD;
	COL_tdst_IndexedTriangles			*pst_CobObj;
	COL_tdst_ElementIndexedTriangles	*pst_CobElement, *pst_CobLastElement;
	COL_tdst_IndexedTriangle			*pst_CobTriangle, *pst_CobLastTriangle;
	ULONG								ul_Triangle, ul_T1, ul_T2, ul_T3;
	MATH_tdst_Vector					*pst_T1, *pst_T2, *pst_T3;
	MATH_tdst_Vector					st_VectT1T2, st_VectT1T3, st_Norm;
	int									i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Object)
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Pos;
		pst_Object = _pst_Object;
	}
	else
	{
		pst_GO = (OBJ_tdst_GameObject *) _pst_Pos;
		if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu)) return;

		pst_Object = (GEO_tdst_Object *) pst_GO->pst_Base->pst_Visu->pst_Object;

		if(pst_Object && ((GRO_tdst_Struct *) pst_Object)->i->ul_Type == GRO_GeoStaticLOD)
		{
			pst_LOD = (GEO_tdst_StaticLOD *) pst_Object;
			pst_Object = (GEO_tdst_Object *) pst_LOD->dpst_Id[0];
		}
	}

	if(!pst_Object || !pst_Object->l_NbPoints || !pst_Object->l_NbElements) return;

	_pst_Cob->uc_Type = COL_C_Zone_Triangles;
	_pst_Cob->pst_TriangleCob = (COL_tdst_IndexedTriangles *) MEM_p_Alloc(sizeof(COL_tdst_IndexedTriangles));

	pst_CobObj = (COL_tdst_IndexedTriangles *) _pst_Cob->pst_TriangleCob;

	pst_CobObj->pst_OK3 = NULL;

	/* If the object has MRM, we want a 200% precision. */
	GEO_MRM_SetNumberOfPoints_Percent(pst_Object, 2.0f);

	/*$F ----------------- Points -------------------*/
	pst_CobObj->l_NbPoints = pst_Object->l_NbPoints;

	if(pst_GO && MATH_b_TestScaleType(OBJ_pst_GetAbsoluteMatrix(pst_GO)))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_Scale, *_pst_CobPoint;
		GEO_Vertex			*pst_Point, *pst_LastPoint;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_CobObj->dst_Point = (MATH_tdst_Vector *) MEM_p_Alloc(sizeof(MATH_tdst_Vector) * pst_CobObj->l_NbPoints);

		MATH_GetScale(&st_Scale, OBJ_pst_GetAbsoluteMatrix(pst_GO));
		pst_Point = pst_Object->dst_Point;
		_pst_CobPoint = pst_CobObj->dst_Point;
		pst_LastPoint = pst_Point + pst_Object->l_NbPoints;
		for(; pst_Point < pst_LastPoint; pst_Point++, _pst_CobPoint++)
		{
			_pst_CobPoint->x = pst_Point->x * st_Scale.x;
			_pst_CobPoint->y = pst_Point->y * st_Scale.y;
			_pst_CobPoint->z = pst_Point->z * st_Scale.z;
		}
	}
	else
	{
		pst_CobObj->dst_Point = (MATH_tdst_Vector *) MEM_p_Alloc(sizeof(MATH_tdst_Vector) * pst_CobObj->l_NbPoints);
		L_memcpy(pst_CobObj->dst_Point, pst_Object->dst_Point, sizeof(MATH_tdst_Vector) * pst_CobObj->l_NbPoints);
	}

	/*$F ----------------- Elements ------------------*/
	pst_CobObj->l_NbElements = pst_Object->l_NbElements;
	pst_CobObj->dst_Element = (COL_tdst_ElementIndexedTriangles *) MEM_p_Alloc(sizeof(COL_tdst_ElementIndexedTriangles) * pst_Object->l_NbElements);

	pst_Element = pst_Object->dst_Element;
	pst_LastElement = pst_Element + pst_Object->l_NbElements;

	pst_CobObj->l_NbFaces = 0;
	for(; pst_Element < pst_LastElement; pst_Element++) pst_CobObj->l_NbFaces += pst_Element->l_NbTriangles;

	/*$F ----------------- Normals ------------------*/
	pst_CobObj->dst_FaceNormal = (MATH_tdst_Vector *) MEM_p_Alloc(sizeof(MATH_tdst_Vector) * pst_CobObj->l_NbFaces);

	ul_Triangle = 0;
	pst_CobElement = pst_CobObj->dst_Element;
	pst_CobLastElement = pst_CobElement + pst_CobObj->l_NbElements;
	pst_Element = pst_Object->dst_Element;
	for(i = 0; pst_CobElement < pst_CobLastElement; pst_CobElement++, pst_Element++, i++)
	{
		/* The Default Element has NOT a GameMaterial. */
		pst_CobElement->uc_Flag = 0;

		pst_CobElement->l_MaterialId = pst_Element->l_MaterialId;;
		pst_CobElement->uw_NbTriangles = (USHORT) pst_Element->l_NbTriangles;
		if(pst_CobElement->uw_NbTriangles)
		{
			pst_CobElement->dst_Triangle = (COL_tdst_IndexedTriangle *) MEM_p_Alloc(pst_Element->l_NbTriangles * sizeof(COL_tdst_IndexedTriangle));

			pst_CobTriangle = pst_CobElement->dst_Triangle;
			pst_Triangle = pst_Element->dst_Triangle;
			pst_CobLastTriangle = pst_CobTriangle + pst_CobElement->uw_NbTriangles;

			/* We go thru all the triangles of the current object. */
			for(; pst_CobTriangle < pst_CobLastTriangle; pst_CobTriangle++, pst_Triangle++, ul_Triangle++)
			{
				L_memcpy(pst_CobTriangle->auw_Index, pst_Triangle->auw_Index, 3 * sizeof(USHORT));

				/* Computation of the Normal to the Face */
				ul_T1 = pst_Triangle->auw_Index[0];
				ul_T2 = pst_Triangle->auw_Index[1];
				ul_T3 = pst_Triangle->auw_Index[2];

				/* We get the triangles points. */
				pst_T1 = VCast(&pst_Object->dst_Point[ul_T1]);
				pst_T2 = VCast(&pst_Object->dst_Point[ul_T2]);
				pst_T3 = VCast(&pst_Object->dst_Point[ul_T3]);

				MATH_SubVector(&st_VectT1T2, &pst_CobObj->dst_Point[ul_T2], &pst_CobObj->dst_Point[ul_T1]);
				MATH_SubVector(&st_VectT1T3, &pst_CobObj->dst_Point[ul_T3], &pst_CobObj->dst_Point[ul_T1]);
				MATH_CrossProduct(&st_Norm, &st_VectT1T2, &st_VectT1T3);
				if((st_Norm.x != 0.0f) || (st_Norm.y != 0.0f) || (st_Norm.z != 0.0f))
					MATH_NormalizeVector(pst_CobObj->dst_FaceNormal + ul_Triangle, &st_Norm);
				else
					MATH_CopyVector(pst_CobObj->dst_FaceNormal + ul_Triangle, &st_Norm);
			}
		}

#ifdef ACTIVE_EDITORS
		pst_CobElement->pst_Cob = _pst_Cob;
#endif
	}

#ifdef ACTIVE_EDITORS
	_pst_Cob->ul_EditedElement = 0xFFFFFFFF;

	/*$F--------- Dummy Name -------------- */
	_pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(5);
	_pst_Cob->p_GeoCob = NULL;
	_pst_Cob->uc_Flag = 0;
	strcpy(_pst_Cob->sz_GMatName, "None");

#endif
}


void COL_ColMap_RecomputeNormals(COL_tdst_IndexedTriangles *pst_CobObj,GEO_tdst_Object *_pst_Object)
{
	ULONG								i,ul_Triangle, ul_T1, ul_T2, ul_T3;
	MATH_tdst_Vector					*pst_T1, *pst_T2, *pst_T3;
	COL_tdst_ElementIndexedTriangles	*pst_CobElement, *pst_CobLastElement;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	COL_tdst_IndexedTriangle			*pst_CobTriangle, *pst_CobLastTriangle;
	GEO_tdst_IndexedTriangle			*pst_Triangle;
	MATH_tdst_Vector					st_VectT1T2, st_VectT1T3, st_Norm;

    ul_Triangle = 0;

	pst_CobElement = pst_CobObj->dst_Element;
	pst_CobLastElement = pst_CobElement + pst_CobObj->l_NbElements;
	pst_Element = _pst_Object->dst_Element;
	for(i = 0; pst_CobElement < pst_CobLastElement; pst_CobElement++, pst_Element++, i++)
	{
		if(  pst_CobElement->uw_NbTriangles 
		 && (pst_CobElement->uw_NbTriangles == pst_Element->l_NbTriangles))
		{
			pst_CobTriangle = pst_CobElement->dst_Triangle;
			pst_Triangle = pst_Element->dst_Triangle;
			
			ERR_X_Assert(pst_Triangle!=NULL);
			ERR_X_Assert(pst_Element->l_NbTriangles == pst_CobElement->uw_NbTriangles);
			
			pst_CobLastTriangle = pst_CobTriangle + pst_CobElement->uw_NbTriangles;

			/* We go thru all the triangles of the current object. */
			for(; pst_CobTriangle < pst_CobLastTriangle; pst_CobTriangle++, pst_Triangle++, ul_Triangle++)
			{
				ERR_X_Assert(pst_Triangle!=NULL);
				/* Computation of the Normal to the Face */
				ul_T1 = pst_Triangle->auw_Index[0];
				ul_T2 = pst_Triangle->auw_Index[1];
				ul_T3 = pst_Triangle->auw_Index[2];

				/* We get the triangles points. */
				pst_T1 = VCast(&_pst_Object->dst_Point[ul_T1]);
				pst_T2 = VCast(&_pst_Object->dst_Point[ul_T2]);
				pst_T3 = VCast(&_pst_Object->dst_Point[ul_T3]);

				MATH_SubVector(&st_VectT1T2, &pst_CobObj->dst_Point[ul_T2], &pst_CobObj->dst_Point[ul_T1]);
				MATH_SubVector(&st_VectT1T3, &pst_CobObj->dst_Point[ul_T3], &pst_CobObj->dst_Point[ul_T1]);
				MATH_CrossProduct(&st_Norm, &st_VectT1T2, &st_VectT1T3);
				if((st_Norm.x != 0.0f) || (st_Norm.y != 0.0f) || (st_Norm.z != 0.0f))
					MATH_NormalizeVector(pst_CobObj->dst_FaceNormal + ul_Triangle, &st_Norm);
				else
					MATH_CopyVector(pst_CobObj->dst_FaceNormal + ul_Triangle, &st_Norm);
			}
		}
	}
}


#ifdef ACTIVE_EDITORS
void	COL_GMat_UpdateFileName(COL_tdst_Cob *);
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ComputeProximity(COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG								i, j, k, l, Pt1, Pt2, Pt3;
	USHORT								m;
	COL_tdst_IndexedTriangles			*pst_IndexedTriangles_Cob;
	COL_tdst_ElementIndexedTriangles	*pst_Element1, *pst_Element2;
	COL_tdst_IndexedTriangle			*pst_Triangle1, *pst_Triangle2;
	USHORT								uw_T1_1, uw_T1_2, uw_T1_3, uw_T2_1, uw_T2_2, uw_T2_3;
	BOOL								b_12, b_13, b_23;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_IndexedTriangles_Cob = _pst_Cob->pst_TriangleCob;

	k = 0;
	for(i = 0; i < pst_IndexedTriangles_Cob->l_NbElements; i++)
	{
		pst_Element1 = &pst_IndexedTriangles_Cob->dst_Element[i];

		for(j = 0; j < (ULONG) pst_Element1->uw_NbTriangles; j++)
		{
			pst_Triangle1 = &pst_Element1->dst_Triangle[j];

			uw_T1_1 = pst_Triangle1->auw_Index[0];
			uw_T1_2 = pst_Triangle1->auw_Index[1];
			uw_T1_3 = pst_Triangle1->auw_Index[2];

			pst_Triangle1->auw_Prox[0] = -1;
			pst_Triangle1->auw_Prox[1] = -1;
			pst_Triangle1->auw_Prox[2] = -1;

			m = 0;
			b_12 = FALSE;
			b_13 = FALSE;
			b_23 = FALSE;
			for(k = 0; k < pst_IndexedTriangles_Cob->l_NbElements; k++)
			{
				pst_Element2 = &pst_IndexedTriangles_Cob->dst_Element[k];

				for(l = 0; l < (ULONG) pst_Element2->uw_NbTriangles; l++, m++)
				{
					pst_Triangle2 = &pst_Element2->dst_Triangle[l];

					if(pst_Triangle2 == pst_Triangle1) continue;

					uw_T2_1 = pst_Triangle2->auw_Index[0];
					uw_T2_2 = pst_Triangle2->auw_Index[1];
					uw_T2_3 = pst_Triangle2->auw_Index[2];

					Pt1 = ((uw_T1_1 == uw_T2_1) || (uw_T1_1 == uw_T2_2) || (uw_T1_1 == uw_T2_3));
					Pt2 = ((uw_T1_2 == uw_T2_1) || (uw_T1_2 == uw_T2_2) || (uw_T1_2 == uw_T2_3));
					Pt3 = ((uw_T1_3 == uw_T2_1) || (uw_T1_3 == uw_T2_2) || (uw_T1_3 == uw_T2_3));

					if(Pt1 + Pt2 + Pt3 == 2)
					{
						if(Pt1 && Pt2)
						{
							pst_Triangle1->auw_Prox[0] = m;
							b_12 = TRUE;
						}
						else
						{
							if(Pt1 && Pt3)
							{
								pst_Triangle1->auw_Prox[1] = m;
								b_13 = TRUE;
							}
							else
							{
								pst_Triangle1->auw_Prox[2] = m;
								b_23 = TRUE;
							}
						}

						if(b_12 && b_13 && b_23) break;
					}
				}

				if(b_12 && b_13 && b_23) break;
			}
		}
	}
}


/*
 =======================================================================================================================
    Aim:    Add a Cob to the ColMap of an object.
 =======================================================================================================================
 */
void COL_ColMap_AddCob(OBJ_tdst_GameObject *_pst_GO, UCHAR uc_Type, void *_pv_Data1, void *_pv_Data2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_Cob	*pst_Cob;
	COL_tdst_ColMap *pst_ColMap;
#ifdef ACTIVE_EDITORS
	char			asz_Path[BIG_C_MaxLenPath];
	char			asz_Name[BIG_C_MaxLenName];
	char			*psz_Temp;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_ColMap = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap;

	if(pst_ColMap->uc_NbOfCob == COL_Cul_MaxNbOfCob)
	{
		ERR_X_ForceError("Sorry, Max number of Cobs reached", NULL);
		return;
	}

	/* Allocation of the Cob */
	if(!pst_ColMap->uc_NbOfCob)
	{
		pst_ColMap->dpst_Cob = (COL_tdst_Cob **) MEM_p_Alloc(sizeof(COL_tdst_Cob *));
		L_memset(pst_ColMap->dpst_Cob, 0, sizeof(COL_tdst_Cob *));
		*(pst_ColMap->dpst_Cob) = (COL_tdst_Cob *) MEM_p_Alloc(sizeof(COL_tdst_Cob));
		L_memset(*(pst_ColMap->dpst_Cob), 0, sizeof(COL_tdst_Cob));
		pst_Cob = *(pst_ColMap->dpst_Cob);
	}
	else
	{
		pst_ColMap->dpst_Cob = (COL_tdst_Cob **) MEM_p_Realloc
			(
				pst_ColMap->dpst_Cob,
				(pst_ColMap->uc_NbOfCob + 1) * sizeof(COL_tdst_Cob *)
			);
		*(pst_ColMap->dpst_Cob + pst_ColMap->uc_NbOfCob) = (COL_tdst_Cob *) MEM_p_Alloc(sizeof(COL_tdst_Cob));
		pst_Cob = *(pst_ColMap->dpst_Cob + pst_ColMap->uc_NbOfCob);
	}

	pst_Cob->uw_NbOfInstances = 0;
	pst_Cob->pst_GMatList = NULL;

	switch(uc_Type)
	{
	case COL_C_Zone_Sphere:
		pst_Cob->pst_MathCob = (COL_tdst_Mathematical *) MEM_p_Alloc(sizeof(COL_tdst_Mathematical));
		L_memset(pst_Cob->pst_MathCob, 0, sizeof(COL_tdst_Mathematical));
		COL_ColMap_AddSphere(pst_Cob, (COL_tdst_Cob *) _pv_Data1, (MATH_tdst_Vector *) _pv_Data2);
		break;

	case COL_C_Zone_Triangles:
		{
			COL_ColMap_AddGeometric(pst_Cob, (GEO_tdst_Object *) _pv_Data1, (MATH_tdst_Vector *) _pst_GO);

#ifdef ACTIVE_EDITORS
			if(1)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				char		asz_PathToCob[BIG_C_MaxLenPath];
				char		asz_CobName[BIG_C_MaxLenName];
				char		*psz_Temp;
				BIG_INDEX	ul_CobFat;
				ULONG		ul_Address;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				if(_pst_GO->pst_World)
				{
					WOR_GetSubPath(_pst_GO->pst_World, EDI_Csz_Path_COLObjects, asz_PathToCob);
					L_strcpy(asz_CobName, _pst_GO->sz_Name);
					psz_Temp = L_strrchr(asz_CobName, '.');
					if(psz_Temp) *psz_Temp = 0;
					L_strcat(asz_CobName, EDI_Csz_ExtCOLObject);

					ul_CobFat = BIG_ul_SearchFileExt(asz_PathToCob, asz_CobName);
					if(ul_CobFat != BIG_C_InvalidIndex)
					{
						ul_Address = LOA_ul_SearchAddress(BIG_PosFile(ul_CobFat));
						if(ul_Address != -1)
						{
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
							COL_tdst_Cob						*pst_MemoryCob;
							USHORT								uw_NbInstances;
							COL_tdst_ElementIndexedTriangles	*pst_Element, *pst_LastElement;
							COL_tdst_IndexedTriangles			*pst_Col;
							COL_tdst_GameMatList				*pst_GMatList;
							LONG								al_ID[500];
							UCHAR								uc_Index, uc_Total;
							/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

							pst_MemoryCob = (COL_tdst_Cob *) ul_Address;

							if(pst_MemoryCob->uc_Type == COL_C_Zone_Triangles)
							{
								pst_Col = pst_MemoryCob->pst_TriangleCob;

								/* Free the Geometric Cob */
								if(pst_MemoryCob->p_GeoCob)
								{
									/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
									GEO_tdst_ElementIndexedTriangles	*pst_GeoElement, *pst_LastElement;
									GEO_tdst_Object						*pst_GeoCob;
									/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

									pst_GeoCob = (GEO_tdst_Object *) pst_MemoryCob->p_GeoCob;
									if(pst_GeoCob->dst_Point) MEM_Free(pst_GeoCob->dst_Point);
									if(pst_GeoCob->dst_PointNormal) MEM_Free(pst_GeoCob->dst_PointNormal);

									pst_GeoElement = pst_GeoCob->dst_Element;
									pst_LastElement = pst_GeoElement + pst_GeoCob->l_NbElements;
									for(; pst_GeoElement < pst_LastElement; pst_GeoElement++)
									{
										if(pst_GeoElement->dst_Triangle) MEM_Free(pst_GeoElement->dst_Triangle);
									}

									if(pst_GeoCob->dst_Element) MEM_Free(pst_GeoCob->dst_Element);

									if(pst_GeoCob->pst_SubObject) GEO_SubObject_Free(pst_GeoCob);

									MEM_Free(pst_GeoCob);
								}

								uc_Index = 0;
								pst_Element = pst_Col->dst_Element;
								pst_LastElement = pst_Element + pst_Col->l_NbElements;
								for(; pst_Element < pst_LastElement; pst_Element++)
								{
									al_ID[uc_Index++] = pst_Element->l_MaterialId;
									if(pst_Element->uw_NbTriangles && pst_Element->dst_Triangle)
										MEM_Free(pst_Element->dst_Triangle);
								}

								MEM_Free(pst_Col->dst_Element);
								MEM_Free(pst_Col->dst_FaceNormal);
								MEM_Free(pst_Col->dst_Point);
								MEM_Free(pst_MemoryCob->pst_TriangleCob);
								MEM_Free(pst_MemoryCob->sz_GMatName);
								pst_MemoryCob->sz_GMatName = NULL;

								pst_GMatList = pst_MemoryCob->pst_GMatList;
								uw_NbInstances = pst_MemoryCob->uw_NbOfInstances;

								MEM_Free(pst_Cob->sz_GMatName);
								pst_Cob->sz_GMatName = NULL;
								L_memcpy(pst_MemoryCob, pst_Cob, sizeof(COL_tdst_Cob));

								pst_MemoryCob->pst_GMatList = pst_GMatList;
								pst_MemoryCob->uw_NbOfInstances = uw_NbInstances;

								*(pst_ColMap->dpst_Cob + pst_ColMap->uc_NbOfCob) = pst_MemoryCob;

								MEM_Free(pst_Cob);

								pst_Cob = pst_MemoryCob;

								uc_Total = uc_Index;
								uc_Index = 0;
								pst_Element = pst_Cob->pst_TriangleCob->dst_Element;
								pst_LastElement = pst_Element + pst_Cob->pst_TriangleCob->l_NbElements;
								for(; pst_Element < pst_LastElement; pst_Element++)
								{
									if(uc_Index < uc_Total) pst_Element->l_MaterialId = al_ID[uc_Index++];

									pst_Element->pst_Cob = pst_Cob;
								}

								COL_GMat_UpdateFileName(pst_Cob);
							}
						}
					}
				}
			}

#endif

			if(!LOA_IsBinaryData() && (pst_Cob->uc_Type == COL_C_Zone_Triangles)) COL_ComputeProximity(pst_Cob);
#ifdef JADEFUSION
			if(!LOA_IsBinaryData() && (pst_Cob->uc_Type == COL_C_Zone_Triangles)) COL_OK3_Build((void*)_pst_GO, (BOOL)TRUE, (BOOL)FALSE);
#else
			if(!LOA_IsBinaryData() && (pst_Cob->uc_Type == COL_C_Zone_Triangles)) COL_OK3_Build(_pst_GO, TRUE, FALSE);
#endif
			break;
		}
	}

	pst_ColMap->uc_NbOfCob++;

	pst_Cob->uw_NbOfInstances++;

#ifdef ACTIVE_EDITORS
	pst_Cob->pst_Itself = pst_Cob;
	pst_Cob->pst_GO = _pst_GO;

	WOR_GetSubPath(_pst_GO->pst_World, EDI_Csz_Path_COLObjects, asz_Path);

	L_strcpy(asz_Name, _pst_GO->sz_Name);
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtCOLObject);

	LINK_RegisterPointer(pst_Cob, LINK_C_Cob, asz_Name, asz_Path);
#endif
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void COL_FindEdgeInFollowDirection
(
	MATH_tdst_Vector	*_pst_Hit,
	MATH_tdst_Vector	**_ppst_A,
	MATH_tdst_Vector	**_ppst_B,
	MATH_tdst_Vector	**_ppst_C,
	MATH_tdst_Vector	*_pst_Follow,
	MATH_tdst_Vector	*_pst_Right,
	MATH_tdst_Vector	*_pst_Normal,
	BOOL				b_Down
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Toto;
	MATH_tdst_Vector	*pst_Zob;
	BOOL				b_Right;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/


	if((_pst_Follow->x == 0.0f) && (_pst_Follow->y == 0.0f) && (_pst_Follow->z == 1.0f))
	{
		if(!b_Down)
			pst_Zob = (((*_ppst_A)->z >= (*_ppst_B)->z) && ((*_ppst_A)->z >= (*_ppst_C)->z)) ? *_ppst_A : (((*_ppst_B)->z >= (*_ppst_A)->z) && ((*_ppst_B)->z >= (*_ppst_C)->z)) ? *_ppst_B : *_ppst_C;
		else
			pst_Zob = (((*_ppst_A)->z <= (*_ppst_B)->z) && ((*_ppst_A)->z <= (*_ppst_C)->z)) ? *_ppst_A : (((*_ppst_B)->z <= (*_ppst_A)->z) && ((*_ppst_B)->z <= (*_ppst_C)->z)) ? *_ppst_B : *_ppst_C;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	st_HA, st_HB, st_HC;
		float				f_A, f_B, f_C;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_SubVector(&st_HA, *_ppst_A, _pst_Hit);
		MATH_SubVector(&st_HB, *_ppst_B, _pst_Hit);
		MATH_SubVector(&st_HC, *_ppst_C, _pst_Hit);

		f_A = MATH_f_DotProduct(&st_HA, _pst_Follow);
		f_B = MATH_f_DotProduct(&st_HB, _pst_Follow);
		f_C = MATH_f_DotProduct(&st_HC, _pst_Follow);

		if(!b_Down)
			pst_Zob = ((f_A >= f_B) && (f_A >= f_C)) ? *_ppst_A : ((f_B >= f_A) && (f_B >= f_C) ? *_ppst_B : *_ppst_C);
		else
			pst_Zob = ((f_A <= f_B) && (f_A <= f_C)) ? *_ppst_A : ((f_B <= f_A) && (f_B <= f_C) ? *_ppst_B : *_ppst_C);
	}

	MATH_SubVector(&st_Toto, pst_Zob, _pst_Hit);
	b_Right = MATH_f_DotProduct(&st_Toto, _pst_Right) <= 0.0f ? TRUE : FALSE;


	if(!b_Down)
	{
		*_ppst_A = (((pst_Zob == *_ppst_B) && !b_Right) || ((pst_Zob == *_ppst_C) && b_Right)) ? *_ppst_C : *_ppst_A;
		*_ppst_B = ((*_ppst_C != *_ppst_A) && (((pst_Zob == *_ppst_A) && b_Right) || ((pst_Zob == *_ppst_C) && !b_Right))) ? *_ppst_C : *_ppst_B;
	}
	else
	{
		*_ppst_A = (((pst_Zob == *_ppst_B) && b_Right) || ((pst_Zob == *_ppst_C) && !b_Right)) ? *_ppst_C : *_ppst_A;
		*_ppst_B = ((*_ppst_C != *_ppst_A) &&(((pst_Zob == *_ppst_A) && !b_Right) || ((pst_Zob == *_ppst_C) && b_Right))) ? *_ppst_C : *_ppst_B;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
COL_tdst_IndexedTriangle *COL_GetPointConnectedTriangleWithMuchHorizontalEdge
(
	MATH_tdst_Vector					*_pst_Pt1,
	MATH_tdst_Vector					*_pst_A,
	MATH_tdst_Vector					*_pst_B,
	MATH_tdst_Vector					*_pst_Follow,
	COL_tdst_IndexedTriangles			*_pst_CobObj,
	MATH_tdst_Matrix					*_pst_Matrix,
	COL_tdst_ElementIndexedTriangles	*_pst_Element
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector			*pst_A, *pst_B, *pst_C;
	MATH_tdst_Vector			st_GCS_A, st_GCS_B, st_GCS_C;
	MATH_tdst_Vector			st_Edge;
	COL_tdst_IndexedTriangle	*pst_Current, *pst_Result;
	float						fMinDot;
	ULONG						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Result = NULL;
	fMinDot = 100000.0f;

	for(i = 0; i < _pst_Element->uw_NbTriangles; i++)
	{
		pst_Current = &_pst_Element->dst_Triangle[i];

		pst_A = &_pst_CobObj->dst_Point[pst_Current->auw_Index[0]];
		pst_B = &_pst_CobObj->dst_Point[pst_Current->auw_Index[1]];
		pst_C = &_pst_CobObj->dst_Point[pst_Current->auw_Index[2]];

		if((_pst_Pt1 == pst_A) || (_pst_Pt1 == pst_B) || (_pst_Pt1 == pst_C))
		{
			MATH_TransformVertexNoScale(&st_GCS_A, _pst_Matrix, pst_A);

			MATH_TransformVertexNoScale(&st_GCS_B, _pst_Matrix, pst_B);
			MATH_TransformVertexNoScale(&st_GCS_C, _pst_Matrix, pst_C);

			if((_pst_Pt1 == pst_A) || (_pst_Pt1 == pst_B))
			{
				MATH_SubVector(&st_Edge, &st_GCS_A, &st_GCS_B);
				MATH_NormalizeEqualVector(&st_Edge);
				if(fAbs(MATH_f_DotProduct(&st_Edge, _pst_Follow)) < fMinDot)
				{
					fMinDot = fAbs(MATH_f_DotProduct(&st_Edge, _pst_Follow));
					pst_Result = pst_Current;
					MATH_CopyVector(_pst_A, (_pst_Pt1 == pst_A) ? &st_GCS_A : &st_GCS_B);
					MATH_CopyVector(_pst_B, (_pst_Pt1 == pst_A) ? &st_GCS_B : &st_GCS_A);
					if(fEqWithEpsilon(fMinDot, 0.0f, 1E-3f)) break;
				}
			}
			
			if((_pst_Pt1 == pst_A) || (_pst_Pt1 == pst_C))
			{
				MATH_SubVector(&st_Edge, &st_GCS_A, &st_GCS_C);
				MATH_NormalizeEqualVector(&st_Edge);
				if(fAbs(MATH_f_DotProduct(&st_Edge, _pst_Follow)) < fMinDot)
				{
					fMinDot = fAbs(MATH_f_DotProduct(&st_Edge, _pst_Follow));
					pst_Result = pst_Current;
					MATH_CopyVector(_pst_A, (_pst_Pt1 == pst_A) ? &st_GCS_A : &st_GCS_C);
					MATH_CopyVector(_pst_B, (_pst_Pt1 == pst_A) ? &st_GCS_C : &st_GCS_A);
					if(fEqWithEpsilon(fMinDot, 0.0f, 1E-3f)) break;
				}
			}

			if((_pst_Pt1 == pst_B) || (_pst_Pt1 == pst_C))
			{
				MATH_SubVector(&st_Edge, &st_GCS_B, &st_GCS_C);
				MATH_NormalizeEqualVector(&st_Edge);
				if(fAbs(MATH_f_DotProduct(&st_Edge, _pst_Follow)) < fMinDot)
				{
					fMinDot = fAbs(MATH_f_DotProduct(&st_Edge, _pst_Follow));
					pst_Result = pst_Current;
					MATH_CopyVector(_pst_A, (_pst_Pt1 == pst_B) ? &st_GCS_B : &st_GCS_C);
					MATH_CopyVector(_pst_B, (_pst_Pt1 == pst_B) ? &st_GCS_C : &st_GCS_B);
					if(fEqWithEpsilon(fMinDot, 0.0f, 1E-3f)) break;
				}
			}
		}
	}

	return pst_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
COL_tdst_IndexedTriangle *COL_GetConnected
(
	MATH_tdst_Vector					*_pst_EdgePt1,
	MATH_tdst_Vector					*_pst_EdgePt2,
	OBJ_tdst_GameObject					*_pst_CobGO,
	COL_tdst_IndexedTriangles			*_pst_CobObj,
	COL_tdst_ElementIndexedTriangles	*_pst_Element,
	COL_tdst_IndexedTriangle			*_pst_Triangle,
	BOOL								_b_OnlyElement,
	BOOL								_b_StopOnGround,
	BOOL								_b_StopOnWall
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World				*pst_World;
	MATH_tdst_Vector			*pst_A, *pst_B, *pst_C;
	MATH_tdst_Vector			st_InvUnitG;
	COL_tdst_ElementIndexedTriangles	*pst_Elem, *pst_Elem2;
	COL_tdst_IndexedTriangle	*pst_Current, *pst_Result;
	ULONG						i, j, k;
	BOOL						b_Ground;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_CobGO);

	pst_Result = NULL;
	if(_b_OnlyElement)
	{
		for(i = 0; i < _pst_Element->uw_NbTriangles; i++)
		{
			pst_Current = &_pst_Element->dst_Triangle[i];
			if(pst_Current == _pst_Triangle) continue;

			pst_A = &_pst_CobObj->dst_Point[pst_Current->auw_Index[0]];
			pst_B = &_pst_CobObj->dst_Point[pst_Current->auw_Index[1]];
			pst_C = &_pst_CobObj->dst_Point[pst_Current->auw_Index[2]];

			if((_pst_EdgePt1 != pst_A) && (_pst_EdgePt1 != pst_B) && (_pst_EdgePt1 != pst_C)) continue;
			if((_pst_EdgePt2 != pst_A) && (_pst_EdgePt2 != pst_B) && (_pst_EdgePt2 != pst_C)) continue;

			if(_b_StopOnGround || _b_StopOnWall)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	*pst_Normal;
				MATH_tdst_Vector	st_GCS_Normal;
				float				f;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_Elem2 = &_pst_CobObj->dst_Element[0];

				k = 0;
				while(pst_Elem2 != _pst_Element) 
				{
					k += pst_Elem2->uw_NbTriangles;
					pst_Elem2 ++;
				}

				k+= i;

				pst_Normal = &_pst_CobObj->dst_FaceNormal[k];

				MATH_TransformVectorNoScale(&st_GCS_Normal, _pst_CobGO->pst_GlobalMatrix, pst_Normal);

				if(COL_gst_GlobalVars.pst_InvUnitG)
					MATH_CopyVector(&st_InvUnitG, COL_gst_GlobalVars.pst_InvUnitG);
				else
					MATH_InitVector(&st_InvUnitG, 0.0f, 0.0f, 1.0f);

				f = MATH_f_DotProduct(&st_InvUnitG, &st_GCS_Normal);

				b_Ground = (!((f >= 0.0f) && (f < COL_gst_GlobalVars.f_WallCosAngle)) && !((f < 0.0f) && (f > -0.80f)));

				if(_b_StopOnGround && b_Ground)
				{
					MATH_CopyVector(&pst_World->st_RayInfo.st_EdgeNormal, &st_GCS_Normal);
					continue;
				}

				if(_b_StopOnWall &&	!b_Ground)
				{
					MATH_CopyVector(&pst_World->st_RayInfo.st_EdgeNormal, &st_GCS_Normal);
					continue;
				}
			}

			MATH_InitVector(&pst_World->st_RayInfo.st_EdgeNormal, 0.0f, 0.0f, 0.0f);
			pst_Result = pst_Current;
			break;
		}
	}
	else
	{
		for(j = 0; j < _pst_CobObj->l_NbElements; j++)
		{
			pst_Elem = &_pst_CobObj->dst_Element[j];

			for(i = 0; i < pst_Elem->uw_NbTriangles; i++)
			{
				pst_Current = &pst_Elem->dst_Triangle[i];
				if(pst_Current == _pst_Triangle) continue;

				pst_A = &_pst_CobObj->dst_Point[pst_Current->auw_Index[0]];
				pst_B = &_pst_CobObj->dst_Point[pst_Current->auw_Index[1]];
				pst_C = &_pst_CobObj->dst_Point[pst_Current->auw_Index[2]];

				if((_pst_EdgePt1 != pst_A) && (_pst_EdgePt1 != pst_B) && (_pst_EdgePt1 != pst_C)) continue;
				if((_pst_EdgePt2 != pst_A) && (_pst_EdgePt2 != pst_B) && (_pst_EdgePt2 != pst_C)) continue;

				if(_b_StopOnGround || _b_StopOnWall)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					MATH_tdst_Vector	*pst_Normal;
					MATH_tdst_Vector	st_GCS_Normal;
					float				f;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_Elem2 = &_pst_CobObj->dst_Element[0];

					k = 0;
					while(pst_Elem2 != pst_Elem) 
					{
						k += pst_Elem2->uw_NbTriangles;
						pst_Elem2 ++;
					}

					k+= i;

					pst_Normal = &_pst_CobObj->dst_FaceNormal[k];

					MATH_TransformVectorNoScale(&st_GCS_Normal, _pst_CobGO->pst_GlobalMatrix, pst_Normal);

					if(COL_gst_GlobalVars.pst_InvUnitG)
						MATH_CopyVector(&st_InvUnitG, COL_gst_GlobalVars.pst_InvUnitG);
					else
						MATH_InitVector(&st_InvUnitG, 0.0f, 0.0f, 1.0f);

					f = MATH_f_DotProduct(&st_InvUnitG, &st_GCS_Normal);

					b_Ground = (!((f >= 0.0f) && (f < COL_gst_GlobalVars.f_WallCosAngle)) && !((f < 0.0f) && (f > -0.80f)));

					if(_b_StopOnGround && b_Ground)
					{
						MATH_CopyVector(&pst_World->st_RayInfo.st_EdgeNormal, &st_GCS_Normal);
						continue;
					}

					if(_b_StopOnWall &&	!b_Ground)
					{
						MATH_CopyVector(&pst_World->st_RayInfo.st_EdgeNormal, &st_GCS_Normal);
						continue;
					}
				}


				MATH_InitVector(&pst_World->st_RayInfo.st_EdgeNormal, 0.0f, 0.0f, 0.0f);
				pst_Result = pst_Current;
				break;
			}
		}
	}

	return pst_Result;
}

float	COL_gf_CosEdgeMaxAngle = 0.5f;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_ComputeJumpPoint
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_Jump,
	MATH_tdst_Vector	*_pst_Follow,
	float				_f_Alpha,
	BOOL				_b_Down,
	BOOL				_b_UseRay,
	BOOL				_b_OnyElement,
	BOOL				_b_StopOnGround,
	BOOL				_b_StopOnWall
	)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World				*pst_World;
	COL_tdst_RayInfo			*pst_RayInfo;
	OBJ_tdst_GameObject			*pst_CobGO;
	COL_tdst_ColMap				*pst_ColMap;
	COL_tdst_Cob				*pst_Cob;
	COL_tdst_IndexedTriangles	*pst_CobObj;
	COL_tdst_IndexedTriangle	*pst_Triangle, *pst_OldTriangle, *pst_OldOldTriangle;
	COL_tdst_Report				*pst_CurrentReport, *pst_LastReport;
	MATH_tdst_Vector			*pst_A, *pst_B, *pst_C;
	MATH_tdst_Vector			st_Right, st_HitA, st_AB, st_AC, st_Norm, st_InvUnitG;
	MATH_tdst_Vector			st_GCS_A, st_GCS_B, st_GCS_C, st_GCS_OldPoint;
	UCHAR						uc_MaxLoop;
	ULONG						ul_Triangle;
	USHORT						uw_Element;
	float						f_Dot, f_Distance;
	BOOL						b_Vertical;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	if(!pst_World) return;


    b_Vertical = (_pst_Follow->x == 0.0f) && (_pst_Follow->y == 0.0f);

	if(!b_Vertical)
		_pst_Follow->z = 0.0f;

	pst_RayInfo = &pst_World->st_RayInfo;
	pst_RayInfo->uc_Flags = 0;

	/* Init */
	MATH_CopyVector(_pst_Jump, &MATH_gst_NulVector);
	MATH_CopyVector(&pst_RayInfo->st_EdgeNormal, &MATH_gst_NulVector);


	if(_b_UseRay)
	{
		MATH_CopyVector(_pst_Jump, &pst_RayInfo->st_CollidedPoint);
		ul_Triangle = pst_RayInfo->ul_Triangle;
		uw_Element = pst_RayInfo->uw_Element;
		pst_CobGO = pst_World->st_RayInfo.pst_CollidedGO;
	}
	else
	{
		pst_CurrentReport = pst_World->ast_Reports;
		pst_LastReport = pst_CurrentReport + pst_World->ul_NbReports;

		for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
		{
			if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Corner) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Inactive) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
			if(pst_CurrentReport->pst_A != _pst_GO) continue;

			if
			(
				(pst_CurrentReport->ul_Flag & (b_Vertical ? COL_Cul_Wall : COL_Cul_Ground))
			&&	(pst_CurrentReport->ul_Flag & (COL_Cul_Triangle | COL_Cul_Edge))
			) break;
		}

		if(pst_CurrentReport == pst_LastReport) 
		{
			return;
		}

		MATH_CopyVector(_pst_Jump, &pst_CurrentReport->st_CollidedPoint);
		ul_Triangle = pst_CurrentReport->ul_Triangle;
		uw_Element = pst_CurrentReport->uw_Element;
		pst_CobGO = pst_CurrentReport->pst_B;
	}

	if(!pst_CobGO || !OBJ_b_TestIdentityFlag(pst_CobGO, OBJ_C_IdentityFlag_ColMap)) 
	{
		return;
	}

	pst_ColMap = ((COL_tdst_Base *) pst_CobGO->pst_Extended->pst_Col)->pst_ColMap;
	pst_Cob = pst_ColMap->dpst_Cob[0];
	if(!pst_Cob || (pst_Cob->uc_Type != COL_C_Zone_Triangles)) 
	{
		return;
	}

	pst_CobObj = pst_Cob->pst_TriangleCob;

	pst_OldOldTriangle = NULL;
	pst_OldTriangle = NULL;

	pst_Triangle = &(pst_CobObj->dst_Element[uw_Element].dst_Triangle[ul_Triangle]);

	if(_b_StopOnGround || _b_StopOnWall)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Vector	*pst_Norm;
		MATH_tdst_Vector	st_GCS_Norm;
		float				f;
		int					i, norm;
		BOOL				b_Ground;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		for(i = 0, norm = ul_Triangle; i < uw_Element; i++)
			norm += pst_CobObj->dst_Element[i].uw_NbTriangles;

		pst_Norm = &pst_CobObj->dst_FaceNormal[norm];

		MATH_TransformVector(&st_GCS_Norm, pst_CobGO->pst_GlobalMatrix, pst_Norm);

		if(COL_gst_GlobalVars.pst_InvUnitG)
			MATH_CopyVector(&st_InvUnitG, COL_gst_GlobalVars.pst_InvUnitG);
		else
			MATH_InitVector(&st_InvUnitG, 0.0f, 0.0f, 1.0f);

		f = MATH_f_DotProduct(&st_InvUnitG, &st_GCS_Norm);

		b_Ground = (!((f >= 0.0f) && (f < COL_gst_GlobalVars.f_WallCosAngle)) && !((f < 0.0f) && (f > -0.80f)));

		if((b_Ground && _b_StopOnGround) ||	(!b_Ground && _b_StopOnWall))
		{
			MATH_CopyVector(&pst_RayInfo->st_Normal, &st_GCS_Norm);
			MATH_CopyVector(&pst_RayInfo->st_EdgeNormal, &st_GCS_Norm);
			return;
		}
	}

	MATH_CopyVector(&st_GCS_OldPoint, _pst_Jump);
	uc_MaxLoop = 0;
	do
	{
		MATH_CopyVector(&st_GCS_OldPoint, _pst_Jump);

		MATH_TransformVertexNoScale
		(
			&st_GCS_A,
			OBJ_pst_GetAbsoluteMatrix(pst_CobGO),
			&pst_CobObj->dst_Point[pst_Triangle->auw_Index[0]]
		);
		MATH_TransformVertexNoScale
		(
			&st_GCS_B,
			OBJ_pst_GetAbsoluteMatrix(pst_CobGO),
			&pst_CobObj->dst_Point[pst_Triangle->auw_Index[1]]
		);
		MATH_TransformVertexNoScale
		(
			&st_GCS_C,
			OBJ_pst_GetAbsoluteMatrix(pst_CobGO),
			&pst_CobObj->dst_Point[pst_Triangle->auw_Index[2]]
		);

		pst_A = &st_GCS_A;
		pst_B = &st_GCS_B;
		pst_C = &st_GCS_C;

		MATH_SubVector(&st_AB, pst_B, pst_A);
		MATH_SubVector(&st_AC, pst_C, pst_A);
		MATH_CrossProduct(&st_Norm, &st_AB, &st_AC);
		MATH_NormalizeEqualVector(&st_Norm);

		MATH_CrossProduct(&st_Right, _pst_Follow, &st_Norm);

		COL_FindEdgeInFollowDirection(_pst_Jump, &pst_A, &pst_B, &pst_C, _pst_Follow, &st_Right, &st_Norm, _b_Down);

		MATH_SubVector(&st_AB, pst_B, pst_A);
		f_Distance = MATH_f_Distance(pst_A, pst_B);

		MATH_SubVector(&st_HitA, pst_A, _pst_Jump);

		/* Anti Bug */
		if(MATH_f_DotProduct(&st_AB, &st_Right) == 0.0f)
			f_Dot = 0.5f;
		else
			f_Dot = fDiv(-MATH_f_DotProduct(&st_HitA, &st_Right), MATH_f_DotProduct(&st_AB, &st_Right));

		if(f_Distance > _f_Alpha * 2.0f)
		{
			if(((f_Dot * f_Distance) < _f_Alpha) || (((1 - f_Dot) * f_Distance) < _f_Alpha))
			{
				if(f_Dot < 0.5f)
					f_Dot = _f_Alpha / f_Distance;
				else
					f_Dot = 1.0f - (_f_Alpha / f_Distance);
			}
		}
		else
			f_Dot = 0.5f;

		MATH_MulVector(_pst_Jump, &st_AB, f_Dot);
		MATH_AddEqualVector(_pst_Jump, pst_A);


		pst_OldOldTriangle = pst_OldTriangle;
		pst_OldTriangle = pst_Triangle;


		pst_A = (pst_C == pst_A) ? &pst_CobObj->dst_Point[pst_Triangle->auw_Index[2]] : &pst_CobObj->dst_Point[pst_Triangle->auw_Index[0]];
		pst_B = (pst_C == pst_B) ? &pst_CobObj->dst_Point[pst_Triangle->auw_Index[2]] : &pst_CobObj->dst_Point[pst_Triangle->auw_Index[1]];

		pst_Triangle = COL_GetConnected(pst_A, pst_B, pst_CobGO, pst_CobObj, &pst_CobObj->dst_Element[uw_Element], pst_Triangle, _b_OnyElement, _b_StopOnGround, _b_StopOnWall);

		/* We go from one Triangle to the other one ... Vertical Edge problem */
		if(pst_OldOldTriangle && (pst_OldOldTriangle == pst_Triangle))
		{
			MATH_tdst_Vector	st_G, st_Temp;

			pst_Triangle = pst_OldTriangle;
			pst_OldTriangle = pst_OldOldTriangle;
			pst_OldOldTriangle = NULL;

			pst_A = &st_GCS_A;
			pst_B = &st_GCS_B;
			pst_C = &st_GCS_C;

			MATH_AddVector(&st_G, &st_GCS_A, &st_GCS_B);
			MATH_AddEqualVector(&st_G, &st_GCS_C);
			MATH_ScaleEqualVector(&st_G, 0.333333f);

			MATH_SubVector(&st_Temp, &st_G, &st_GCS_OldPoint);
			MATH_NormalizeEqualVector(&st_Temp);
			MATH_ScaleEqualVector(&st_Temp, 0.1f);
			MATH_AddVector(_pst_Jump, &st_GCS_OldPoint, &st_Temp);
		}

		/* No Triangle are found. Do we want to "climb up" the Edge ?? */
NoTriangles:

		if(!pst_Triangle && (_pst_Follow->z != 0.0f))
		{
			MATH_TransformVertexNoScale(&st_GCS_A, OBJ_pst_GetAbsoluteMatrix(pst_CobGO), pst_A);
			MATH_TransformVertexNoScale(&st_GCS_B, OBJ_pst_GetAbsoluteMatrix(pst_CobGO), pst_B);

			MATH_SubVector(&st_AB, &st_GCS_A, &st_GCS_B);
			MATH_NormalizeEqualVector(&st_AB);

			if(fAbs(MATH_f_DotProduct(&st_AB, _pst_Follow)) > COL_gf_CosEdgeMaxAngle)
			{
				if(_pst_Follow->z != 0.0f)
					pst_A = (_b_Down) ? (st_GCS_A.z > st_GCS_B.z) ? pst_B : pst_A : (st_GCS_A.z > st_GCS_B.z) ? pst_A : pst_B;
				else
				{
					MATH_tdst_Vector	st_Temp;
					float				f_Dot;

					MATH_SubVector(&st_Temp, &st_GCS_B, &st_GCS_A);
					f_Dot = MATH_f_DotProduct(&st_Temp, _pst_Follow);

					pst_A = (_b_Down) ? ((f_Dot > 0.0f) ? pst_A : pst_B) : ((f_Dot > 0.0f) ? pst_B : pst_A);
				}

				pst_Triangle = COL_GetPointConnectedTriangleWithMuchHorizontalEdge
					(
						pst_A,
						&st_GCS_A,
						&st_GCS_B,
						_pst_Follow,
						pst_CobObj,
						OBJ_pst_GetAbsoluteMatrix(pst_CobGO),
						&pst_CobObj->dst_Element[uw_Element]
					);
#ifdef ACTIVE_EDITORS
				if(!pst_Triangle)
				{
					LINK_PrintStatusMsg("Theorically Impossible case in the TopPointGet Procedure");
					break;
				}
#endif

				MATH_SubVector(&st_AB, &st_GCS_B, &st_GCS_A);
				f_Distance = MATH_f_Distance( &st_GCS_B, &st_GCS_A);

				if(f_Distance > _f_Alpha * 2.0f)
					f_Dot = _f_Alpha / f_Distance;
				else
					f_Dot = f_Distance * 0.5f;

				MATH_MulVector(_pst_Jump, &st_AB, f_Dot);
				MATH_AddEqualVector(_pst_Jump, &st_GCS_A) ;
				pst_Triangle = NULL;
				break;
			}
		}

		uc_MaxLoop++;

		if(uc_MaxLoop == 29)
		{
			pst_Triangle = NULL;
			goto NoTriangles;
		}

	} while(pst_Triangle && (uc_MaxLoop < 30));

	if(!pst_Triangle)
	{
		/* Face Normal */
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_AB, st_AC, st_Temp;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_A = &pst_CobObj->dst_Point[pst_OldTriangle->auw_Index[0]];
			pst_B = &pst_CobObj->dst_Point[pst_OldTriangle->auw_Index[1]];
			pst_C = &pst_CobObj->dst_Point[pst_OldTriangle->auw_Index[2]];

			MATH_SubVector(&st_AB, pst_B, pst_A);
			MATH_SubVector(&st_AC, pst_C, pst_A);
			MATH_CrossProduct(&st_Temp, &st_AB, &st_AC);
			MATH_NormalizeEqualVector(&st_Temp);
			MATH_TransformVector(&pst_RayInfo->st_Normal, pst_CobGO->pst_GlobalMatrix, &st_Temp);
		}

		/* Edge Normal */
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Norm;
			float				f_Dot;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			f_Dot = MATH_f_DotProduct(_pst_Follow, &pst_RayInfo->st_EdgeNormal);

			pst_RayInfo->uc_Flags = (f_Dot > 0.0f) ? 1 : ((f_Dot < 0.0f) ? 2 : 0);

			MATH_CrossProduct(&st_Norm, &pst_RayInfo->st_Normal, &st_AB);

			if((!_b_Down && MATH_f_DotProduct(&st_Norm, _pst_Follow) < 0.0f) || (_b_Down && MATH_f_DotProduct(&st_Norm, _pst_Follow) > 0.0f))
				MATH_NegEqualVector(&st_Norm);

			MATH_NormalizeEqualVector(&st_Norm);

			MATH_CopyVector(&pst_RayInfo->st_EdgeNormal, &st_Norm);
		}
	}
	else
	{
		MATH_InitVector(&pst_RayInfo->st_Normal, 0.0f, 0.0f, 0.0f);
	}

		if((_pst_Jump->x == 0.0f) && (_pst_Jump->y == 0.0f) && (_pst_Jump->z == 0.0f))
		{
			_pst_Jump->x = 0.0f;
		}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
COL_tdst_GameMat *COL_pst_GMat_Get(COL_tdst_Cob *_pst_Cob, COL_tdst_ElementIndexedTriangles *_pst_Element)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	i_Deb, i_Fin, i_Mil;
	ULONG	i, ul_ID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Cob->pst_GMatList || !_pst_Cob->pst_GMatList->pst_GMat)
	{
		return NULL;
	}

	if(!_pst_Element && (_pst_Cob->uc_Type == COL_C_Zone_Triangles))
	{
		return NULL;
	}

	if(!_pst_Element)
		ul_ID = _pst_Cob->pst_MathCob->l_MaterialId;
	else
		ul_ID = (ULONG) _pst_Element->l_MaterialId;

	if(_pst_Cob->pst_GMatList->ul_GMat > 10)
	{
		i_Deb = 0;
		i_Fin = _pst_Cob->pst_GMatList->ul_GMat;
		while(1)
		{
			i_Mil = (i_Deb + i_Fin) / 2;

			if(_pst_Cob->pst_GMatList->pal_Id[i_Mil] == ul_ID)
			{
				_pst_Cob->pst_GMatList->pst_GMat[i_Mil].uw_Dummy = (USHORT) ul_ID;
				return(&_pst_Cob->pst_GMatList->pst_GMat[i_Mil]);
			}

			if(i_Deb + 1 == i_Fin) return NULL;

			if(_pst_Cob->pst_GMatList->pal_Id[i_Mil] > ul_ID)
				i_Fin = i_Mil;
			else
				i_Deb = i_Mil;
		}
	}
	else
	{
		for(i = 0; i < _pst_Cob->pst_GMatList->ul_GMat; i++)
		{
			if(_pst_Cob->pst_GMatList->pal_Id[i] == ul_ID)
			{
				_pst_Cob->pst_GMatList->pst_GMat[i].uw_Dummy = (USHORT) ul_ID;
				return(&_pst_Cob->pst_GMatList->pst_GMat[i]);
			}
		}
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG COL_ul_GMat_GetIndex(COL_tdst_Cob *_pst_Cob, COL_tdst_ElementIndexedTriangles *_pst_Element)
{
	/*~~~~~~*/
	ULONG	i;
	/*~~~~~~*/

	if(!_pst_Cob->pst_GMatList || !_pst_Cob->pst_GMatList->pst_GMat) return (ULONG) -1;

	if(!_pst_Element) return 0;

	for(i = 0; i < _pst_Cob->pst_GMatList->ul_GMat; i++)
	{
		if(_pst_Cob->pst_GMatList->pal_Id[i] == (ULONG) _pst_Element->l_MaterialId) return(i);
	}

	return (ULONG) -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_GMat_RemoveAtIndex(COL_tdst_Cob *_pst_Cob, ULONG _ul_Index)
{
	if(!_pst_Cob->pst_GMatList || !_pst_Cob->pst_GMatList->pst_GMat || (_pst_Cob->pst_GMatList->ul_GMat < _ul_Index))
		return;

	/* If this Cob has only one GameMaterial. We destroy the entire structure. */
	if(_pst_Cob->pst_GMatList->ul_GMat == 1)
	{
		if(_pst_Cob->pst_GMatList->pst_GMat) MEM_Free(_pst_Cob->pst_GMatList->pst_GMat);
		if(_pst_Cob->pst_GMatList->pal_Id) MEM_Free(_pst_Cob->pst_GMatList->pal_Id);
		MEM_Free(_pst_Cob->pst_GMatList);
		_pst_Cob->pst_GMatList = NULL;
	}
	else
	{
		L_memcpy
		(
			_pst_Cob->pst_GMatList->pst_GMat + _ul_Index,
			_pst_Cob->pst_GMatList->pst_GMat + _ul_Index + 1,
			(_pst_Cob->pst_GMatList->ul_GMat - (_ul_Index + 1)) * sizeof(COL_tdst_GameMat)
		);
		_pst_Cob->pst_GMatList->pst_GMat = (COL_tdst_GameMat *) MEM_p_Realloc
			(
				_pst_Cob->pst_GMatList->pst_GMat,
				(_pst_Cob->pst_GMatList->ul_GMat - 1) * sizeof(COL_tdst_GameMat)
			);

		L_memcpy
		(
			_pst_Cob->pst_GMatList->pal_Id + _ul_Index,
			_pst_Cob->pst_GMatList->pal_Id + _ul_Index + 1,
			(_pst_Cob->pst_GMatList->ul_GMat - (_ul_Index + 1)) * sizeof(ULONG)
		);
		_pst_Cob->pst_GMatList->pal_Id = (ULONG *) MEM_p_Realloc
			(
				_pst_Cob->pst_GMatList->pal_Id,
				(_pst_Cob->pst_GMatList->ul_GMat - 1) * sizeof(ULONG)
			);

		_pst_Cob->pst_GMatList->ul_GMat--;
	}
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_GMat_DisplayInfo(COL_tdst_GameMatList *_pst_GMatList)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_GameMat	*pst_GMat;
	BIG_KEY				ul_GMatListKey;
	BIG_INDEX			ul_GMatListIndex;
	char				asz_Log[300];
	ULONG				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GMatList) return;

	ul_GMatListKey = LOA_ul_SearchKeyWithAddress((ULONG) _pst_GMatList);
	ul_GMatListIndex = BIG_ul_SearchKeyToFat(ul_GMatListKey);

	if(ul_GMatListIndex != BIG_C_InvalidIndex)
	{
		sprintf(asz_Log, "[%s] (used by %u cob(s)).", BIG_NameFile(ul_GMatListIndex), _pst_GMatList->ul_NbOfInstances);
		LINK_PrintStatusMsg(asz_Log);
		LINK_PrintStatusMsg("--------------------");
	}

	for(i = 0; i < _pst_GMatList->ul_GMat; i++)
	{
		pst_GMat = &(_pst_GMatList->pst_GMat[i]);
		sprintf
		(
			asz_Log,
			"[ID %u] Slide: %.2f\tRebound: %.2f\tSound: %u\tCustom: %x",
			_pst_GMatList->pal_Id[i],
			pst_GMat->f_Slide,
			pst_GMat->f_Rebound,
			pst_GMat->uc_Sound,
			pst_GMat->ul_CustomBits
		);
		LINK_PrintStatusMsg(asz_Log);
	}

	LINK_PrintStatusMsg("--------------------");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_GMat_UpdateFileName(COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_GMatName[100];
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Cob) return;

	if(_pst_Cob->sz_GMatName)
	{
		MEM_Free(_pst_Cob->sz_GMatName);
		_pst_Cob->sz_GMatName = NULL;
	}

	if
	(
		(_pst_Cob->pst_GMatList)
	&&	(LOA_ul_SearchIndexWithAddress((ULONG) _pst_Cob->pst_GMatList) != BIG_C_InvalidIndex)
	)
	{
		L_strcpy(asz_GMatName, BIG_NameFile(LOA_ul_SearchIndexWithAddress((ULONG) _pst_Cob->pst_GMatList)));
		_pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(L_strlen(asz_GMatName) + 1);
		L_strcpy(_pst_Cob->sz_GMatName, asz_GMatName);
	}
	else
	{
		L_strcpy(asz_GMatName, "None");
		_pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(L_strlen(asz_GMatName) + 1);
		L_strcpy(_pst_Cob->sz_GMatName, asz_GMatName);
	}
}

/*
 =======================================================================================================================
    Aim:    Returns all the ColMap's triangles that collide a given sphere.
 =======================================================================================================================
 */
void COL_SphereVsColMap
(
	char				*_pc_Buf,
	ULONG				_ul_MaxFaces,
	ULONG				*_pf_NbFaces,
	MATH_tdst_Vector	*_pst_Center,
	float				_f_Radius,
	WOR_tdst_World		*_pst_World
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem						*pst_CurrentElem;
	TAB_tdst_PFelem						*pst_EndElem;
	OBJ_tdst_GameObject					*pst_GO;
	COL_tdst_ColMap						*pst_ColMap;
	COL_tdst_IndexedTriangles			*pst_CobObj;
	COL_tdst_ElementIndexedTriangles	*pst_CobElement, *pst_LastCobElement;
	COL_tdst_IndexedTriangle			*pst_CobFace, *pst_LastCobFace;
	MATH_tdst_Vector					*pst_Normal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Check the result buffer. */
	if(!_pc_Buf) return;

	if(_pf_NbFaces) *_pf_NbFaces = 0;

	/* Loop thru all the ColMap Objects. */
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_EOT.st_ColMap);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_EOT.st_ColMap);

	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GO)) continue;

		/* BV Culling Test */
		if(OBJ_BV_IsAABBox(pst_GO->pst_BV))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			MATH_tdst_Vector	st_Max, st_Min;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_GO));
			MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_GO->pst_BV), OBJ_pst_GetAbsolutePosition(pst_GO));
			if(!INT_SphereAABBox(_pst_Center, _f_Radius, &st_Min, &st_Max)) continue;
		}
		else
		{
			if
			(
				!INT_SphereSphere
				(
					_pst_Center,
					_f_Radius,
					OBJ_pst_BV_GetCenter(pst_GO->pst_BV),
					OBJ_f_BV_GetRadius(pst_GO->pst_BV)
				)
			) continue;
		}

		pst_ColMap = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap;

		/* Dont want to deal with Mathematical Cobs. */
		if(pst_ColMap->dpst_Cob[0]->uc_Type != COL_C_Zone_Triangles) continue;

		pst_CobObj = pst_ColMap->dpst_Cob[0]->pst_TriangleCob;

		/* Loop thru all the Cob Elements */
		pst_CobElement = pst_CobObj->dst_Element;
		pst_LastCobElement = pst_CobElement + pst_CobObj->l_NbElements;
		pst_Normal = pst_CobObj->dst_FaceNormal;
		for(; pst_CobElement < pst_LastCobElement; pst_CobElement++)
		{
			pst_CobFace = pst_CobElement->dst_Triangle;
			pst_LastCobFace = pst_CobFace + pst_CobElement->uw_NbTriangles;
			for(; pst_CobFace < pst_LastCobFace; pst_CobFace++, pst_Normal++)
			{
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
				MATH_tdst_Vector	*pst_T1, *pst_T2, *pst_T3, st_AToCenter;
				float				f_Max, f_Min;
				/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

				pst_T1 = &pst_CobObj->dst_Point[pst_CobFace->auw_Index[0]];
				pst_T2 = &pst_CobObj->dst_Point[pst_CobFace->auw_Index[1]];
				pst_T3 = &pst_CobObj->dst_Point[pst_CobFace->auw_Index[2]];

				/* --- X rejection */
				f_Max = f_Max3(pst_T1->x, pst_T2->x, pst_T3->x);
				if(f_Max < _pst_Center->x - _f_Radius) continue;;

				f_Min = f_Min3(pst_T1->x, pst_T2->x, pst_T3->x);
				if(_pst_Center->x + _f_Radius < f_Min) continue;

				/* --- Y rejection */
				f_Max = f_Max3(pst_T1->y, pst_T2->y, pst_T3->y);
				if(f_Max < _pst_Center->y - _f_Radius) continue;

				f_Min = f_Min3(pst_T1->y, pst_T2->y, pst_T3->y);
				if(_pst_Center->y + _f_Radius < f_Min) continue;

				/* --- Z rejection */
				f_Max = f_Max3(pst_T1->z, pst_T2->z, pst_T3->z);
				if(f_Max < _pst_Center->z - _f_Radius) continue;

				f_Min = f_Min3(pst_T1->z, pst_T2->z, pst_T3->z);
				if(_pst_Center->z + _f_Radius < f_Min) continue;

				MATH_SubVector(&st_AToCenter, _pst_Center, pst_T1);

				/* BackFace Triangle */
				if(MATH_f_DotProduct(&st_AToCenter, pst_Normal) < 0.0f) continue;

				/* Max Triangles reached. We return; */
				if(_pf_NbFaces && (*_pf_NbFaces == _ul_MaxFaces)) return;

				L_memcpy(_pc_Buf, pst_Normal, sizeof(MATH_tdst_Vector));
				_pc_Buf += sizeof(MATH_tdst_Vector);
				L_memcpy(_pc_Buf, pst_T1, sizeof(MATH_tdst_Vector));
				_pc_Buf += sizeof(MATH_tdst_Vector);
				L_memcpy(_pc_Buf, pst_T2, sizeof(MATH_tdst_Vector));
				_pc_Buf += sizeof(MATH_tdst_Vector);
				L_memcpy(_pc_Buf, pst_T3, sizeof(MATH_tdst_Vector));
				_pc_Buf += sizeof(MATH_tdst_Vector);

				if(_pf_NbFaces) *_pf_NbFaces++;
			}
		}
	}
}

extern void COL_OK3_RecursiveFree(COL_tdst_OK3_Node *);
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_SynchronizeCob(COL_tdst_Cob *_pst_Cob, BOOL _b_RecreateFromVisual, BOOL _b_KeepIDMaterial)
{
	if
	(
		(_b_RecreateFromVisual || (!_b_RecreateFromVisual && _pst_Cob->uc_Flag & COL_C_Cob_Updated))
	&&	(_b_RecreateFromVisual || (!_b_RecreateFromVisual && _pst_Cob->p_GeoCob))
	&&	(_pst_Cob->uc_Type == COL_C_Zone_Triangles)
	)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		COL_tdst_GameMatList				*pst_GMatList;
		GEO_tdst_ElementIndexedTriangles	*pst_GeoElement, *pst_LastGeoElement;
		COL_tdst_ElementIndexedTriangles	*pst_CobElement, *pst_LastCobElement;
		GEO_tdst_Object						*pst_GeoCob;
		COL_tdst_IndexedTriangles			*pst_Col;
		LONG								al_ID[500];
		UCHAR								uc_Index, i;
		ULONG								ul_EditedElement;
		UCHAR								uc_Flags;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_EditedElement = _pst_Cob->ul_EditedElement;

		pst_GMatList = _pst_Cob->pst_GMatList;

		uc_Index = 0;

		uc_Flags = _pst_Cob->uc_Flag;

		pst_Col = _pst_Cob->pst_TriangleCob;
		pst_CobElement = pst_Col->dst_Element;
		pst_LastCobElement = pst_CobElement + pst_Col->l_NbElements;

		pst_GeoCob = (GEO_tdst_Object *) _pst_Cob->p_GeoCob;
		if(!pst_GeoCob) return;
		pst_GeoElement = pst_GeoCob->dst_Element;
		pst_LastGeoElement = pst_GeoElement + pst_GeoCob->l_NbElements;

		/* Save Element GameMaterial ID */
		for(; pst_CobElement < pst_LastCobElement; pst_CobElement++)
		{
			al_ID[uc_Index++] = pst_CobElement->l_MaterialId;
		}

		/* Deleting Cob Elements */
		pst_CobElement = pst_Col->dst_Element;
		for(; pst_CobElement < pst_LastCobElement; pst_CobElement++)
		{
			if(pst_CobElement->uw_NbTriangles && pst_CobElement->dst_Triangle) MEM_Free(pst_CobElement->dst_Triangle);
		}

		if(_b_RecreateFromVisual && (_pst_Cob->p_GeoCob))
		{
			if(pst_GeoCob->dst_Point) MEM_Free(pst_GeoCob->dst_Point);
			if(pst_GeoCob->dst_PointNormal) MEM_Free(pst_GeoCob->dst_PointNormal);

			for(; pst_GeoElement < pst_LastGeoElement; pst_GeoElement++)
			{
				if(pst_GeoElement->dst_Triangle) MEM_Free(pst_GeoElement->dst_Triangle);
			}

			if(pst_GeoCob->dst_Element) MEM_Free(pst_GeoCob->dst_Element);

			if(pst_GeoCob->pst_SubObject) GEO_SubObject_Free(pst_GeoCob);

			MEM_Free(pst_GeoCob);

			_pst_Cob->p_GeoCob = NULL;
		}


		if(pst_Col->pst_OK3)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			int						i, j;
			COL_tdst_OK3_Box		*pst_Box;
			COL_tdst_OK3_Element	*pst_Element;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			for(i = 0; i < (int) pst_Col->pst_OK3->ul_NumBox; i++)
			{
				pst_Box = &pst_Col->pst_OK3->pst_OK3_Boxes[i];

				for(j = 0; j < (int) pst_Box->ul_NumElement; j++)
				{
					pst_Element = &pst_Box->pst_OK3_Element[j];

					if(pst_Element->puw_OK3_Triangle) MEM_Free(pst_Element->puw_OK3_Triangle);
				}

				MEM_Free(pst_Box->pst_OK3_Element);
			}

			MEM_Free(pst_Col->pst_OK3->pst_OK3_Boxes);
			MEM_Free(pst_Col->pst_OK3->paul_Tag);

			COL_OK3_RecursiveFree(pst_Col->pst_OK3->pst_OK3_God);

			MEM_Free(pst_Col->pst_OK3);
		}


		MEM_Free(pst_Col->dst_Element);
		MEM_Free(pst_Col->dst_FaceNormal);
		MEM_Free(pst_Col->dst_Point);
		MEM_Free(_pst_Cob->pst_TriangleCob);
		MEM_Free(_pst_Cob->sz_GMatName);

		_pst_Cob->pst_TriangleCob = NULL;
		_pst_Cob->sz_GMatName = NULL;

		if(!_b_RecreateFromVisual)
		{
			pst_GeoCob = (GEO_tdst_Object *) _pst_Cob->p_GeoCob;

			/* Save Element GameMaterial ID */
			uc_Index = 0;
			for(; pst_GeoElement < pst_LastGeoElement; pst_GeoElement++)
			{
				al_ID[uc_Index++] = pst_GeoElement->l_MaterialId;
			}

			COL_ColMap_AddGeometric(_pst_Cob, pst_GeoCob, NULL);
		}
		else
		{
			COL_ColMap_AddGeometric(_pst_Cob, NULL, (MATH_tdst_Vector *) _pst_Cob->pst_GO);
		}

		pst_Col = _pst_Cob->pst_TriangleCob;
		if(pst_Col)
		{
			pst_CobElement = pst_Col->dst_Element;

			/* Restore Element GameMaterial ID. */
            if (!_b_RecreateFromVisual || _b_KeepIDMaterial)
            {
    			for(i = 0;
    				i < ((uc_Index < pst_Col->l_NbElements) ? uc_Index : pst_Col->l_NbElements);
	    			pst_CobElement++, i++
                    )
    			{
	    			pst_CobElement->l_MaterialId = al_ID[i];
		    	}
    		}
        }

		if(!_b_RecreateFromVisual) _pst_Cob->p_GeoCob = pst_GeoCob;

		_pst_Cob->pst_GMatList = pst_GMatList;

		COL_GMat_UpdateFileName(_pst_Cob);

		_pst_Cob->uc_Flag = uc_Flags;
		_pst_Cob->uc_Flag &= ~COL_C_Cob_Updated;

		_pst_Cob->ul_EditedElement = ul_EditedElement;

		if(!LOA_IsBinaryData()) COL_ComputeProximity(_pst_Cob);

		if((_pst_Cob->uw_NbOfInstances == 1) && _pst_Cob->pst_GO) COL_OK3_Build(_pst_Cob->pst_GO, TRUE, TRUE);
	}

	LINK_UpdatePointer(_pst_Cob);
}

#endif

/*
 =======================================================================================================================
    Aim:    Update Cob From visual in Real Time. that means that the vertices has just moved.
 =======================================================================================================================
 */
void COL_UpdateCobFromVisualRealTime(OBJ_tdst_GameObject *_pst_GO, COL_tdst_Cob *_pst_Cob, GEO_tdst_Object *_pst_Visual)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_IndexedTriangles			*pst_CobObj;
	COL_tdst_ElementIndexedTriangles	*pst_CobElement, *pst_CobLastElement;
	COL_tdst_IndexedTriangle			*pst_CobTriangle, *pst_CobLastTriangle;
	MDF_tdst_Modifier					*pst_Modifier, *pst_Mod;
	GEO_tdst_ModifierOnduleTonCorps		*pst_Data;
	MATH_tdst_Vector					st_VectT1T2, st_VectT1T3, st_Norm;
	MATH_tdst_Vector					*pst_Point, *pst_Last;
	ULONG								*pul_RLI;
	ULONG								ul_Triangle, ul_T1, ul_T2, ul_T3;
	int									i;
	float								f;
#ifdef ACTIVE_EDITORS
	GEO_tdst_Object						*pst_GeoCob;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Cob || !_pst_Visual || (_pst_Cob->uc_Type != COL_C_Zone_Triangles)) return;

	pst_CobObj = _pst_Cob->pst_TriangleCob;

	if((int) pst_CobObj->l_NbPoints != (int) _pst_Visual->l_NbPoints)
	{
		/*~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
		char	asz_Log[500];
		/*~~~~~~~~~~~~~~~~~*/

		sprintf
		(
			asz_Log,
			"Visual and ColMap have not the same number of vertices. Cannot update Cob from visual in real time (%s)",
			_pst_GO->sz_Name
		);
		LINK_PrintStatusMsg(asz_Log);
#endif
		_pst_Cob->uc_Flag &= ~COL_C_RealTimeComputation;
		return;
	}

	/*
	 * Only Modifier used to update ColMaps in RealTime --> OnduletonCorps. If the
	 * GameObject has not this Modifier, we return
	 */
	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) || !(_pst_GO->pst_Extended)) return;

	PRO_StartTrameRaster(&ENG_gapst_RasterEng_User[6]);

	pst_Mod = NULL;
	pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
	while(pst_Modifier)
	{
		if
		(
			(
				pst_Modifier->ul_Flags &
					(
						MDF_C_Modifier_ApplyGao | MDF_C_Modifier_Inactive | MDF_C_Modifier_ApplyGen |
							MDF_C_Modifier_NoApply
					)
			) == 0
		)
		{
			if(pst_Modifier->i->ul_Type == MDF_C_Modifier_OnduleTonCorps)
			{
				pst_Mod = pst_Modifier;
				break;
			}
		}

		pst_Modifier = pst_Modifier->pst_Next;
	}

	if(!pst_Mod) return;

	/*
	 * Resets the Array of points with the un-modified vertices stored in geometric
	 * object
	 */
	L_memcpy(pst_CobObj->dst_Point, _pst_Visual->dst_Point, sizeof(MATH_tdst_Vector) * _pst_Visual->l_NbPoints);

	pst_Data = (GEO_tdst_ModifierOnduleTonCorps *) pst_Mod->p_Data;

	pst_Point = pst_CobObj->dst_Point;
	pst_Last = pst_Point + pst_CobObj->l_NbPoints;
	pul_RLI = _pst_GO->pst_Base->pst_Visu->dul_VertexColors;

	if((pst_Data->ul_Flags & GEO_C_OTCF_UseAlphaOfRLI) && pul_RLI)
	{
		pul_RLI++;
		if(pst_Data->ul_Flags & GEO_C_OTCF_InvertAlpha)
		{
			if(pst_Data->ul_Flags & GEO_C_OTCF_X)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->x += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->x += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->y += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptCos((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
			else if(pst_Data->ul_Flags & GEO_C_OTCF_Y)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->y += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->y += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->x += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptCos((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
			else if(pst_Data->ul_Flags & GEO_C_OTCF_Z)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->z += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (1.0f - (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f));
						pst_Point->x += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->y += f * fOptCos((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
		}
		else
		{
			if(pst_Data->ul_Flags & GEO_C_OTCF_X)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->x += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->x += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->y += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptCos((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
			else if(pst_Data->ul_Flags & GEO_C_OTCF_Y)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->y += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->y += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->x += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptCos((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
			else if(pst_Data->ul_Flags & GEO_C_OTCF_Z)
			{
				if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->z += f * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->z += f * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
				else
				{
					for(; pst_Point < pst_Last; pst_Point++, pul_RLI++)
					{
						f = pst_Data->f_Amplitude * (((*pul_RLI) & 0xFF000000) >> 24) * (1 / 256.0f);
						pst_Point->x += f * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
						pst_Point->y += f * fOptCos((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					}
				}
			}
		}
	}
	else
	{
		if(pst_Data->ul_Flags & GEO_C_OTCF_X)
		{
			if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
			{
				for(; pst_Point < pst_Last; pst_Point++)
				{
					pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
				}
			}
			else
			{
				for(; pst_Point < pst_Last; pst_Point++)
				{
					pst_Point->y += pst_Data->f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
					pst_Point->z += pst_Data->f_Amplitude * fOptCos((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
				}
			}
		}
		else if(pst_Data->ul_Flags & GEO_C_OTCF_Y)
		{
			if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
			{
				for(; pst_Point < pst_Last; pst_Point++)
				{
					pst_Point->y += pst_Data->f_Amplitude * fOptSin((pst_Point->x * pst_Data->f_Factor) + pst_Data->f_Angle);
					pst_Point->y += pst_Data->f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
				}
			}
			else
			{
				for(; pst_Point < pst_Last; pst_Point++)
				{
					pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
					pst_Point->z += pst_Data->f_Amplitude * fOptCos((pst_Point->y * pst_Data->f_Factor) + pst_Data->f_Angle);
				}
			}
		}
		else if(pst_Data->ul_Flags & GEO_C_OTCF_Z)
		{
			if(pst_Data->ul_Flags & GEO_C_OTCF_Planar)
			{
				f = pst_Data->f_Amplitude * 2 * fOptSin(pst_Data->f_Angle);
				for(; pst_Point < pst_Last; pst_Point++)
					pst_Point->z += f * (fOptSin(pst_Point->x * pst_Data->f_Factor) + fOptSin(pst_Point->y * pst_Data->f_Factor));
			}
			else
			{
				for(; pst_Point < pst_Last; pst_Point++)
				{
					pst_Point->x += pst_Data->f_Amplitude * fOptSin((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
					pst_Point->y += pst_Data->f_Amplitude * fOptCos((pst_Point->z * pst_Data->f_Factor) + pst_Data->f_Angle);
				}
			}
		}
	}

#ifdef ACTIVE_EDITORS
	if(GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_ShowCOB) pst_Data->f_Angle += (10.0f*pst_Data->f_Delta*TIM_gf_dt);;
#endif
	/* Update ColMap face normal ... */
	{
		ul_Triangle = 0;
		pst_CobElement = pst_CobObj->dst_Element;
		pst_CobLastElement = pst_CobElement + pst_CobObj->l_NbElements;
		for(i = 0; pst_CobElement < pst_CobLastElement; pst_CobElement++, i++)
		{
			if(pst_CobElement->uw_NbTriangles)
			{
				pst_CobTriangle = pst_CobElement->dst_Triangle;
				pst_CobLastTriangle = pst_CobTriangle + pst_CobElement->uw_NbTriangles;

				/* We go thru all the triangles of the current object. */
				for(; pst_CobTriangle < pst_CobLastTriangle; pst_CobTriangle++, ul_Triangle++)
				{
					/* Computation of the Normal to the Face */
					ul_T1 = pst_CobTriangle->auw_Index[0];
					ul_T2 = pst_CobTriangle->auw_Index[1];
					ul_T3 = pst_CobTriangle->auw_Index[2];

					MATH_SubVector(&st_VectT1T2, &pst_CobObj->dst_Point[ul_T2], &pst_CobObj->dst_Point[ul_T1]);
					MATH_SubVector(&st_VectT1T3, &pst_CobObj->dst_Point[ul_T3], &pst_CobObj->dst_Point[ul_T1]);
					MATH_CrossProduct(&st_Norm, &st_VectT1T2, &st_VectT1T3);

					if(!MATH_b_NulVector(&st_Norm))
						MATH_NormalizeVector(pst_CobObj->dst_FaceNormal + ul_Triangle, &st_Norm);
					else
						MATH_CopyVector(pst_CobObj->dst_FaceNormal + ul_Triangle, &st_Norm);
				}
			}
		}
	}

#ifdef ACTIVE_EDITORS
	pst_GeoCob = (GEO_tdst_Object *) _pst_Cob->p_GeoCob;

	if(pst_GeoCob)
	{
		L_memcpy(pst_GeoCob->dst_Point, pst_CobObj->dst_Point, sizeof(MATH_tdst_Vector) * _pst_Visual->l_NbPoints);
	}

#endif

    COL_OK3_Build(_pst_GO, TRUE, TRUE);

	PRO_StopTrameRaster(&ENG_gapst_RasterEng_User[6]);
}

#ifdef ACTIVE_EDITORS
extern void GEO_CreateGeoFromCob(OBJ_tdst_GameObject *, GEO_tdst_Object *, COL_tdst_Cob *);
extern LONG GEO_l_SaveInBuffer(GEO_tdst_Object *, void *);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_CreateGroFromCob(COL_tdst_Cob *_pst_Cob)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object *pst_Geo;
	BIG_KEY			ul_Key;
	char			asz_Name[200];
	char			*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Cob) return;
	if(_pst_Cob->uc_Type != COL_C_Zone_Triangles) return;

	pst_Geo = (GEO_tdst_Object*)MEM_p_Alloc(sizeof(GEO_tdst_Object));

	/* Fill the st_GeoCob structure with info needed to see the Cob. */
	L_memset(pst_Geo, 0, sizeof(GEO_tdst_Object));

	pst_Geo->st_Id.i = &GRO_gast_Interface[GRO_Geometric];

	GEO_CreateGeoFromCob(NULL, pst_Geo, _pst_Cob);

	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) _pst_Cob);

	if(ul_Key != BIG_C_InvalidKey)
	{
		L_strcpy(asz_Name, BIG_NameFile(BIG_ul_SearchKeyToFat(ul_Key)));
		psz_Temp = L_strrchr(asz_Name, '.');
		if(psz_Temp) *psz_Temp = 0;
		L_strcat(asz_Name, ".gro");
	}
	else
	{
		sprintf(asz_Name, "Marcel.gro");
	}

	SAV_Begin("ROOT", asz_Name);
	GEO_l_SaveInBuffer(pst_Geo, NULL);
	SAV_ul_End();
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_ElementIndexedTriangles	*pst_GeoElement, *pst_LastElement;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		if(pst_Geo->dst_Point) MEM_Free(pst_Geo->dst_Point);
		if(pst_Geo->dst_PointNormal) MEM_Free(pst_Geo->dst_PointNormal);

		pst_GeoElement = pst_Geo->dst_Element;
		pst_LastElement = pst_GeoElement + pst_Geo->l_NbElements;
		for(; pst_GeoElement < pst_LastElement; pst_GeoElement++)
		{
			if(pst_GeoElement->l_NbTriangles && pst_GeoElement->dst_Triangle) MEM_Free(pst_GeoElement->dst_Triangle);
		}

		if(pst_Geo->dst_Element) MEM_Free(pst_Geo->dst_Element);

		if(pst_Geo->pst_SubObject) GEO_SubObject_Free(pst_Geo);

		MEM_Free(pst_Geo);
	}
}

#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
COL_tdst_IndexedTriangle *COL_TipTopNextTriangle
(
	COL_tdst_IndexedTriangles	*_pst_CobObj,
	MATH_tdst_Vector			*_pst_BCS_TipTop,
 	MATH_tdst_Vector			*_pst_BCS_Follow,
	COL_tdst_IndexedTriangle	*_pst_OldTriangle,
	ULONG						*_pul_Triangle,
	USHORT						*_puw_Element,
	BOOL						_b_FirstTriangle,
	BOOL						_b_Vertical
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector			*pst_BCS_A,	*pst_BCS_B, *pst_BCS_C;
	MATH_tdst_Vector			*pst_BCS_A2, *pst_BCS_B2, *pst_BCS_C2;
	MATH_tdst_Vector			*pst_BCS_Norm, st_BCS_Right;
	MATH_tdst_Vector			*pst_BCS_Far;
	MATH_tdst_Vector			st_BCS_Temp1, st_BCS_Temp2;
	MATH_tdst_Vector			st_BCS_Temp3, st_BCS_Temp4;
	MATH_tdst_Vector			st_BCS_TipA, st_BCS_TipB, st_BCS_TipC;
	MATH_tdst_Vector			st_BCS_EdgeAB, st_BCS_EdgeAC, st_BCS_EdgeBC;
	MATH_tdst_Vector			st_BCS_Proj;
	float						f1, f2, f3, f4, fMax;
	float						fA, fB, fC;
	float						fAB, fAC, fBC;
	COL_tdst_ElementIndexedTriangles	*pst_Element;
	COL_tdst_IndexedTriangle	*pst_Triangle, *pst_T;
	int							i, j, norm, newT=0, Point;
	USHORT						uw_PossibleElement;
	ULONG						ul_PossibleTriangle;
	BOOL						b_TipTopOnVertex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_TipTopOnVertex = FALSE;

	/* Get all the info of the current triangle */
	for(i = 0, norm = *_pul_Triangle; i < *_puw_Element; i++)
		norm += _pst_CobObj->dst_Element[i].uw_NbTriangles;

	pst_BCS_Norm = &_pst_CobObj->dst_FaceNormal[norm];

	MATH_CrossProduct(&st_BCS_Right, _pst_BCS_Follow, pst_BCS_Norm);

	pst_Triangle = &(_pst_CobObj->dst_Element[*_puw_Element].dst_Triangle[*_pul_Triangle]);

	pst_BCS_A = &_pst_CobObj->dst_Point[pst_Triangle->auw_Index[0]];
	pst_BCS_B = &_pst_CobObj->dst_Point[pst_Triangle->auw_Index[1]];
	pst_BCS_C = &_pst_CobObj->dst_Point[pst_Triangle->auw_Index[2]];

	MATH_SubVector(&st_BCS_TipA, pst_BCS_A, _pst_BCS_TipTop);
	if(MATH_f_NormVector(&st_BCS_TipA) < 0.01f)
	{
		MATH_InitVector(&st_BCS_TipA, 0.0f, 0.0f, 0.0f);
		MATH_CopyVector(_pst_BCS_TipTop, pst_BCS_A);
		fA = 0.0f;
	}
	else
		fA = MATH_f_DotProduct(&st_BCS_TipA, _pst_BCS_Follow);

	MATH_SubVector(&st_BCS_TipB, pst_BCS_B, _pst_BCS_TipTop);
	if(MATH_f_NormVector(&st_BCS_TipB) < 0.01f)
	{
		MATH_InitVector(&st_BCS_TipB, 0.0f, 0.0f, 0.0f);
		MATH_CopyVector(_pst_BCS_TipTop, pst_BCS_B);
		fB = 0.0f;
	}
	else
		fB = MATH_f_DotProduct(&st_BCS_TipB, _pst_BCS_Follow);

	MATH_SubVector(&st_BCS_TipC, pst_BCS_C, _pst_BCS_TipTop);
	if(MATH_f_NormVector(&st_BCS_TipC) < 0.01f)
	{
		MATH_InitVector(&st_BCS_TipC, 0.0f, 0.0f, 0.0f);
		MATH_CopyVector(_pst_BCS_TipTop, pst_BCS_C);
		fC = 0.0f;
	}
	else
		fC = MATH_f_DotProduct(&st_BCS_TipC, _pst_BCS_Follow);

	fMax = -10000.0f;
	/* -- Edge AB -- Try to find scalar fAB that fits vec(AM) = fAB.vec(AB) where M the the projected point of TipTop onto edge AB along the Follow vector */
	fAB = -10000.0f;
	if((fA >= 0.0f) || (fB >= 0.0f))	/* If Both A and B are "behind" current Tiptop concerning the Follow vector, skip edge */
	{
		MATH_SubVector(&st_BCS_EdgeAB, pst_BCS_B, pst_BCS_A);
		f1 = MATH_f_DotProduct(&st_BCS_EdgeAB, &st_BCS_Right);

		/* If Edge and Right are almost // */
		if(fAbs(f1) < 1e-3f)
		{
			if(fA == 0.0f)
				fAB = 0.0f;
			else
			{
				/*
				MATH_NormalizeVector(&st_BCS_Temp1, &st_BCS_TipA);
				MATH_NormalizeVector(&st_BCS_Temp2, &st_BCS_EdgeAB);
				MATH_CrossProduct(&st_BCS_Temp3, &st_BCS_Temp1, &st_BCS_Temp2);
				if(MATH_f_NormVector(&st_BCS_Temp3) < 0.1f)
					fAB = (fA > fB) ? 0.0f : 1.0f;
				else
					fAB = -10000.0f;
				*/
				float		Alpha;

				Alpha = - (MATH_f_DotProduct(&st_BCS_TipA, &st_BCS_EdgeAB) / MATH_f_DotProduct(&st_BCS_EdgeAB, &st_BCS_EdgeAB));
				if((Alpha < 0.0f) || (Alpha > 1.0f))
				{
						fAB = -10000.0f;
				}
				else
				{
					MATH_MulVector(&st_BCS_Temp2, &st_BCS_EdgeAB, Alpha);
					MATH_AddEqualVector(&st_BCS_Temp2, pst_BCS_A);
					MATH_SubVector(&st_BCS_Temp3, &st_BCS_Temp2, _pst_BCS_TipTop);
					if(MATH_f_NormVector(&st_BCS_Temp3) < 0.01f)
						fAB = (fA > fB) ? 0.0f : 1.0f;
					else
						fAB = -10000.0f;
				}
			}
		}
		else
		{
			fAB = - MATH_f_DotProduct(&st_BCS_TipA, &st_BCS_Right) / MATH_f_DotProduct(&st_BCS_EdgeAB, &st_BCS_Right);
			if((fAB < 0.0f) || (fAB > 1.0f))
				fAB = -10000.0f;
		}

		if(fAB != -10000.0f)
		{
			MATH_MulVector(&st_BCS_Temp1, &st_BCS_EdgeAB, fAB);
			MATH_AddVector(&st_BCS_Temp2, &st_BCS_Temp1, pst_BCS_A);
			MATH_SubVector(&st_BCS_Temp3, &st_BCS_Temp2, _pst_BCS_TipTop);
			if((MATH_f_NormVector(&st_BCS_Temp3) > 0.01f) && MATH_f_DotProduct(&st_BCS_Temp3, _pst_BCS_Follow) < 0.0f)
				fAB = -10000.0f;
			else
			{
				fAB = MATH_f_NormVector(&st_BCS_Temp3);
				if(fAB > fMax)
				{
					MATH_CopyVector(&st_BCS_Proj, &st_BCS_Temp2);
					fMax = fAB;
				}
			}
		}
	}

	/* -- Edge AC -- */
	fAC = -10000.0f;
	if((fA >= 0.0f) || (fC >= 0.0f))
	{
		MATH_SubVector(&st_BCS_EdgeAC, pst_BCS_C, pst_BCS_A);
		fAC = MATH_f_DotProduct(&st_BCS_EdgeAC, &st_BCS_Right);

		/* If Edge and Right are almost // */
		if(fAbs(fAC) < 1e-3f)
		{
			if(fA == 0.0f)
				fAC = 0.0f;
			else
			{
				/*
				MATH_NormalizeVector(&st_BCS_Temp1, &st_BCS_TipA);
				MATH_NormalizeVector(&st_BCS_Temp2, &st_BCS_EdgeAC);
				MATH_CrossProduct(&st_BCS_Temp3, &st_BCS_Temp1, &st_BCS_Temp2);
				if(MATH_f_NormVector(&st_BCS_Temp3) < 0.1f)
					fAC = (fA > fC) ? 0.0f : 1.0f;
				else
					fAC = -10000.0f;
				*/
				float		Alpha;

				Alpha = - (MATH_f_DotProduct(&st_BCS_TipA, &st_BCS_EdgeAC) / MATH_f_DotProduct(&st_BCS_EdgeAC, &st_BCS_EdgeAC));
				if((Alpha < 0.0f) || (Alpha > 1.0f))
				{
						fAC = -10000.0f;
				}
				else
				{
					MATH_MulVector(&st_BCS_Temp2, &st_BCS_EdgeAC, Alpha);
					MATH_AddEqualVector(&st_BCS_Temp2, pst_BCS_A);
					MATH_SubVector(&st_BCS_Temp3, &st_BCS_Temp2, _pst_BCS_TipTop);
					if(MATH_f_NormVector(&st_BCS_Temp3) < 0.01f)
						fAC = (fA > fC) ? 0.0f : 1.0f;
					else
						fAC = -10000.0f;
				}


			}
		}
		else
		{
			fAC = - MATH_f_DotProduct(&st_BCS_TipA, &st_BCS_Right) / MATH_f_DotProduct(&st_BCS_EdgeAC, &st_BCS_Right);
			if((fAC < 0.0f) || (fAC > 1.0f))
				fAC = -10000.0f;
		}

		if(fAC != -10000.0f)
		{
			MATH_MulVector(&st_BCS_Temp1, &st_BCS_EdgeAC, fAC);
			MATH_AddVector(&st_BCS_Temp2, &st_BCS_Temp1, pst_BCS_A);
			MATH_SubVector(&st_BCS_Temp3, &st_BCS_Temp2, _pst_BCS_TipTop);
			if((MATH_f_NormVector(&st_BCS_Temp3) > 0.01f) && MATH_f_DotProduct(&st_BCS_Temp3, _pst_BCS_Follow) < 0.0f)
				fAC = -10000.0f;
			else
			{
				fAC = MATH_f_NormVector(&st_BCS_Temp3);
				if(fAbs(fMax - fAC) < 1e-3f)
				{
					MATH_NormalizeVector(&st_BCS_Temp1, &st_BCS_EdgeAB);

					if(fB < fA)
						MATH_NegEqualVector(&st_BCS_Temp1);

					MATH_NormalizeVector(&st_BCS_Temp3, &st_BCS_EdgeAC);

					if(MATH_f_DotProduct(&st_BCS_Temp1, _pst_BCS_Follow) < MATH_f_DotProduct(&st_BCS_Temp3, _pst_BCS_Follow))
					{
						MATH_CopyVector(&st_BCS_Proj, &st_BCS_Temp2);
						fMax = fAC;
					}
				}
				else 
					if(fAC > fMax) 
					{
						MATH_CopyVector(&st_BCS_Proj, &st_BCS_Temp2);
						fMax = fAC;
					}
			}
		}

	}

	/* -- Edge BC -- */
	fBC = -10000.0f;
	if((fB >= 0.0f) || (fC >= 0.0f))
	{
		MATH_SubVector(&st_BCS_EdgeBC, pst_BCS_C, pst_BCS_B);
		fBC = MATH_f_DotProduct(&st_BCS_EdgeBC, &st_BCS_Right);

		/* If Edge and Right are almost // */
		if(fAbs(fBC) < 1e-3f)
		{
			if(fB == 0.0f)
				fBC = 0.0f;
			else
			{
				/*
				MATH_NormalizeVector(&st_BCS_Temp1, &st_BCS_TipB);
				MATH_NormalizeVector(&st_BCS_Temp2, &st_BCS_EdgeBC);
				MATH_CrossProduct(&st_BCS_Temp3, &st_BCS_Temp1, &st_BCS_Temp2);
				if(MATH_f_NormVector(&st_BCS_Temp3) < 0.1f)
					fBC = (fB > fC) ? 0.0f : 1.0f;
				else
					fBC = -10000.0f;
				*/
				float		Alpha;

				Alpha = - (MATH_f_DotProduct(&st_BCS_TipB, &st_BCS_EdgeBC) / MATH_f_DotProduct(&st_BCS_EdgeBC, &st_BCS_EdgeBC));
				if((Alpha < 0.0f) || (Alpha > 1.0f))
				{
					fBC = -10000.0f;
				}
				else
				{
					MATH_MulVector(&st_BCS_Temp2, &st_BCS_EdgeBC, Alpha);
					MATH_AddEqualVector(&st_BCS_Temp2, pst_BCS_B);
					MATH_SubVector(&st_BCS_Temp3, &st_BCS_Temp2, _pst_BCS_TipTop);
					if(MATH_f_NormVector(&st_BCS_Temp3) < 0.01f)
						fBC = (fB > fC) ? 0.0f : 1.0f;
					else
						fBC = -10000.0f;
				}

			}

		}
		else
		{
			fBC = - MATH_f_DotProduct(&st_BCS_TipB, &st_BCS_Right) / MATH_f_DotProduct(&st_BCS_EdgeBC, &st_BCS_Right);
			if((fBC < 0.0f) || (fBC > 1.0f))
				fBC = -10000.0f;
		}

		if(fBC != -10000.0f)
		{
			MATH_MulVector(&st_BCS_Temp1, &st_BCS_EdgeBC, fBC);
			MATH_AddVector(&st_BCS_Temp2, &st_BCS_Temp1, pst_BCS_B);
			MATH_SubVector(&st_BCS_Temp3, &st_BCS_Temp2, _pst_BCS_TipTop);
			if((MATH_f_NormVector(&st_BCS_Temp3) > 0.01f) && MATH_f_DotProduct(&st_BCS_Temp3, _pst_BCS_Follow) < 0.0f)
				fBC = -10000.0f;
			else
			{
				fBC = MATH_f_NormVector(&st_BCS_Temp3);

				if(fAbs(fMax - fBC) < 1e-3f)
				{
					if(fMax == fAB)
					{
						MATH_NormalizeVector(&st_BCS_Temp1, &st_BCS_EdgeAB);
						MATH_NormalizeVector(&st_BCS_Temp3, &st_BCS_EdgeBC);

						if(fB < fA)
							MATH_NegEqualVector(&st_BCS_Temp1);

						if(fC < fB)
							MATH_NegEqualVector(&st_BCS_Temp3);


					}
					else
					{
						MATH_NormalizeVector(&st_BCS_Temp1, &st_BCS_EdgeAC);
						MATH_NormalizeVector(&st_BCS_Temp3, &st_BCS_EdgeBC);

						if(fC < fA)
							MATH_NegEqualVector(&st_BCS_Temp1);

						if(fC < fB)
							MATH_NegEqualVector(&st_BCS_Temp3);

					}

					if(MATH_f_DotProduct(&st_BCS_Temp1, _pst_BCS_Follow) < MATH_f_DotProduct(&st_BCS_Temp3, _pst_BCS_Follow))
					{
						MATH_CopyVector(&st_BCS_Proj, &st_BCS_Temp2);
						fMax = fBC;
					}
				}
				else
					if(fBC > fMax) 
					{
						MATH_CopyVector(&st_BCS_Proj, &st_BCS_Temp2);
						fMax = fBC;
					}
			}
		}

	}

#ifdef _DEBUG
	if(fMax == -10000.0f)
	{
//		LINK_PrintStatusMsg("TipTop error17");
	}
#endif

	/* If we "are" on a Triangle vertex */
	if((fMax == 0.0) && ((fA == 0.0f) || (fB == 0.0f) || (fC == 0.0f)))
	{
		pst_BCS_Far = (fA == 0.0f) ? pst_BCS_A : ((fB == 0.0f) ? pst_BCS_B : pst_BCS_C);
		b_TipTopOnVertex = TRUE;
		goto FindGoodPointConnectedFace;
	}

#ifdef JADEFUSION
	if(fEqWithEpsilon(fMax,fAB,Cf_EpsilonBig))
#else
	if(fMax == fAB)
#endif
	{
		MATH_CopyVector(_pst_BCS_TipTop, &st_BCS_Proj);
		newT = pst_Triangle->auw_Prox[0];
		pst_BCS_Far = (fA > fB) ? pst_BCS_A : pst_BCS_B;
	}
	else
	{
#ifdef JADEFUSION
		if(fEqWithEpsilon(fMax,fAC,Cf_EpsilonBig))
#else
		if(fMax == fAC)
#endif
		{
			MATH_CopyVector(_pst_BCS_TipTop, &st_BCS_Proj);
			newT = pst_Triangle->auw_Prox[1];
			pst_BCS_Far = (fA > fC) ? pst_BCS_A : pst_BCS_C;
		}
		else
#ifdef JADEFUSION
			if(fEqWithEpsilon(fMax,fBC,Cf_EpsilonBig))
#else
			if(fMax == fBC)
#endif
			{
				MATH_CopyVector(_pst_BCS_TipTop, &st_BCS_Proj);
				newT = pst_Triangle->auw_Prox[2];
				pst_BCS_Far = (fB > fC) ? pst_BCS_B : pst_BCS_C;
			}
	}


	if(newT == 0xFFFF)
	{
		return NULL;
	}


	j = newT;
	for(i = 0; i < (int) _pst_CobObj->l_NbElements; i++)
	{
		if(_pst_CobObj->dst_Element[i].uw_NbTriangles > j)
		{
			break;
		}

		j -= _pst_CobObj->dst_Element[i].uw_NbTriangles;
	}

	if(i == _pst_CobObj->l_NbElements)
	{
#ifdef _DEBUG
		LINK_PrintStatusMsg("TipTop error5");
#endif
		return NULL;
	}

	if(_pst_OldTriangle && (_pst_OldTriangle == &(_pst_CobObj->dst_Element[i].dst_Triangle[j])))	/* Our New Triangle is also our last one ... try something else to recover from this error */
	{
FindGoodPointConnectedFace:

		uw_PossibleElement = 0xFFFF;
		ul_PossibleTriangle = 0xFFFFFFFF;

		fMax = 0.0f;

		Point = (pst_BCS_Far == pst_BCS_A) ? pst_Triangle->auw_Index[0] : ( (pst_BCS_Far == pst_BCS_B) ? pst_Triangle->auw_Index[1] : pst_Triangle->auw_Index[2]);
		newT = 0;
		for(i = 0; i < (int) _pst_CobObj->l_NbElements; i++)
		{
			pst_Element = &_pst_CobObj->dst_Element[i];

			for(j = 0; j < (USHORT) pst_Element->uw_NbTriangles; j++, newT++)
			{
				pst_T = &(_pst_CobObj->dst_Element[i].dst_Triangle[j]);
				if(pst_T == pst_Triangle) continue;

				if
				(
					(pst_T->auw_Index[0] == Point)
				||	(pst_T->auw_Index[1] == Point)
				||	(pst_T->auw_Index[2] == Point)
				)

				{
					pst_BCS_A2 = &_pst_CobObj->dst_Point[pst_T->auw_Index[0]];
					pst_BCS_B2 = &_pst_CobObj->dst_Point[pst_T->auw_Index[1]];
					pst_BCS_C2 = &_pst_CobObj->dst_Point[pst_T->auw_Index[2]];

					pst_BCS_Norm = &_pst_CobObj->dst_FaceNormal[newT];
					MATH_CrossProduct(&st_BCS_Right, _pst_BCS_Follow, pst_BCS_Norm);	/* Et si les 2, Follow et Norm sont // ? */

					if(pst_T->auw_Index[0] != Point)
					{
						if(pst_T->auw_Index[1] == Point)
						{
							pst_BCS_A2 = &_pst_CobObj->dst_Point[pst_T->auw_Index[1]];
							pst_BCS_B2 = &_pst_CobObj->dst_Point[pst_T->auw_Index[0]];
						}
						else
						{
							pst_BCS_A2 = &_pst_CobObj->dst_Point[pst_T->auw_Index[2]];
							pst_BCS_C2 = &_pst_CobObj->dst_Point[pst_T->auw_Index[0]];
						}
					}


					MATH_SubVector(&st_BCS_Temp1, pst_BCS_B2, pst_BCS_Far);
					MATH_NormalizeVector(&st_BCS_Temp3, &st_BCS_Temp1);

					f1 = MATH_f_DotProduct(&st_BCS_Temp3, _pst_BCS_Follow);
					if(f1 > fMax)
					{
						fMax = f1;
						uw_PossibleElement = (USHORT) i;
						ul_PossibleTriangle =  j;
					}

					f2 = MATH_f_DotProduct(&st_BCS_Temp1, &st_BCS_Right);

					MATH_SubVector(&st_BCS_Temp2, pst_BCS_C2, pst_BCS_Far);
					MATH_NormalizeVector(&st_BCS_Temp4, &st_BCS_Temp2);

					f4 = MATH_f_DotProduct(&st_BCS_Temp4, _pst_BCS_Follow);

					if((f1 <= 0.0f) && (f4 <= 0.0f))
						continue;

					if((f1 <= 0.0f) && (fAbs(MATH_f_DotProduct(&st_BCS_Temp1, _pst_BCS_Follow)) > fAbs(MATH_f_DotProduct(&st_BCS_Temp2, _pst_BCS_Follow))))
						continue;

					if((f4 <= 0.0f) && (fAbs(MATH_f_DotProduct(&st_BCS_Temp2, _pst_BCS_Follow)) > fAbs(MATH_f_DotProduct(&st_BCS_Temp1, _pst_BCS_Follow))))
						continue;


					if(f4 > fMax)
					{
						fMax = f4;
						uw_PossibleElement = (USHORT) i;
						ul_PossibleTriangle =  j;
					}

					f3 = MATH_f_DotProduct(&st_BCS_Temp2, &st_BCS_Right);

					if((f2 < 0.0f) || (f4 > 0.0f)) continue;
					if((f1 <= 0.0f) && (f3 <= 0.0f)) continue;


					MATH_CopyVector(_pst_BCS_TipTop, pst_BCS_Far);

					if(b_TipTopOnVertex && (&(_pst_CobObj->dst_Element[i].dst_Triangle[j]) == _pst_OldTriangle))
					{
						pst_BCS_Far = (fA == 0.0f) ? ((fB > fC) ? pst_BCS_B : pst_BCS_C) : ((fB == 0.0f) ? ((fA > fC) ? pst_BCS_A : pst_BCS_C) : ((fA > fB) ? pst_BCS_A : pst_BCS_B));
						MATH_CopyVector(_pst_BCS_TipTop, pst_BCS_Far);

						return &(_pst_CobObj->dst_Element[*_puw_Element].dst_Triangle[*_pul_Triangle]);
					}
					else
					{
						*_puw_Element = (USHORT) i;
						*_pul_Triangle = j;

						return &(_pst_CobObj->dst_Element[*_puw_Element].dst_Triangle[*_pul_Triangle]);
					}
				}
			}
		}

		if(_b_Vertical)
		{
			if((uw_PossibleElement != 0xFFFF))
			{
				MATH_CopyVector(_pst_BCS_TipTop, pst_BCS_Far);

				if(b_TipTopOnVertex && (&(_pst_CobObj->dst_Element[uw_PossibleElement].dst_Triangle[ul_PossibleTriangle]) == _pst_OldTriangle))
				{
					pst_BCS_Far = (fA == 0.0f) ? ((fB > fC) ? pst_BCS_B : pst_BCS_C) : ((fB == 0.0f) ? ((fA > fC) ? pst_BCS_A : pst_BCS_C) : ((fA > fB) ? pst_BCS_A : pst_BCS_B));
					MATH_CopyVector(_pst_BCS_TipTop, pst_BCS_Far);

					return &(_pst_CobObj->dst_Element[*_puw_Element].dst_Triangle[*_pul_Triangle]);
				}
				else
				{
					*_puw_Element = uw_PossibleElement ;
					*_pul_Triangle = ul_PossibleTriangle;

					return &(_pst_CobObj->dst_Element[*_puw_Element].dst_Triangle[*_pul_Triangle]);
				}
			}
			else
			{
				MATH_CopyVector(_pst_BCS_TipTop, pst_BCS_Far);
				return NULL;
			}
		}
		else
			return NULL;

	}
	else
	{
		*_puw_Element = (USHORT) i;
		*_pul_Triangle = j;

		return &(_pst_CobObj->dst_Element[*_puw_Element].dst_Triangle[*_pul_Triangle]);
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_TipTopPointGet
(
	OBJ_tdst_GameObject *_pst_GO,
	MATH_tdst_Vector	*_pst_GCS_TipTop,
	MATH_tdst_Vector	*_pst_GCS_Follow,
	float				_f_Alpha,
	BOOL				_b_Down,
	BOOL				_b_UseRay,
	BOOL				_b_StopOnGround,
	BOOL				_b_StopOnWall
	)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_CobGO;
	COL_tdst_ColMap				*pst_ColMap;
	COL_tdst_Cob				*pst_Cob;
	COL_tdst_IndexedTriangles	*pst_CobObj;
	COL_tdst_IndexedTriangle	*pst_Triangle, *pst_OldTriangle;
	WOR_tdst_World				*pst_World;
	COL_tdst_RayInfo			*pst_RayInfo;
	COL_tdst_Report				*pst_CurrentReport, *pst_LastReport;
	MATH_tdst_Vector			*pst_A, *pst_B, *pst_C, *pst_BCS_Norm;
	MATH_tdst_Vector			st_GCS_Norm, st_BCS_TipTop, st_BCS_Follow;
	MATH_tdst_Vector			st_InvUnitG;
	MATH_tdst_Matrix			st_B_InvGlobalMatrix;
	UCHAR						uc_MaxLoop;
	ULONG						ul_Triangle, ul_OldTriangle;
	USHORT						uw_Element, uw_OldElement;
	BOOL						b_Vertical, b_Ground;
	int							i, norm;
	float						f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	if(!pst_World) return;

    b_Vertical = (_pst_GCS_Follow->z != 0.0f);

	if(_b_Down)
		MATH_NegEqualVector(_pst_GCS_Follow);

	if(!b_Vertical)
		_pst_GCS_Follow->z = 0.0f;

	pst_RayInfo = &pst_World->st_RayInfo;
	pst_RayInfo->uc_Flags = 0;

	/* Init */
	MATH_CopyVector(&pst_RayInfo->st_EdgeNormal, &MATH_gst_NulVector);

	/* Inverse gravity needed to determine if we are on a Wall/Ground */
	if(COL_gst_GlobalVars.pst_InvUnitG)
		MATH_CopyVector(&st_InvUnitG, COL_gst_GlobalVars.pst_InvUnitG);
	else
		MATH_InitVector(&st_InvUnitG, 0.0f, 0.0f, 1.0f);


	/* Init Initial Algorithm Point/Element/Triangle */
	if(_b_UseRay)
	{
		MATH_CopyVector(_pst_GCS_TipTop, &pst_RayInfo->st_CollidedPoint);
		ul_Triangle = pst_RayInfo->ul_Triangle;
		uw_Element = pst_RayInfo->uw_Element;
		pst_CobGO = pst_World->st_RayInfo.pst_CollidedGO;
	}
	else
	{
		pst_CurrentReport = pst_World->ast_Reports;
		pst_LastReport = pst_CurrentReport + pst_World->ul_NbReports;

		for(; pst_CurrentReport < pst_LastReport; pst_CurrentReport++)
		{
			if(pst_CurrentReport->ul_Flag & COL_Cul_Extra_Corner) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Inactive) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Invalid) continue;
			if(pst_CurrentReport->ul_Flag & COL_Cul_Crossable) continue;
			if(pst_CurrentReport->pst_A != _pst_GO) continue;

			if
			(
				(pst_CurrentReport->ul_Flag & (b_Vertical ? COL_Cul_Wall : COL_Cul_Ground))
			&&	(pst_CurrentReport->ul_Flag & (COL_Cul_Triangle | COL_Cul_Edge))
			) break;
		}

		if(pst_CurrentReport == pst_LastReport) 
		{
#ifdef JADEFUSION
			MATH_InitVector(_pst_GCS_TipTop, 0.0f, 0.0f, 0.0f);
#endif
			return;
		}

		MATH_CopyVector(_pst_GCS_TipTop, &pst_CurrentReport->st_CollidedPoint);
		ul_Triangle = pst_CurrentReport->ul_Triangle;
		uw_Element = pst_CurrentReport->uw_Element;
		pst_CobGO = pst_CurrentReport->pst_B;
	}

	if(!pst_CobGO || !OBJ_b_TestIdentityFlag(pst_CobGO, OBJ_C_IdentityFlag_ColMap)) 
	{
		return;
	}

	MATH_InvertMatrix(&st_B_InvGlobalMatrix, pst_CobGO->pst_GlobalMatrix);


	pst_ColMap = ((COL_tdst_Base *) pst_CobGO->pst_Extended->pst_Col)->pst_ColMap;
	pst_Cob = pst_ColMap->dpst_Cob[0];
	if(!pst_Cob || (pst_Cob->uc_Type != COL_C_Zone_Triangles)) 
	{
		return;
	}

	pst_CobObj = pst_Cob->pst_TriangleCob;

	pst_Triangle = &(pst_CobObj->dst_Element[uw_Element].dst_Triangle[ul_Triangle]);

	/* We can be asked to climp up a wall till a we find a ground. But our initial point is already on a ground triangle :/ */
	if(_b_StopOnGround || _b_StopOnWall)
	{
		for(i = 0, norm = ul_Triangle; i < uw_Element; i++)
			norm += pst_CobObj->dst_Element[i].uw_NbTriangles;

		pst_BCS_Norm = &pst_CobObj->dst_FaceNormal[norm];

		MATH_TransformVector(&st_GCS_Norm, pst_CobGO->pst_GlobalMatrix, pst_BCS_Norm);

		f = MATH_f_DotProduct(&st_InvUnitG, &st_GCS_Norm);

		b_Ground = (!((f >= 0.0f) && (f < COL_gst_GlobalVars.f_WallCosAngle)) && !((f < 0.0f) && (f > -0.80f)));

		if((b_Ground && _b_StopOnGround) ||	(!b_Ground && _b_StopOnWall))
		{
			MATH_CopyVector(&pst_RayInfo->st_Normal, &st_GCS_Norm);
			MATH_CopyVector(&pst_RayInfo->st_EdgeNormal, &st_GCS_Norm);
			return;
		}
	}

	MATH_TransformVertexNoScale(&st_BCS_TipTop, &st_B_InvGlobalMatrix, _pst_GCS_TipTop);
	MATH_TransformVectorNoScale(&st_BCS_Follow, &st_B_InvGlobalMatrix, _pst_GCS_Follow);

	uw_OldElement = uw_Element;
	ul_OldTriangle = ul_Triangle;
	pst_OldTriangle = NULL;

	uc_MaxLoop = 0;

	do
	{
		ul_OldTriangle = ul_Triangle;
		uw_OldElement = uw_Element;
#ifdef ACTIVE_EDITORS
		if(1)
		{	
			MATH_TransformVertexNoScale(&st_GCS_Norm, pst_CobGO->pst_GlobalMatrix, &st_BCS_TipTop);
		}
#endif

		pst_Triangle = COL_TipTopNextTriangle(pst_CobObj, &st_BCS_TipTop, &st_BCS_Follow, pst_OldTriangle, &ul_Triangle, &uw_Element, (uc_MaxLoop == 0), b_Vertical);

		pst_OldTriangle = &(pst_CobObj->dst_Element[uw_OldElement].dst_Triangle[ul_OldTriangle]);

#ifdef ACTIVE_EDITORS
		if(1)
		{	
			MATH_tdst_Vector	st_Toto, st_Zob;


			MATH_TransformVertexNoScale(&st_Toto, pst_CobGO->pst_GlobalMatrix, &st_BCS_TipTop);
			MATH_SubVector(&st_Zob, &st_Toto, &st_GCS_Norm);

			MATH_CopyVector(&pst_World->st_Origin[pst_World->uc_Vector], &st_GCS_Norm);
			MATH_CopyVector(&pst_World->st_Vector[pst_World->uc_Vector], &st_Zob);
			pst_World->aul_Color[pst_World->uc_Vector] = 0x000096FF;
			pst_World->ap_VectorGAO[pst_World->uc_Vector] = NULL; //pst_CobGO;
			pst_World->uc_Vector = (pst_World->uc_Vector == (WOR_Cte_DbgVectorRender-1)) ? 0 : pst_World->uc_Vector + 1;

		}
#endif

		if(_b_StopOnGround || _b_StopOnWall)
		{
			for(i = 0, norm = ul_Triangle; i < uw_Element; i++)
				norm += pst_CobObj->dst_Element[i].uw_NbTriangles;

			pst_BCS_Norm = &pst_CobObj->dst_FaceNormal[norm];
	
			MATH_TransformVector(&st_GCS_Norm, pst_CobGO->pst_GlobalMatrix, pst_BCS_Norm);

			f = MATH_f_DotProduct(&st_InvUnitG, &st_GCS_Norm);

			b_Ground = (!((f >= 0.0f) && (f < COL_gst_GlobalVars.f_WallCosAngle)) && !((f < 0.0f) && (f > -0.80f)));
	
			if((b_Ground && _b_StopOnGround) ||	(!b_Ground && _b_StopOnWall))
			{
				break;
			}
		}

	} while(pst_Triangle && ((++uc_MaxLoop) < 30));


	pst_A = &pst_CobObj->dst_Point[pst_OldTriangle->auw_Index[0]];
	pst_B = &pst_CobObj->dst_Point[pst_OldTriangle->auw_Index[1]];
	pst_C = &pst_CobObj->dst_Point[pst_OldTriangle->auw_Index[2]];

	/* Tip top */
	{
		MATH_TransformVertexNoScale(&pst_RayInfo->st_CollidedPoint, pst_CobGO->pst_GlobalMatrix, &st_BCS_TipTop);
		MATH_CopyVector(_pst_GCS_TipTop, &pst_RayInfo->st_CollidedPoint);
	}

	/* Face Normal */
	{
		for(i = 0, norm = ul_OldTriangle; i < uw_OldElement; i++)
			norm += pst_CobObj->dst_Element[i].uw_NbTriangles;

		pst_BCS_Norm = &pst_CobObj->dst_FaceNormal[norm];

		MATH_TransformVectorNoScale(&pst_RayInfo->st_Normal, pst_CobGO->pst_GlobalMatrix, pst_BCS_Norm);
	}

	/* Edge Normal */
	{
		for(i = 0, norm = ul_Triangle; i < uw_Element; i++)
			norm += pst_CobObj->dst_Element[i].uw_NbTriangles;

		pst_BCS_Norm = &pst_CobObj->dst_FaceNormal[norm];

		MATH_TransformVectorNoScale(&pst_RayInfo->st_EdgeNormal, pst_CobGO->pst_GlobalMatrix, pst_BCS_Norm);
	}
}

