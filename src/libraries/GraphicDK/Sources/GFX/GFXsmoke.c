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
#include "GFX/GFX.h"
#include "GFX/GFXsmoke.h"
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
void *GFX_Smoke_Create(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Smoke *pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Smoke *) MEM_p_Alloc(sizeof(GFX_tdst_Smoke));
    L_memset( pst_Data, 0, sizeof(GFX_tdst_Smoke) );
    pst_Data->f_Friction = 0.9f;
    pst_Data->f_StartSize = 0.5f;
    pst_Data->f_EndSize = 2;
    pst_Data->f_LifeTime = 2;
    pst_Data->ul_StartColor = 0xFFFFFFFF;
    pst_Data->ul_EndColor = 0;

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Smoke_Destroy( void *p_Data )
{
    GFX_tdst_Smoke *pst_Data;

    if (!p_Data) return;
    pst_Data = (GFX_tdst_Smoke *) p_Data;

    if (pst_Data->ul_NbMax)
    {
        pst_Data->ul_NbMax = 0;
        MEM_Free( pst_Data->dst_Smoke );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Smoke_Alloc( GFX_tdst_Smoke *_pst_Data, LONG _l_Nb )
{
    LONG l;

    if ( _l_Nb <= 0 )
    {
        if (_pst_Data->ul_NbMax)
        {
            _pst_Data->ul_NbMax = 0;
            MEM_Free( _pst_Data->dst_Smoke );
        }
        return;
    }

    if ((ULONG) _l_Nb == _pst_Data->ul_NbMax) return;

    l = _l_Nb * sizeof( GFX_tdst_OneSmoke );
    
    if ( _pst_Data->ul_NbMax )
        _pst_Data->dst_Smoke = (GFX_tdst_OneSmoke *) MEM_p_Realloc( _pst_Data->dst_Smoke, l );
    else
        _pst_Data->dst_Smoke = (GFX_tdst_OneSmoke *) MEM_p_Alloc( l );

    _pst_Data->ul_NbMax = _l_Nb;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Smoke_AddOne( GFX_tdst_Smoke *_pst_Data )
{
    GFX_tdst_OneSmoke   *s;

    if (_pst_Data->ul_CurNb >= _pst_Data->ul_NbMax)
        return;

    s = _pst_Data->dst_Smoke + _pst_Data->ul_CurNb++;
    s->f_Time = 0;
    MATH_CopyVector( &s->st_Pos, &_pst_Data->st_Pos );
    MATH_CopyVector( &s->st_Speed, &_pst_Data->st_Speed );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Smoke_Seti( void *p_Data, int _i_Param, int _i_Value )
{
    GFX_tdst_Smoke *pst_Data;
    
    if (!p_Data) return;
    pst_Data = (GFX_tdst_Smoke *) p_Data;

    if (_i_Param == GFX_SmokeSetNumber)
        GFX_Smoke_Alloc( pst_Data, _i_Value );
    else if (_i_Param == GFX_SmokeAddOne)
        GFX_Smoke_AddOne( pst_Data );
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Smoke_Update( GFX_tdst_Smoke *_pst_Data, float _f_Time, int n )
{
    int                 i_Trame;
    GFX_tdst_OneSmoke   *s, *last;

    /* update */
    s = _pst_Data->dst_Smoke;
    last = s + _pst_Data->ul_CurNb;

    while ( s < last )
    {
        s->f_Time += _f_Time;
        if (s->f_Time >= _pst_Data->f_LifeTime)
        {
            *s = _pst_Data->dst_Smoke[ --_pst_Data->ul_CurNb ];
            last--;
            continue;
        }

        i_Trame = n;
        while (i_Trame--)
        {
            MATH_AddScaleVector( &s->st_Pos, &s->st_Pos, &s->st_Speed, (1.0f/60.0f) );
            MATH_ScaleEqualVector(&s->st_Speed, _pst_Data->f_Friction);
            MATH_AddEqualVector( &s->st_Speed, &_pst_Data->st_Wind );
        }
        s++;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Smoke_Render(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Smoke      *pst_Data;
    GFX_tdst_OneSmoke   *s, *last;
    GEO_Vertex          *V;
    GEO_tdst_IndexedTriangle *T;
	MATH_tdst_Vector	*X, *Y, DX, DY;
    ULONG               DM, ul_Color, *pul_Color;
    float               f, factor, f_Size, time;
    int                 trame;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Smoke *) p_Data;

    pst_Data->f_TimeLeft += TIM_gf_dt;

    
    trame = (int) (pst_Data->f_TimeLeft * 60.0f);
    time = trame * (1.0f / 60.0f);
    pst_Data->f_TimeLeft -= time;

    if (trame)
        GFX_Smoke_Update( pst_Data, time, trame );

    /* test si le gfx est dans un secteur invisible */
    if(((OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner)->uc_LOD_Vis == 0)
        return 1;

    if ( !pst_Data->ul_CurNb ) return 1;

	GFX_NeedGeom(4 * pst_Data->ul_CurNb, 4, 2 * pst_Data->ul_CurNb, 1);

    /* set UV */
    GFX_gpst_Geo->dst_UV[0].fU = 0;
    GFX_gpst_Geo->dst_UV[0].fV = 0;
    GFX_gpst_Geo->dst_UV[1].fU = 1;
    GFX_gpst_Geo->dst_UV[1].fV = 0;
    GFX_gpst_Geo->dst_UV[2].fU = 1;
    GFX_gpst_Geo->dst_UV[2].fV = 1;
    GFX_gpst_Geo->dst_UV[3].fU = 0;
    GFX_gpst_Geo->dst_UV[3].fV = 1;

    X = MATH_pst_GetXAxis( &GDI_gpst_CurDD->st_Camera.st_Matrix );
    Y = MATH_pst_GetYAxis( &GDI_gpst_CurDD->st_Camera.st_Matrix );
    V = GFX_gpst_Geo->dst_Point;
    T = GFX_gpst_Geo->dst_Element->dst_Triangle;
    pul_Color = GFX_gpst_Geo->dul_PointColors + 1;
    s = pst_Data->dst_Smoke;
    last = s + pst_Data->ul_CurNb;
    f = (pst_Data->f_LifeTime) ? (1.0f / pst_Data->f_LifeTime) : 0;

    while ( s < last )
    {
        factor = s->f_Time * f;
        f_Size = pst_Data->f_StartSize + factor * (pst_Data->f_EndSize - pst_Data->f_StartSize);
        ul_Color = COLOR_ul_Blend( pst_Data->ul_StartColor, pst_Data->ul_EndColor, factor );

        /* set vertex */
        MATH_ScaleVector( &DX, X, f_Size );
        MATH_ScaleVector( &DY, Y, f_Size );

	    MATH_SubScaleVector( VCast( V ), &s->st_Pos, &DX, 0.5);
        MATH_AddScaleVector( VCast( V ), VCast( V ), &DY, 0.5);
	    MATH_AddVector(VCast( V+1 ), VCast( V ), &DX);
        MATH_SubVector(VCast( V+2 ), VCast( V+1 ), &DY);
        MATH_SubVector(VCast( V+3 ), VCast( V+2 ), &DX);
        
	    trame = V - GFX_gpst_Geo->dst_Point;
        V += 4;
        T->auw_Index[0] = (unsigned short) trame;
        T->auw_Index[1] = (unsigned short) trame + 1;
        T->auw_Index[2] = (unsigned short) trame + 2;
        T->auw_UV[0] = 0;
	    T->auw_UV[1] = 1;
	    T->auw_UV[2] = 2;
        T++;

        T->auw_Index[0] = (unsigned short) trame + 2;
	    T->auw_Index[1] = (unsigned short) trame + 3;
        T->auw_Index[2] = (unsigned short) trame;
        T->auw_UV[0] = 2;
        T->auw_UV[1] = 3;
        T->auw_UV[2] = 0;
        T++;

        *pul_Color++ = ul_Color;
        *pul_Color++ = ul_Color;
        *pul_Color++ = ul_Color;
        *pul_Color++ = ul_Color;

        s++;
    }

    M_GFX_CheckGeom();

    /* render */
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
