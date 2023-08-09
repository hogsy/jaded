/*$T GFXripple.c GC! 1.081 11/27/01 15:59:24 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "GDInterface/GDInterface.h"
#include "GFX/GFX.h"
#include "GFX/GFXripple.h"
#include "SOFT/SOFTcolor.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/OBJects/OBJorient.h"

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
void *GFX_Ripple_Create( OBJ_tdst_GameObject *_pst_GO )
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Ripple	 *pst_Data;
    MATH_tdst_Matrix *M;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Ripple *) MEM_p_Alloc(sizeof(GFX_tdst_Ripple));
	L_memset( pst_Data, 0, sizeof(GFX_tdst_Ripple));
    MATH_CopyVector( &pst_Data->st_Pos, OBJ_pst_GetAbsolutePosition( _pst_GO ) );
    M = OBJ_pst_GetAbsoluteMatrix( _pst_GO );
    MATH_NegVector( &pst_Data->st_Dir, MATH_pst_GetYAxis( M ) );
    MATH_CopyVector( &pst_Data->st_Banking, MATH_pst_GetZAxis( M ) );

    pst_Data->f_LifeTime = 5;
	pst_Data->f_IncreaseRate = 1;
	pst_Data->f_GenerationRate = 0.2f;
	pst_Data->f_Speed = 1;
    
	pst_Data->ul_StartColor = 0xFFFFFFFF;
	pst_Data->ul_EndColor = 0;

	return (void *) pst_Data;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Ripple_Destroy(void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Ripple	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!p_Data) return;
	pst_Data = (GFX_tdst_Ripple *) p_Data;

	if(pst_Data->ul_NbMax)
	{
		pst_Data->ul_NbMax = 0;
		MEM_Free(pst_Data->dst_Ripple);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Ripple_Alloc(GFX_tdst_Ripple *_pst_Data, LONG _l_Nb)
{
	/*~~~~~~*/
	LONG	l;
	/*~~~~~~*/

	if(_l_Nb <= 0)
	{
		if(_pst_Data->ul_NbMax)
		{
			_pst_Data->ul_NbMax = 0;
			MEM_Free(_pst_Data->dst_Ripple);
		}
		return;
	}

	if((ULONG) _l_Nb == _pst_Data->ul_NbMax) return;

	l = _l_Nb * sizeof(GFX_tdst_OneRipple);

	if(_pst_Data->ul_NbMax)
		_pst_Data->dst_Ripple = (GFX_tdst_OneRipple *) MEM_p_Realloc(_pst_Data->dst_Ripple, l);
	else
		_pst_Data->dst_Ripple = (GFX_tdst_OneRipple *) MEM_p_Alloc(l);

	_pst_Data->ul_NbMax = _l_Nb;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Ripple_Seti(void *p_Data, int _i_Param, int _i_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Ripple	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!p_Data) return;
	pst_Data = (GFX_tdst_Ripple *) p_Data;

	if(_i_Param == GFX_RippleSetNumber)
    {
        if (_i_Value == -1)
            pst_Data->ul_Flags |= GFX_C_Ripple_Die;
        else if (_i_Value == -2)
            pst_Data->ul_Flags |= GFX_C_Ripple_DieAfterGenAll;
        else
		    GFX_Ripple_Alloc(pst_Data, _i_Value);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Ripple_Setf( void *p_Data, int _i_Param, float _f_Value )
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Ripple	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!p_Data) return;
	pst_Data = (GFX_tdst_Ripple *) p_Data;

	if(_i_Param == GFX_RippleGenerationRate)
    {
        pst_Data->f_GenerationRate = _f_Value;
        pst_Data->f_GenTimeLeft = _f_Value;
    }
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Ripple_Generate(GFX_tdst_Ripple *_pst_Data, float _f_Time, OBJ_tdst_GameObject *_pst_GO )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_OneRipple	*s;
    MATH_tdst_Vector    *P;
    MATH_tdst_Vector    *P1;
    MATH_tdst_Vector    *P2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (_pst_Data->f_GenerationRate < 0) return;
    if (_pst_Data->ul_Flags & GFX_C_Ripple_Die) return;

    _pst_Data->f_GenTimeLeft += _f_Time;
    P = &_pst_Data->st_Pos;
    P1 = &_pst_Data->st_Dir;
    P2 = &_pst_Data->st_Banking;

    while (_pst_Data->f_GenTimeLeft > _pst_Data->f_GenerationRate )
    {
        if (_pst_Data->ul_CurNb == _pst_Data->ul_NbMax)
            break;

        _pst_Data->ul_GenNb++;
        s = &_pst_Data->dst_Ripple[ _pst_Data->ul_CurNb++ ];
        s->f_Time = _pst_Data->f_LifeTime;
        
        MATH_CopyVector( &s->st_Pos, P );
        MATH_NegVector( &s->st_Dir, P1);
		MATH_CopyVector( &s->st_Banking, P2);
        
        if (_pst_Data->f_GenerationRate == 0)
        {
            _pst_Data->f_GenerationRate = -1;
            break;
        }

        _pst_Data->f_GenTimeLeft -= _pst_Data->f_GenerationRate;
    }

    if ( (_pst_Data->ul_GenNb == _pst_Data->ul_NbMax) && (_pst_Data->ul_Flags & GFX_C_Ripple_DieAfterGenAll) )
        _pst_Data->ul_Flags |= GFX_C_Ripple_Die;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GFX_Ripple_Update(GFX_tdst_Ripple *_pst_Data, float _f_Time, int n)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_OneRipple	*s, *last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* update */
	s = _pst_Data->dst_Ripple;
	last = s + _pst_Data->ul_CurNb;

	while(s < last)
	{
        s->f_Time -= _f_Time;
        if (s->f_Time < 0)
		{
			*s = _pst_Data->dst_Ripple[--_pst_Data->ul_CurNb];
			last--;
			continue;
		}
		MATH_AddScaleVector(&s->st_Pos, &s->st_Pos, &s->st_Dir, _pst_Data->f_Speed * _f_Time );
		s++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GFX_i_Ripple_Render(void *p_Data, OBJ_tdst_GameObject *_pst_GO )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GFX_tdst_Ripple				*pst_Data;
	GFX_tdst_OneRipple			*s, *last;
	GEO_Vertex					*V;
	GEO_tdst_IndexedTriangle	*T;
	MATH_tdst_Vector			*X, *Y, DX, DY;
	ULONG						DM, *pul_Color, ul_Color;
	float						factor, f;
    int                         index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (GFX_tdst_Ripple *) p_Data;

	GFX_Ripple_Generate( pst_Data, TIM_gf_dt, _pst_GO );
    GFX_Ripple_Update(pst_Data, TIM_gf_dt, 0);

    if (!pst_Data->ul_CurNb)
        return (pst_Data->ul_Flags & GFX_C_Ripple_Die) ? 0 : 1;

    /* test si le flare est dans un secteur invisible */
    if( !(pst_Data->ul_Flags & GFX_C_Ripple_DieAfterGenAll) && (((OBJ_tdst_GameObject *) GFX_gpst_Current->p_Owner)->uc_LOD_Vis == 0) )
        return 1;


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

	X = MATH_pst_GetXAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix);
	Y = MATH_pst_GetYAxis(&GDI_gpst_CurDD->st_Camera.st_Matrix);
	V = GFX_gpst_Geo->dst_Point;
	T = GFX_gpst_Geo->dst_Element->dst_Triangle;
	pul_Color = GFX_gpst_Geo->dul_PointColors + 1;
	s = pst_Data->dst_Ripple;
	last = s + pst_Data->ul_CurNb;
    f = (pst_Data->f_LifeTime != 0) ? fOptInv( pst_Data->f_LifeTime) : 0;

	while(s < last)
	{
        factor = pst_Data->f_StartSize + ((pst_Data->f_LifeTime - s->f_Time) * pst_Data->f_IncreaseRate);
        MATH_ScaleVector( &DX, &s->st_Banking, factor );
        MATH_CrossProduct( &DY, &s->st_Dir, &DX );
        
		MATH_SubScaleVector(VCast(V), &s->st_Pos, &DX, 0.5);
		MATH_AddScaleVector(VCast(V), VCast(V), &DY, 0.5);
		MATH_AddVector(VCast(V + 1), VCast(V), &DX);
		MATH_SubVector(VCast(V + 2), VCast(V + 1), &DY);
		MATH_SubVector(VCast(V + 3), VCast(V + 2), &DX);

		index = V - GFX_gpst_Geo->dst_Point;
		V += 4;
		T->auw_Index[0] = index;
		T->auw_Index[1] = index + 1;
		T->auw_Index[2] = index + 2;
		T->auw_UV[0] = 0;
		T->auw_UV[1] = 1;
		T->auw_UV[2] = 2;
		T++;

		T->auw_Index[0] = index + 2;
		T->auw_Index[1] = index + 3;
		T->auw_Index[2] = index;
		T->auw_UV[0] = 2;
		T->auw_UV[1] = 3;
		T->auw_UV[2] = 0;
		T++;

        ul_Color = COLOR_ul_Blend( pst_Data->ul_EndColor, pst_Data->ul_StartColor, s->f_Time * f );
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
