/*$T COLedit.c GC!1.59 12/21/99 17:48:18 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    GC Dependencies 11/22/99
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/Mem.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGfat.h"

#include "MATHs/MATH.h"

#include "COLstruct.h"
#include "COLconst.h"
#include "COLaccess.h"
#include "COLset.h"
#include "COLcob.h"

#include "OBJects/OBJstruct.h"
#include "OBJects/OBJconst.h"
#include "OBJects/OBJaccess.h"
#include "OBJects/OBJorient.h"

#include "GEOmetric/GEOobject.h"
#include "WORld/WORstruct.h"
#include "SELection/SELection.h"


/*
 =======================================================================================================================
 =======================================================================================================================
*/
void COL_UpdateCameraNumberOfFaces(COL_tdst_Cob *_pst_Cob)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	COL_tdst_IndexedTriangles			*pst_Tcob;
	COL_tdst_ElementIndexedTriangles	*pst_Element;
	COL_tdst_GameMat					*pst_GMat;
	int									i;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	if(!_pst_Cob) return;

	if(_pst_Cob->uc_Type != COL_C_Zone_Triangles) return;

	pst_Tcob = _pst_Cob->pst_TriangleCob;
	pst_Tcob->uw_NumCameraFaces = 0;

	for(i = 0; i < (int) pst_Tcob->l_NbElements; i++)
	{
		pst_Element = &pst_Tcob->dst_Element[i];

		pst_GMat = COL_pst_GMat_Get(_pst_Cob, pst_Element);

		if(pst_GMat && (pst_GMat->ul_CustomBits & COL_Cul_GMat_Camera))
			pst_Tcob->uw_NumCameraFaces += pst_Element->uw_NbTriangles;
	}

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_Zone_SphereToBox(COL_tdst_ZDx *_pst_ZDx)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_Min, st_Max;
    float               f_Radius, f_InsideRadius;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_CopyVector(&st_Min, COL_pst_Shape_GetCenter(_pst_ZDx->p_Shape));
    MATH_CopyVector(&st_Max, &st_Min);
    f_Radius = COL_f_Shape_GetRadius(_pst_ZDx->p_Shape);

    /*$off*/
    f_InsideRadius = f_Radius * Cf_InvSqrt3;

    /*$F------- Min ------------ */
    st_Min.x = st_Min.x - f_InsideRadius;
    st_Min.y = st_Min.y - f_InsideRadius;
    st_Min.z = st_Min.z - f_InsideRadius;

    /*$F------- Max ------------ */
    st_Max.x = st_Max.x + f_InsideRadius;
    st_Max.y = st_Max.y + f_InsideRadius;
    st_Max.z = st_Max.z + f_InsideRadius;

    /*$on*/
    _pst_ZDx->p_Shape = MEM_p_Realloc(_pst_ZDx->p_Shape, sizeof(COL_tdst_Box));

    MATH_CopyVector(COL_pst_Shape_GetMin(_pst_ZDx->p_Shape), &st_Min);
    MATH_CopyVector(COL_pst_Shape_GetMax(_pst_ZDx->p_Shape), &st_Max);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_Zone_BoxToSphere(COL_tdst_ZDx *_pst_ZDx)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_Center;
    float               f_Radius;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    MATH_SubVector(&st_Center, COL_pst_Shape_GetMax(_pst_ZDx->p_Shape), COL_pst_Shape_GetMin(_pst_ZDx->p_Shape));

    MATH_MulEqualVector(&st_Center, 0.5f);
    f_Radius = MATH_f_NormVector(&st_Center);

    MATH_AddEqualVector(&st_Center, COL_pst_Shape_GetMin(_pst_ZDx->p_Shape));

    _pst_ZDx->p_Shape = MEM_p_Realloc(_pst_ZDx->p_Shape, sizeof(COL_tdst_Sphere));

    MATH_CopyVector(COL_pst_Shape_GetCenter(_pst_ZDx->p_Shape), &st_Center);
    COL_Shape_SetRadius(_pst_ZDx->p_Shape, f_Radius);
}

/*
 =======================================================================================================================
    Aim:    We have just changed a zone flag.
 =======================================================================================================================
 */
void COL_Zone_ChangeFlags(COL_tdst_ZDx *_pst_ZDx, ULONG _ul_Flags, ULONG _ul_OldFlags)
{
}

#ifdef CONVEX_OBJECT

/*
 =======================================================================================================================
    Aim:    Determines whether an object is convex or not.
 =======================================================================================================================
 */
