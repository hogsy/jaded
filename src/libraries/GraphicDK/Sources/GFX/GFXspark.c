/*$T GFXline.c GC! 1.081 09/19/00 09:14:29 */


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
#include "GFX/GFXspark.h"
#include "GEOmetric/GEODebugObject.h"
#include "SOFT/SOFTcolor.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "SOFT/SOFTzlist.h"

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
void *GFX_Spark_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Spark *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Spark *) MEM_p_VMAlloc(sizeof(GFX_tdst_Spark));
    L_memset( pst_Data, 0, sizeof(GFX_tdst_Spark) );

    pst_Data->f_Friction = 0.9f;
    MATH_InitVector( &pst_Data->st_Norm, 0, 0, 1.0f );
    MATH_InitVector( &pst_Data->st_Gravity, 0, 0, -2.0f );
    pst_Data->f_Size = 2.0f;
    pst_Data->f_SpeedMax = 4.0f;
    pst_Data->f_SpeedMin = 2.0f;
    pst_Data->ul_Color = 0xFFFFFFFF;
    pst_Data->f_TimeMin = 1.0f;
    pst_Data->f_TimeMax = 2.0f;
    pst_Data->f_Scale = 1.0f;
    pst_Data->f_Period = 0.1f;
    pst_Data->l_Flags = SPARK_OneTriangle;
	pst_Data->Zorder = 4;

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Spark_Destroy( void *p_Data )
{
    GFX_tdst_Spark *pst_Data;

    if (!p_Data) return;
    pst_Data = (GFX_tdst_Spark *) p_Data;

    if (pst_Data->l_NbMax)
    {
        pst_Data->l_NbMax = 0;
        MEM_Free( pst_Data->dst_Spark );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Spark_Generate( GFX_tdst_Spark *_pst_Data )
{
    GFX_tdst_OneSpark   *s;
    MATH_tdst_Vector    X, Y;
    float               alpha, beta, scale;

    /* generate new particle */
    _pst_Data->f_GenTime -= TIM_gf_dt;
    
    while (_pst_Data->f_GenTime < 0)
    {
        if (_pst_Data->l_CurNb >= _pst_Data->l_Nb)
            break;

        _pst_Data->f_GenTime += _pst_Data->f_Period;

        MATH_CrossProduct(&X, &_pst_Data->st_Norm, &MATH_gst_BaseVectorK );
        if ( MATH_b_NulVector(&X) )
            MATH_CrossProduct(&X, &_pst_Data->st_Norm, &MATH_gst_BaseVectorJ );
        MATH_CrossProduct( &Y, &X, &_pst_Data->st_Norm );
        MATH_NormalizeEqualVector( &X );
        MATH_NormalizeEqualVector( &Y );

        s = _pst_Data->dst_Spark + _pst_Data->l_CurNb++;
        MATH_CopyVector( &s->st_Pos, &_pst_Data->st_Pos );
            
        alpha = fRand(-_pst_Data->f_Angle, _pst_Data->f_Angle);
		beta = fRand(0.0f, Cf_2Pi);
		scale = fRand(_pst_Data->f_SpeedMin, _pst_Data->f_SpeedMax);
		MATH_ScaleVector(&s->st_Speed, &X, scale * fOptCos(beta) * fOptSin(alpha));
		MATH_AddScaleVector( &s->st_Speed, &s->st_Speed, &Y, scale * fOptSin(beta) * fOptSin(alpha) );
        MATH_AddScaleVector( &s->st_Speed, &s->st_Speed, &_pst_Data->st_Norm, scale * fOptCos(alpha) );

        s->f_Time = fRand( _pst_Data->f_TimeMin, _pst_Data->f_TimeMax );
        s->f_Factor = 0;
        s->l_Flags = 0;
    }

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Spark_Update( GFX_tdst_Spark *_pst_Data, GEO_Vertex *vOld, GEO_Vertex *vNew, LONG l_Pitch )
{
    int                 i;
    MATH_tdst_Vector    v, middle;
    GFX_tdst_OneSpark   *s,  *slast;
    float               t, f, size2;

    /* kill one particle if too far from source */
    if ( ( _pst_Data->l_Flags & SPARK_MaxDist ) && _pst_Data->f_MaxDist )
    {
        s = _pst_Data->dst_Spark;
        slast = s + _pst_Data->l_CurNb;
		size2 = _pst_Data->f_MaxDist * _pst_Data->f_MaxDist;

		for ( ; s < slast ; s++ )
		{
			MATH_SubVector(&v, &_pst_Data->st_Pos, &s->st_Pos );
			if (MATH_f_SqrNormVector( &v ) > size2)
				s->f_Time = 0;
		}
	}

	
	/* update */
    s = _pst_Data->dst_Spark;
    size2 = _pst_Data->f_Size * _pst_Data->f_Size;

    for (i = 0; i< _pst_Data->l_CurNb; )
    {
        s->f_Time -= TIM_gf_dt;
        if (s->f_Time < 0)
        {
            if ( !(_pst_Data->l_Flags & SPARK_ColorDeath) || (s->l_Flags & SPARK_Dying) )
            {
                *s = _pst_Data->dst_Spark[ --_pst_Data->l_CurNb ];
                continue;
            }
            
            f = fRand( _pst_Data->f_DeathMin, _pst_Data->f_DeathMax );
            if ( f <= 0 )
            {
                *s = _pst_Data->dst_Spark[ --_pst_Data->l_CurNb ];
                continue;
            }
            s->f_Factor = fOptInv( f );
            s->f_Time = s->f_Time + f;
            s->l_Flags |= SPARK_Dying;
        }

        t = TIM_gf_dt;
        while ( t >= (2.0f/60.0f) )
        {
            MATH_AddScaleVector( &s->st_Pos, &s->st_Pos, &s->st_Speed, (1.0f/60.0f) );
            MATH_AddScaleVector( &s->st_Speed, &s->st_Speed, &_pst_Data->st_Gravity, (1.0f/60.0f) );
            MATH_ScaleEqualVector(&s->st_Speed, _pst_Data->f_Friction);
            t -= 1.0f/60.0f;
        }

        /* update position */
        MATH_CopyVector( VCast(vOld), &s->st_Pos );
        MATH_AddScaleVector( &s->st_Pos, &s->st_Pos, &s->st_Speed, t );
        MATH_CopyVector( VCast(vNew), &s->st_Pos );
        /* update speed */
        MATH_AddScaleVector( &s->st_Speed, &s->st_Speed, &_pst_Data->st_Gravity, t );
        MATH_ScaleEqualVector(&s->st_Speed, _pst_Data->f_Friction);

        MATH_SubVector( &v, VCast(vOld), VCast(vNew) );
        f = MATH_f_SqrVector( &v );

        switch( _pst_Data->l_Flags & SPARK_EnlargeMask)
        {
        case SPARK_EnlargeFromOld:
            MATH_ScaleEqualVector( &v, (1.0f - _pst_Data->f_Scale ));
            MATH_AddEqualVector( VCast(vNew), &v);
            f *= _pst_Data->f_Scale;
            break;
        case SPARK_EnlargeFromCenter:
            MATH_ScaleEqualVector( &v, (_pst_Data->f_Scale - 1.0f) * 0.5f );
            MATH_AddEqualVector( VCast(vOld), &v);
            MATH_SubEqualVector( VCast(vNew), &v);
            f *= _pst_Data->f_Scale;
            break;
        case SPARK_EnlargeFromNew:
            MATH_ScaleEqualVector( &v, _pst_Data->f_Scale - 1.0f);
            MATH_AddEqualVector( VCast(vOld), &v);
            f *= _pst_Data->f_Scale;
            break;
        }

        /* store size in vOld + 1 vertex, x component */
        if ( f < size2 )
            (vOld + 1)->x = fOptSqrt( f );
        else
            (vOld + 1)->x = _pst_Data->f_Size;

        {
			(vOld + 1)->x *= 4;
            MATH_SubVector( &v, VCast(vOld), VCast(vNew) );
            MATH_AddVector( &middle, VCast(vOld), VCast(vNew) );
            MATH_ScaleEqualVector( &middle, 0.5f );
            MATH_AddVector( VCast( vNew ), &middle, &v );
            MATH_SubVector( VCast( vOld ), &middle, &v );
        }

        /* store color in vOld + 1 vertex, y component */
		LIGHT_gpul_CurrentTable = LIGHT_gaul_LinearTable;
        if (s->l_Flags & SPARK_Dying)
            *(LONG *) &(vOld + 1)->y = LIGHT_ul_MulColor( _pst_Data->ul_Color, s->f_Time * s->f_Factor );
        else
            *(LONG *) &(vOld + 1)->y = _pst_Data->ul_Color;
        
        /* update speed */
        MATH_AddScaleVector( &s->st_Speed, &s->st_Speed, &_pst_Data->st_Gravity, TIM_gf_dt );
        MATH_ScaleEqualVector(&s->st_Speed, _pst_Data->f_Friction);

        i++;
        s++;
        vNew += l_Pitch;
        vOld += l_Pitch;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_AllocSpark( GFX_tdst_Spark *_pst_Data )
{
    LONG l;

    if (_pst_Data->l_Nb == 0)
    {
        _pst_Data->l_NbMax = 0;
        MEM_Free( _pst_Data->dst_Spark );
        return;
    }

    l = _pst_Data->l_Nb * sizeof( GFX_tdst_OneSpark );
    
    if ( _pst_Data->l_NbMax )
        _pst_Data->dst_Spark = (GFX_tdst_OneSpark *) MEM_p_VMRealloc( _pst_Data->dst_Spark, l );
    else
        _pst_Data->dst_Spark = (GFX_tdst_OneSpark *) MEM_p_VMAlloc( l );

    _pst_Data->l_NbMax = _pst_Data->l_Nb;

    if (_pst_Data->l_CurNb > _pst_Data->l_NbMax )
        _pst_Data->l_CurNb = _pst_Data->l_NbMax;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Spark_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Spark      *pst_Data;
	MATH_tdst_Vector	X;
    MATH_tdst_Vector    v, w;
    int                 i;
    GEO_Vertex          *Pt;
    GEO_tdst_IndexedTriangle *Tri;
    ULONG               DM, *pul_RLI, ul_Color;
    LONG                l_Pitch;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/


    /* init, test */
    if (!p_Data) return 1;
	pst_Data = (GFX_tdst_Spark *) p_Data;

    pst_Data->l_Flags |= SPARK_OneTriangle;


    if (pst_Data->l_NbMax != pst_Data->l_Nb )
        GFX_AllocSpark( pst_Data );

    if (!pst_Data->l_NbMax) return 1;

    l_Pitch = ( pst_Data->l_Flags & SPARK_OneTriangle) ? 3 : 4;

    /* generate and update */
    if ( !(pst_Data->l_Flags & SPARK_NoMoreGeneration ) )
    {
        GFX_Spark_Generate( pst_Data );

        if ( ( pst_Data->l_CurNb >= pst_Data->l_Nb ) && (pst_Data->l_Flags & SPARK_GenerateOnlyOnce) )
            pst_Data->l_Flags |= SPARK_NoMoreGeneration;

        GFX_NeedGeom( pst_Data->l_CurNb * l_Pitch, 4, pst_Data->l_CurNb * (l_Pitch-2), 1);
        GFX_Spark_Update( pst_Data, GFX_gpst_Geo->dst_Point, GFX_gpst_Geo->dst_Point + 2, l_Pitch );
        if ( !pst_Data->l_CurNb ) 
            return 1;
    }
    else
    {
        GFX_NeedGeom( pst_Data->l_CurNb * l_Pitch, 4, pst_Data->l_CurNb * (l_Pitch-2), 1);
        GFX_Spark_Update( pst_Data, GFX_gpst_Geo->dst_Point, GFX_gpst_Geo->dst_Point + 2, l_Pitch );
        if ( !pst_Data->l_CurNb )
            return 0;
    }

    if(((OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner)->uc_LOD_Vis == 0)
        return 1;

    if ( pst_Data->l_Flags & SPARK_OneTriangle)
    {
        GFX_NeedGeom( pst_Data->l_CurNb * 3, 3, pst_Data->l_CurNb, 1);
    
        /* set UV */
        
        GFX_gpst_Geo->dst_UV[0].fU = 1;
        GFX_gpst_Geo->dst_UV[0].fV = 0;
        GFX_gpst_Geo->dst_UV[1].fU = 0;
        GFX_gpst_Geo->dst_UV[1].fV = 0;
        GFX_gpst_Geo->dst_UV[2].fU = 0.5f;
        GFX_gpst_Geo->dst_UV[2].fV = 1;
    }
    else
    {
        GFX_NeedGeom( pst_Data->l_CurNb * 4, 4, pst_Data->l_CurNb * 2, 1);
    
        /* set UV */
        GFX_gpst_Geo->dst_UV[0].fU = 0;
        GFX_gpst_Geo->dst_UV[0].fV = 0;
        GFX_gpst_Geo->dst_UV[1].fU = 1;
        GFX_gpst_Geo->dst_UV[1].fV = 0;
        GFX_gpst_Geo->dst_UV[2].fU = 1;
        GFX_gpst_Geo->dst_UV[2].fV = 1;
        GFX_gpst_Geo->dst_UV[3].fU = 0;
        GFX_gpst_Geo->dst_UV[3].fV = 1;
    }

    /* compute point and triangles */
    Pt = GFX_gpst_Geo->dst_Point;
    pul_RLI = GFX_gpst_Geo->dul_PointColors + 1;
    Tri = GFX_gpst_Geo->dst_Element->dst_Triangle;

    if ( pst_Data->l_Flags & SPARK_OneTriangle)
    {
        for (i = 0; i < pst_Data->l_CurNb; i++)
        {
            ul_Color = *(LONG *) &Pt[1].y;
            *pul_RLI++ = ul_Color;
            *pul_RLI++ = ul_Color;
            *pul_RLI++ = ul_Color;

            MATH_SubVector( &w, VCast( Pt + 2 ), &GDI_gpst_CurDD->st_Camera.st_Matrix.T );
            MATH_SubVector( &v, VCast( Pt + 2 ), VCast( Pt ) );
            MATH_CrossProduct( &X, &v, &w );
            MATH_NormalizeEqualVector( &X );
            MATH_ScaleEqualVector( &X, Pt[1].x * 0.5f );

            MATH_SubVector(VCast( Pt + 1), VCast( Pt + 0),  &X);
            MATH_AddEqualVector( VCast( Pt ), &X);

            Tri->auw_Index[0] = (unsigned short)(i * 3 + 0);
	        Tri->auw_Index[1] = (unsigned short)(i * 3 + 1);
	        Tri->auw_Index[2] = (unsigned short)(i * 3 + 2);
            Tri->auw_UV[0] = 0;
            Tri->auw_UV[1] = 1;
            Tri->auw_UV[2] = 2;
            Tri++;
            Pt += 3;
        }
    }
    else
    {
        for (i = 0; i < pst_Data->l_CurNb; i++)
        {
            ul_Color = *(LONG *) &Pt[1].y;
            *pul_RLI++ = ul_Color;
            *pul_RLI++ = ul_Color;
            *pul_RLI++ = ul_Color;
            *pul_RLI++ = ul_Color;

            MATH_SubVector( &w, VCast( Pt + 2 ), &GDI_gpst_CurDD->st_Camera.st_Matrix.T );
            MATH_SubVector( &v, VCast( Pt + 2 ), VCast( Pt ) );
            MATH_CrossProduct( &X, &v, &w );
            MATH_NormalizeEqualVector( &X );
            MATH_ScaleEqualVector( &X, Pt[1].x * 0.5f );

            MATH_SubVector(VCast( Pt + 3), VCast( Pt + 0), &X);
            MATH_AddVector(VCast( Pt + 0), VCast( Pt + 0), &X);
            MATH_AddVector(VCast( Pt + 1), VCast( Pt + 2),  &X);
            MATH_SubVector(VCast( Pt + 2), VCast( Pt + 2), &X);

            Tri[0].auw_Index[0] = (unsigned short)(i * 4 + 0);
	        Tri[0].auw_Index[1] = (unsigned short)(i * 4 + 1);
	        Tri[0].auw_Index[2] = (unsigned short)(i * 4 + 2);
            Tri[0].auw_UV[0] = 0;
            Tri[0].auw_UV[1] = 1;
            Tri[0].auw_UV[2] = 2;
	        
            Tri[1].auw_Index[0] = (unsigned short)(i * 4 + 2);
	        Tri[1].auw_Index[1] = (unsigned short)(i * 4 + 3);
	        Tri[1].auw_Index[2] = (unsigned short)(i * 4 + 0);
            Tri[1].auw_UV[0] = 2;
            Tri[1].auw_UV[1] = 3;
            Tri[1].auw_UV[2] = 0;    

            Pt += 4;
            Tri += 2;
        }
    }

    M_GFX_CheckGeom();

    /* render */
    GFX_gpst_Geo->dst_Element->l_MaterialId = 0;
    DM = GDI_gpst_CurDD->ul_CurrentDrawMask;
    GDI_gpst_CurDD->ul_CurrentDrawMask &= ~(GDI_Cul_DM_TestBackFace | GDI_Cul_DM_Lighted | GDI_Cul_DM_UseAmbient);
	SOFT_ZList_SelectZListe(pst_Data->Zorder);
    GFX_gpst_Geo->st_Id.i->pfn_Render(GFX_gpst_GO);
	SOFT_ZList_SelectZListe(4);
    GDI_gpst_CurDD->ul_CurrentDrawMask = DM;
    return 1;
}

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
