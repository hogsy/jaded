/*$T GFXlightning.c GC! 1.081 08/22/00 14:52:21 */


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
#include "GDInterface/GDIrasters.h"
#include "GFX/GFX.h"
#include "GFX/GFXlightning.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"

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
void GFX_Lightning_SampleFractal( GFX_tdst_Lightning *pst_Data )
{
    int     i, j, k, dk, i_P2;
    float   t, x, y, z, X, Y, Z;
    float   newx, newy;


    if (pst_Data->l_MaxPattern == 0) return;

    i = pst_Data->l_NbPattern;
    i_P2 = 0;
    while (i)
    {
        i_P2++;
        i>>=1;
    }

    for(i = 0; i < i_P2; i++)
	{
        dk = 1 << (i_P2 - 1 - i);
        t = 1.0f / dk;
        k = -1;
        x = 0;
        y = 0;
        z = 0;

        for ( j = 0; j < (1 << i); j++)
        {
            k += dk;
            if (k + dk == pst_Data->l_NbPattern)
            {
                X = 0;
                Y = 0;
                Z = 1;
            }
            else
            {
                X = pst_Data->pst_Pattern[k + dk].x;
                Y = pst_Data->pst_Pattern[k + dk].y;
                Z = pst_Data->pst_Pattern[k + dk].z;
            }

            newx = (x + X) / 2;
            newy = (y + Y) / 2;
            pst_Data->pst_Pattern[k].z = (z + Z) / 2;

            newx += fRand( -Cf_2Pi * (1.0f / (1 << i)), Cf_2Pi * (1.0f / (1 << i)));
            newy += fRand( 0, pst_Data->f_Radius * (1.0f / (1 << i)));

            pst_Data->pst_Pattern[k].x = (1 - t) * pst_Data->pst_Pattern[k].x + t * newx;
            pst_Data->pst_Pattern[k].y = (1 - t) * pst_Data->pst_Pattern[k].y + t * newy;

            x = X; y = Y; z = Z; k+= dk;
        }
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Lightning_SampleZero( GFX_tdst_Lightning *pst_Data )
{
    int i;

    if (!pst_Data->l_MaxPattern) return ;

    for(i = 0; i < pst_Data->l_NbPattern; i++)
	{
		pst_Data->pst_Pattern[i].x = 0;
		pst_Data->pst_Pattern[i].y = 0;
		pst_Data->pst_Pattern[i].z = ((float) (i + 1)) / (pst_Data->l_NbPattern + 1);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void *GFX_Lightning_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Lightning	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Lightning *) MEM_p_Alloc(sizeof(GFX_tdst_Lightning));

	MATH_InitVector(&pst_Data->st_A, -10.0f, -10.0f, 3.0f);
	MATH_InitVector(&pst_Data->st_B, 10.0f, 10.0f, 3.0f);
	MATH_InitVector(&pst_Data->Z, 0.0f, 0.0f, 1.0f);
	pst_Data->f_Size = 0.1f;
	pst_Data->f_Radius = 2.0f;
	pst_Data->l_NbPattern = 15;
    pst_Data->l_MaxPattern = 0;
    pst_Data->l_Color = 0xFFFFFFFF;
    pst_Data->f_Time[0] = 0.0f;
    pst_Data->f_Time[1] = 0.0f;
    pst_Data->f_Time[2] = Cf_Infinit;
    pst_Data->f_Time[3] = Cf_Infinit;
    pst_Data->pst_Pattern = NULL;

    GFX_Lightning_SampleZero( pst_Data );

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Lightning_Destroy(void *_p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Lightning	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (!_p_Data) return;
	pst_Data = (GFX_tdst_Lightning *) _p_Data;

    if (pst_Data->l_MaxPattern)
    {
        pst_Data->l_MaxPattern = 0;
        MEM_Free( pst_Data->pst_Pattern );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_AllocPattern( GFX_tdst_Lightning *_pst_Data )
{
    LONG l_Number, l_P2, l1, l2;
    int  i;

    /* get nearest power 2 number */
    l_Number = _pst_Data->l_NbPattern;

    if (l_Number == 0)
    {
        _pst_Data->l_MaxPattern = 0;
        MEM_Free( _pst_Data->pst_Pattern );
        return;
    }

    l_P2 = 0;
    while ( l_Number )
    {
        l_Number >>= 1;
        l_P2++;
    }

    l1 = (1 << l_P2) - 1;
    l2 = (1 << (l_P2 - 1)) - 1;

    if ( ( l1 - _pst_Data->l_NbPattern ) > (_pst_Data->l_NbPattern - l2) )
        _pst_Data->l_NbPattern = l2;
    else
        _pst_Data->l_NbPattern = l1;

    l1 = _pst_Data->l_NbPattern * sizeof( MATH_tdst_Vector );
    
    if ( _pst_Data->l_MaxPattern )
    {
        if ( _pst_Data->l_NbPattern < _pst_Data->l_MaxPattern)
        {
            l_P2 = (_pst_Data->l_MaxPattern + 1) / (_pst_Data->l_NbPattern + 1);
            l2 = l_P2 - 1;
            for (i = 0; i < _pst_Data->l_NbPattern; i++, l2 += l_P2)
                MATH_CopyVector( _pst_Data->pst_Pattern + i, _pst_Data->pst_Pattern + l2 );
            _pst_Data->pst_Pattern = (MATH_tdst_Vector *) MEM_p_Realloc( _pst_Data->pst_Pattern, l1 );
        }
        else
        {
            _pst_Data->pst_Pattern = (MATH_tdst_Vector *) MEM_p_Realloc( _pst_Data->pst_Pattern, l1 );
            L_memset( _pst_Data->pst_Pattern, 0, l1);
            l_P2 = (_pst_Data->l_NbPattern + 1) / (_pst_Data->l_MaxPattern + 1);
            l2 = l_P2 - 1;
            for (i = 0; i < _pst_Data->l_MaxPattern; i++, l2 += l_P2)
                MATH_CopyVector( _pst_Data->pst_Pattern + l2, _pst_Data->pst_Pattern + i );
        }
        _pst_Data->l_MaxPattern = _pst_Data->l_NbPattern;
    }
    else
    {
        _pst_Data->pst_Pattern = (MATH_tdst_Vector *) MEM_p_Alloc( l1 );
        _pst_Data->l_MaxPattern = _pst_Data->l_NbPattern;
        GFX_Lightning_SampleZero( _pst_Data );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Lightning_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Lightning			*pst_Data;
	MATH_tdst_Vector			st_X, X, Y, P, Q;
    GEO_Vertex                  *V;
    MATH_tdst_Vector			*pst_Prev, *pst_Cur, *pst_Next;
	int							i, istart;
	GEO_tdst_IndexedTriangle	*T;
    float                       f1, f2, zref;
    ULONG                       DM, *pul_RLI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Lightning *) p_Data;

    pst_Data->f_Time[0] += TIM_gf_dt;
    if (pst_Data->f_Time[0] > pst_Data->f_Time[3])
        return 0;

    if (pst_Data->l_MaxPattern != pst_Data->l_NbPattern)
        GFX_AllocPattern( pst_Data );

    if ( !pst_Data->l_MaxPattern )
        return 1;

    /* test si le flare est dans un secteur invisible */
    if(((OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner)->uc_LOD_Vis == 0)
        return 1;

    /* culling */
    MATH_TransformVertex( &X, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix, &pst_Data->st_A );
    MATH_TransformVertex( &Y, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix, &pst_Data->st_B );
    if ( (X.z < 0.01f) && (Y.z < 0.01f) )
    {
        PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_NbGFXrendered, -1);
        return 1;
    }
    else
    {
        f1 = MATH_f_DotProduct(&X, &GDI_gpst_CurDD->st_Camera.st_NormPlaneLeft);
        f2 = MATH_f_DotProduct(&Y, &GDI_gpst_CurDD->st_Camera.st_NormPlaneLeft);
        if ( fInfZero(f1) && fInfZero(f2) )
        {
            PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_NbGFXrendered, -1);
            return 1;
        }
        else 
        {
            f1 = MATH_f_DotProduct(&X, &GDI_gpst_CurDD->st_Camera.st_NormPlaneRight);
            f2 = MATH_f_DotProduct(&Y, &GDI_gpst_CurDD->st_Camera.st_NormPlaneRight);
            if ( fInfZero(f1) && fInfZero(f2) )
            {
                PRO_IncRasterLong(&GDI_gpst_CurDD->pst_Raster->st_NbGFXrendered, -1);
                return 1;
            }
        }
    }

    if(ENG_gb_EngineRunning) 
        GFX_Lightning_SampleFractal( pst_Data );

	i = pst_Data->l_NbPattern;

    if ( pst_Data->l_Flags & LIGHTNING_UvCut)
    {
        GFX_NeedGeom(4 + 2 * i, 4 + 2 * i, 2 * (i + 1), 1);
    }
    else
    {
        GFX_NeedGeom(4 + 2 * i, 4, 2 * (i + 1), 1);

        /* set UV */
        GFX_gpst_Geo->dst_UV[0].fU = 0;
        GFX_gpst_Geo->dst_UV[0].fV = 0;
        GFX_gpst_Geo->dst_UV[1].fU = 1;
        GFX_gpst_Geo->dst_UV[1].fV = 0;
        GFX_gpst_Geo->dst_UV[2].fU = 0;
        GFX_gpst_Geo->dst_UV[2].fV = 1;
        GFX_gpst_Geo->dst_UV[3].fU = 1;
        GFX_gpst_Geo->dst_UV[3].fV = 1;
    }

	MATH_SubVector(&X, &pst_Data->st_B, &pst_Data->st_A);
	MATH_CrossProduct(&Y, &pst_Data->Z, &X);
	MATH_NormalizeEqualVector(&Y);

    MATH_SubVector( &P, &GDI_gpst_CurDD->st_Camera.st_Matrix.T, &pst_Data->st_A );
    MATH_SubVector( &Q, &pst_Data->st_B, &pst_Data->st_A );
    zref = MATH_f_DotProduct( &P, &Q ) / MATH_f_DotProduct( &Q, &Q );
    if (zref < 0)
        MATH_CopyVector( &P, &pst_Data->st_A );
    else if (zref > 1)
        MATH_CopyVector( &P, &pst_Data->st_B );
    else
        MATH_AddScaleVector( &P, &pst_Data->st_A, &Q, zref );
    MATH_SubEqualVector( &P, &GDI_gpst_CurDD->st_Camera.st_Matrix.T );
    MATH_CrossProduct( &st_X, &X, &P );
    
    if ( MATH_b_NulVector( &st_X ) )
    {
        MATH_ScaleVector( &st_X, &pst_Data->Z, pst_Data->f_Size );
    }
    else
    {
        MATH_NormalizeEqualVector( &st_X );
        MATH_ScaleEqualVector( &st_X, pst_Data->f_Size );
    }

    pst_Prev = &pst_Data->st_A;
    pst_Cur = &P;
    pst_Next = &Q;
    V = GFX_gpst_Geo->dst_Point;
    pul_RLI = GFX_gpst_Geo->dul_PointColors + 1;

    for ( i = 0; i <= pst_Data->l_NbPattern; i++)
    {
        if (i < pst_Data->l_NbPattern)
        {
            MATH_AddScaleVector(pst_Cur, &pst_Data->st_A, &X, pst_Data->pst_Pattern[i].z);
		    MATH_AddScaleVector(pst_Cur, pst_Cur, &Y, pst_Data->pst_Pattern[i].y * fCos(pst_Data->pst_Pattern[i].x));
		    MATH_AddScaleVector(pst_Cur, pst_Cur, &pst_Data->Z, pst_Data->pst_Pattern[i].y * fSin(pst_Data->pst_Pattern[i].x));
        }
        else
            pst_Cur = &pst_Data->st_B;

        MATH_AddVector(VCast(V++), pst_Prev, &st_X);
	    MATH_SubVector(VCast(V++), pst_Prev, &st_X);
        
        pst_Prev = pst_Cur;
        pst_Cur = pst_Next;
        pst_Next = pst_Prev;

        *pul_RLI++ = pst_Data->l_Color;
        *pul_RLI++ = pst_Data->l_Color;
    }

    MATH_AddVector(VCast(V++), pst_Prev, &st_X);
    MATH_SubVector(VCast(V++), pst_Prev, &st_X);
    *pul_RLI++ = 0;
    *pul_RLI++ = 0;

    pul_RLI = GFX_gpst_Geo->dul_PointColors + 1;
    pul_RLI[0] = 0;
    pul_RLI[1] = 0;

    pst_Data->f_Time[0] += TIM_gf_dt;
    istart = 0;
    
    if (pst_Data->f_Time[0] < pst_Data->f_Time[1])
    {
        zref = pst_Data->f_Time[0] / pst_Data->f_Time[1];
        i = 0;
        while ( ( i < pst_Data->l_NbPattern) && (pst_Data->pst_Pattern[i].z < zref) )
            i++;

        if ( i == 0)
        {
            if ( pst_Data->l_NbPattern)
                zref = (zref) / pst_Data->pst_Pattern[i].z;
        }
        else if ( i == pst_Data->l_NbPattern)
            zref = (zref - pst_Data->pst_Pattern[i - 1].z) / (1 - pst_Data->pst_Pattern[i - 1].z);
        else
            zref = (zref - pst_Data->pst_Pattern[i - 1].z) / (pst_Data->pst_Pattern[i].z - pst_Data->pst_Pattern[i - 1].z);

        V = GFX_gpst_Geo->dst_Point + (i * 2);
        MATH_BlendVector( VCast(V + 2), VCast(V), VCast(V + 2), zref );
        MATH_BlendVector( VCast(V + 3), VCast(V + 1), VCast(V + 3), zref );
        
        GFX_gpst_Geo->dst_Element->l_NbTriangles = 2 + 2 * i;
    }
    
    if (pst_Data->f_Time[0] > pst_Data->f_Time[2])
    {
        zref = (pst_Data->f_Time[0] - pst_Data->f_Time[2])/ (pst_Data->f_Time[3] - pst_Data->f_Time[2]);
        i = 0;
        while ( ( i < pst_Data->l_NbPattern) && (pst_Data->pst_Pattern[i].z < zref) )
            i++;

        if ( i == 0)
        {
            if ( pst_Data->l_NbPattern)
                zref = (zref) / pst_Data->pst_Pattern[i].z;
        }
        else if ( i == pst_Data->l_NbPattern)
            zref = (zref - pst_Data->pst_Pattern[i - 1].z) / (1 - pst_Data->pst_Pattern[i - 1].z);
        else
            zref = (zref - pst_Data->pst_Pattern[i - 1].z) / (pst_Data->pst_Pattern[i].z - pst_Data->pst_Pattern[i - 1].z);

        V = GFX_gpst_Geo->dst_Point + (i * 2);
        MATH_BlendVector( VCast(V), VCast( V), VCast( V + 2), zref );
        MATH_BlendVector( VCast( V + 1 ), VCast( V + 1 ), VCast( V + 3 ), zref );
        
        istart = i;
        GFX_gpst_Geo->dst_Element->l_NbTriangles -= 2 * i;
    }

	T = GFX_gpst_Geo->dst_Element->dst_Triangle;
    if (pst_Data->l_Flags & LIGHTNING_UvCut)
    {
        for(i = istart; i <= pst_Data->l_NbPattern; i++)
	    {
            if (i == 0)
            {
                GFX_gpst_Geo->dst_UV[0].fU = 0.0f;
                GFX_gpst_Geo->dst_UV[0].fV = 0.0f;
                GFX_gpst_Geo->dst_UV[1].fU = 0.0f;
                GFX_gpst_Geo->dst_UV[1].fV = 1.0f;
            }
            else
            {
                GFX_gpst_Geo->dst_UV[i * 2].fU = pst_Data->pst_Pattern[i - 1].z;
                GFX_gpst_Geo->dst_UV[i * 2].fV = 0;
                GFX_gpst_Geo->dst_UV[i * 2 + 1].fU = pst_Data->pst_Pattern[i - 1].z;
                GFX_gpst_Geo->dst_UV[i * 2 + 1].fV = 1;
            }

            T->auw_Index[0] = i * 2;
		    T->auw_Index[1] = i * 2 + 1;
		    T->auw_Index[2] = i * 2 + 3;
            T->auw_UV[0] = i * 2;
            T->auw_UV[1] = i * 2 + 1;
            T->auw_UV[2] = i * 2 + 3;
            T++;
        
            T->auw_Index[0] = i * 2 + 3;
		    T->auw_Index[1] = i * 2 + 2;
		    T->auw_Index[2] = i * 2;
            T->auw_UV[0] = i * 2 + 3;
            T->auw_UV[1] = i * 2 + 2;
            T->auw_UV[2] = i * 2 + 0;
            T++;
	    }

        GFX_gpst_Geo->dst_UV[i * 2].fU = 1.0f;
        GFX_gpst_Geo->dst_UV[i * 2].fV = 0.0f;
        GFX_gpst_Geo->dst_UV[i * 2 + 1].fU = 1.0f;
        GFX_gpst_Geo->dst_UV[i * 2 + 1].fV = 1.0f;

    }
    else
    {
	    for(i = istart; i <= pst_Data->l_NbPattern; i++)
	    {
            T->auw_Index[0] = i * 2;
		    T->auw_Index[1] = i * 2 + 1;
		    T->auw_Index[2] = i * 2 + 3;
            T->auw_UV[0] = 2;
            T->auw_UV[1] = 0;
            T->auw_UV[2] = 1;
            T++;
        
            T->auw_Index[0] = i * 2 + 3;
		    T->auw_Index[1] = i * 2 + 2;
		    T->auw_Index[2] = i * 2;
            T->auw_UV[0] = 1;
            T->auw_UV[1] = 3;
            T->auw_UV[2] = 2;
            T++;
	    }
    }

    M_GFX_CheckGeom();

	GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_Lighted | GDI_Cul_DM_UseAmbient);
	GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;

    return 1;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