BOOL COL_IsObjectConvex(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_Object                     *pst_Geo;
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
    MATH_tdst_Vector                    *pst_Point, *pst_LastPoint;
    MATH_tdst_Vector                    *pst_A, *pst_B, *pst_C;
    MATH_tdst_Vector                    st_Norm, st_AB, st_AC, st_AM;
    float                               f_Dot, f_Dot2;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* If the object has no visu, we return FALSE; */
    if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)) return FALSE;

    /* We get the geometric object. */
    pst_Geo = (GEO_tdst_Object *) _pst_GO->pst_Base->pst_Visu->pst_Object;
    pst_Element = pst_Geo->dst_Element;
    pst_LastElement = pst_Element + pst_Geo->l_NbElements;

    if(!pst_Element) return FALSE;

    /* Loop thru all the object's elements. */
    for(; pst_Element < pst_LastElement; pst_Element++)
    {
        pst_Triangle = pst_Element->dst_Triangle;
        pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

        if(!pst_Triangle) return FALSE;

        /* Loop thru all the element's triangle. */
        for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
        {
            pst_A = &pst_Geo->dst_Point[pst_Triangle->auw_Index[0]];
            pst_B = &pst_Geo->dst_Point[pst_Triangle->auw_Index[1]];
            pst_C = &pst_Geo->dst_Point[pst_Triangle->auw_Index[2]];

            /* We compute the normal of the Triangle. */
            MATH_SubVector(&st_AB, pst_B, pst_A);
            MATH_SubVector(&st_AC, pst_C, pst_A);
            MATH_CrossProduct(&st_Norm, &st_AB, &st_AC);
            MATH_NormalizeEqualVector(&st_Norm);

            pst_Point = pst_Geo->dst_Point;
            pst_LastPoint = pst_Geo->dst_Point + pst_Geo->l_NbPoints;

            f_Dot = 0.0f;

            /* We loop thru all the points */
            for(; pst_Point < pst_LastPoint; pst_Point++)
            {
                if((pst_Point == pst_A) || (pst_Point == pst_B) || (pst_Point == pst_C)) continue;
                MATH_SubVector(&st_AM, pst_Point, pst_A);
                if(!f_Dot)
                    f_Dot = MATH_f_DotProduct(&st_AM, &st_Norm);
                else
                {
                    f_Dot2 = MATH_f_DotProduct(&st_AM, &st_Norm);

                    /* The computed point can be on the same plan. */
                    if(fAbs(f_Dot2) < 0.01f) continue;
                    if(!(fSameSign(f_Dot2, f_Dot))) return FALSE;
                }
            }
        }
    }

    return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void COL_CreatePlans(OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    COL_tdst_Convex                     *pst_Convex;
    COL_tdst_Plan                       *pst_Plan, *pst_LastPlan, *pst_CurrentPlan;
    GEO_tdst_Object                     *pst_Geo;
    GEO_tdst_ElementIndexedTriangles    *pst_Element, *pst_LastElement;
    GEO_tdst_IndexedTriangle            *pst_Triangle, *pst_LastTriangle;
    MATH_tdst_Vector                    *pst_A, *pst_B, *pst_C;
    MATH_tdst_Vector                    st_Norm, st_AB, st_AC;
    COL_tdst_Instance                   *pst_Instance;
    BOOL                                b_Cull;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Instance = ((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_Instance;

    /* If the object has no visu, we return FALSE; */
    if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)) return;

    /* If the object is not convex, we return NULL */
    if(!COL_IsObjectConvex(_pst_GO)) return;

    pst_Instance->pst_Convex = (COL_tdst_Convex *) MEM_p_Alloc(sizeof(COL_tdst_Convex));
    pst_Convex = pst_Instance->pst_Convex;

    pst_Convex->ul_NbPlans = 0;

    /* We get the geometric object. */
    pst_Geo = (GEO_tdst_Object *) _pst_GO->pst_Base->pst_Visu->pst_Object;
    pst_Element = pst_Geo->dst_Element;
    pst_LastElement = pst_Element + pst_Geo->l_NbElements;

    if(!pst_Element) return;

    /* Loop thru all the object's elements. */
    for(; pst_Element < pst_LastElement; pst_Element++)
    {
        pst_Triangle = pst_Element->dst_Triangle;
        pst_LastTriangle = pst_Triangle + pst_Element->l_NbTriangles;

        if(!pst_Triangle) return;

        /* Loop thru all the element's triangle. */
        for(; pst_Triangle < pst_LastTriangle; pst_Triangle++)
        {
            pst_A = &pst_Geo->dst_Point[pst_Triangle->auw_Index[0]];
            pst_B = &pst_Geo->dst_Point[pst_Triangle->auw_Index[1]];
            pst_C = &pst_Geo->dst_Point[pst_Triangle->auw_Index[2]];

            /* We compute the normal of the Triangle. */
            MATH_SubVector(&st_AB, pst_B, pst_A);
            MATH_SubVector(&st_AC, pst_C, pst_A);
            MATH_CrossProduct(&st_Norm, &st_AB, &st_AC);
            MATH_NormalizeEqualVector(&st_Norm);

            pst_Plan = pst_Convex->dst_Plan;
            pst_LastPlan = pst_Plan + pst_Convex->ul_NbPlans;
            b_Cull = FALSE;
            for(; pst_Plan < pst_LastPlan; pst_Plan++)
            {
                /*
                 * If the Normal of the triangle is (nearly) equal to the
                 * normal of the current plan, we assume that the triangle is
                 * part of that plan and continue.
                 */
                if(MATH_b_EqVectorWithEpsilon(&st_Norm, &pst_Plan->st_Norm, 0.01f))
                {
                    b_Cull = TRUE;
                    break;
                }
            }

            if(!b_Cull)
            {
                /*
                 * We didnt find a plan that (nearly) contains our triangle,
                 * we add another Plan.
                 */
                if(!pst_Convex->ul_NbPlans)
                {
                    pst_Convex->dst_Plan = (COL_tdst_Plan *) MEM_p_Alloc(sizeof(COL_tdst_Plan));
                    pst_Convex->dst_Plan->pst_A = pst_A;
                    MATH_CopyVector(&pst_Convex->dst_Plan->st_Norm, &st_Norm);
                    pst_Convex->ul_NbPlans++;
                }
                else
                {
                    pst_Convex->dst_Plan = MEM_p_Realloc
                        (
                            pst_Convex->dst_Plan,
                            (pst_Convex->ul_NbPlans + 1) * sizeof(COL_tdst_Plan)
                        );
                    (pst_Convex->dst_Plan + pst_Convex->ul_NbPlans)->pst_A = pst_A;
                    pst_CurrentPlan = pst_Convex->dst_Plan + pst_Convex->ul_NbPlans;
                    MATH_CopyVector(&pst_CurrentPlan->st_Norm, &st_Norm);
                    pst_Convex->ul_NbPlans++;
                }
            }
        }
    }
}

#endif /* CONVEX_OBJECT */

#endif /* ACTIVE_EDITORS */

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
