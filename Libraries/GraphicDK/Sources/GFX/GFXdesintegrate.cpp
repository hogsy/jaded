/*$T GFXdesintegrate.c GC! 1.081 10/12/00 11:46:10 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "GDInterface/GDInterface.h"
#include "GFX/GFX.h"
#include "GFX/GFXdesintegrate.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "SOFT/SOFTcolor.h"
#include "PArticleGenerator/PAGstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"

#ifdef PSX2_TARGET

/* mamagouille */
#include "PSX2debug.h"
#endif
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Desintegrate_Create(void *_p_GO )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    OBJ_tdst_GameObject                 *pst_GO;
	GEO_tdst_Object						*pst_Obj;
	GFX_tdst_Desintegrate			    *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_GO = (OBJ_tdst_GameObject *) _p_GO;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetGro(pst_GO);
	if((!pst_Obj) || (pst_Obj->st_Id.i->ul_Type != GRO_Geometric))
	{
		return NULL;
	}

	pst_Data = (GFX_tdst_Desintegrate *) MEM_p_Alloc(sizeof(GFX_tdst_Desintegrate));
    pst_Data->ul_Flags = 0;
    MATH_CopyMatrix( &pst_Data->st_Matrix, OBJ_pst_GetAbsoluteMatrix( pst_GO ) );
    pst_Data->pst_GO = pst_GO;
    pst_Data->pst_Obj = pst_Obj;
    pst_Data->pst_Material = (GRO_tdst_Struct *) OBJ_p_GetGrm( pst_GO );

    pst_Data->f_Time = 0;
    pst_Data->f_MaxTime = 5;
    pst_Data->st_Grav.x = 0;
    pst_Data->st_Grav.y = 0;
	pst_Data->st_Grav.z = -0.5f;
	pst_Data->f_MulMin = 1;
    pst_Data->f_MulMax = 2;
    pst_Data->f_Friction = 1;
    pst_Data->f_ZMinStrength = 10000.0f;

    return pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Desintegrate_Init(GFX_tdst_Desintegrate *pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_Object						*pst_Obj;
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	int									i_Elem, i_Triangle, i_Point;
	GEO_tdst_IndexedTriangle			*t;
    GEO_Vertex                          *d_GlobalPoint, *p, *plast, *p2;
	MATH_tdst_Vector					*n, N;
    MATH_tdst_Matrix                    *M;
    PAG_tdst_P                          *pst_P;
    MDF_tdst_Modifier                   *pst_Mod;
    GEO_tdst_ModifierStoreTransformedPoint *pst_ModData;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data->ul_Flags |= GFX_C_Desintegrate_Init;

    pst_Obj = pst_Data->pst_Obj;
	pst_Data->l_NbTriangles = GEO_l_GetNumberOfTriangles(pst_Obj);
	pst_Data->l_NbPoints = pst_Data->l_NbTriangles * 3;
	pst_Data->l_NbElements = pst_Obj->l_NbElements;

    /* init particle generator */
    pst_Data->pst_Particles = PAG_pst_Create();
    pst_Data->pst_Particles->l_NbMaxP = pst_Data->l_NbTriangles;
    pst_Data->pst_Particles->l_NbP = pst_Data->pst_Particles->l_NbMaxP;
    pst_Data->pst_Particles->dst_P = (PAG_tdst_P *) MEM_p_Alloc(sizeof(PAG_tdst_P) * pst_Data->pst_Particles->l_NbMaxP);
    L_memset( pst_Data->pst_Particles->dst_P, 0, sizeof(PAG_tdst_P) * pst_Data->pst_Particles->l_NbMaxP);
    pst_Data->pst_Particles->ul_Flags |= PAG_Flags_UseFriction;
    pst_Data->pst_Particles->ul_Flags |= PAG_Flags_NoParticleDesctruction;
    MATH_CopyVector( &pst_Data->pst_Particles->st_Acc, &pst_Data->st_Grav );
    pst_Data->pst_Particles->f_Friction = pst_Data->f_Friction;

    if (pst_Data->f_ZMinStrength != 10000.0f)
    {
        pst_Data->ul_Flags |= GFX_C_Desintegrate_Collision;
        pst_Data->pst_Particles->ul_Flags |= PAG_Flags_UseZMin;
        pst_Data->pst_Particles->f_ZMin = pst_Data->f_ZMin;
        pst_Data->pst_Particles->f_ZMinStrength = pst_Data->f_ZMinStrength;
    }

	pst_Data->pst_Obj = GEO_pst_Create(pst_Data->l_NbPoints, 0, pst_Data->l_NbElements, 0);
	pst_Data->pst_Obj->l_NbUVs = pst_Obj->l_NbUVs;
	pst_Data->pst_Obj->dst_UV = pst_Obj->dst_UV;

    d_GlobalPoint = (GEO_Vertex *) MEM_p_Alloc( sizeof( GEO_Vertex ) * pst_Obj->l_NbPoints );

    p =  pst_Obj->dst_Point;
    pst_Mod = MDF_pst_GetByType( pst_Data->pst_GO, MDF_C_Modifier_StoreTransformedPoints );
    if (pst_Mod)
    {
        pst_ModData = (GEO_tdst_ModifierStoreTransformedPoint *) pst_Mod->p_Data;
        if (pst_ModData->i_PointsAreValid )
            p = pst_ModData->dst_Points;
    }

    plast = p + pst_Obj->l_NbPoints;
    p2 = d_GlobalPoint;
    M = &pst_Data->st_Matrix;
    for ( ; p < plast; p++, p2++ )
    {
        MATH_TransformVertex( VCast(p2), M, VCast(p) );
    }

	i_Point = 0;
	p = pst_Data->pst_Obj->dst_Point;
	n = pst_Data->pst_Obj->dst_PointNormal;
    pst_P = pst_Data->pst_Particles->dst_P;

	for(i_Elem = 0; i_Elem < pst_Data->l_NbElements; i_Elem++)
	{
        pst_Elem = pst_Data->pst_Obj->dst_Element + i_Elem;

        if ( pst_Obj->dst_Element[i_Elem].l_NbTriangles == 0 )
        {
            L_memset( pst_Elem, 0, sizeof( GEO_tdst_ElementIndexedTriangles ) );
            continue;
        }

		pst_Elem = pst_Data->pst_Obj->dst_Element + i_Elem;
		pst_Elem->l_NbTriangles = pst_Obj->dst_Element[i_Elem].l_NbTriangles;
		pst_Elem->l_MaterialId = pst_Obj->dst_Element[i_Elem].l_MaterialId;
		pst_Elem->ul_NumberOfUsedIndex = 0;

		GEO_AllocElementContent(pst_Elem);

		L_memcpy
		(
			pst_Elem->dst_Triangle,
			pst_Obj->dst_Element[i_Elem].dst_Triangle,
			pst_Elem->l_NbTriangles * sizeof(GEO_tdst_IndexedTriangle)
		);
		t = pst_Elem->dst_Triangle;
		for(i_Triangle = 0; i_Triangle < pst_Elem->l_NbTriangles; i_Triangle++, t++, pst_P++ )
		{
			
            MATH_CopyVector(VCast(p++), VCast(&d_GlobalPoint[t->auw_Index[0]]));
			MATH_CopyVector(VCast(p++), VCast(&d_GlobalPoint[t->auw_Index[1]]));
			MATH_CopyVector(VCast(p++), VCast(&d_GlobalPoint[t->auw_Index[2]]));

			MATH_AddVector(&N, VCast(p - 3), VCast(p - 2));
			MATH_AddEqualVector(&N, VCast(p - 1));
            MATH_ScaleEqualVector( &N, 0.33333333f);

            if (pst_Data->ul_Flags & GFX_C_Desintegrate_Collision)
                MATH_CopyVector( &pst_P->st_Pos, &N);
            else
                MATH_InitVectorToZero( &pst_P->st_Pos );

            MATH_SubEqualVector( &N, &M->T );
			MATH_NormalizeAnyVector(&N, &N);

			MATH_CopyVector(n++, &N);
			MATH_CopyVector(n++, &N);
			MATH_CopyVector(n++, &N);

            MATH_ScaleVector( &pst_P->st_Speed, &N, fRand( pst_Data->f_MulMin, pst_Data->f_MulMax) );
            pst_P->f_TimeLeft = fRand( pst_Data->f_MinTime, pst_Data->f_MaxTime );

			t->auw_Index[0] = i_Point++;
			t->auw_Index[1] = i_Point++;
			t->auw_Index[2] = i_Point++;
		}
	}

    MEM_Free( d_GlobalPoint );
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Desintegrate_Destroy( void *p_Data )
{
    GFX_tdst_Desintegrate   	*pst_Data;

    if ( !p_Data) return ;
    pst_Data = (GFX_tdst_Desintegrate *) p_Data;

    /* UV was taken from originale geometry so don't deallocate UV list */
    pst_Data->pst_Obj->l_NbUVs = 0;
    pst_Data->pst_Obj->dst_UV = NULL;

    GEO_Free( pst_Data->pst_Obj );
    PAG_Free( pst_Data->pst_Particles );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Desintegrate_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GRO_tdst_Struct             *pst_SaveGeo;
	OBJ_tdst_GameObject			*pst_GO;
	GFX_tdst_Desintegrate   	*pst_Data;
	GEO_Vertex                  *pst_Point;
    PAG_tdst_P                  *pst_P, *pst_LastP;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ( !p_Data) return 0;
    pst_Data = (GFX_tdst_Desintegrate *) p_Data;

    pst_Data->f_Time += TIM_gf_dt;
    if ( pst_Data->f_Time > pst_Data->f_MaxTime) 
        return 0;

    if ( !(pst_Data->ul_Flags & GFX_C_Desintegrate_Init) )
        GFX_Desintegrate_Init( pst_Data );

    pst_Point = pst_Data->pst_Obj->dst_Point;
	pst_P = pst_Data->pst_Particles->dst_P;
    pst_LastP = pst_P + pst_Data->pst_Particles->l_NbP;

    if (pst_Data->ul_Flags & GFX_C_Desintegrate_Collision)
    {
        for(; pst_P < pst_LastP; pst_P++)
        {
            MATH_SubEqualVector( VCast(pst_Point++), &pst_P->st_Pos );
            MATH_SubEqualVector( VCast(pst_Point++), &pst_P->st_Pos );
            MATH_SubEqualVector( VCast(pst_Point++), &pst_P->st_Pos );
        }
    }
    
    PAG_Update( NULL, pst_Data->pst_Particles, TIM_gf_dt );

    pst_Point = pst_Data->pst_Obj->dst_Point;
    pst_P = pst_Data->pst_Particles->dst_P;
    if (pst_Data->ul_Flags & GFX_C_Desintegrate_Collision)
    {
        for(; pst_P < pst_LastP; pst_P++)
	    {
            if (pst_P->f_TimeLeft < 0)
            {
                MATH_InitVectorToZero( VCast(pst_Point++) );
                MATH_InitVectorToZero( VCast(pst_Point++) );
                MATH_InitVectorToZero( VCast(pst_Point++) );
            }
            else
            {
                MATH_AddEqualVector( VCast(pst_Point++), &pst_P->st_Pos );
                MATH_AddEqualVector( VCast(pst_Point++), &pst_P->st_Pos );
                MATH_AddEqualVector( VCast(pst_Point++), &pst_P->st_Pos );
            }
        }
    }
    else
    {
        for(; pst_P < pst_LastP; pst_P++)
	    {
            if (pst_P->f_TimeLeft < 0)
            {
                MATH_InitVectorToZero( VCast(pst_Point++) );
                MATH_InitVectorToZero( VCast(pst_Point++) );
                MATH_InitVectorToZero( VCast(pst_Point++) );
            }
            else
            {
                MATH_AddEqualVector( VCast(pst_Point++), &pst_P->st_Pos );
                MATH_AddEqualVector( VCast(pst_Point++), &pst_P->st_Pos );
                MATH_AddEqualVector( VCast(pst_Point++), &pst_P->st_Pos );
            }
            MATH_InitVectorToZero( &pst_P->st_Pos );
        }
    }

    /* to be sure that GFX_gpst_GO exist one call to GFX_NeedGeom */
    GFX_NeedGeom(4, 4, 4, 0);
	pst_GO = GFX_gpst_GO;
	pst_SaveGeo = pst_GO->pst_Base->pst_Visu->pst_Object;

    pst_GO->pst_Base->pst_Visu->pst_Object = &pst_Data->pst_Obj->st_Id;
    pst_GO->pst_Base->pst_Visu->pst_Material = pst_Data->pst_Material;
    pst_GO->pst_Base->pst_Visu->ul_DrawMask &= ~GDI_Cul_DM_TestBackFace;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~GDI_Cul_DM_TestBackFace;
    
    pst_Data->pst_Obj->st_Id.i->pfn_Render(GFX_gpst_GO);

    pst_GO->pst_Base->pst_Visu->ul_DrawMask |= GDI_Cul_DM_TestBackFace;
    GDI_gpst_CurDD->ul_CurrentDrawMask |= GDI_Cul_DM_TestBackFace;


    pst_GO->pst_Base->pst_Visu->pst_Object = pst_SaveGeo;
    
    return 1;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
