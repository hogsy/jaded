/*$T GEOsubobject.c GC! 1.081 01/04/02 11:44:31 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "GRObject/GROedit.h"
#include "GEOmetric/GEOobjectcomputing.h"
#include "GEOmetric/GEOobjectaccess.h"
#include "GEOmetric/GEOstaticLOD.h"
#include "GEOmetric/GEO_SKIN.h"
#include "GEOmetric/GEO_MRM.h"
#include "MATHs/MATH.h"
#include "GDInterface/GDInterface.h"
#include "SOFT/SOFTlinear.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTcolor.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEO_STRIP.h"
#include "ENGine/sources/WORld/WORupdate.h"
#include "ENGine/sources/WORld/WORuniverse.h"
#include "ENGine/sources/OBJects/OBJorient.h"
#include "ENGine/sources/MoDiFier/MDFmodifier_GEO.h"

#ifdef _XENON_RENDER
#include "XenonGraphics/XeIndexBuffer.h"
#endif


// -- Bug: modif temporaire --
MATH_tdst_Vector mst_MoveBugLocal;
// ---------------------------

void GEO_ComputeFloatingSelection(GEO_tdst_Object *_pst_Obj);
/*$4
 ***********************************************************************************************************************
    macros
 ***********************************************************************************************************************
 */

#define M_GEOSO_EdgeTriangle2Triangle(_o_, _e_, _i_) \
		_o_->dst_Element[((_e_->ul_Triangle[_i_] >> 16) & 0xFFF)].dst_Triangle + \
		(_e_->ul_Triangle[_i_] & 0xFFFF);

ULONG SmoothSelIsActivate = 0;
ULONG InvertSel = 0;
float DistanceMax = .08f;
ULONG SmoothSelMustBeUpdate = 0;
float topologicCare = 1.5f;
ULONG SmoothSelMode = 1;

/*$4
 *******************************************************************************************************************
	Functions
 *******************************************************************************************************************
 */

/*
 ===================================================================================================================
 ===================================================================================================================
 */
void GEO_SubObject_AllocBuffers(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject	*pst_SO;
    LONG				l_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_SO = _pst_Obj->pst_SubObject;
	l_Size = (_pst_Obj->l_NbPoints < (LONG) pst_SO->ul_NbFaces) ? pst_SO->ul_NbFaces : _pst_Obj->l_NbPoints;

	if(l_Size == (LONG) pst_SO->ul_PtInBuffer) return;

	if(pst_SO->ul_PtInBuffer)
	{
		L_free(pst_SO->dst_2D);
		L_free(pst_SO->dst_3D);
	}

    if (pst_SO->dst_PointTransformed)
    {
        L_free( pst_SO->dst_PointTransformed );
        pst_SO->dst_PointTransformed = NULL;
    }

	pst_SO->ul_PtInBuffer = l_Size;
	l_Size *= sizeof(MATH_tdst_Vector);
	if(l_Size)
	{
		pst_SO->dst_2D = (MATH_tdst_Vector *) L_malloc(l_Size);
		pst_SO->dst_3D = (MATH_tdst_Vector *) L_malloc(l_Size);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_CreateForLOD(GEO_tdst_StaticLOD *_pst_LOD)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < _pst_LOD->uc_NbLOD; i++) GEO_SubObject_Create((GEO_tdst_Object *) _pst_LOD->dpst_Id[i]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_Create(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject	*pst_SO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* temp */

	/*$F
    if ( (GetAsyncKeyState( 'A' ) < 0) && (GetAsyncKeyState( 'Z' ) < 0) )
        GEO_Dump( _pst_Obj );
        */

	/* fin temp */
	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type == GRO_GeoStaticLOD)
	{
		GEO_SubObject_CreateForLOD((GEO_tdst_StaticLOD *) _pst_Obj);
		return;
	}

	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(_pst_Obj->pst_SubObject) return;

	pst_SO = (GEO_tdst_SubObject*)L_malloc(sizeof(GEO_tdst_SubObject));
	L_memset(pst_SO, 0, sizeof(GEO_tdst_SubObject));
	_pst_Obj->pst_SubObject = pst_SO;

	if(_pst_Obj->l_NbPoints)
	{
		pst_SO->dc_VSel = (char *) L_malloc(_pst_Obj->l_NbPoints);
		L_memset(pst_SO->dc_VSel, 0, _pst_Obj->l_NbPoints);
	}

	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_SubObject_AllocBuffers(_pst_Obj);
    GEO_SubObject_HideUpdate( _pst_Obj, TRUE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_FreeForLOD(GEO_tdst_StaticLOD *_pst_LOD)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < _pst_LOD->uc_NbLOD; i++) GEO_SubObject_Free((GEO_tdst_Object *) _pst_LOD->dpst_Id[i]);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_Free(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject	*pst_SO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type == GRO_GeoStaticLOD)
	{
		GEO_SubObject_FreeForLOD((GEO_tdst_StaticLOD *) _pst_Obj);
		return;
	}

	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

    if (_pst_Obj->pst_SubObject->pfnl_UVMapper_Request)
        if (_pst_Obj->pst_SubObject->pfnl_UVMapper_Request( _pst_Obj->pst_SubObject->ul_UVMapper_Param, 2) ) 

    /* cleaning */
    GEO_Clean_Faces( _pst_Obj );
    GEO_SubObject_UVClean( _pst_Obj );

    /* free */
	pst_SO = _pst_Obj->pst_SubObject;
	if(_pst_Obj->l_NbPoints) L_free(pst_SO->dc_VSel);

	if(pst_SO->ul_NbEdges)
	{
		L_free(pst_SO->dst_Edge);
		L_free(pst_SO->dc_ESel);
	}

	if(pst_SO->ul_NbFaces)
	{
		L_free(pst_SO->dst_Faces);
		L_free(pst_SO->dc_FSel);
	}

	if(pst_SO->ul_PtInBuffer)
	{
		L_free(pst_SO->dst_2D);
		L_free(pst_SO->dst_3D);
	}

    if (pst_SO->dst_PointTransformed)
        L_free( pst_SO->dst_PointTransformed );

    if (pst_SO->dst_UVUpdate)
        L_free( pst_SO->dst_UVUpdate );

	L_free(pst_SO);
	_pst_Obj->pst_SubObject = NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_SubObject_IsAllocatedForLOD(GEO_tdst_StaticLOD *_pst_LOD)
{
	/*~~~~~~~~~~~~~*/
	int		i;
	LONG	l_Result;
	/*~~~~~~~~~~~~~*/

	l_Result = 0;
	for(i = 0; i < _pst_LOD->uc_NbLOD; i++)
		l_Result |= GEO_l_SubObject_IsAllocated((GEO_tdst_Object *) _pst_LOD->dpst_Id[i]);
	return l_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_SubObject_IsAllocated(GEO_tdst_Object *_pst_Obj)
{
	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type == GRO_GeoStaticLOD)
		return GEO_l_SubObject_IsAllocatedForLOD((GEO_tdst_StaticLOD *) _pst_Obj);
	else if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_UnselAll(GEO_tdst_Object *_pst_Obj)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
	char	*dc_Sel, *dc_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

    dc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	dc_Last = dc_Sel + _pst_Obj->l_NbPoints;
	for(; dc_Sel < dc_Last; dc_Sel++) *dc_Sel &= 0x80;

	dc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
	dc_Last = dc_Sel + _pst_Obj->pst_SubObject->ul_NbEdges;
	for(; dc_Sel < dc_Last; dc_Sel++) *dc_Sel &= 0x80;

	dc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	dc_Last = dc_Sel + _pst_Obj->pst_SubObject->ul_NbFaces;
	for(; dc_Sel < dc_Last; dc_Sel++) *dc_Sel &= 0x80;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_InvertSel(GEO_tdst_Object *_pst_Obj, int _i_Type )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*pc_Sel, *pc_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (_i_Type & GRO_Cul_EOF_Vertex )
    {
        pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
        pc_Last = pc_Sel +  _pst_Obj->l_NbPoints;
    }
    else if (_i_Type & GRO_Cul_EOF_Edge )
    {
        pc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
        pc_Last = pc_Sel + _pst_Obj->pst_SubObject->ul_NbEdges;
    }
    else if (_i_Type & GRO_Cul_EOF_Face )
    {
        pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
        pc_Last = pc_Sel + _pst_Obj->pst_SubObject->ul_NbFaces;
    }

    for ( ; pc_Sel < pc_Last; pc_Sel++ )
    {
        if (*pc_Sel & 0x80) continue;
        *pc_Sel = (*pc_Sel & 1) ^ 1;
    }

    if (_i_Type & GRO_Cul_EOF_Edge )
	    GEO_SubObject_EUpdateVSel(_pst_Obj);
    else if (_i_Type & GRO_Cul_EOF_Face )
        GEO_SubObject_FUpdateVSel(_pst_Obj);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_CannotSelAll(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	char	*dc_Sel, *dc_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	dc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	dc_Last = dc_Sel + _pst_Obj->l_NbPoints;
	for(; dc_Sel < dc_Last; dc_Sel++) *dc_Sel &= ~2;

	dc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
	dc_Last = dc_Sel + _pst_Obj->pst_SubObject->ul_NbEdges;
	for(; dc_Sel < dc_Last; dc_Sel++) *dc_Sel &= ~2;

	dc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	dc_Last = dc_Sel + _pst_Obj->pst_SubObject->ul_NbFaces;
	for(; dc_Sel < dc_Last; dc_Sel++) *dc_Sel &= ~2;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *GEO_pc_SubObject_GetSelData(GEO_tdst_Object *_pst_Obj, int _i_Type, LONG *_pl_Number)
{
	if(!_pst_Obj) return NULL;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return NULL;
	if(!_pst_Obj->pst_SubObject) return NULL;

	if(_i_Type & GRO_Cul_EOF_Vertex)
	{
		if(*_pl_Number) *_pl_Number = _pst_Obj->l_NbPoints;
		return _pst_Obj->pst_SubObject->dc_VSel;
	}
	else if(_i_Type & GRO_Cul_EOF_Edge)
	{
		if(*_pl_Number) *_pl_Number = _pst_Obj->pst_SubObject->ul_NbEdges;
		return _pst_Obj->pst_SubObject->dc_ESel;
	}
	else if(_i_Type & GRO_Cul_EOF_Face)
	{
		if(*_pl_Number) *_pl_Number = _pst_Obj->pst_SubObject->ul_NbFaces;
		return _pst_Obj->pst_SubObject->dc_FSel;
	}

	return NULL;
}

/*$4
 ***********************************************************************************************************************
    vertices
 ***********************************************************************************************************************
 */

extern BOOL GEO_ColMap_VerticeVisible(OBJ_tdst_GameObject *, USHORT);
extern BOOL GEO_ColMap_ElementVisible(OBJ_tdst_GameObject *, USHORT);
/*
 =======================================================================================================================
 =======================================================================================================================
 */

SOFT_tdst_PickingBuffer *pPickForZCull = NULL;
int GEO_SOFT_PickWithZBufferCulling(MATH_tdst_Vector	*pst_Vertex)
{
	float Ooz;
	if (pPickForZCull)
	{
		int X,Y;
		SOFT_tdst_PickingBuffer_Pixel	*dst_Pixel;
		X = lFloatToLongOpt(pst_Vertex->x);
		Y = lFloatToLongOpt(pst_Vertex->y);
		if (X <= 0) X = 1;
		if (Y <= 0) Y = 1;
		if (X >= pPickForZCull->l_Width - 1) X = pPickForZCull->l_Width - 2;
		if (Y >= pPickForZCull->l_Height - 1) Y = pPickForZCull->l_Height - 2;

		dst_Pixel = pPickForZCull->dst_Pixel + Y  * pPickForZCull->l_Width + X;
		Ooz = dst_Pixel->f_Ooz;
		dst_Pixel -= pPickForZCull->l_Width + 1;
		if (Ooz > (dst_Pixel)->f_Ooz) Ooz = (dst_Pixel)->f_Ooz;
		dst_Pixel ++;
		if (Ooz > (dst_Pixel)->f_Ooz) Ooz = (dst_Pixel)->f_Ooz;
		dst_Pixel ++;
		if (Ooz > (dst_Pixel)->f_Ooz) Ooz = (dst_Pixel)->f_Ooz;
		dst_Pixel += pPickForZCull->l_Width - 2;
		if (Ooz > (dst_Pixel)->f_Ooz) Ooz = (dst_Pixel)->f_Ooz;
		dst_Pixel ++;
		if (Ooz > (dst_Pixel)->f_Ooz) Ooz = (dst_Pixel)->f_Ooz;
		dst_Pixel ++;
		if (Ooz > (dst_Pixel)->f_Ooz) Ooz = (dst_Pixel)->f_Ooz;
		dst_Pixel += pPickForZCull->l_Width - 2;
		if (Ooz > (dst_Pixel)->f_Ooz) Ooz = (dst_Pixel)->f_Ooz;
		dst_Pixel ++;
		if (Ooz > (dst_Pixel)->f_Ooz) Ooz = (dst_Pixel)->f_Ooz;
		dst_Pixel ++;
		if (Ooz > (dst_Pixel)->f_Ooz) Ooz = (dst_Pixel)->f_Ooz;

		if (Ooz > pst_Vertex->z) return 1;

	}
	return 0;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_VPick
(
	GEO_tdst_Object		*_pst_Obj,
	MATH_tdst_Vector	*_pst_2D,
	char				_b_Sel,
	OBJ_tdst_GameObject *_pst_GO
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Vertex;
	float				f, f_Dist;
	int					i, i_Result;
	UCHAR				*pc_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return -1;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return -1;
	if(!_pst_Obj->pst_SubObject) return -1;

	pst_Vertex = _pst_Obj->pst_SubObject->dst_2D;
	f_Dist = 8;
	i_Result = -1;

	if(_b_Sel)
	{
		pc_Sel = (UCHAR*)_pst_Obj->pst_SubObject->dc_VSel;
		for(i = 0; i < _pst_Obj->l_NbPoints; i++, pst_Vertex++, pc_Sel++)
		{
			if(*pc_Sel & 0x80) continue;
			if(!(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) i))) continue;
			if (GEO_SOFT_PickWithZBufferCulling(pst_Vertex)) continue;
			f = (float) (fabs(_pst_2D->x - pst_Vertex->x) + fabs(_pst_2D->y - pst_Vertex->y));
			if(f < f_Dist)
			{
				f_Dist = f;
				i_Result = i; 
			}
		}
	}
	else
	{
		pc_Sel = (UCHAR*)_pst_Obj->pst_SubObject->dc_VSel;
		for(i = 0; i < _pst_Obj->l_NbPoints; i++, pst_Vertex++, pc_Sel++)
		{
			if(*pc_Sel & 0x81) continue;
			if(!(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) i))) continue;
			if (GEO_SOFT_PickWithZBufferCulling(pst_Vertex)) continue;
			f = (float) (fabs(_pst_2D->x - pst_Vertex->x) + fabs(_pst_2D->y - pst_Vertex->y));
			if(f < f_Dist)
			{
				f_Dist = f;
				i_Result = i;
			}
		}
	}
	

	return i_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

int GEO_i_SubObject_VPickBox
(
	GEO_tdst_Object		*_pst_Obj,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	OBJ_tdst_GameObject *_pst_GO
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_Vertex;
	int					i, i_Result;
	char				*pc_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	pst_Vertex = _pst_Obj->pst_SubObject->dst_2D;
	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	i_Result = 0;

	for(i = 0; i < _pst_Obj->l_NbPoints; i++, pst_Vertex++, pc_Sel++)
	{
		if(*pc_Sel & 0x80) continue;

		if(!(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) i))) continue;
		if((pst_Vertex->x < A->x) || (pst_Vertex->x > B->x)) continue;
		if((pst_Vertex->y < A->y) || (pst_Vertex->y > B->y)) continue;
		/* Philippe Check for Z Buffer Begin */
		if (GEO_SOFT_PickWithZBufferCulling(pst_Vertex)) continue;
		/* Philippe Check for Z Buffer End */
		*pc_Sel |= 2;
		i_Result++;
	}

	return i_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_SubObject_ComputePickedVerticesCenter(GEO_tdst_Object *_pst_Obj, MATH_tdst_Vector *C, int _i_Vertex )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_Vertex			            *pst_Vertex, *pst_Dest;
	MATH_tdst_Vector	            st_Temp;
	LONG				            l_Number;
	UCHAR				            *puc_Sel, *puc_Last;
    GEO_tdst_ModifierMorphing_Data	*pst_Morph;
    int                             i;
    MATH_tdst_Vector				st_Min, st_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_InitVectorToZero(C);

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	GEO_ComputeFloatingSelection(_pst_Obj);	

	puc_Sel = (UCHAR*)_pst_Obj->pst_SubObject->dc_VSel;
	puc_Last = puc_Sel + _pst_Obj->l_NbPoints;

    if (_pst_Obj->pst_SubObject->dst_PointTransformed)
        pst_Vertex = _pst_Obj->pst_SubObject->dst_PointTransformed;
    else
	    pst_Vertex = _pst_Obj->dst_Point;
	l_Number = 0;

    if ( _pst_Obj->pst_SubObject->pst_MorphData ) 
    {
		pst_Vertex = _pst_Obj->dst_Point;
        pst_Dest = GDI_gpst_CurDD->pst_ComputingBuffers->ast_SpecialVB;
        pst_Morph = (GEO_tdst_ModifierMorphing_Data	*) _pst_Obj->pst_SubObject->pst_MorphData;

        L_memcpy( pst_Dest, pst_Vertex, sizeof( GEO_Vertex ) * _pst_Obj->l_NbPoints );
        for (i = 0; i < pst_Morph->l_NbVector; i++)
            MATH_AddEqualVector( pst_Dest + pst_Morph->dl_Index[i], pst_Morph->dst_Vector + i );
        pst_Vertex = pst_Dest;
    }

	if ( _i_Vertex == -3 )
	{
		l_Number = _pst_Obj->l_NbPoints;
		MATH_CopyVector( &st_Min, VCast( pst_Vertex ) );
		MATH_CopyVector( &st_Max, VCast( pst_Vertex ) );
		
		for(; puc_Sel < puc_Last; puc_Sel++, pst_Vertex++)
		{
			if (pst_Vertex->x < st_Min.x ) st_Min.x = pst_Vertex->x;
			if (pst_Vertex->y < st_Min.y ) st_Min.y = pst_Vertex->y;
			if (pst_Vertex->z < st_Min.z ) st_Min.z = pst_Vertex->z;
			if (pst_Vertex->x > st_Max.x ) st_Max.x = pst_Vertex->x;
			if (pst_Vertex->y > st_Max.y ) st_Max.y = pst_Vertex->y;
			if (pst_Vertex->z > st_Max.z ) st_Max.z = pst_Vertex->z;
		}
		MATH_AddVector( C, &st_Min, &st_Max );
		MATH_ScaleEqualVector( C, 0.5f );
	}
	else if ( _i_Vertex == -2 )
	{
		l_Number = _pst_Obj->l_NbPoints;
		for(; puc_Sel < puc_Last; puc_Sel++, pst_Vertex++)
			MATH_AddEqualVector(C, VCast(pst_Vertex));
		MATH_ScaleEqualVector( C, (1.0f / (float) l_Number ) );
	}
	else if ( (_i_Vertex < 0 ) || (_i_Vertex >= _pst_Obj->l_NbPoints ) )
	{
		if (_pst_Obj->pst_SubObject->pf_FloatSelection_V)
		{
			float *p_FloatingSelection;
			float GlobalWeight;
			GlobalWeight = 0.0f;
			p_FloatingSelection = _pst_Obj->pst_SubObject->pf_FloatSelection_V;
			for(; puc_Sel < puc_Last; puc_Sel++, pst_Vertex++,p_FloatingSelection++)
			{
				if (*p_FloatingSelection > 0.99f)
				{
					MATH_AddScaleVector(C, C, VCast(pst_Vertex) , *p_FloatingSelection);
					GlobalWeight += *p_FloatingSelection;
					l_Number++;
				}
			}
			if (GlobalWeight != 0.0f)
				MATH_ScaleEqualVector(C , 1.0f / GlobalWeight);

		} else
		for(; puc_Sel < puc_Last; puc_Sel++, pst_Vertex++)
		{
			if(*puc_Sel & 1)
			{
				MATH_ScaleEqualVector(C, ((float) l_Number / (float) (l_Number + 1)));
				MATH_ScaleVector(&st_Temp, VCast(pst_Vertex), ((float) 1 / (float) (l_Number + 1)));
				MATH_AddEqualVector(C, &st_Temp);
				l_Number++;
			}
		}
	}
	else
	{
		MATH_CopyVector( C, pst_Vertex + _i_Vertex );
		l_Number = 1;
	}

	return l_Number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_SubObject_GetNbPickedVertices(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	UCHAR	*puc_Sel, *puc_Last;
	LONG	l_Number;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	puc_Sel = (UCHAR*)_pst_Obj->pst_SubObject->dc_VSel;
	puc_Last = puc_Sel + _pst_Obj->l_NbPoints;
	l_Number = 0;

	for(; puc_Sel < puc_Last; puc_Sel++)
	{
		if(*puc_Sel & 1) l_Number++;
	}

	return l_Number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_SubObject_GetNbPickedVerticesAndOneIndex(GEO_tdst_Object *_pst_Obj, int *_i_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef JADEFUSION
	CHAR	*puc_Sel, *puc_Last;
#else
	UCHAR	*puc_Sel, *puc_Last;
#endif
	LONG	l_Number;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(!_i_Index) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	puc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	puc_Last = puc_Sel + _pst_Obj->l_NbPoints;
	l_Number = 0;

	for(; puc_Sel < puc_Last; puc_Sel++)
	{
		if(*puc_Sel & 1)
		{
			l_Number++;
			*_i_Index = puc_Sel - _pst_Obj->pst_SubObject->dc_VSel;
		}
	}

	return l_Number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_ColorVertices(GEO_tdst_Object *_pst_Obj, ULONG **_ppul_RLI, ULONG _ul_Color, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	*pul_RLI, *pul_Last;
	char	*pc_Sel;
	LONG	l_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	if((*_ppul_RLI) == NULL)
	{
		(*_ppul_RLI) = (ULONG *) MEM_p_Alloc(sizeof(ULONG) * (_pst_Obj->l_NbPoints + 1));
		(*_ppul_RLI)[0] = _pst_Obj->l_NbPoints;
		L_memset((*_ppul_RLI) + 1, 0, sizeof(ULONG) * _pst_Obj->l_NbPoints);
	}

	l_Max = (*_ppul_RLI)[0];
	if(l_Max > _pst_Obj->l_NbPoints) l_Max = _pst_Obj->l_NbPoints;

	pul_RLI = &(*_ppul_RLI)[1];
	pul_Last = pul_RLI + l_Max;
	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;

	for(; pul_RLI < pul_Last; pc_Sel++, pul_RLI++)
	{
		if(*pc_Sel & 1)
			*pul_RLI = (LIGHT_ul_Interpol2Colors(*pul_RLI, _ul_Color, f) & 0xFFFFFF) | (*pul_RLI & 0xFF000000);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_ColorAlphaVertices(GEO_tdst_Object *_pst_Obj, ULONG **_ppul_RLI, ULONG _ul_Color, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	*pul_RLI, *pul_Last;
	char	*pc_Sel;
	LONG	l_Max;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	if((*_ppul_RLI) == NULL)
	{
		(*_ppul_RLI) = (ULONG *) MEM_p_Alloc(sizeof(ULONG) * (_pst_Obj->l_NbPoints + 1));
		(*_ppul_RLI)[0] = _pst_Obj->l_NbPoints;
		L_memset((*_ppul_RLI) + 1, 0, sizeof(ULONG) * _pst_Obj->l_NbPoints);
	}

	l_Max = (*_ppul_RLI)[0];
	if(l_Max > _pst_Obj->l_NbPoints) l_Max = _pst_Obj->l_NbPoints;

	pul_RLI = &(*_ppul_RLI)[1];
	pul_Last = pul_RLI + l_Max;
	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;

	for(; pul_RLI < pul_Last; pc_Sel++, pul_RLI++)
	{
		if(*pc_Sel & 1)
			*pul_RLI = (LIGHT_ul_Interpol2Colors(*pul_RLI, _ul_Color, f) & 0xFF000000) | (*pul_RLI & 0x00FFFFFF);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_SubObject_PickVertexColor(GEO_tdst_Object *_pst_Obj, ULONG *_pul_RLI, ULONG *_pul_Color)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	*pul_RLI, *pul_Last;
	char	*pc_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;
	if(_pul_RLI == NULL) return 0;

	pul_RLI = _pul_RLI + 1;
	pul_Last = pul_RLI + (((LONG) _pul_RLI[0] > _pst_Obj->l_NbPoints) ? _pst_Obj->l_NbPoints : _pul_RLI[0]);
	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;

	for(; pul_RLI < pul_Last; pc_Sel++, pul_RLI++)
	{
		if(*pc_Sel & 1)
		{
			*_pul_Color = *pul_RLI;
			return 1;
		}
	}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_InitVMoves_AddUV( GEO_tdst_Object *_pst_Obj, GEO_tdst_IndexedTriangle *T, int index )
{
    int                             i;
    GEO_tdst_SubObject_UVUpdate     *UV;

    /* search */
    if (_pst_Obj->pst_SubObject->dst_UVUpdate)
    {
        for (i = 0; i < (int) _pst_Obj->pst_SubObject->ul_UVUpdate_Nb; i++)
            if (_pst_Obj->pst_SubObject->dst_UVUpdate[ i ].uv == T->auw_UV[ index ]) 
            {
                _pst_Obj->pst_SubObject->dst_UVUpdate[ i ].TMax = T;
                return;
            }
    }
    else
    {
        _pst_Obj->pst_SubObject->ul_UVUpdate_Max = 256;
        _pst_Obj->pst_SubObject->dst_UVUpdate = (GEO_tdst_SubObject_UVUpdate *) L_malloc( 256 * sizeof(GEO_tdst_SubObject_UVUpdate) );
    }

    /* not found : add */
    if (_pst_Obj->pst_SubObject->ul_UVUpdate_Nb == _pst_Obj->pst_SubObject->ul_UVUpdate_Max)
    {
        _pst_Obj->pst_SubObject->ul_UVUpdate_Max += 256;
        _pst_Obj->pst_SubObject->dst_UVUpdate = (GEO_tdst_SubObject_UVUpdate *) L_realloc( _pst_Obj->pst_SubObject->dst_UVUpdate, _pst_Obj->pst_SubObject->ul_UVUpdate_Max * sizeof(GEO_tdst_SubObject_UVUpdate) );
    }

    UV = _pst_Obj->pst_SubObject->dst_UVUpdate + _pst_Obj->pst_SubObject->ul_UVUpdate_Nb++;
    UV->TMin = (void *) T;
    UV->TMax = (void *) T;
    UV->uv = T->auw_UV[ index ];
    UV->xyz = T->auw_Index[ index ];
    MATH_CopyVector( &UV->C, VCast( _pst_Obj->dst_Point + T->auw_Index[ index ]) );
    MATH_CopyVector( &UV->CA, VCast( _pst_Obj->dst_Point + T->auw_Index[ (index + 1) % 3 ] ) );
    MATH_CopyVector( &UV->CB, VCast( _pst_Obj->dst_Point + T->auw_Index[ (index + 2) % 3 ] ) );
    MATH_SubEqualVector( &UV->CA, &UV->C );
    MATH_SubEqualVector( &UV->CB, &UV->C );
    L_memcpy( &UV->c, _pst_Obj->dst_UV + T->auw_UV[ index ], sizeof (GEO_tdst_UV) );
    L_memcpy( &UV->ca, _pst_Obj->dst_UV + T->auw_UV[ (index + 1) % 3], sizeof (GEO_tdst_UV) );
    L_memcpy( &UV->cb, _pst_Obj->dst_UV + T->auw_UV[ (index + 2) % 3], sizeof (GEO_tdst_UV) );
    UV->ca.fU -= UV->c.fU;
    UV->ca.fV -= UV->c.fV;
    UV->cb.fU -= UV->c.fU;
    UV->cb.fV -= UV->c.fV;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_InitVMoves( GEO_tdst_Object *_pst_Obj, BOOL _b_UpdateUV )
{
    int                         i, j, k;
    GEO_tdst_IndexedTriangle	*T;
    char                        *dc_UVused;
    char						*pc_Sel;

    if (!_pst_Obj) return;
    if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

    if ( _pst_Obj->pst_SubObject->dst_UVUpdate )
    {
        L_free( _pst_Obj->pst_SubObject->dst_UVUpdate );
        _pst_Obj->pst_SubObject->dst_UVUpdate = NULL;
        _pst_Obj->pst_SubObject->ul_UVUpdate_Nb = 0;
        _pst_Obj->pst_SubObject->ul_UVUpdate_Max = 0;
    }

    if (!_b_UpdateUV) return;

    dc_UVused = (char *) L_malloc( _pst_Obj->l_NbUVs);
    L_memset( dc_UVused, 0, _pst_Obj->l_NbUVs);
    pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;

    for ( i = 0; i < _pst_Obj->l_NbElements; i++)
    {
        for (j = 0; j < _pst_Obj->dst_Element[ i ].l_NbTriangles; j++)
        {
            T = _pst_Obj->dst_Element[i].dst_Triangle + j;
            for (k = 0; k < 3; k++)
            {
				if ( pc_Sel[ T->auw_Index[ k ]] )
					GEO_SubObject_InitVMoves_AddUV( _pst_Obj, T, k );
			}
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_UseVMoves( GEO_tdst_Object *_pst_Obj )
{
    GEO_tdst_SubObject_UVUpdate *pst_Data, *pst_LastData;
    MATH_tdst_Vector            N, *D, CD, Temp;
    GEO_tdst_UV                 *pst_UV;
    float                       t, x, y;

    if ( !_pst_Obj->pst_SubObject->dst_UVUpdate ) return;
    pst_Data = _pst_Obj->pst_SubObject->dst_UVUpdate;
    pst_LastData = pst_Data + _pst_Obj->pst_SubObject->ul_UVUpdate_Nb;

    for ( ; pst_Data < pst_LastData; pst_Data++ )
    {
        MATH_CrossProduct( &N, &pst_Data->CA, &pst_Data->CB );
        D = VCast( _pst_Obj->dst_Point + pst_Data->xyz );
        MATH_SubVector( &CD, D, &pst_Data->C );
        t = MATH_f_DotProduct( &N, &N );
        if (t == 0) continue;
        t = MATH_f_DotProduct( &CD, &N) / t;
        MATH_AddScaleVector( &CD, &CD, &N, -t );

        t = MATH_f_NormVector( &N );
        MATH_CrossProduct( &Temp, &pst_Data->CA, &CD );
        y = MATH_f_NormVector( &Temp ) / t;
        if (MATH_f_DotProduct( &Temp, &N) < 0) y *= -1;
        MATH_CrossProduct( &Temp, &CD, &pst_Data->CB );
        x = MATH_f_NormVector( &Temp ) / t;
        if (MATH_f_DotProduct( &Temp, &N) < 0) x *= -1;
        pst_UV = _pst_Obj->dst_UV + pst_Data->uv;
        pst_UV->fU = pst_Data->c.fU + x * pst_Data->ca.fU + y * pst_Data->cb.fU;
        pst_UV->fV = pst_Data->c.fV + x * pst_Data->ca.fV + y * pst_Data->cb.fV;
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SpecialSquare(float X)
{
	return fAcos(X) / 3.1415927f;
}
void GEO_ComputeFloatingSelection(GEO_tdst_Object *_pst_Obj)
{
	ULONG SmoothCounter,HasChanged;
	float	MaxValue;
	char		                    *pc_Sel, *pc_Last;

	float *pf_FloatSelection_V,*pf_FloatSelection_V_Last;
	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;
	if ((_pst_Obj->pst_SubObject->FloatSelNumber != _pst_Obj->l_NbPoints) || (!SmoothSelIsActivate))
	{
		if (_pst_Obj->pst_SubObject->pf_FloatSelection_V)
			L_free(_pst_Obj->pst_SubObject->pf_FloatSelection_V);
		_pst_Obj->pst_SubObject->pf_FloatSelection_V = NULL;
		_pst_Obj->pst_SubObject->FloatSelNumber =  0;
	}
	if (!SmoothSelIsActivate) return;
	if (!_pst_Obj->pst_SubObject->pf_FloatSelection_V)
	{
		_pst_Obj->pst_SubObject->pf_FloatSelection_V = (float*)L_malloc(4 * _pst_Obj->l_NbPoints);
		L_memset(_pst_Obj->pst_SubObject->pf_FloatSelection_V , 0 , 4 * _pst_Obj->l_NbPoints);
	}
	_pst_Obj->pst_SubObject->FloatSelNumber = _pst_Obj->l_NbPoints;

	
	pf_FloatSelection_V=  _pst_Obj->pst_SubObject->pf_FloatSelection_V ;
	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	pc_Last = pc_Sel + _pst_Obj->l_NbPoints;

	/* Detect Chnge */
	HasChanged=0;
	if (InvertSel)
	{
		for(; pc_Sel < pc_Last; pc_Sel++, pf_FloatSelection_V++)
		{
			*pf_FloatSelection_V = 1.0f - *pf_FloatSelection_V;
			if (!(*pc_Sel & 1))
			{
				if ((*pf_FloatSelection_V) != 1.0f) 
					HasChanged = 1;
			}
			else
			{
				if ((*pf_FloatSelection_V) == 1.0f) 
					HasChanged = 1;
			}
			*pf_FloatSelection_V = 1.0f - *pf_FloatSelection_V;
		}
	} else
	{
		for(; pc_Sel < pc_Last; pc_Sel++, pf_FloatSelection_V++)
		{
			if ((*pc_Sel & 1))
			{
				if ((*pf_FloatSelection_V) != 1.0f) 
					HasChanged = 1;
			}
			else
			{
				if ((*pf_FloatSelection_V) == 1.0f) 
					HasChanged = 1;
			}
		}
	}
	if ((!HasChanged) && (!SmoothSelMustBeUpdate))
		return;


	pf_FloatSelection_V=  _pst_Obj->pst_SubObject->pf_FloatSelection_V ;
	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	pc_Last = pc_Sel + _pst_Obj->l_NbPoints;
	/* BruptSel */
	for(; pc_Sel < pc_Last; pc_Sel++, pf_FloatSelection_V++)
	{
		if (InvertSel) *pc_Sel ^= 1;
		if(*pc_Sel & 1) 
			*pf_FloatSelection_V = 0.0f;
		else
			*pf_FloatSelection_V = DistanceMax * 4.0f; // Distance To Selection Max 
	}

	/* SmoothSel : Compute Distance To Selection */
	SmoothCounter = 100;
	while (SmoothCounter--)
	{
		GEO_tdst_ElementIndexedTriangles	*pst_Elem,*pst_ElemLast;
		float MaxDetector;
		pst_Elem = _pst_Obj->dst_Element;
		pst_ElemLast = pst_Elem +_pst_Obj->l_NbElements;
		pf_FloatSelection_V=  _pst_Obj->pst_SubObject->pf_FloatSelection_V ;
		pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
		while (pst_Elem < pst_ElemLast )
		{
			GEO_tdst_IndexedTriangle			*pst_Tri,*pst_TriLast;
			pst_Tri = pst_Elem->dst_Triangle;
			pst_TriLast = pst_Tri +pst_Elem->l_NbTriangles;
			while (pst_Tri<pst_TriLast)
			{
				float Distances[3];
				MATH_tdst_Vector	stLocal0;
				if (SmoothSelMode & 1)
				{
					MATH_SubVector(&stLocal0 , _pst_Obj->dst_Point + pst_Tri->auw_Index[0] , _pst_Obj->dst_Point + pst_Tri->auw_Index[1]);
					Distances[0] = MATH_f_NormVector(&stLocal0);
					MATH_SubVector(&stLocal0 , _pst_Obj->dst_Point + pst_Tri->auw_Index[1] , _pst_Obj->dst_Point + pst_Tri->auw_Index[2]);
					Distances[1] = MATH_f_NormVector(&stLocal0);
					MATH_SubVector(&stLocal0 , _pst_Obj->dst_Point + pst_Tri->auw_Index[2] , _pst_Obj->dst_Point + pst_Tri->auw_Index[0]);
					Distances[2] = MATH_f_NormVector(&stLocal0);
				} else
					Distances[0] = Distances[1] = Distances[2] = 0.0f;
				if (SmoothSelMode & 2)
				{
					float Alpha;
					Alpha = SpecialSquare(MATH_f_DotProduct(_pst_Obj->dst_PointNormal + pst_Tri->auw_Index[0] , _pst_Obj->dst_PointNormal + pst_Tri->auw_Index[1] ));
					Alpha *= DistanceMax;
					Distances[0] += ( topologicCare * Alpha);
					Alpha = SpecialSquare(MATH_f_DotProduct(_pst_Obj->dst_PointNormal + pst_Tri->auw_Index[1] , _pst_Obj->dst_PointNormal + pst_Tri->auw_Index[2] ));
					Alpha *= DistanceMax;
					Distances[1] += ( topologicCare * Alpha);
					Alpha = SpecialSquare(MATH_f_DotProduct(_pst_Obj->dst_PointNormal + pst_Tri->auw_Index[2] , _pst_Obj->dst_PointNormal + pst_Tri->auw_Index[0] ));
					Alpha *= DistanceMax;
					Distances[2] += ( topologicCare * Alpha);
				}
				
				if ((Distances[0] + pf_FloatSelection_V[pst_Tri->auw_Index[0]]) < pf_FloatSelection_V[pst_Tri->auw_Index[1]]) pf_FloatSelection_V[pst_Tri->auw_Index[1]] = Distances[0] + pf_FloatSelection_V[pst_Tri->auw_Index[0]];
				if ((Distances[0] + pf_FloatSelection_V[pst_Tri->auw_Index[1]]) < pf_FloatSelection_V[pst_Tri->auw_Index[0]]) pf_FloatSelection_V[pst_Tri->auw_Index[0]] = Distances[0] + pf_FloatSelection_V[pst_Tri->auw_Index[1]];
				if ((Distances[1] + pf_FloatSelection_V[pst_Tri->auw_Index[1]]) < pf_FloatSelection_V[pst_Tri->auw_Index[2]]) pf_FloatSelection_V[pst_Tri->auw_Index[2]] = Distances[1] + pf_FloatSelection_V[pst_Tri->auw_Index[1]];
				if ((Distances[1] + pf_FloatSelection_V[pst_Tri->auw_Index[2]]) < pf_FloatSelection_V[pst_Tri->auw_Index[1]]) pf_FloatSelection_V[pst_Tri->auw_Index[1]] = Distances[1] + pf_FloatSelection_V[pst_Tri->auw_Index[2]];
				if ((Distances[2] + pf_FloatSelection_V[pst_Tri->auw_Index[2]]) < pf_FloatSelection_V[pst_Tri->auw_Index[0]]) pf_FloatSelection_V[pst_Tri->auw_Index[0]] = Distances[2] + pf_FloatSelection_V[pst_Tri->auw_Index[2]];
				if ((Distances[2] + pf_FloatSelection_V[pst_Tri->auw_Index[0]]) < pf_FloatSelection_V[pst_Tri->auw_Index[2]]) pf_FloatSelection_V[pst_Tri->auw_Index[2]] = Distances[2] + pf_FloatSelection_V[pst_Tri->auw_Index[0]];
				pst_Tri++;
			}
			pst_Elem++;
		}
		/* CopyBack and Normalize to MinMax */
		MaxDetector = 0.0f;
		pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
		pf_FloatSelection_V =  _pst_Obj->pst_SubObject->pf_FloatSelection_V ;
		pf_FloatSelection_V_Last = pf_FloatSelection_V + _pst_Obj->l_NbPoints;
		while (pf_FloatSelection_V < pf_FloatSelection_V_Last)
		{
			if (*pf_FloatSelection_V > DistanceMax)
				MaxDetector = DistanceMax;
			pf_FloatSelection_V ++;
		}
		if (!MaxDetector) 
			SmoothCounter = 0;
	}
	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
 	pf_FloatSelection_V=  _pst_Obj->pst_SubObject->pf_FloatSelection_V ;
	pf_FloatSelection_V_Last = pf_FloatSelection_V + _pst_Obj->l_NbPoints;
	MaxValue = 0;
	while (pf_FloatSelection_V < pf_FloatSelection_V_Last)
	{
		if (*pc_Sel & 1)
			*(pf_FloatSelection_V) = 1.0f;
		else
		{
			*(pf_FloatSelection_V) = DistanceMax - *(pf_FloatSelection_V) ;
			if (*(pf_FloatSelection_V) < 0.0f) *(pf_FloatSelection_V) = 0.0f;
			*(pf_FloatSelection_V) *= 1.0f/ DistanceMax ;

			*(pf_FloatSelection_V) = - 2.0f * *(pf_FloatSelection_V) * *(pf_FloatSelection_V) *  *(pf_FloatSelection_V) + 3.0f * *(pf_FloatSelection_V) *  *(pf_FloatSelection_V);

			if (*(pf_FloatSelection_V) > 0.9999f) *(pf_FloatSelection_V) = 0.9999f;
		}
		pf_FloatSelection_V++;
		pc_Sel ++;
	}

	/* BruptSel */
	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
 	pf_FloatSelection_V=  _pst_Obj->pst_SubObject->pf_FloatSelection_V ;
	if (InvertSel) 
	for(; pc_Sel < pc_Last; pc_Sel++, pf_FloatSelection_V++)
	{
		*pc_Sel ^= 1;
	 	*pf_FloatSelection_V =  1.0f - *pf_FloatSelection_V;
	}

}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_MoveVertices(GEO_tdst_Object *_pst_Obj, MATH_tdst_Vector *v, char _c_All)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		                    *pc_Sel, *pc_Last;
	GEO_Vertex	                    *pst_Vertex;
    GEO_tdst_ModifierMorphing_Data	*pst_Morph;
    int                             index, i;
    float							*pf_FloatSel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;


	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	pc_Last = pc_Sel + _pst_Obj->l_NbPoints;
	pst_Vertex = _pst_Obj->dst_Point;
	pf_FloatSel = _pst_Obj->pst_SubObject->pf_FloatSelection_V;
	
	
	if (_pst_Obj->pst_SubObject->pst_MorphData )
    {
        pst_Morph = (GEO_tdst_ModifierMorphing_Data	*) _pst_Obj->pst_SubObject->pst_MorphData;
        if (pf_FloatSel)
        {
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++, pf_FloatSel++)
			{
				if(*pf_FloatSel) 
				{
					index = pc_Sel - _pst_Obj->pst_SubObject->dc_VSel;
					for (i = 0; i < pst_Morph->l_NbVector; i++)
					{
						if (pst_Morph->dl_Index[i] == index )
						{
							MATH_AddScaleVector( VCast(pst_Morph->dst_Vector + i), VCast(pst_Morph->dst_Vector + i), v , *pf_FloatSel);
							break;
						}

					}
					if (i == pst_Morph->l_NbVector)
					{
						pst_Morph->l_NbVector++;
						if (i == 0)
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector* )MEM_p_Alloc( sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG* )MEM_p_Alloc( sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						else
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector* )MEM_p_Realloc( pst_Morph->dst_Vector, sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG* )MEM_p_Realloc( pst_Morph->dl_Index, sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						MATH_ScaleVector( VCast(pst_Morph->dst_Vector + i), v , *pf_FloatSel);
						pst_Morph->dl_Index[ i ] = index;
					}
				}
			}
		}
		else
		{
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
			{
				if(*pc_Sel & 1) 
				{
					index = pc_Sel - _pst_Obj->pst_SubObject->dc_VSel;
					for (i = 0; i < pst_Morph->l_NbVector; i++)
					{
						if (pst_Morph->dl_Index[i] == index )
						{
							MATH_AddEqualVector(pst_Morph->dst_Vector + i, v);
							break;
						}

					}
					if (i == pst_Morph->l_NbVector)
					{
						pst_Morph->l_NbVector++;
						if (i == 0)
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector*)MEM_p_Alloc( sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG*)MEM_p_Alloc( sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						else
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector*)MEM_p_Realloc( pst_Morph->dst_Vector, sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG*)MEM_p_Realloc( pst_Morph->dl_Index, sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						MATH_CopyVector( pst_Morph->dst_Vector + i, v );
						pst_Morph->dl_Index[ i ] = index;
					}
				}
			}
		}
        return;
    }
	// -- Bug: modif temporaire --
	MATH_AddScaleVector( &mst_MoveBugLocal, v ,&mst_MoveBugLocal , 1.0f);
	// ---------------------------

	if(_c_All)
	{
		for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++) MATH_AddEqualVector(VCast(pst_Vertex), v);
	}
	else
	{
		
		if (pf_FloatSel)
		{
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++,pf_FloatSel++)
				MATH_AddScaleVector(VCast(pst_Vertex), VCast(pst_Vertex), v , *pf_FloatSel);
		}
		else
		{
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
				if(*pc_Sel & 1) 
					MATH_AddScaleVector(VCast(pst_Vertex), VCast(pst_Vertex), v , 1.0f);
		}
	}

    GEO_SubObject_UseVMoves( _pst_Obj );
	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
	GEO_SubObject_BuildFaceData(_pst_Obj);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_RotateVertices(GEO_tdst_Object *_pst_Obj, MATH_tdst_Matrix *M, char _c_All, int _i_UpdateMorph )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				            *pc_Sel, *pc_Last;
	GEO_Vertex			            *pst_Vertex;
	MATH_tdst_Vector	            *pst_Center;
    GEO_tdst_ModifierMorphing_Data	*pst_Morph;
    int                             index, i;
    MATH_tdst_Vector                V;
	WOR_tdst_Update_RLI				st_UpdateRLIData;
	float							*pf_FloatSel ;
	MATH_tdst_Vector				stLocal0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

    pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	pc_Last = pc_Sel + _pst_Obj->l_NbPoints;
	pst_Vertex = _pst_Obj->dst_Point;
	pst_Center = MATH_pst_GetTranslation(M);
	pf_FloatSel = _pst_Obj->pst_SubObject->pf_FloatSelection_V;

    if (_pst_Obj->pst_SubObject->pst_MorphData )
    {
        pst_Morph = (GEO_tdst_ModifierMorphing_Data	*) _pst_Obj->pst_SubObject->pst_MorphData;
        if (pf_FloatSel)
        {
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++, pf_FloatSel++)
			{
				if(*pf_FloatSel) 
				{
					index = pc_Sel - _pst_Obj->pst_SubObject->dc_VSel;
					for (i = 0; i < pst_Morph->l_NbVector; i++)
					{
						if (pst_Morph->dl_Index[i] == index )
							break;
					}
					if (i == pst_Morph->l_NbVector)
					{
						pst_Morph->l_NbVector++;
						if (i == 0)
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector* )MEM_p_Alloc( sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG* )MEM_p_Alloc( sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						else
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector* )MEM_p_Realloc( pst_Morph->dst_Vector, sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG* )MEM_p_Realloc( pst_Morph->dl_Index, sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						MATH_InitVectorToZero( pst_Morph->dst_Vector + i );
						pst_Morph->dl_Index[ i ] = index;
					}

					MATH_AddVector( &V, VCast( pst_Vertex ), pst_Morph->dst_Vector + i );
					MATH_SubVector(&stLocal0, &V, pst_Center);
					MATH_TransformVertex(&stLocal0, M, &stLocal0);
					MATH_BlendVector( &V, &V, &stLocal0 , *pf_FloatSel);
					MATH_SubVector( pst_Morph->dst_Vector + i, &V, VCast( pst_Vertex ) );
				}
			}
		}
		else
		{
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
			{
				if(*pc_Sel & 1) 
				{
					index = pc_Sel - _pst_Obj->pst_SubObject->dc_VSel;
					for (i = 0; i < pst_Morph->l_NbVector; i++)
					{
						if (pst_Morph->dl_Index[i] == index )
							break;
					}
					if (i == pst_Morph->l_NbVector)
					{
						pst_Morph->l_NbVector++;
						if (i == 0)
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector*)MEM_p_Alloc( sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG*)MEM_p_Alloc( sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						else
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector*)MEM_p_Realloc( pst_Morph->dst_Vector, sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG*)MEM_p_Realloc( pst_Morph->dl_Index, sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						MATH_InitVectorToZero( pst_Morph->dst_Vector + i );
						pst_Morph->dl_Index[ i ] = index;
					}

					MATH_AddVector( &V, VCast( pst_Vertex ), pst_Morph->dst_Vector + i );
					MATH_SubEqualVector( &V, pst_Center );
					MATH_TransformVertex( &V, M, &V);
					MATH_SubVector( pst_Morph->dst_Vector + i, &V, VCast( pst_Vertex ) );
				}
			}
		}
        return;
    }

	/* now update eventually morph */
	if (_i_UpdateMorph)
	{
		st_UpdateRLIData.p_Geo = _pst_Obj;
		st_UpdateRLIData.l_Op = WOR_Update_RLI_RotateVertices;
		st_UpdateRLIData.l_Ind0 = (LONG) pst_Center;
		st_UpdateRLIData.l_Ind1 = (LONG) M;
		WOR_Universe_Update_RLI(&st_UpdateRLIData);
	}

	if(_c_All)
	{
		for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
		{
			MATH_SubEqualVector(VCast(pst_Vertex), pst_Center);
			MATH_TransformVertex(VCast(pst_Vertex), M, VCast(pst_Vertex));
		}
	}
	else
	{
		if (pf_FloatSel)
		{
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++,pf_FloatSel ++)
			{
				MATH_SubVector(&stLocal0,VCast(pst_Vertex), pst_Center);
				MATH_TransformVertex(&stLocal0, M, &stLocal0);
				MATH_BlendVector(VCast(pst_Vertex), VCast(pst_Vertex), &stLocal0 , *pf_FloatSel);
			}
		}
		else
		for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
		{
			if(*pc_Sel & 1)
			{
				MATH_SubEqualVector(VCast(pst_Vertex), pst_Center);
				MATH_TransformVertex(VCast(pst_Vertex), M, VCast(pst_Vertex));
			}
		}
	}

    GEO_SubObject_UseVMoves( _pst_Obj );
	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
	GEO_SubObject_BuildFaceData(_pst_Obj);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_ScaleVertices(GEO_tdst_Object *_pst_Obj, MATH_tdst_Vector *C, MATH_tdst_Vector *v, float _f_Scale, GEO_Vertex *_pst_OldVertex, void *morph, int updtmorph )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		                    *pc_Sel, *pc_Last;
	GEO_Vertex	                    *pst_Vertex;
    GEO_tdst_ModifierMorphing_Data	*pst_Morph;
    int                             index, i;
    MATH_tdst_Vector                V;
	WOR_tdst_Update_RLI				st_UpdateRLIData;
	float							*pf_FloatSel;
	MATH_tdst_Vector				stLocal0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

    pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	pc_Last = pc_Sel + _pst_Obj->l_NbPoints;
	pst_Vertex = _pst_Obj->dst_Point;
	pf_FloatSel = _pst_Obj->pst_SubObject->pf_FloatSelection_V;

    if (_pst_Obj->pst_SubObject->pst_MorphData )
    {
        pst_Morph = (GEO_tdst_ModifierMorphing_Data	*) _pst_Obj->pst_SubObject->pst_MorphData;
        if (pf_FloatSel)
		{
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++, pf_FloatSel++ )
			{
				if( *pf_FloatSel ) 
				{
					index = pc_Sel - _pst_Obj->pst_SubObject->dc_VSel;
					for (i = 0; i < pst_Morph->l_NbVector; i++)
					{
						if (pst_Morph->dl_Index[i] == index )
							break;
					}
					if (i == pst_Morph->l_NbVector)
					{
						pst_Morph->l_NbVector++;
						if (i == 0)
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector* )MEM_p_Alloc( sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG* )MEM_p_Alloc( sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						else
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector* )MEM_p_Realloc( pst_Morph->dst_Vector, sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG* )MEM_p_Realloc( pst_Morph->dl_Index, sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						MATH_InitVectorToZero( pst_Morph->dst_Vector + i );
						pst_Morph->dl_Index[ i ] = index;
					}

					MATH_AddVector( &V, VCast( pst_Vertex ), pst_Morph->dst_Vector + i );
					
					MATH_SubVector(&stLocal0 , &V, C);
					MATH_MulEqualTwoVectors(&stLocal0, v);
					MATH_AddEqualVector(&stLocal0, C);
					MATH_BlendVector(&V, &V, &stLocal0 , *pf_FloatSel);
					
					MATH_SubVector( pst_Morph->dst_Vector + i, &V, VCast( pst_Vertex ) );
				}
			}
		}
		else
		{
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
			{
				if(*pc_Sel & 1) 
				{
					index = pc_Sel - _pst_Obj->pst_SubObject->dc_VSel;
					for (i = 0; i < pst_Morph->l_NbVector; i++)
					{
						if (pst_Morph->dl_Index[i] == index )
							break;
					}
					if (i == pst_Morph->l_NbVector)
					{
						pst_Morph->l_NbVector++;
						if (i == 0)
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector*)MEM_p_Alloc( sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG*)MEM_p_Alloc( sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						else
						{
							pst_Morph->dst_Vector = (MATH_tdst_Vector*)MEM_p_Realloc( pst_Morph->dst_Vector, sizeof(MATH_tdst_Vector) * pst_Morph->l_NbVector );
							pst_Morph->dl_Index = (LONG*)MEM_p_Realloc( pst_Morph->dl_Index, sizeof( LONG ) * pst_Morph->l_NbVector );
						}
						MATH_InitVectorToZero( pst_Morph->dst_Vector + i );
						pst_Morph->dl_Index[ i ] = index;
					}

					MATH_AddVector( &V, VCast( pst_Vertex ), pst_Morph->dst_Vector + i );
					MATH_SubEqualVector( &V, C);
					MATH_MulEqualTwoVectors(&V, v);
					MATH_AddEqualVector(&V, C);
					MATH_SubVector( pst_Morph->dst_Vector + i, &V, VCast( pst_Vertex ) );
				}
			}
		}
        return;
    }

    if (_f_Scale)
    {
        MATH_tdst_Vector    A, B;
        float               t, f_vNorm2;

		if (!_pst_OldVertex) return;
        f_vNorm2 = MATH_f_SqrVector( v );
        if ( f_vNorm2 == 0) return;

		/* now update all RLI of all loaded object */
		if (updtmorph)
		{
			st_UpdateRLIData.p_Geo = _pst_Obj;
			st_UpdateRLIData.l_Op = WOR_Update_RLI_ScaleVerticesComplex;
			st_UpdateRLIData.l_Ind0 = (LONG) C;
			st_UpdateRLIData.l_Ind1 = (LONG) v;
			st_UpdateRLIData.l_Ind2 = (LONG) _pst_OldVertex;
			st_UpdateRLIData.f_Blend = _f_Scale;
			*((LONG*) &st_UpdateRLIData.f0) = (LONG) morph;
			WOR_Universe_Update_RLI(&st_UpdateRLIData);
		}
        
        for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++, _pst_OldVertex++)
	    {
		    if ( !(*pc_Sel & 1) ) continue;

            MATH_SubVector(&A, VCast(_pst_OldVertex), C);
            t = MATH_f_DotProduct( &A, v ) / f_vNorm2;
            MATH_ScaleVector( &B, v, t );
            MATH_SubEqualVector( &A, &B );
            MATH_ScaleEqualVector( &B, _f_Scale );
            MATH_AddEqualVector( &A, &B );
            MATH_AddVector( VCast(pst_Vertex), &A, C);
        }
    }
    else
    {
		/* now update all RLI of all loaded object */
		if (updtmorph)
		{
			st_UpdateRLIData.p_Geo = _pst_Obj;
			st_UpdateRLIData.l_Op = WOR_Update_RLI_ScaleVertices;
			st_UpdateRLIData.l_Ind0 = (LONG) C;
			st_UpdateRLIData.l_Ind1 = (LONG) v;
			WOR_Universe_Update_RLI(&st_UpdateRLIData);
		}

		if (pf_FloatSel)
		{
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++,pf_FloatSel ++)
			{
				MATH_tdst_Vector	stLocal0;
				MATH_SubVector(&stLocal0 , VCast(pst_Vertex), C);
				MATH_MulEqualTwoVectors(&stLocal0, v);
				MATH_AddEqualVector(&stLocal0, C);
				MATH_BlendVector(VCast(pst_Vertex), VCast(pst_Vertex), &stLocal0 , *pf_FloatSel);
			}
		}
		else
		{
			for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
			{
				if(*pc_Sel & 1)
				{
					MATH_SubEqualVector(VCast(pst_Vertex), C);
					MATH_MulEqualTwoVectors(VCast(pst_Vertex), v);
					MATH_AddEqualVector(VCast(pst_Vertex), C);
				}
			}
		}
    }

    GEO_SubObject_UseVMoves( _pst_Obj );
	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
	GEO_SubObject_BuildFaceData(_pst_Obj);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_SubObject_VSet( GEO_tdst_Object *_pst_Obj, MATH_tdst_Vector *V )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*pc_Sel, *pc_Last;
	GEO_Vertex	*pst_Vertex;
    int         res = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	pc_Last = pc_Sel + _pst_Obj->l_NbPoints;
	pst_Vertex = _pst_Obj->dst_Point;
	for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
	{
		if(*pc_Sel & 1)
		{
            MATH_CopyVector( VCast(pst_Vertex), V );
            res++;
		}
	}

    if (!res) return 0;
    GEO_SubObject_UseVMoves( _pst_Obj );
	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
	GEO_SubObject_BuildFaceData(_pst_Obj);
    return res;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_VWeld(GEO_tdst_Object *_pst_Obj, int _i_Vertex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char								*pc_Sel;
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*pst_Tri;
	int									i_Elem, t, i_Vertex;
    LONG                                *dl_NewIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	for(i_Vertex = 0; i_Vertex < _pst_Obj->l_NbPoints; i_Vertex++, pc_Sel++)
	{
		if(!(*pc_Sel & 1)) continue;

		for(i_Elem = 0; i_Elem < _pst_Obj->l_NbElements; i_Elem++)
		{
			pst_Elem = _pst_Obj->dst_Element + i_Elem;
			pst_Tri = pst_Elem->dst_Triangle;
			for(t = 0; t < pst_Elem->l_NbTriangles; t++, pst_Tri++)
			{
				if(pst_Tri->auw_Index[0] == i_Vertex) pst_Tri->auw_Index[0] = _i_Vertex;
				if(pst_Tri->auw_Index[1] == i_Vertex) pst_Tri->auw_Index[1] = _i_Vertex;
				if(pst_Tri->auw_Index[2] == i_Vertex) pst_Tri->auw_Index[2] = _i_Vertex;
			}
		}
	}

	/* now delete all point */
	GEO_SubObject_VDelSel(_pst_Obj, &dl_NewIndex );

	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_SubObject_BuildEdgeData(_pst_Obj);

	/* unselect vertex */
	GEO_SubObject_UnselAll(_pst_Obj);

    /* select vertex were all points are welded */
    _pst_Obj->pst_SubObject->dc_VSel[ dl_NewIndex[ _i_Vertex ] ] |= 1;
    GEO_SubObject_HideUpdate( _pst_Obj, TRUE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_VWeldThresh(GEO_tdst_Object *_pst_Obj, float _f_Thresh)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	typedef struct	tdst_WeldPoint_
	{
		MATH_tdst_Vector	V;
		int					i, n;
	} tdst_WeldPoint;

	tdst_WeldPoint						*W;
	LONG								*dl_NewIndex;
	int									NbDel, NbW, i, j;
	float								f, dist;
	GEO_tdst_ElementIndexedTriangles	*pst_Element, *pst_LElement;
	GEO_tdst_IndexedTriangle			*pst_Face, *pst_LFace;
	MATH_tdst_Vector					Delta;
	GEO_Vertex							*Point;
	WOR_tdst_Update_RLI					st_UpdateRLIData;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	W = (tdst_WeldPoint *) L_malloc(_pst_Obj->l_NbPoints * sizeof(tdst_WeldPoint));
	L_memset(W, 0, _pst_Obj->l_NbPoints * sizeof(tdst_WeldPoint));
	dl_NewIndex = (LONG*)L_malloc(4 * _pst_Obj->l_NbPoints);
	NbDel = NbW = 0;
	f = _f_Thresh * _f_Thresh;
	Point = _pst_Obj->dst_Point;

	for(i = 0; i < _pst_Obj->l_NbPoints; i++, Point++)
	{
		dl_NewIndex[i] = i;
		if(_pst_Obj->pst_SubObject->dc_VSel[i] & 1)
		{
			for(j = 0; j < NbW; j++)
			{
				MATH_SubVector(&Delta, &W[j].V, VCast(Point));
				dist = MATH_f_SqrVector(&Delta);

				if(dist < f)
				{
					MATH_AddScaleVector(&Delta, VCast(Point), &W[j].V, (float) W[j].n);
					W[j].n++;
					MATH_ScaleVector(&W[j].V, &Delta, 1.0f / (float) W[j].n);
					dl_NewIndex[i] = W[j].i;
					NbDel++;
					break;
				}
			}

			if(j == NbW)
			{
				MATH_CopyVector(&W[NbW].V, VCast(Point));
				W[NbW].i = i;
				W[NbW].n = 1;
				NbW++;
			}
		}
	}

	if(NbDel == 0) 
	{
		LINK_PrintStatusMsg( "WeldThresh : No vertex welded, threshold too small" );
		return 0;
	}

    for (j = 0; j < NbW; j++)
    {
        if (W[j].n != 1)
            MATH_CopyVector( VCast( _pst_Obj->dst_Point + W[j].i), &W[j].V );
    }

	for(j = 0, i = 0; i < _pst_Obj->l_NbPoints; i++)
	{
		if(dl_NewIndex[i] != i)
		{
			dl_NewIndex[i] = dl_NewIndex[dl_NewIndex[i]];
			continue;
		}

		if(i != j)
		{
			MATH_CopyVector(VCast(_pst_Obj->dst_Point + j), VCast(_pst_Obj->dst_Point + i));
			MATH_CopyVector(_pst_Obj->dst_PointNormal + j, _pst_Obj->dst_PointNormal + i);
			_pst_Obj->pst_SubObject->dc_VSel[j] = _pst_Obj->pst_SubObject->dc_VSel[i];
			if(_pst_Obj->dul_PointColors) _pst_Obj->dul_PointColors[j + 1] = _pst_Obj->dul_PointColors[i + 1];
			dl_NewIndex[i] = j;
		}

		j++;
	}

	/* now update all RLI of all loaded object */
	st_UpdateRLIData.p_Geo = _pst_Obj;
	st_UpdateRLIData.l_Op = WOR_Update_RLI_DelSome;
	st_UpdateRLIData.l_Ind0 = _pst_Obj->l_NbPoints - NbDel;
	st_UpdateRLIData.l_Ind1 = (LONG) dl_NewIndex;
	WOR_Universe_Update_RLI(&st_UpdateRLIData);

	/* update eventually skin */
	GEO_SKN_UpdateAfterVertexEdition(_pst_Obj, &st_UpdateRLIData);

	_pst_Obj->l_NbPoints = j;
	if(_pst_Obj->dul_PointColors) _pst_Obj->dul_PointColors[0] = j;

	pst_Element = _pst_Obj->dst_Element;
	pst_LElement = pst_Element + _pst_Obj->l_NbElements;
	for(; pst_Element < pst_LElement; pst_Element++)
	{
		pst_Face = pst_Element->dst_Triangle;
		pst_LFace = pst_Face + pst_Element->l_NbTriangles;
		for(; pst_Face < pst_LFace; pst_Face++)
		{
			for(i = 0; i < 3; i++) pst_Face->auw_Index[i] = (USHORT) dl_NewIndex[pst_Face->auw_Index[i]];
		}
	}

	GEO_SubObject_RemoveBadFaces(_pst_Obj);

	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_SubObject_BuildEdgeData(_pst_Obj);

	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);

	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_VDelSel(GEO_tdst_Object *_pst_Object, LONG **_ppl_NewIndex )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG								*dl_NewIndex, l_Last, l_Nb;
	char								*pc_Sel;
	ULONG								*pul_RLI;
	WOR_tdst_Update_RLI					st_UpdateRLIData;
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*pst_Tri;
	int									i, t, i_Elem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	if(!_pst_Object) return;
	if(!_pst_Object->pst_SubObject) return;
	if(_pst_Object->st_Id.i->ul_Type != GRO_Geometric) return;
	
	dl_NewIndex = (LONG*)L_malloc(4 * _pst_Object->l_NbPoints);
	l_Last = _pst_Object->l_NbPoints - 1;
	pc_Sel = _pst_Object->pst_SubObject->dc_VSel;

	l_Nb = 0;
	for(i = 0; i <= l_Last; i++)
	{
		if(pc_Sel[i] & 1)
		{
			dl_NewIndex[i] = -1;
			while(l_Last > i)
			{
				if(!(pc_Sel[l_Last] & 1))
				{
					l_Nb++;
					dl_NewIndex[l_Last--] = i;
					break;
				}

				dl_NewIndex[l_Last--] = -1;
			}
		}
		else
		{
			l_Nb++;
			dl_NewIndex[i] = i;
		}
	}
	
	/* prepare for RLI update */
	if(pul_RLI = _pst_Object->dul_PointColors) pul_RLI[0] = l_Nb;

	/* update point, normals and RLI list */
	for(i = 0; i < _pst_Object->l_NbPoints; i++)
	{
		if((dl_NewIndex[i] == i) || (dl_NewIndex[i] == -1)) continue;
		MATH_CopyVector(VCast(&_pst_Object->dst_Point[dl_NewIndex[i]]), VCast(&_pst_Object->dst_Point[i]));
		MATH_CopyVector(&_pst_Object->dst_PointNormal[dl_NewIndex[i]], &_pst_Object->dst_PointNormal[i]);

		if(pul_RLI) pul_RLI[1 + dl_NewIndex[i]] = pul_RLI[1 + i];
	}

	/* loop through triangles to change index */
	for(i_Elem = 0; i_Elem < _pst_Object->l_NbElements; i_Elem++)
	{
		pst_Elem = _pst_Object->dst_Element + i_Elem;
		pst_Tri = pst_Elem->dst_Triangle;
		for(t = 0; t < pst_Elem->l_NbTriangles; t++, pst_Tri++)
		{
			pst_Tri->auw_Index[0] = (USHORT) dl_NewIndex[pst_Tri->auw_Index[0]];
			pst_Tri->auw_Index[1] = (USHORT) dl_NewIndex[pst_Tri->auw_Index[1]];
			pst_Tri->auw_Index[2] = (USHORT) dl_NewIndex[pst_Tri->auw_Index[2]];
		}
	}
	
	/* now update all RLI of all loaded object */
	st_UpdateRLIData.p_Geo = _pst_Object;
	st_UpdateRLIData.l_Op = WOR_Update_RLI_DelSome;
	st_UpdateRLIData.l_Ind0 = l_Nb;
	st_UpdateRLIData.l_Ind1 = (LONG) dl_NewIndex;
	WOR_Universe_Update_RLI(&st_UpdateRLIData);

	/* update eventually skin */
	GEO_SKN_UpdateAfterVertexEdition(_pst_Object, &st_UpdateRLIData);

	/* look for bad triangles */
	if(GEO_i_SubObject_FSelBad(_pst_Object)) GEO_SubObject_FDelSel(_pst_Object, FALSE);
	
	/* change number of points */
	_pst_Object->l_NbPoints = l_Nb;

	/* Unsel all vertex */
	L_memset(_pst_Object->pst_SubObject->dc_VSel, 0, l_Nb);

    if (_ppl_NewIndex)
        *_ppl_NewIndex = dl_NewIndex;
    else
	    L_free(dl_NewIndex);
	    
	GEO_SubObject_HideUpdate( _pst_Object, TRUE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_VBuildFace(struct GEO_tdst_Object_ *_pst_Obj, ULONG v0, ULONG v1, ULONG v2, int _i_Id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			st_Tri, *pst_Triangle;
	GEO_tdst_SubObject_Face				*pst_F[ 3 ][ 16 ], *pst_BestF[ 3 ];
	GEO_tdst_IndexedTriangle			*pst_T[ 3 ][ 16 ];
	int									nb_T[ 3 ];
	GEO_tdst_SubObject_Face				*pst_Face, *pst_Last;
	int									i, j, k;
	int									score, best;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;

	/* check validity of points */
	if(v0 >= (ULONG) _pst_Obj->l_NbPoints) return 0;
	if(v1 >= (ULONG) _pst_Obj->l_NbPoints) return 0;
	if(v2 >= (ULONG) _pst_Obj->l_NbPoints) return 0;
	if((v0 == v1) || (v0 == v2) || (v1 == v2)) return 0;

	st_Tri.auw_Index[0] = (USHORT) v0;
	st_Tri.auw_Index[1] = (USHORT) v1;
	st_Tri.auw_Index[2] = (USHORT) v2;
	
	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pst_Last = pst_Face + _pst_Obj->pst_SubObject->ul_NbFaces;
	nb_T[ 0 ] = nb_T[ 1 ] = nb_T[ 2 ] = 0;
	for ( ;pst_Face < pst_Last; pst_Face++ )
	{
		pst_Triangle = _pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
		for (i = 0; i < 3; i++)
		{
			if (
				( pst_Triangle->auw_Index[ 0 ] == st_Tri.auw_Index[i] ) ||
				( pst_Triangle->auw_Index[ 1 ] == st_Tri.auw_Index[i] ) ||
				( pst_Triangle->auw_Index[ 2 ] == st_Tri.auw_Index[i] )
				)
				if (nb_T[i] < 16) 
				{
					pst_F[ i ][ nb_T[ i ] ] = pst_Face;
					pst_T[ i ][ nb_T[ i ] ] = pst_Triangle;
					nb_T[ i ]++;
				}
		}
	}
	
	// cherche le meilleur triangle pour vertex 0
	pst_BestF[ 0 ] = NULL;
	best = -1;
	for ( i = 0; i < nb_T[0]; i++ )
	{
		score = 0;
		
		// best score for face with same ID
		if (_pst_Obj->dst_Element[ pst_F[ 0 ][ i ]->uw_Element ].l_MaterialId == _i_Id) 
			score += 10;
			
		for (j = 0; j < nb_T[ 1 ]; j++ )
		{
			if (pst_F[1][j] == pst_F[0][i] )
				score += 3;
		}
		
		for (k = 0; k < nb_T[ 2 ]; k++ )
		{
			if ( pst_F[2][k] == pst_F[0][i] )
				score += 3;
		}
		
		if (score > best)
		{
			best = score;
			pst_BestF[ 0 ] = pst_F[ 0 ][ i ];
		}
	}
	
	// cherche le meilleur triangle pour vertex 1
	pst_BestF[ 1 ] = NULL;
	best = -1;
	for ( j = 0; j < nb_T[1]; j++ )
	{
		score = 0;
		
		// best score for face with same ID
		if (_pst_Obj->dst_Element[ pst_F[ 1 ][ j ]->uw_Element ].l_MaterialId == _i_Id) 
			score += 10;
			
		for (i = 0; i < nb_T[ 0 ]; i++ )
		{
			if (pst_F[1][j] == pst_F[0][i] )
				score += 3;
		}
		
		for (k = 0; k < nb_T[ 2 ]; k++ )
		{
			if ( pst_F[2][k] == pst_F[1][j] )
				score += 3;
		}
		
		if (pst_F[1][j] == pst_BestF[ 0 ]) score++;
		
		if (score > best)
		{
			best = score;
			pst_BestF[ 1 ] = pst_F[ 1 ][ j ];
		}
	}
	
	// cherche le meilleur triangle pour vertex 2
	pst_BestF[ 2 ] = NULL;
	best = -1;
	for ( k = 0; k < nb_T[2]; k++ )
	{
		score = 0;
		
		// best score for face with same ID
		if (_pst_Obj->dst_Element[ pst_F[ 2 ][ k ]->uw_Element ].l_MaterialId == _i_Id) 
			score += 10;
			
		for (i = 0; i < nb_T[ 0 ]; i++ )
		{
			if (pst_F[2][k] == pst_F[0][i] )
				score += 3;
		}
		
		for (j = 0; j < nb_T[ 1 ]; j++ )
		{
			if ( pst_F[2][k] == pst_F[1][j] )
				score += 3;
		}
		
		if (pst_F[2][k] == pst_BestF[ 0 ]) score++;
		if (pst_F[2][k] == pst_BestF[ 1 ]) score++;
		
		if (score > best)
		{
			best = score;
			pst_BestF[ 2 ] = pst_F[ 2 ][ k ];
		}
	}
	
	for (i = 0; i < 3; i++)
	{
		if (pst_BestF[ i ] == NULL )
		{
			GEO_AddUV( _pst_Obj, 0, 0 );
			st_Tri.auw_UV[ i ] = (USHORT) _pst_Obj->l_NbUVs - 1;
		}
		else
		{
			pst_Triangle = _pst_Obj->dst_Element[pst_BestF[ i ]->uw_Element].dst_Triangle + pst_BestF[ i ]->uw_Index;
			for (j = 0; j < 3; j++ )
			{
				if (pst_Triangle->auw_Index[ j ] == st_Tri.auw_Index[ i ] )
				{
					st_Tri.auw_UV[ i ] = pst_Triangle->auw_UV[ j ];
					break;
				}
			}
		}
	}
	
	for ( i = 0; i < _pst_Obj->l_NbElements; i++ )
	{
		if ( _pst_Obj->dst_Element[ i ].l_MaterialId == _i_Id )
			break;
	}
	if ( i == _pst_Obj->l_NbElements )
	{
		GEO_AddElement(_pst_Obj, _i_Id, 0);
		pst_Elem = _pst_Obj->dst_Element + (_pst_Obj->l_NbElements - 1);
	}
	else
		pst_Elem = _pst_Obj->dst_Element + i;
	GEO_AddTriangle(pst_Elem, &st_Tri);

	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_AllocBuffers(_pst_Obj);
    GEO_SubObject_HideUpdate( _pst_Obj, TRUE );
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_VSym( GEO_tdst_Object *_pst_Obj, int axe )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*pc_Sel, *pc_Last;
	GEO_Vertex	*pst_Vertex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	pc_Last = pc_Sel + _pst_Obj->l_NbPoints;
	pst_Vertex = _pst_Obj->dst_Point;

    if( axe == 0) // symetrie X
    {
	    for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
	    {
		    if (!(*pc_Sel & 1)) continue;
            pst_Vertex->x = -pst_Vertex->x;
		}
	}
    else if (axe == 1) // symetrie Y
    {
        for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
	    {
		    if (!(*pc_Sel & 1)) continue;
            pst_Vertex->y = -pst_Vertex->y;
		}
    }
    else if (axe == 2) // symetrie Z
    {
        for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
	    {
		    if (!(*pc_Sel & 1)) continue;
            pst_Vertex->z = -pst_Vertex->z;
		}
    }

    _pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
	GEO_SubObject_BuildFaceData(_pst_Obj);

}

/*
 =======================================================================================================================
	symétrise les points sélectionnés 
	le plan de symétrie est définie par un point et la normal au plan
	si le vecteur N ( normal au plan ) est un pointeur nulle, une symétrie centrale est effectuée
	le centre et la normale sont exprimée dans le repère local de l'objet
	la normale doit etre un vecteur normé
 =======================================================================================================================
 */
void GEO_SubObject_VSymAll( GEO_tdst_Object *_pst_Obj, GEO_Vertex *C, MATH_tdst_Vector *N )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*pc_Sel, *pc_Last;
	GEO_Vertex			*pst_Vertex;
	MATH_tdst_Vector	Center, V0;
	float				f_dot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	pc_Last = pc_Sel + _pst_Obj->l_NbPoints;
	pst_Vertex = _pst_Obj->dst_Point;

	if (C == NULL)
	{
		C = &Center;
		MATH_InitVectorToZero( &Center );
	}

	// symétrie centrale
	if (N == NULL )
	{
		for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
	    {
		    if (!(*pc_Sel & 1)) continue;
			MATH_SubVector( &V0, C, pst_Vertex );
			MATH_AddVector( pst_Vertex, C, &V0 );
		}
	}
	else
	{
		for(; pc_Sel < pc_Last; pc_Sel++, pst_Vertex++)
	    {
		    if (!(*pc_Sel & 1)) continue;
			MATH_SubVector( &V0, C, pst_Vertex );
			f_dot = MATH_f_DotProduct( &V0, N );
			MATH_ScaleVector( &V0, N, 2 * f_dot );
			MATH_AddEqualVector( pst_Vertex, &V0 );
		}
	}

    _pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
	GEO_SubObject_BuildFaceData(_pst_Obj);

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_VSelUnused(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*pst_Tri;
	int									i, i_Elem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;

	/* select all points */
	L_memset(_pst_Obj->pst_SubObject->dc_VSel, 1, _pst_Obj->l_NbPoints);

	/* loop through triangles to deselect all used vertex */
	for(i_Elem = 0; i_Elem < _pst_Obj->l_NbElements; i_Elem++)
	{
		pst_Elem = _pst_Obj->dst_Element + i_Elem;
		pst_Tri = pst_Elem->dst_Triangle;
		for(i = 0; i < pst_Elem->l_NbTriangles; i++, pst_Tri++)
		{
			_pst_Obj->pst_SubObject->dc_VSel[pst_Tri->auw_Index[0]] = 0;
			_pst_Obj->pst_SubObject->dc_VSel[pst_Tri->auw_Index[1]] = 0;
			_pst_Obj->pst_SubObject->dc_VSel[pst_Tri->auw_Index[2]] = 0;
		}
	}

	/* search a selected point to return 1 */
	for(i = 0; i < _pst_Obj->l_NbPoints; i++)
	{
		if(_pst_Obj->pst_SubObject->dc_VSel[i] == 1) return 1;
	}

	/* no vertex selected => no vertex unused : return 0 */
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_VUpdateESel(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	char					*pc_Sel, *pc_ESel;
	GEO_tdst_SubObject_Edge *pst_Edge;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pc_ESel = _pst_Obj->pst_SubObject->dc_ESel;
    pc_Sel = pc_ESel + _pst_Obj->pst_SubObject->ul_NbEdges;
    while (pc_ESel < pc_Sel)
        *pc_ESel++ &= 0x80;

    
    pc_ESel = _pst_Obj->pst_SubObject->dc_ESel;
    pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbEdges; i++, pst_Edge++)
	{
		if((pc_Sel[pst_Edge->ul_Point[0]] & 1) && (pc_Sel[pst_Edge->ul_Point[1]] & 1)) pc_ESel[i] |= 1;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_VUpdateFSel(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	char						*pc_Sel, *pc_FSel;
	GEO_tdst_SubObject_Face		*pst_Face;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	pc_FSel = _pst_Obj->pst_SubObject->dc_FSel;

    for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++)
        pc_FSel[i] &= 0x80;

	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Face++)
	{
		pst_Triangle = _pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
		if
		(
			(pc_Sel[pst_Triangle->auw_Index[0]] & 1)
		&&	(pc_Sel[pst_Triangle->auw_Index[1]] & 1)
		&&	(pc_Sel[pst_Triangle->auw_Index[2]] & 1)
		) pc_FSel[i] |= 1;
	}
}

/*$4
 ***********************************************************************************************************************
    Edges
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_EUpdateVSel(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	char					*pc_Sel, *pc_VSel;
	GEO_tdst_SubObject_Edge *pst_Edge;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;


    pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
    for (i = 0; i < _pst_Obj->l_NbPoints; i++, pc_Sel++)
        *pc_Sel &= 0x80;

	pc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
	pc_VSel = _pst_Obj->pst_SubObject->dc_VSel;
	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbEdges; i++, pc_Sel++, pst_Edge++)
	{
		if(*pc_Sel & 1)
		{
			if(pst_Edge->ul_Point[0] != (ULONG) - 1)
			{
				pc_VSel[pst_Edge->ul_Point[0]] |= 1;
				if(pst_Edge->ul_Point[1] != (ULONG) - 1) pc_VSel[pst_Edge->ul_Point[1]] |= 1;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_ESelectBad(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i;
	char					*pc_Sel;
	GEO_tdst_SubObject_Edge *pst_Edge;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	L_memset(_pst_Obj->pst_SubObject->dc_ESel, 0, _pst_Obj->pst_SubObject->ul_NbEdges);

	pc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;

	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbEdges; i++, pc_Sel++, pst_Edge++)
	{
		if(pst_Edge->ul_Triangle[2] != (ULONG) - 1) *pc_Sel = 1;
	}

	GEO_SubObject_EUpdateVSel(_pst_Obj);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_EPick(GEO_tdst_Object *_pst_Obj, MATH_tdst_Vector *C, MATH_tdst_Vector *D, float *_pf_Depth, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		*A, *B, H1, H2;
	float					depth, f, d, t1, t2;
	int						i, i_Result;
	GEO_tdst_SubObject_Edge *pst_Edge;
	static float			fmax = 0.01f;
    char                    *pc_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return -1;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return -1;
	if(!_pst_Obj->pst_SubObject) return -1;

	i_Result = -1;
	f = fmax;

	*_pf_Depth = Cf_Infinit;

	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
    pc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbEdges; i++, pst_Edge++, pc_Sel++)
	{
        if (*pc_Sel & 0x80 )
            continue;

		if(_pst_GO && (!(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) (pst_Edge->ul_Point[0]))) ||!(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) (pst_Edge->ul_Point[1]))))) 
			continue;
				
		A = &_pst_Obj->pst_SubObject->dst_3D[pst_Edge->ul_Point[0]];
		B = &_pst_Obj->pst_SubObject->dst_3D[pst_Edge->ul_Point[1]];
		if(!SOFT_l_NearestPointOf2Lines(A, B, C, D, &d, &t1, &H1, &t2, &H2)) continue;
		if((t1 < 0) || (t1 > 1.0f)) continue;

		depth = t1 * B->z + (1 - t1) * A->z;
		if(depth <= 0) continue;

		d /= depth;

		if(depth > (*_pf_Depth + 0.1) ) continue;
		if ( d > f) continue;

		f = d;
		*_pf_Depth = depth;
		i_Result = i;
	}

	return i_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_EPickBox
(
	GEO_tdst_Object		*_pst_Obj,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	char				_c_Inside,
	OBJ_tdst_GameObject *_pst_GO
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		*C, *D, *E;
	int						i, i_Result;
	char					*pc_Sel;
	GEO_tdst_SubObject_Edge *pst_Edge;
	float					y, y2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	pc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
	i_Result = 0;
	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;

	if(_c_Inside)
	{
		for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbEdges; i++, pst_Edge++, pc_Sel++)
		{
            if (*pc_Sel & 0x80) continue;

			C = &_pst_Obj->pst_SubObject->dst_2D[pst_Edge->ul_Point[0]];
			if(C->x == 0x80000000) continue;
			D = &_pst_Obj->pst_SubObject->dst_2D[pst_Edge->ul_Point[1]];
			if(D->x == 0x80000000) continue;

			if
			(
				!(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) (pst_Edge->ul_Point[0])))
			||	!(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) (pst_Edge->ul_Point[1])))
			) continue;

			if((C->x < A->x) || (C->x > B->x) || (C->y < A->y) || (C->y > B->y)) continue;
			if((D->x < A->x) || (D->x > B->x) || (D->y < A->y) || (D->y > B->y)) continue;
			if (GEO_SOFT_PickWithZBufferCulling(C) && GEO_SOFT_PickWithZBufferCulling(D)) continue;

			*pc_Sel |= 2;
			i_Result++;
		}
	}
	else
	{
		for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbEdges; i++, pst_Edge++, pc_Sel++)
		{
            if (*pc_Sel & 0x80) continue;

			C = &_pst_Obj->pst_SubObject->dst_2D[pst_Edge->ul_Point[0]];
			if(C->x == 0x80000000) continue;
			D = &_pst_Obj->pst_SubObject->dst_2D[pst_Edge->ul_Point[1]];
			if(D->x == 0x80000000) continue;

			if
			(
				!(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) (pst_Edge->ul_Point[0])))
			||	!(GEO_ColMap_VerticeVisible(_pst_GO, (USHORT) (pst_Edge->ul_Point[1])))
			) continue;

			if(C->x > D->x)
			{
				E = D;
				D = C;
				C = E;
			}

			if((D->x < A->x) || (C->x > B->x)) continue;
			if((C->y < A->y) && (D->y < A->y)) continue;
			if((C->y > B->y) && (D->y > B->y)) continue;
			if (GEO_SOFT_PickWithZBufferCulling(C) && GEO_SOFT_PickWithZBufferCulling(D)) continue;

			if(C->x < A->x)
			{
				y = (D->y - C->y) / (D->x - C->x) * (A->x - C->x) + C->y;
				if(D->x < B->x)
					y2 = D->y;
				else
					y2 = (D->y - C->y) / (D->x - C->x) * (B->x - C->x) + C->y;

				if(((y < A->y) && (y2 < A->y)) || ((y > B->y) && (y2 > B->y))) continue;
			}
			else
			{
				if(((C->y < A->y) || (C->y > B->y)) && (D->x > B->x))
				{
					y = C->y;
					y2 = (D->y - C->y) / (D->x - C->x) * (B->x - C->x) + C->y;
					if(((y < A->y) && (y2 < A->y)) || ((y > B->y) && (y2 > B->y))) continue;
				}
			}

			*pc_Sel |= 2;
			i_Result++;
		}
	}

	return i_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GEO_tdst_IndexedTriangle *GEO_SubObject_GetTriangleFromEdgeIndex(GEO_tdst_Object *_pst_Obj, ULONG _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Triangle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Element = _pst_Obj->dst_Element + ((_ul_Index >> 16) & 0xFFF);
	pst_Triangle = pst_Element->dst_Triangle + (_ul_Index & 0xFFFF);

	return pst_Triangle;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_BuildEdgeData(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject					*pst_SO;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Triangle;
	ULONG								*pul_V2T, *dul_Vertex2Triangle;
	ULONG								ul_Vertex, ul_Vertex2, ul_FreeV2T;
	LONG								l_Triangle, l_Elem, l_Index, l_Index2;
	GEO_tdst_SubObject_Edge				*pst_Edge;
	ULONG								*dul_ElementOffset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pst_SO = _pst_Obj->pst_SubObject;

	if(pst_SO->ul_NbEdges)
	{
		L_free(pst_SO->dst_Edge);
		L_free(pst_SO->dc_ESel);
	}

	pst_SO->ul_NbEdges = GEO_l_GetNumberOfTriangles(_pst_Obj) * 3;
	pst_SO->ul_NbEdges += _pst_Obj->l_NbPoints;

	dul_Vertex2Triangle = (ULONG *) L_malloc(pst_SO->ul_NbEdges * 24);
	L_memset(dul_Vertex2Triangle, 0xFF, pst_SO->ul_NbEdges * 24);
	ul_FreeV2T = _pst_Obj->l_NbPoints;

	dul_ElementOffset = (ULONG *) L_malloc((_pst_Obj->l_NbElements + 1) * 4);
	dul_ElementOffset[0] = 0;

	pst_Element = _pst_Obj->dst_Element;
	for(l_Elem = 0; l_Elem < _pst_Obj->l_NbElements; l_Elem++, pst_Element++)
	{
		dul_ElementOffset[l_Elem + 1] = dul_ElementOffset[l_Elem] + pst_Element->l_NbTriangles;

		pst_Triangle = pst_Element->dst_Triangle;
		for(l_Triangle = 0; l_Triangle < pst_Element->l_NbTriangles; l_Triangle++, pst_Triangle++)
		{
			for(l_Index = 0; l_Index < 3; l_Index++)
			{
				ul_Vertex = pst_Triangle->auw_Index[l_Index];
				for(l_Index2 = 1; l_Index2 <= 2; l_Index2++)
				{
					ul_Vertex2 = pst_Triangle->auw_Index[(l_Index + l_Index2) % 3];
					if(ul_Vertex2 <= ul_Vertex) continue;
					pul_V2T = &dul_Vertex2Triangle[ul_Vertex * 6];
					if(*(pul_V2T + 1) != -1)
					{
						while(*pul_V2T != (ULONG) - 1)
						{
							if(*(pul_V2T + 1) == ul_Vertex2) break;
							pul_V2T = &dul_Vertex2Triangle[(*pul_V2T) * 6];
						}

						if(*(pul_V2T + 1) != ul_Vertex2)
						{
							*pul_V2T = ul_FreeV2T;
							pul_V2T = &dul_Vertex2Triangle[ul_FreeV2T * 6];
							ul_FreeV2T++;
						}
					}

					*(pul_V2T + 1) = ul_Vertex2;
					if(*(pul_V2T + 2) == (ULONG) - 1)
						*(pul_V2T + 2) = (l_Elem << 16) + l_Triangle;
					else if(*(pul_V2T + 3) == (ULONG) - 1)
						*(pul_V2T + 3) = (l_Elem << 16) + l_Triangle;
					else if(*(pul_V2T + 4) == (ULONG) - 1)
						*(pul_V2T + 4) = (l_Elem << 16) + l_Triangle;
					else
						*(pul_V2T + 5) = (l_Elem << 16) + l_Triangle;
				}
			}
		}
	}

	pst_SO->ul_NbEdges = ul_FreeV2T;
	pst_SO->dst_Edge = (GEO_tdst_SubObject_Edge*)L_malloc(pst_SO->ul_NbEdges * sizeof(GEO_tdst_SubObject_Edge));
	pst_SO->dc_ESel = (char*)L_malloc(pst_SO->ul_NbEdges);
	L_memset(pst_SO->dc_ESel, 0, pst_SO->ul_NbEdges);
	pst_Edge = pst_SO->dst_Edge;

	for(ul_Vertex = 0; (LONG) ul_Vertex < _pst_Obj->l_NbPoints; ul_Vertex++)
	{
		pul_V2T = &dul_Vertex2Triangle[ul_Vertex * 6];
		if(*(pul_V2T + 1) == (ULONG) - 1)
		{
			pst_SO->ul_NbEdges--;
			continue;
		}

		while(1)
		{
			pst_Edge->ul_Point[0] = ul_Vertex;
			pst_Edge->ul_Point[1] = *(pul_V2T + 1);
			pst_Edge->ul_Triangle[0] = *(pul_V2T + 2);
			pst_Edge->ul_Triangle[1] = *(pul_V2T + 3);
			pst_Edge->ul_Triangle[2] = *(pul_V2T + 4);
			pst_Edge->ul_Triangle[3] = *(pul_V2T + 5);

			if(pst_Edge->ul_Triangle[0] != (ULONG) - 1)
			{
				pst_Edge->uw_Index[0] = (USHORT) (dul_ElementOffset[pst_Edge->ul_Triangle[0] >> 16] + (pst_Edge->ul_Triangle[0] & 0xFFFF));
			}
			else
				pst_Edge->uw_Index[0] = (USHORT) - 1;

			if(pst_Edge->ul_Triangle[1] != (ULONG) - 1)
			{
				pst_Edge->uw_Index[1] = (USHORT) (dul_ElementOffset[pst_Edge->ul_Triangle[1] >> 16] + (pst_Edge->ul_Triangle[1] & 0xFFFF));
			}
			else
				pst_Edge->uw_Index[1] = (USHORT) - 1;

			if(pst_Edge->ul_Triangle[2] != (ULONG) - 1)
			{
				pst_Edge->uw_Index[2] = (USHORT) (dul_ElementOffset[pst_Edge->ul_Triangle[2] >> 16] + (pst_Edge->ul_Triangle[2] & 0xFFFF));
			}
			else
				pst_Edge->uw_Index[2] = (USHORT) - 1;

			if(pst_Edge->ul_Triangle[3] != (ULONG) - 1)
			{
				pst_Edge->uw_Index[3] = (USHORT) (dul_ElementOffset[pst_Edge->ul_Triangle[3] >> 16] + (pst_Edge->ul_Triangle[3] & 0xFFFF));
			}
			else
				pst_Edge->uw_Index[3] = (USHORT) - 1;

			pst_Edge++;
			if(*pul_V2T == (ULONG) - 1) break;
			pul_V2T = &dul_Vertex2Triangle[(*pul_V2T) * 6];
		}
	}

	L_free(dul_Vertex2Triangle);
	L_free(dul_ElementOffset);

	GEO_SubObject_TestEdge(_pst_Obj);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_EFind(GEO_tdst_Object *_pst_Obj, ULONG p1, ULONG p2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG					p;
	GEO_tdst_SubObject_Edge *pst_Edge, *pst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(p1 > p2)
	{
		p = p1;
		p1 = p2;
		p2 = p;
	}

	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
	pst_Last = pst_Edge + _pst_Obj->pst_SubObject->ul_NbEdges;

	for(; pst_Edge < pst_Last; pst_Edge++)
	{
		if(pst_Edge->ul_Point[0] == p1) break;
		if(pst_Edge->ul_Point[0] > p1) return -1;;
	}

	for(; pst_Edge < pst_Last; pst_Edge++)
	{
		if(pst_Edge->ul_Point[0] > p1) return -1;
		if(pst_Edge->ul_Point[1] == p2) return(pst_Edge - _pst_Obj->pst_SubObject->dst_Edge);
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_TestEdge(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Triangle;
	LONG								l_Index, l_Index2;
	GEO_tdst_SubObject_Edge				*pst_Edge;
	GEO_tdst_SubObject					*pst_SO;
	ULONG								ul_CurVertexIndex;

	/* char sz_Text[ 256 ]; */
	ULONG								ul_OldF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	ul_CurVertexIndex = 0;
	pst_SO = _pst_Obj->pst_SubObject;

	ul_OldF = pst_SO->ul_Flags;

	pst_SO->ul_Flags &= ~(GEO_Cul_SOF_NonMagnifold);

	for(l_Index = 0; l_Index < (LONG) pst_SO->ul_NbEdges; l_Index++)
	{
		pst_Edge = pst_SO->dst_Edge + l_Index;

		if(pst_Edge->ul_Triangle[2] != (ULONG) - 1)
		{
			pst_SO->ul_Flags |= GEO_Cul_SOF_NonMagnifold;
		}

		if(pst_Edge->ul_Point[0] < ul_CurVertexIndex)
		{
			continue;
		}

		ul_CurVertexIndex = pst_Edge->ul_Point[0];

		if(pst_Edge->ul_Point[0] == pst_Edge->ul_Point[1])
		{
			continue;
		}

		if(pst_Edge->ul_Triangle[0] == (ULONG) - 1)
		{
			continue;
		}

		pst_Element = _pst_Obj->dst_Element + ((pst_Edge->ul_Triangle[0] >> 16) & 0xFFF);
		pst_Triangle = pst_Element->dst_Triangle + (pst_Edge->ul_Triangle[0] & 0xFFFF);
		for(l_Index2 = 0; l_Index2 < 3; l_Index2++)
			if(pst_Edge->ul_Point[0] == pst_Triangle->auw_Index[l_Index2]) break;
		if(l_Index2 == 3)
		{
			continue;
		}

		for(l_Index2 = 0; l_Index2 < 3; l_Index2++)
			if(pst_Edge->ul_Point[1] == pst_Triangle->auw_Index[l_Index2]) break;
		if(l_Index2 == 3)
		{
			continue;
		}

		if(pst_Edge->ul_Triangle[1] == (ULONG) - 1) continue;
		pst_Element = _pst_Obj->dst_Element + ((pst_Edge->ul_Triangle[1] >> 16) & 0xFFF);
		pst_Triangle = pst_Element->dst_Triangle + (pst_Edge->ul_Triangle[1] & 0xFFFF);
		for(l_Index2 = 0; l_Index2 < 3; l_Index2++)
			if(pst_Edge->ul_Point[0] == pst_Triangle->auw_Index[l_Index2]) break;
		if(l_Index2 == 3)
		{
			continue;
		}

		for(l_Index2 = 0; l_Index2 < 3; l_Index2++)
			if(pst_Edge->ul_Point[1] == pst_Triangle->auw_Index[l_Index2]) break;
		if(l_Index2 == 3)
		{
			continue;
		}
	}
	{
		/*~~~~~~~~~~~~~~~~~~~*/
		GEO_tdst_SubObject	SO;
		/*~~~~~~~~~~~~~~~~~~~*/

		L_memcpy(&SO, pst_SO, sizeof(GEO_tdst_SubObject));

		if((ul_OldF != pst_SO->ul_Flags) && (pst_SO->pfn_Tool_Update))
			pst_SO->pfn_Tool_Update(pst_SO->ul_Tool_Param, _pst_Obj);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_ETurn
(
	GEO_tdst_Object					*_pst_Obj,
	GEO_tdst_SubObject_Edge			*_pst_Edge,
	GEO_tdst_SubObject_ETurnData	*_pst_Data
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_IndexedTriangle	*pst_Tri1, *pst_Tri2;
	int							save1, save2, new1, new2, other1, other2, temp;
	int							uv1[2], uv2[2];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* test data */
	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	if((_pst_Edge->ul_Triangle[0] == (ULONG) - 1) || (_pst_Edge->ul_Triangle[1] == (ULONG) - 1)) return;

	/* set data for undo */
	if(_pst_Data)
	{
		_pst_Data->aul_Old[0] = _pst_Edge->ul_Point[0];
		_pst_Data->aul_Old[1] = _pst_Edge->ul_Point[1];
	}

	pst_Tri1 = GEO_SubObject_GetTriangleFromEdgeIndex(_pst_Obj, _pst_Edge->ul_Triangle[0]);
	pst_Tri2 = GEO_SubObject_GetTriangleFromEdgeIndex(_pst_Obj, _pst_Edge->ul_Triangle[1]);

	/* find vertex to change */
	for(new1 = 0; new1 < 3; new1++)
	{
		if
		(
			(pst_Tri1->auw_Index[new1] != _pst_Edge->ul_Point[0])
		&&	(pst_Tri1->auw_Index[new1] != _pst_Edge->ul_Point[1])
		) break;
	}

	if(new1 == 3) return;

	for(new2 = 0; new2 < 3; new2++)
	{
		if
		(
			(pst_Tri2->auw_Index[new2] != _pst_Edge->ul_Point[0])
		&&	(pst_Tri2->auw_Index[new2] != _pst_Edge->ul_Point[1])
		) break;
	}

	if(new2 == 3) return;

	if(_pst_Data)
	{
		save1 = (new1 + 1) % 3;
		other1 = (new1 + 2) % 3;
		save2 = (new2 + 1) % 3;
		other2 = (new2 + 2) % 3;
		
	}
	else
	{
		save1 = (new1 + 2) % 3;
		save2 = (new2 + 2) % 3;
		other1 = (new1 + 1) % 3;
		other2 = (new2 + 1) % 3;
	}
	if (pst_Tri1->auw_Index[save1] == pst_Tri2->auw_Index[save2] )
	{
		temp = save2;
		save2 = other2;
		other2 = temp;
	}

	if(pst_Tri1->auw_Index[save1] == _pst_Edge->ul_Point[0])
	{
		uv1[0] = pst_Tri1->auw_UV[save1];
		uv1[1] = pst_Tri1->auw_UV[other1];
	}
	else
	{
		uv1[0] = pst_Tri1->auw_UV[other1];
		uv1[1] = pst_Tri1->auw_UV[save1];
	}

	if(pst_Tri2->auw_Index[save2] == _pst_Edge->ul_Point[0])
	{
		uv2[0] = pst_Tri2->auw_UV[save2];
		uv2[1] = pst_Tri2->auw_UV[other2];
	}
	else
	{
		uv2[0] = pst_Tri2->auw_UV[other2];
		uv2[1] = pst_Tri2->auw_UV[save2];
	}

	/* change vertex */
	pst_Tri1->auw_Index[save1] = pst_Tri2->auw_Index[new2];
	pst_Tri2->auw_Index[save2] = pst_Tri1->auw_Index[new1];
	if((uv1[0] == uv2[0]) && (uv1[1] == uv2[1]))
	{
		pst_Tri1->auw_UV[save1] = pst_Tri2->auw_UV[new2];
		pst_Tri2->auw_UV[save2] = pst_Tri1->auw_UV[new1];
	}

	/* set data for undo */
	if(_pst_Data)
	{
		_pst_Data->aul_New[0] = pst_Tri1->auw_Index[save1];
		_pst_Data->aul_New[1] = pst_Tri2->auw_Index[save2];
	}

	/* Change edge */
	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_ETurnSel(GEO_tdst_Object *_pst_Obj, GEO_tdst_SubObject_ETurnData *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject_Edge *pst_Edge, *pst_Last;
	CHAR					*pc_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
	pst_Last = pst_Edge + _pst_Obj->pst_SubObject->ul_NbEdges;
	pc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
	for(; pst_Edge < pst_Last; pst_Edge++, pc_Sel++)
		if(*pc_Sel & 1)
		{
			GEO_SubObject_ETurn(_pst_Obj, pst_Edge, _pst_Data);
			return 1;
		}

	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_ECut
(
	GEO_tdst_Object				*_pst_Obj,
	GEO_tdst_SubObject_Edge		*_pst_Edge,
	float						_f_Cut,
	GEO_tdst_SubObject_ECutData *_pst_Data
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Triangle, st_NewTri;
	int									i_Triangle, i, save, newpoint, newuv, newsave;
	MATH_tdst_Vector					v, v2;
	GEO_tdst_UV							*pst_UV1, *pst_UV2, st_UV;
	WOR_tdst_Update_RLI					st_UpdateRLIData;
	ULONG								ul_C0, ul_C1;
	USHORT								uw_UV[2];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	uw_UV[0] = uw_UV[1] = (USHORT) - 1;
	newuv = 0;

    if (_pst_Data->i_Divide)
        _f_Cut = 0.5f;

	/* Create new point */
	MATH_ScaleVector(&v, VCast(_pst_Obj->dst_Point + _pst_Edge->ul_Point[0]), _f_Cut);
	MATH_ScaleVector(&v2, VCast(_pst_Obj->dst_Point + _pst_Edge->ul_Point[1]), (1 - _f_Cut));
	MATH_AddEqualVector(&v, &v2);
	newpoint = _pst_Obj->l_NbPoints;
	GEO_AddPoint(_pst_Obj, v.x, v.y, v.z, 1.0f, 0.0f, 0.0f);
    _pst_Obj->pst_SubObject->dc_VSel[ newpoint ] = 1;

	if(_pst_Data)
	{
		_pst_Data->l_AddVertex = _pst_Obj->l_NbPoints - 1;
		_pst_Data->f_Cut = _f_Cut;
		_pst_Data->al_EdgeVertex[0] = _pst_Edge->ul_Point[0];
		_pst_Data->al_EdgeVertex[1] = _pst_Edge->ul_Point[1];
		_pst_Data->al_OppVertex[0] = -1;
		_pst_Data->al_OppVertex[1] = -1;
		_pst_Data->al_AddUV[0] = -1;
		_pst_Data->al_AddUV[1] = -1;
	}

	/* Update eventually RLI */
	if(_pst_Obj->dul_PointColors)
	{
		ul_C0 = _pst_Obj->dul_PointColors[1 + _pst_Edge->ul_Point[0]];
		ul_C1 = _pst_Obj->dul_PointColors[1 + _pst_Edge->ul_Point[1]];
		_pst_Obj->dul_PointColors[_pst_Obj->l_NbPoints] = LIGHT_ul_Interpol2Colors(ul_C0, ul_C1, _f_Cut);
	}

	/* update RLI of GameObject that used this object */
	st_UpdateRLIData.p_Geo = _pst_Obj;
	st_UpdateRLIData.l_Op = WOR_Update_RLI_Add;
	st_UpdateRLIData.l_Ind0 = _pst_Edge->ul_Point[0];
	st_UpdateRLIData.l_Ind1 = _pst_Edge->ul_Point[1];
	st_UpdateRLIData.f_Blend = _f_Cut;
	WOR_Universe_Update_RLI(&st_UpdateRLIData);

	/* update eventually skin */
	GEO_SKN_UpdateAfterVertexEdition(_pst_Obj, &st_UpdateRLIData);

	for(i_Triangle = 0; i_Triangle < 2; i_Triangle++)
	{
		if(_pst_Edge->ul_Triangle[i_Triangle] == (ULONG) - 1) continue;

		pst_Element = _pst_Obj->dst_Element + ((_pst_Edge->ul_Triangle[i_Triangle] >> 16) & 0xFFF);
		pst_Triangle = pst_Element->dst_Triangle + (_pst_Edge->ul_Triangle[i_Triangle] & 0xFFFF);

		/* Compute new UV */
		for(i = 0; i < 3; i++)
		{
			if(pst_Triangle->auw_Index[i] == _pst_Edge->ul_Point[0])
				newsave = i;
			else if(pst_Triangle->auw_Index[i] == _pst_Edge->ul_Point[1])
				save = i;
			else if(_pst_Data) _pst_Data->al_OppVertex[i_Triangle] = pst_Triangle->auw_Index[i];
		}

		st_NewTri = *pst_Triangle;

		if(_pst_Obj->dst_UV)
		{
			if((pst_Triangle->auw_UV[newsave] != uw_UV[0]) || (pst_Triangle->auw_UV[save] != uw_UV[1]))
			{
				uw_UV[0] = pst_Triangle->auw_UV[newsave];
				uw_UV[1] = pst_Triangle->auw_UV[save];
				pst_UV1 = _pst_Obj->dst_UV + uw_UV[0];
				pst_UV2 = _pst_Obj->dst_UV + uw_UV[1];
				st_UV.fU = pst_UV1->fU * _f_Cut + pst_UV2->fU * (1.0f - _f_Cut);
				st_UV.fV = pst_UV1->fV * _f_Cut + pst_UV2->fV * (1.0f - _f_Cut);
				newuv = _pst_Obj->l_NbUVs;
				GEO_AddUV(_pst_Obj, st_UV.fU, st_UV.fV);
				if(_pst_Data) _pst_Data->al_AddUV[i_Triangle] = newuv;
			}

			pst_Triangle->auw_UV[save] = newuv;
			st_NewTri.auw_UV[newsave] = newuv;
		}

		st_NewTri.auw_Index[newsave] = newpoint;
		pst_Triangle->auw_Index[save] = newpoint;
		GEO_AddTriangle(pst_Element, &st_NewTri);
	}

	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_BuildFaceData(_pst_Obj);
    GEO_SubObject_VUpdateESel(_pst_Obj);
    GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
	GEO_SubObject_AllocBuffers(_pst_Obj);
    GEO_SubObject_HideUpdate( _pst_Obj, TRUE );
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_ECutSel
(
	GDI_tdst_DisplayData		*_pst_DD,
	GEO_tdst_Object				*_pst_Obj,
	MATH_tdst_Vector			*_pst_2D,
	GEO_tdst_SubObject_ECutData *_pst_Data
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject_Edge *pst_Edge, *pst_Last;
	MATH_tdst_Vector		P0, P, *P1, *P2;
	char					*pc_Sel;
	float					f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
	pst_Last = pst_Edge + _pst_Obj->pst_SubObject->ul_NbEdges;
	pc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
	for(; pst_Edge < pst_Last; pst_Edge++, pc_Sel++)
	{
		if(*pc_Sel & 1)
		{
			P1 = _pst_Obj->pst_SubObject->dst_3D + pst_Edge->ul_Point[0];
			P2 = _pst_Obj->pst_SubObject->dst_3D + pst_Edge->ul_Point[1];

			_pst_2D->y = 2 * _pst_DD->st_Camera.f_CenterY - _pst_2D->y;
			_pst_2D->z = _pst_DD->st_Camera.f_NearPlane;
			CAM_2Dto3DCamera(&_pst_DD->st_Camera, &P, _pst_2D);

			if(_pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
				MATH_CopyVector(&P0, &MATH_gst_NulVector);
			else
			{
				_pst_2D->z = 0;
				CAM_2Dto3DCamera(&_pst_DD->st_Camera, &P0, _pst_2D);
			}

			SOFT_l_NearestPointOf2Lines(&P0, &P, P1, P2, NULL, NULL, NULL, &f, NULL);

			return GEO_i_SubObject_ECut(_pst_Obj, pst_Edge, 1 - f, _pst_Data);
		}
	}

	return 0;
}

typedef struct	GEOSO_tdst_ECutterFace_
{
	int		mark;
	ULONG	edge[2];
} GEOSO_tdst_ECutterFace;

typedef struct	GEOSO_tdst_ECutterEdge_
{
	int		cut;
	float	length, ratio;
	int		pt;
	int		uv, uv0, uv1;
} GEOSO_tdst_ECutterEdge;

#define M_SwapEdge() \
	{ \
		imacro = pst_FData->edge[0]; \
		pst_FData->edge[0] = pst_FData->edge[1]; \
		pst_FData->edge[1] = imacro; \
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_SubObject_i_TGetOppositeE(GEO_tdst_IndexedTriangle *T, GEO_tdst_SubObject_Edge *E)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < 3; i++)
	{
		if((T->auw_Index[i] != E->ul_Point[0]) && (T->auw_Index[i] != E->ul_Point[1])) return i;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_SubObject_i_TGetCommonE(GEO_tdst_IndexedTriangle *T, GEO_tdst_SubObject_Edge *E0, GEO_tdst_SubObject_Edge *E1)
{
	/*~~~~~~*/
	int i, pt;
	/*~~~~~~*/

	if((E0->ul_Point[0] == E1->ul_Point[0]) || (E0->ul_Point[0] == E1->ul_Point[1]))
		pt = E0->ul_Point[0];
	else
		pt = E0->ul_Point[1];

	for(i = 0; i < 3; i++)
	{
		if(T->auw_Index[i] == pt) return i;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_ECutterSetUV
(
	GEO_tdst_Object				*_pst_Obj,
	GEO_tdst_IndexedTriangle	*pst_Tri,
	int							k,
	int							l,
	GEOSO_tdst_ECutterEdge		*pst_EData,
	GEO_tdst_SubObject_Edge		*pst_Edge
)
{
	/*~~~~~~~~~~~*/
	int		adduv;
	float	fu, fv;
	/*~~~~~~~~~~~*/

	if(pst_EData->uv == -1)
		adduv = 1;
	else
	{
		adduv = 0;
		if(pst_Edge->ul_Point[0] == pst_Tri->auw_Index[k])
		{
			if((pst_Tri->auw_UV[k] != pst_EData->uv0) || (pst_Tri->auw_UV[l] != pst_EData->uv1)) adduv = 1;
		}
		else
		{
			if((pst_Tri->auw_UV[l] != pst_EData->uv0) || (pst_Tri->auw_UV[k] != pst_EData->uv1)) adduv = 1;
		}
	}

	if(adduv && _pst_Obj->dst_UV)
	{
		if(pst_Edge->ul_Point[0] == pst_Tri->auw_Index[k])
		{
			pst_EData->uv0 = pst_Tri->auw_UV[k];
			pst_EData->uv1 = pst_Tri->auw_UV[l];
		}
		else
		{
			pst_EData->uv0 = pst_Tri->auw_UV[l];
			pst_EData->uv1 = pst_Tri->auw_UV[k];
		}

		pst_EData->uv = _pst_Obj->l_NbUVs;
		fu = (_pst_Obj->dst_UV[pst_EData->uv0].fU * pst_EData->ratio) + (_pst_Obj->dst_UV[pst_EData->uv1].fU * (1 - pst_EData->ratio));
		fv = (_pst_Obj->dst_UV[pst_EData->uv0].fV * pst_EData->ratio) + (_pst_Obj->dst_UV[pst_EData->uv1].fV * (1 - pst_EData->ratio));
		GEO_AddUV(_pst_Obj, fu, fv);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_ECutter
(
	GDI_tdst_DisplayData	*_pst_DD,
	GEO_tdst_Object			*_pst_Obj,
	MATH_tdst_Vector		*C,
	MATH_tdst_Vector		*D,
	MATH_tdst_Vector		*N
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject_Edge		*pst_Edge, *pst_Last, *pst_Edge2;
	GEO_tdst_SubObject_Face		*pst_Face, *pst_Face2;
	MATH_tdst_Vector			V0, V1, E, EN, *P0, *P1;
	float						t0, t1;
	char						*pc_Sel;
	USHORT						*duw_Face, uw_FaceNb, uw_FaceStart, uw_PtNb;
	USHORT						*duw_Edge, uw_EdgeNb, uw_EdgeStart, uw_TriNb, uw_TriModifNb;
	int							ret, i, j, k, l, face, edge, imacro;
	int							j2, k2;
	GEOSO_tdst_ECutterFace		*dst_FData, *pst_FData, *pst_FData2;
	GEOSO_tdst_ECutterEdge		*dst_EData, *pst_EData, *pst_EData1, *pst_EData2;
	GEO_tdst_IndexedTriangle	*dst_Tri, *pst_TriModif, *pst_Tri, *pst_Tri2;
	WOR_tdst_Update_RLI			st_UpdateRLIData;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	ret = 0;
	GEO_SubObject_UnselAll(_pst_Obj);

	dst_EData = (GEOSO_tdst_ECutterEdge *) L_malloc(_pst_Obj->pst_SubObject->ul_NbEdges * sizeof(GEOSO_tdst_ECutterEdge));
	L_memset(dst_EData, 0, _pst_Obj->pst_SubObject->ul_NbEdges * sizeof(GEOSO_tdst_ECutterEdge));
	duw_Edge = (USHORT *) L_malloc(_pst_Obj->pst_SubObject->ul_NbEdges * sizeof(USHORT));
	uw_EdgeNb = 0;
	uw_EdgeStart = 0;
	edge = 0;

	dst_FData = (GEOSO_tdst_ECutterFace *) L_malloc(_pst_Obj->pst_SubObject->ul_NbFaces * sizeof(GEOSO_tdst_ECutterFace));
	L_memset(dst_FData, 0, _pst_Obj->pst_SubObject->ul_NbFaces * sizeof(GEOSO_tdst_ECutterFace));
	duw_Face = (USHORT *) L_malloc(2 * (_pst_Obj->pst_SubObject->ul_NbFaces + 1));
	duw_Face[0] = (USHORT) - 1;
	uw_FaceNb = 0;
	uw_FaceStart = 0;
	dst_Tri = NULL;

	/* Cherche les edge qui sont coupés */
	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
	pst_Last = pst_Edge + _pst_Obj->pst_SubObject->ul_NbEdges;
	pc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
	for(i = 0; pst_Edge < pst_Last; pst_Edge++, pc_Sel++, i++)
	{
        if (*pc_Sel & 0x80) continue;

		P0 = _pst_Obj->pst_SubObject->dst_3D + pst_Edge->ul_Point[0];
		P1 = _pst_Obj->pst_SubObject->dst_3D + pst_Edge->ul_Point[1];

		MATH_SubVector(&V0, P0, C);
		t0 = MATH_f_DotProduct(&V0, N);
		MATH_SubVector(&V1, P1, C);
		t1 = MATH_f_DotProduct(&V1, N);

		if((t0 * t1) < 0)
		{
			MATH_SubVector(&E, P1, P0);
			dst_EData[i].ratio = 1 + t0 / MATH_f_DotProduct(&E, N);
			dst_EData[i].length = MATH_f_SqrVector(&E);
			dst_EData[i].uv = -1;

			if(_pst_DD->st_Camera.ul_Flags & CAM_Cul_Flags_Perspective)
				MATH_CrossProduct(&EN, &E, P0);
			else
				MATH_CrossProduct(&EN, &E, &MATH_gst_BaseVectorK);

			MATH_SubVector(&V1, P0, D);

			t0 = MATH_f_DotProduct(&V0, &EN);
			t1 = MATH_f_DotProduct(&V1, &EN);

			if((t0 * t1) < 0)
			{
				/* edge coupé : selectionne le edge, mémorise les faces touchées */
				*pc_Sel = 1;
				pst_FData = dst_FData + pst_Edge->uw_Index[0];
				pst_FData->edge[pst_FData->mark] = i;
				pst_FData->mark++;

				if(pst_Edge->uw_Index[1] != (USHORT) - 1)
				{
					pst_FData = dst_FData + pst_Edge->uw_Index[1];
					pst_FData->edge[pst_FData->mark] = i;
					pst_FData->mark++;
				}
				else
					/* l'edge coupé n'appartient qu'à un triangle : il sera le premier pour le tri */
					duw_Face[0] = pst_Edge->uw_Index[0];
			}
		}
	}

ECutter_onceagain:
	if(ret == 1)
	{
		duw_Face[uw_FaceNb] = (USHORT) - 1;

		/* cherche une face de bord : mark = 2 et un des edge n'a qu'une face */
		for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++)
		{
			if(dst_FData[i].mark == 2)
			{
				pst_EData = dst_EData + dst_FData[i].edge[0];

				pst_Edge = _pst_Obj->pst_SubObject->dst_Edge + dst_FData[i].edge[0];
				if(pst_Edge->uw_Index[1] == (USHORT) - 1)
				{
					duw_Face[uw_FaceNb] = (USHORT) i;
					break;
				}

				pst_Edge = _pst_Obj->pst_SubObject->dst_Edge + dst_FData[i].edge[1];
				if(pst_Edge->uw_Index[1] == (USHORT) - 1)
				{
					duw_Face[uw_FaceNb] = (USHORT) i;
					break;
				}
			}
		}
	}

	/* si pas de face de bord, on cherche une face dont un seul edge est touché */
	if(duw_Face[uw_FaceNb] == (USHORT) - 1)
	{
		for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++)
		{
			if(dst_FData[i].mark == 1)
			{
				duw_Face[uw_FaceNb] = i;
				break;
			}
		}

		/*
		 * on a pas trouvé de face par laquelle commencer le calcul : ca boucle, on prend
		 * une dont le premier edge est le plus court
		 */
		if(duw_Face[uw_FaceNb] == (USHORT) - 1)
		{
			t1 = Cf_Infinit;
			for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++)
			{
				if(dst_FData[i].mark == 2)
				{
					pst_EData = dst_EData + dst_FData[i].edge[0];
					if(pst_EData->length < t1)
					{
						duw_Face[uw_FaceNb] = i;
						t1 = pst_EData->length;
					}
				}
			}
		}

		if(duw_Face[uw_FaceNb] == (USHORT) - 1)
			if(ret == 1)
				goto ECutter_nomore;
			else
				goto ECutter_end;
	}

	/* on a des faces à cutter, le retour sera 1 */
	ret = 1;

	/* on trie les face touchées en partant de duw_Face[ 0 ] */
	pst_FData = dst_FData + duw_Face[uw_FaceNb++];
	if(pst_FData->mark == 1)
	{
		pst_FData->edge[1] = pst_FData->edge[0];
		pst_FData->edge[0] = -1;
	}
	else if(pst_FData->mark == 2)
	{
		if((_pst_Obj->pst_SubObject->dst_Edge + pst_FData->edge[1])->uw_Index[1] == (USHORT) - 1)
			M_SwapEdge() pst_FData->mark--;
	}
	else
		goto ECutter_end;

	pst_FData = dst_FData + duw_Face[uw_FaceNb - 1];
	while(1)
	{
		if(pst_FData->mark != 1) goto ECutter_end;
		pst_FData->mark--;

		edge = pst_FData->edge[1];
		pst_Edge = _pst_Obj->pst_SubObject->dst_Edge + edge;

		if(pst_Edge->uw_Index[0] != duw_Face[uw_FaceNb - 1])
			face = pst_Edge->uw_Index[0];
		else
			face = pst_Edge->uw_Index[1];
		if(face == (USHORT) - 1) break;

		pst_FData = dst_FData + face;
		if(pst_FData->mark == 0) break;

		duw_Face[uw_FaceNb++] = face;

		if((int) pst_FData->edge[0] != edge) M_SwapEdge();
		if(--pst_FData->mark == 0)
		{
			pst_FData->edge[1] = -1;
			break;
		}
	}

	/* construit la liste triée des edges qui sont touchées */
	pst_FData = dst_FData + duw_Face[uw_FaceStart];
	if(pst_FData->edge[0] != -1) duw_Edge[uw_EdgeNb++] = (USHORT) pst_FData->edge[0];
	for(i = uw_FaceStart; i < uw_FaceNb; i++)
	{
		pst_FData = dst_FData + duw_Face[i];
		if(pst_FData->edge[1] != -1) duw_Edge[uw_EdgeNb++] = (USHORT) pst_FData->edge[1];
	}

	/* on doit couper le premier */
	edge = uw_EdgeStart;
	pst_EData = dst_EData + duw_Edge[edge];
	pst_EData->cut = 1;

	while(1)
	{
		if((uw_EdgeNb - edge) == 1) break;
		if((uw_EdgeNb - edge) == 2)
		{
			dst_EData[duw_Edge[edge + 1]].cut = 1;
			break;
		}

		pst_Edge = _pst_Obj->pst_SubObject->dst_Edge + duw_Edge[edge];
		pst_Last = _pst_Obj->pst_SubObject->dst_Edge + duw_Edge[edge + 2];

		if
		(
			(pst_Edge->ul_Point[0] == pst_Last->ul_Point[0])
		||	(pst_Edge->ul_Point[0] == pst_Last->ul_Point[1])
		||	(pst_Edge->ul_Point[1] == pst_Last->ul_Point[0])
		||	(pst_Edge->ul_Point[1] == pst_Last->ul_Point[1])
		)
		{
			dst_EData[duw_Edge[edge + 1]].cut = 1;
			edge++;
			continue;
		}

		pst_Edge = _pst_Obj->pst_SubObject->dst_Edge + duw_Edge[edge + 1];

		pst_Face = _pst_Obj->pst_SubObject->dst_Faces + pst_Edge->uw_Index[0];
		pst_Tri = _pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
		j = pst_Face->uw_Element;
		for(i = 0; i < 3; i++)
		{
			if(pst_Tri->auw_Index[i] == pst_Edge->ul_Point[0])
				pst_EData->uv0 = pst_Tri->auw_UV[i];
			else if(pst_Tri->auw_Index[i] == pst_Edge->ul_Point[1]) pst_EData->uv1 = pst_Tri->auw_UV[i];
		}

		pst_Face = _pst_Obj->pst_SubObject->dst_Faces + pst_Edge->uw_Index[1];
		pst_Tri = _pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
		j -= pst_Face->uw_Element;
		for(i = 0; i < 3; i++)
		{
			if(pst_Tri->auw_Index[i] == pst_Edge->ul_Point[0])
				pst_EData->uv0 -= pst_Tri->auw_UV[i];
			else if(pst_Tri->auw_Index[i] == pst_Edge->ul_Point[1]) pst_EData->uv1 -= pst_Tri->auw_UV[i];
		}

		if(j || (pst_EData->uv0) || (pst_EData->uv1))
		{
			dst_EData[duw_Edge[edge + 1]].cut = 1;
			edge++;
			continue;
		}

		if((uw_EdgeNb - edge) == 3)
		{
			dst_EData[duw_Edge[edge + 1]].cut = 0;
			dst_EData[duw_Edge[edge + 2]].cut = 1;
			break;
		}

		pst_Last = _pst_Obj->pst_SubObject->dst_Edge + duw_Edge[edge + 3];

		if
		(
			(pst_Edge->ul_Point[0] == pst_Last->ul_Point[0])
		||	(pst_Edge->ul_Point[0] == pst_Last->ul_Point[1])
		||	(pst_Edge->ul_Point[1] == pst_Last->ul_Point[0])
		||	(pst_Edge->ul_Point[1] == pst_Last->ul_Point[1])
		)
		{
			dst_EData[duw_Edge[edge + 1]].cut = 0;
			dst_EData[duw_Edge[edge + 2]].cut = 1;
			edge += 2;
			continue;
		}

		pst_Edge = _pst_Obj->pst_SubObject->dst_Edge + duw_Edge[edge + 2];
		pst_Face = _pst_Obj->pst_SubObject->dst_Faces + pst_Edge->uw_Index[0];
		pst_Tri = _pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
		j = pst_Face->uw_Element;
		for(i = 0; i < 3; i++)
		{
			if(pst_Tri->auw_Index[i] == pst_Edge->ul_Point[0])
				pst_EData->uv0 = pst_Tri->auw_UV[i];
			else if(pst_Tri->auw_Index[i] == pst_Edge->ul_Point[1]) pst_EData->uv1 = pst_Tri->auw_UV[i];
		}

		pst_Face = _pst_Obj->pst_SubObject->dst_Faces + pst_Edge->uw_Index[1];
		pst_Tri = _pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
		j -= pst_Face->uw_Element;
		for(i = 0; i < 3; i++)
		{
			if(pst_Tri->auw_Index[i] == pst_Edge->ul_Point[0])
				pst_EData->uv0 -= pst_Tri->auw_UV[i];
			else if(pst_Tri->auw_Index[i] == pst_Edge->ul_Point[1]) pst_EData->uv1 -= pst_Tri->auw_UV[i];
		}

		if(j || (pst_EData->uv0) || (pst_EData->uv1))
		{
			dst_EData[duw_Edge[edge + 1]].cut = 0;
			dst_EData[duw_Edge[edge + 2]].cut = 1;
			edge += 2;
			continue;
		}

		t0 = dst_EData[duw_Edge[edge]].length + dst_EData[duw_Edge[edge + 2]].length;
		t1 = dst_EData[duw_Edge[edge + 1]].length + dst_EData[duw_Edge[edge + 3]].length;

		if(t0 < t1)
		{
			dst_EData[duw_Edge[edge + 1]].cut = 0;
			dst_EData[duw_Edge[edge + 2]].cut = 1;
			edge += 2;
		}
		else
		{
			dst_EData[duw_Edge[edge + 1]].cut = 1;
			dst_EData[duw_Edge[edge + 2]].cut = 0;
			dst_EData[duw_Edge[edge + 3]].cut = 1;
			edge += 3;
		}
	}

	/*
	 * si on a une boucle on a dernier edge = premier edge : il faut virer le dernier
	 * edge pour pas le couper deux fois
	 */
	if(duw_Edge[uw_EdgeStart] == duw_Edge[uw_EdgeNb - 1]) uw_EdgeNb--;

	/* on recommence */
	uw_FaceStart = uw_FaceNb;
	uw_EdgeStart = uw_EdgeNb;
	goto ECutter_onceagain;

ECutter_nomore:

	/* on va couper les edges : rajout des points */
	uw_PtNb = 0;
	for(i = 0; i < uw_EdgeNb; i++)
	{
		if(!(dst_EData[duw_Edge[i]].cut)) continue;
		dst_EData[duw_Edge[i]].pt = _pst_Obj->l_NbPoints + uw_PtNb++;
	}

	/* set data to update rli */
	st_UpdateRLIData.p_Geo = _pst_Obj;
	st_UpdateRLIData.l_Op = WOR_Update_RLI_AddSomeCenter;
	st_UpdateRLIData.l_NbAdded = uw_PtNb;
	st_UpdateRLIData.l_OldNbPoints = _pst_Obj->l_NbPoints;
	st_UpdateRLIData.l_Ind0 = (LONG) L_malloc(4 * uw_PtNb);
	st_UpdateRLIData.l_Ind1 = (LONG) L_malloc(4 * uw_PtNb);
	st_UpdateRLIData.l_Ind2 = (LONG) L_malloc(4 * uw_PtNb);

	GEO_AddSeveralPoint(_pst_Obj, uw_PtNb);

	for(i = 0; i < uw_EdgeNb; i++)
	{
		if(!(dst_EData[duw_Edge[i]].cut)) continue;

		pst_Edge = _pst_Obj->pst_SubObject->dst_Edge + duw_Edge[i];
		P0 = _pst_Obj->dst_Point + pst_Edge->ul_Point[0];
		P1 = _pst_Obj->dst_Point + pst_Edge->ul_Point[1];

		MATH_ScaleVector(&V0, P0, dst_EData[duw_Edge[i]].ratio);
		MATH_AddScaleVector(_pst_Obj->dst_Point + dst_EData[duw_Edge[i]].pt, &V0, P1, 1 - dst_EData[duw_Edge[i]].ratio);

		j = dst_EData[duw_Edge[i]].pt - st_UpdateRLIData.l_OldNbPoints;
		((LONG *) st_UpdateRLIData.l_Ind0)[j] = pst_Edge->ul_Point[0];
		((LONG *) st_UpdateRLIData.l_Ind1)[j] = pst_Edge->ul_Point[1];
		((float *) st_UpdateRLIData.l_Ind2)[j] = 1 - dst_EData[duw_Edge[i]].ratio;
	}

	/* update eventually skin */
	GEO_SKN_UpdateAfterVertexEdition(_pst_Obj, &st_UpdateRLIData);

	/* update eventually RLI */
	WOR_Universe_Update_RLI(&st_UpdateRLIData);
	L_free((LONG *) st_UpdateRLIData.l_Ind0);
	L_free((LONG *) st_UpdateRLIData.l_Ind1);
	L_free((float *) st_UpdateRLIData.l_Ind2);

	/* finalement on va peut être couper les faces */
	dst_Tri = (GEO_tdst_IndexedTriangle *) L_malloc(3 * uw_FaceNb * sizeof(GEO_tdst_IndexedTriangle));
	pst_TriModif = dst_Tri + (2 * uw_FaceNb);
	uw_TriNb = 0;
	uw_TriModifNb = 0;
	for(i = 0; i < uw_FaceNb; i++)
	{
		pst_FData = dst_FData + duw_Face[i];
		pst_Face = _pst_Obj->pst_SubObject->dst_Faces + duw_Face[i];
		pst_Tri = _pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;

		/* un seul edge du triangle est touché */
		if((pst_FData->edge[0] == -1) || (pst_FData->edge[1] == -1))
		{
			edge = (pst_FData->edge[0] == -1) ? 1 : 0;
			pst_Edge = _pst_Obj->pst_SubObject->dst_Edge + pst_FData->edge[edge];
			j = GEO_SubObject_i_TGetOppositeE(pst_Tri, pst_Edge);
			pst_EData = dst_EData + pst_FData->edge[edge];

			GEO_SubObject_ECutterSetUV(_pst_Obj, pst_Tri, (j + 1) % 3, (j + 2) % 3, pst_EData, pst_Edge);

			/* modifie la face courante */
			L_memcpy(&pst_TriModif[uw_TriModifNb], pst_Tri, sizeof(GEO_tdst_IndexedTriangle));
			pst_TriModif[uw_TriModifNb].auw_Index[(j + 2) % 3] = pst_EData->pt;
			pst_TriModif[uw_TriModifNb].auw_UV[(j + 2) % 3] = pst_EData->uv;
			pst_TriModif[uw_TriModifNb].ul_SmoothingGroup = (ULONG) pst_Face;
			uw_TriModifNb++;

			/* rajoute une face */
			L_memcpy(&dst_Tri[uw_TriNb], pst_Tri, sizeof(GEO_tdst_IndexedTriangle));
			dst_Tri[uw_TriNb].auw_Index[(j + 1) % 3] = pst_EData->pt;
			dst_Tri[uw_TriNb].auw_UV[(j + 1) % 3] = pst_EData->uv;
			dst_Tri[uw_TriNb].ul_SmoothingGroup = (ULONG) pst_Face->uw_Element;
			uw_TriNb++;
			continue;
		}

		/* 2 edges touchés et coupés */
		if((dst_EData[pst_FData->edge[0]].cut) && (dst_EData[pst_FData->edge[1]].cut))
		{
			pst_EData = dst_EData + pst_FData->edge[0];
			pst_EData1 = dst_EData + pst_FData->edge[1];
			pst_Edge = _pst_Obj->pst_SubObject->dst_Edge + pst_FData->edge[0];
			pst_Last = _pst_Obj->pst_SubObject->dst_Edge + pst_FData->edge[1];
			j = GEO_SubObject_i_TGetCommonE(pst_Tri, pst_Edge, pst_Last);
			k = pst_Tri->auw_Index[(j + 1) % 3];
			if((k == (int) pst_Edge->ul_Point[0]) || (k == (int) pst_Edge->ul_Point[1]))
			{
				k = (j + 1) % 3;
				l = (j + 2) % 3;
			}
			else
			{
				k = (j + 2) % 3;
				l = (j + 1) % 3;
			}

			GEO_SubObject_ECutterSetUV(_pst_Obj, pst_Tri, j, k, pst_EData, pst_Edge);
			GEO_SubObject_ECutterSetUV(_pst_Obj, pst_Tri, l, j, pst_EData1, pst_Last);

			L_memcpy(&pst_TriModif[uw_TriModifNb], pst_Tri, sizeof(GEO_tdst_IndexedTriangle));
			pst_TriModif[uw_TriModifNb].auw_Index[k] = pst_EData->pt;
			pst_TriModif[uw_TriModifNb].auw_UV[k] = pst_EData->uv;
			pst_TriModif[uw_TriModifNb].auw_Index[l] = pst_EData1->pt;
			pst_TriModif[uw_TriModifNb].auw_UV[l] = pst_EData1->uv;
			pst_TriModif[uw_TriModifNb].ul_SmoothingGroup = (ULONG) pst_Face;
			uw_TriModifNb++;

			/* rajoute 2 faces */
			L_memcpy(&dst_Tri[uw_TriNb], pst_Tri, sizeof(GEO_tdst_IndexedTriangle));
			dst_Tri[uw_TriNb].auw_Index[j] = pst_EData->pt;
			dst_Tri[uw_TriNb].auw_UV[j] = pst_EData->uv;
			dst_Tri[uw_TriNb].auw_Index[l] = pst_EData1->pt;
			dst_Tri[uw_TriNb].auw_UV[l] = pst_EData1->uv;
			dst_Tri[uw_TriNb].ul_SmoothingGroup = (ULONG) pst_Face->uw_Element;
			uw_TriNb++;

			L_memcpy(&dst_Tri[uw_TriNb], pst_Tri, sizeof(GEO_tdst_IndexedTriangle));
			dst_Tri[uw_TriNb].auw_Index[j] = pst_EData1->pt;
			dst_Tri[uw_TriNb].auw_UV[j] = pst_EData1->uv;
			dst_Tri[uw_TriNb].ul_SmoothingGroup = (ULONG) pst_Face->uw_Element;
			uw_TriNb++;
			continue;
		}

		if(!(dst_EData[pst_FData->edge[0]].cut)) continue;

		pst_FData2 = dst_FData + duw_Face[i + 1];
		pst_Face2 = _pst_Obj->pst_SubObject->dst_Faces + duw_Face[i + 1];
		pst_Tri2 = _pst_Obj->dst_Element[pst_Face2->uw_Element].dst_Triangle + pst_Face2->uw_Index;

		pst_EData = dst_EData + pst_FData->edge[0];
		pst_EData1 = dst_EData + pst_FData->edge[1];
		pst_EData2 = dst_EData + pst_FData2->edge[1];

		pst_Edge = _pst_Obj->pst_SubObject->dst_Edge + pst_FData->edge[0];
		pst_Last = _pst_Obj->pst_SubObject->dst_Edge + pst_FData->edge[1];
		pst_Edge2 = _pst_Obj->pst_SubObject->dst_Edge + pst_FData2->edge[1];

		j = GEO_SubObject_i_TGetCommonE(pst_Tri, pst_Edge, pst_Last);
		k = pst_Tri->auw_Index[(j + 1) % 3];
		if((k != (int) pst_Edge->ul_Point[0]) && (k != (int) pst_Edge->ul_Point[1]))
			k = (j + 2) % 3;
		else
			k = (j + 1) % 3;

		j2 = GEO_SubObject_i_TGetCommonE(pst_Tri2, pst_Last, pst_Edge2);
		k2 = pst_Tri2->auw_Index[(j2 + 1) % 3];
		if((k2 != (int) pst_Edge2->ul_Point[0]) && (k2 != (int) pst_Edge2->ul_Point[1]))
			k2 = (j2 + 2) % 3;
		else
			k2 = (j2 + 1) % 3;

		GEO_SubObject_ECutterSetUV(_pst_Obj, pst_Tri, j, k, pst_EData, pst_Edge);
		GEO_SubObject_ECutterSetUV(_pst_Obj, pst_Tri2, j2, k2, pst_EData2, pst_Edge2);

		L_memcpy(&pst_TriModif[uw_TriModifNb], pst_Tri, sizeof(GEO_tdst_IndexedTriangle));
		pst_TriModif[uw_TriModifNb].auw_Index[j] = pst_EData->pt;
		pst_TriModif[uw_TriModifNb].auw_UV[j] = pst_EData->uv;
		pst_TriModif[uw_TriModifNb].ul_SmoothingGroup = (ULONG) pst_Face;
		uw_TriModifNb++;

		L_memcpy(&dst_Tri[uw_TriNb], pst_Tri, sizeof(GEO_tdst_IndexedTriangle));
		dst_Tri[uw_TriNb].auw_Index[j] = pst_EData2->pt;
		dst_Tri[uw_TriNb].auw_UV[j] = pst_EData2->uv;
		dst_Tri[uw_TriNb].auw_Index[k] = pst_EData->pt;
		dst_Tri[uw_TriNb].auw_UV[k] = pst_EData->uv;
		dst_Tri[uw_TriNb].ul_SmoothingGroup = (ULONG) pst_Face->uw_Element;
		uw_TriNb++;

		L_memcpy(&pst_TriModif[uw_TriModifNb], pst_Tri2, sizeof(GEO_tdst_IndexedTriangle));
		pst_TriModif[uw_TriModifNb].auw_Index[j2] = pst_EData2->pt;
		pst_TriModif[uw_TriModifNb].auw_UV[j2] = pst_EData2->uv;
		pst_TriModif[uw_TriModifNb].ul_SmoothingGroup = (ULONG) pst_Face2;
		uw_TriModifNb++;

		L_memcpy(&dst_Tri[uw_TriNb], pst_Tri2, sizeof(GEO_tdst_IndexedTriangle));
		dst_Tri[uw_TriNb].auw_Index[j2] = pst_EData->pt;
		dst_Tri[uw_TriNb].auw_UV[j2] = pst_EData->uv;
		dst_Tri[uw_TriNb].auw_Index[k2] = pst_EData2->pt;
		dst_Tri[uw_TriNb].auw_UV[k2] = pst_EData2->uv;
		dst_Tri[uw_TriNb].ul_SmoothingGroup = (ULONG) pst_Face2->uw_Element;
		uw_TriNb++;

		i++;
	}

	/* modifi les triangles modifies */
	for(i = 0; i < uw_TriModifNb; i++)
	{
		pst_Face = (GEO_tdst_SubObject_Face *) pst_TriModif[i].ul_SmoothingGroup;
		pst_Tri = _pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
		pst_TriModif[i].ul_SmoothingGroup = pst_Tri->ul_SmoothingGroup;
		L_memcpy(pst_Tri, &pst_TriModif[i], sizeof(GEO_tdst_IndexedTriangle));
	}

	/* ajoute les nouveaux triangles */
	for(i = 0; i < uw_TriNb; i++)
	{
		j = dst_Tri[i].ul_SmoothingGroup;
		dst_Tri[i].ul_SmoothingGroup = 0;
		GEO_AddTriangle(_pst_Obj->dst_Element + j, &dst_Tri[i]);
	}

	/* update sub object data */
	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_VUpdateESel(_pst_Obj);
	GEO_SubObject_AllocBuffers(_pst_Obj);
    GEO_SubObject_HideUpdate( _pst_Obj, TRUE );
	//GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
	GEO_ComputeNormals( _pst_Obj );

ECutter_end:
	L_free(duw_Face);
	L_free(dst_FData);
	L_free(dst_EData);
	if(dst_Tri) L_free(dst_Tri);
	return ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_EExtrude(GEO_tdst_Object *_pst_Obj, int _i_Id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			st_NewTri;
	GEO_tdst_SubObject_Edge				*pst_Edge, *pst_Last;
	int									*pi_New;
	char								*pc_Sel;
	int									i_Nb, i_Last;
	WOR_tdst_Update_RLI					st_UpdateRLIData;
	int									i;
	ULONG								*pul_RLI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	i_Last = _pst_Obj->l_NbPoints;
	pi_New = (int *) L_malloc(4 * (i_Last + 1));
	pi_New[i_Last] = -1;

	/* first count selected vertices */
	for(i = 0, i_Nb = 0; i < i_Last; i++)
	{
		pi_New[i] = i;
		if(pc_Sel[i] & 1)
		{
			pi_New[i] = i_Last + i_Nb;
			i_Nb++;
		}
	}

	/* test number : no vertex selected => nothing to do */
	if(i_Nb == 0)
	{
		L_free(pi_New);
		return;
	}

	/* add new point */
	GEO_AddSeveralPoint(_pst_Obj, i_Nb);
	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;

	for(i = 0, i_Nb = 0; i < i_Last; i++)
	{
		if(pc_Sel[i] & 1)
		{
			MATH_CopyVector(VCast(&_pst_Obj->dst_Point[i_Last + i_Nb++]), VCast(&_pst_Obj->dst_Point[i]));
		}
	}

	/* update geometry RLI */
	if(pul_RLI = _pst_Obj->dul_PointColors)
	{
		for(i = 0, i_Nb = 0; i < i_Last; i++)
		{
			if(!(pc_Sel[i] & 1)) continue;
			pul_RLI[1 + i_Last + i_Nb++] = pul_RLI[1 + i];
		}
	}

	/* update eventually RLI */
	st_UpdateRLIData.p_Geo = _pst_Obj;
	st_UpdateRLIData.l_Op = WOR_Update_RLI_AddSome;
	st_UpdateRLIData.l_NbAdded = i_Nb;
	st_UpdateRLIData.l_OldNbPoints = i_Last;
	st_UpdateRLIData.l_Ind1 = (LONG) pi_New;
	WOR_Universe_Update_RLI(&st_UpdateRLIData);

	/* update eventually skin */
	GEO_SKN_UpdateAfterVertexEdition(_pst_Obj, &st_UpdateRLIData);

	/* get good element */
	pst_Element = _pst_Obj->dst_Element;

	/* Create 2 triangles for each selected edge */
	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
	pst_Last = pst_Edge + _pst_Obj->pst_SubObject->ul_NbEdges;
	pc_Sel = _pst_Obj->pst_SubObject->dc_ESel;
	for(; pst_Edge < pst_Last; pst_Edge++, pc_Sel++)
	{
		if(*pc_Sel & 1)
		{
			st_NewTri.auw_Index[0] = (USHORT) pst_Edge->ul_Point[0];
			st_NewTri.auw_Index[1] = (USHORT) pst_Edge->ul_Point[1];
			st_NewTri.auw_Index[2] = (USHORT) pi_New[pst_Edge->ul_Point[1]];
			st_NewTri.auw_UV[0] = 0;
			st_NewTri.auw_UV[1] = 0;
			st_NewTri.auw_UV[2] = 0;
			GEO_AddTriangle(pst_Element, &st_NewTri);

			st_NewTri.auw_Index[0] = (USHORT) pi_New[pst_Edge->ul_Point[1]];
			st_NewTri.auw_Index[1] = (USHORT) pi_New[pst_Edge->ul_Point[0]];
			st_NewTri.auw_Index[2] = (USHORT) pst_Edge->ul_Point[0];
			st_NewTri.auw_UV[0] = 0;
			st_NewTri.auw_UV[1] = 0;
			st_NewTri.auw_UV[2] = 0;
			GEO_AddTriangle(pst_Element, &st_NewTri);
		}
	}

	L_free(pi_New);

	/* update selection */
	L_memset(_pst_Obj->pst_SubObject->dc_VSel, 0, i_Last);
	L_memset(_pst_Obj->pst_SubObject->dc_VSel + i_Last, 1, i_Nb);

	/* update sub object data */
	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_VUpdateESel(_pst_Obj);
	GEO_SubObject_AllocBuffers(_pst_Obj);
    GEO_SubObject_HideUpdate( _pst_Obj, TRUE );
}

/*$4
 ***********************************************************************************************************************
    faces
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG GEO_l_SubObject_FNbSel(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	UCHAR	*puc_Sel, *puc_Last;
	LONG	l_Number;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	puc_Sel = (UCHAR*)_pst_Obj->pst_SubObject->dc_FSel;
	puc_Last = puc_Sel + _pst_Obj->pst_SubObject->ul_NbFaces;
	l_Number = 0;

	for(; puc_Sel < puc_Last; puc_Sel++)
	{
		if(*puc_Sel & 1) l_Number++;
	}

	return l_Number;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_FUpdateVSel(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	char						*pc_Sel, *pc_VSel;
	GEO_tdst_SubObject_Face		*pst_Face;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

    pc_VSel = _pst_Obj->pst_SubObject->dc_VSel;
    for (i = 0; i < _pst_Obj->l_NbPoints; i++, pc_VSel++)
        *pc_VSel &= 0x80;

	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	pc_VSel = _pst_Obj->pst_SubObject->dc_VSel;
	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pc_Sel++, pst_Face++)
	{
		if(*pc_Sel & 1)
		{
			pst_Triangle = _pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
			pc_VSel[pst_Triangle->auw_Index[0]] |= 1;
			pc_VSel[pst_Triangle->auw_Index[1]] |= 1;
			pc_VSel[pst_Triangle->auw_Index[2]] |= 1;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_FSelectBad(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						i, j;
	char					*pc_Sel;
	GEO_tdst_SubObject_Edge *pst_Edge;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	L_memset(pc_Sel, 0, _pst_Obj->pst_SubObject->ul_NbFaces);

	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbEdges; i++, pst_Edge++)
	{
		if(pst_Edge->ul_Triangle[2] != (ULONG) - 1)
		{
			for(j = 0; j < 4; j++)
			{
				if(pst_Edge->uw_Index[j] == (USHORT) - 1) break;
				pc_Sel[pst_Edge->uw_Index[j]] = 1;
			}
		}
	}

	GEO_SubObject_FUpdateVSel(_pst_Obj);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_FPick(GEO_tdst_Object *_pst_Obj, MATH_tdst_Vector *_pst_2D, char _b_Sel, int _i_Id, char _b_Backface )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		    *pst_Vertex;
	float					    f, f_Dist;
	int						    i, i_Result;
	UCHAR					    *pc_Sel;
	GEO_tdst_SubObject_Face     *pst_Face;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return -1;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return -1;
	if(!_pst_Obj->pst_SubObject) return -1;

	pst_Vertex = _pst_Obj->pst_SubObject->dst_2D;
	f_Dist = 8;
	i_Result = -1;
    pc_Sel = (UCHAR*)_pst_Obj->pst_SubObject->dc_FSel;

    if (_b_Backface == 0 )
    {
        if (_i_Id == -1)
        {
            if(_b_Sel)
	        {
		        for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++ )
		        {
                    if (*pc_Sel & 0x80) continue;
			        f = (float) (fabs(_pst_2D->x - pst_Vertex->x) + fabs(_pst_2D->y - pst_Vertex->y));
			        if(f < f_Dist)
			        {
				        f_Dist = f;
				        i_Result = i;
			        }
		        }
	        }
	        else
	        {
		        for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++)
		        {
			        if(*pc_Sel & 0x81) continue;
			        f = (float) (fabs(_pst_2D->x - pst_Vertex->x) + fabs(_pst_2D->y - pst_Vertex->y));
			        if(f < f_Dist)
			        {
			            f_Dist = f;
				        i_Result = i;
			        }
		        }
	        }
        }
        else
        {
            if(_b_Sel)
	        {
		        for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++ )
    		    {
                    if (*pc_Sel & 0x80) continue;
		    	    f = (float) (fabs(_pst_2D->x - pst_Vertex->x) + fabs(_pst_2D->y - pst_Vertex->y));
    			    if(f < f_Dist)
	    		    {
					    pst_Face = _pst_Obj->pst_SubObject->dst_Faces + i;
					    if(_pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId == _i_Id)
					    {
						    f_Dist = f;
						    i_Result = i;
					    }
                    }
			    }
            }
	        else
	        {
		        for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++)
		        {
			        if(*pc_Sel & 0x81) continue;
			        f = (float) (fabs(_pst_2D->x - pst_Vertex->x) + fabs(_pst_2D->y - pst_Vertex->y));
			        if(f < f_Dist)
			        {
					    pst_Face = _pst_Obj->pst_SubObject->dst_Faces + i;
					    if(_pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId == _i_Id)
					    {
						    f_Dist = f;
						    i_Result = i;
					    }
				    }
			    }
		    }
	    }
    }
    else
    {
        pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
        if (_i_Id == -1)
        {
            if(_b_Sel)
	        {
		        for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++, pst_Face++ )
		        {
                    if (*pc_Sel & 0x80) continue;
                    if ( pst_Face->fNZ > 0) continue;
			        f = (float) (fabs(_pst_2D->x - pst_Vertex->x) + fabs(_pst_2D->y - pst_Vertex->y));
			        if(f < f_Dist)
			        {

				        f_Dist = f;
				        i_Result = i;
			        }
		        }
	        }
	        else
	        {
		        for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++, pst_Face++)
		        {
			        if(*pc_Sel & 0x81) continue;
                    if ( pst_Face->fNZ > 0) continue;
			        f = (float) (fabs(_pst_2D->x - pst_Vertex->x) + fabs(_pst_2D->y - pst_Vertex->y));
			        if(f < f_Dist)
			        {
			            f_Dist = f;
				        i_Result = i;
			        }
		        }
	        }
        }
        else
        {
            if(_b_Sel)
	        {
		        for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++, pst_Face++ )
    		    {
                    if (*pc_Sel & 0x80) continue;
                    if ( pst_Face->fNZ > 0) continue;
		    	    f = (float) (fabs(_pst_2D->x - pst_Vertex->x) + fabs(_pst_2D->y - pst_Vertex->y));
    			    if(f < f_Dist)
	    		    {
					    if(_pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId == _i_Id)
					    {
						    f_Dist = f;
						    i_Result = i;
					    }
                    }
			    }
            }
	        else
	        {
		        for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++, pst_Face++ )
		        {
			        if(*pc_Sel & 0x81) continue;
                    if ( pst_Face->fNZ > 0) continue;
			        f = (float) (fabs(_pst_2D->x - pst_Vertex->x) + fabs(_pst_2D->y - pst_Vertex->y));
			        if(f < f_Dist)
			        {
					    pst_Face = _pst_Obj->pst_SubObject->dst_Faces + i;
					    if(_pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId == _i_Id)
					    {
						    f_Dist = f;
						    i_Result = i;
					    }
				    }
			    }
		    }
	    }
    }

	return i_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_FPickBox
(
	GEO_tdst_Object		    *_pst_Obj,
	MATH_tdst_Vector	    *A,
	MATH_tdst_Vector	    *B,
	int					    _i_Id,
    char                    _c_Backface, 
	OBJ_tdst_GameObject     *_pst_GO
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		*pst_Vertex;
	int						i, i_Result;
	char					*pc_Sel;
	GEO_tdst_SubObject_Face *pst_Face;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

	pst_Vertex = _pst_Obj->pst_SubObject->dst_2D;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	i_Result = 0;

    if ( !_c_Backface )
    {
	    if(_i_Id == -1)
	    {
		    for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++)
		    {
                if (*pc_Sel & 0x80) continue;

			    if(!(GEO_ColMap_ElementVisible(_pst_GO, (_pst_Obj->pst_SubObject->dst_Faces + i)->uw_Element))) continue;

			    if((pst_Vertex->x < A->x) || (pst_Vertex->x > B->x)) continue;
			    if((pst_Vertex->y < A->y) || (pst_Vertex->y > B->y)) continue;
				if (GEO_SOFT_PickWithZBufferCulling(pst_Vertex)) continue;

			    *pc_Sel |= 2;
			    i_Result++;
		    }
	    }
	    else
	    {
		    pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
		    for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++, pst_Face++)
		    {
                if (*pc_Sel & 0x80) continue;
			    if(!(GEO_ColMap_ElementVisible(_pst_GO, pst_Face->uw_Element))) continue;

			    if(_pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId != _i_Id) continue;
			    if((pst_Vertex->x < A->x) || (pst_Vertex->x > B->x)) continue;
			    if((pst_Vertex->y < A->y) || (pst_Vertex->y > B->y)) continue;
				if (GEO_SOFT_PickWithZBufferCulling(pst_Vertex)) continue;

			    *pc_Sel |= 2;
			    i_Result++;
		    }
	    }
    }
    else
    {
        pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
        if(_i_Id == -1)
	    {
		    for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++, pst_Face++)
		    {
                if (*pc_Sel & 0x80) continue;
                if ( pst_Face->fNZ > 0) continue;

			    if(!(GEO_ColMap_ElementVisible(_pst_GO, (_pst_Obj->pst_SubObject->dst_Faces + i)->uw_Element))) continue;

			    if((pst_Vertex->x < A->x) || (pst_Vertex->x > B->x)) continue;
			    if((pst_Vertex->y < A->y) || (pst_Vertex->y > B->y)) continue;
				if (GEO_SOFT_PickWithZBufferCulling(pst_Vertex)) continue;

			    *pc_Sel |= 2;
			    i_Result++;
		    }
	    }
	    else
	    {
		    for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pst_Vertex++, pc_Sel++, pst_Face++)
		    {
                if (*pc_Sel & 0x80) continue;
                if ( pst_Face->fNZ > 0) continue;
			    if(!(GEO_ColMap_ElementVisible(_pst_GO, pst_Face->uw_Element))) continue;

			    if(_pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId != _i_Id) continue;
			    if((pst_Vertex->x < A->x) || (pst_Vertex->x > B->x)) continue;
			    if((pst_Vertex->y < A->y) || (pst_Vertex->y > B->y)) continue;
				if (GEO_SOFT_PickWithZBufferCulling(pst_Vertex)) continue;


			    *pc_Sel |= 2;
			    i_Result++;
		    }
	    }
    }

	return i_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_FPickBoxWithPB
(
	GEO_tdst_Object		    *_pst_Obj,
	MATH_tdst_Vector	    *A,
	MATH_tdst_Vector	    *B,
	int					    _i_Id,
	OBJ_tdst_GameObject     *_pst_GO,
    SOFT_tdst_PickingBuffer *_pst_PB
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		*pst_Vertex;
	int						x, y, e, t, i_Result, i_Face, i_SendFace;
	char					*pc_Sel;
	SOFT_tdst_PickingBuffer_Pixel *pst_Pixel;
    ULONG                   ul_PrevValue;
    ULONG                   aul_ElementOffset[1024];
    GEO_tdst_SubObject_Face *pst_Face;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;

    y = 0;
    for (x = 0; x <  _pst_Obj->l_NbElements; x++)
    {
        aul_ElementOffset[ x ] = y;
        y += _pst_Obj->dst_Element[ x ].l_NbTriangles;
    }

    if (!B)
    {
        B = A;
        i_SendFace = 1;
    }
    else
        i_SendFace = 0;

	pst_Vertex = _pst_Obj->pst_SubObject->dst_2D;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	i_Result = 0;
    ul_PrevValue = 0xFFFFFFFF;
    pst_Face = _pst_Obj->pst_SubObject->dst_Faces;

	if(_i_Id == -1)
	{
        y = (int) A->y;
        while ( y <= (int) B->y)
        {
            x = (int) A->x;
            pst_Pixel = _pst_PB->dst_Pixel + (y * _pst_PB->l_Width + x );
            while( x <= (int) B->x )
            {
                if (pst_Pixel->ul_ValueExt != ul_PrevValue )
                {
                    if (pst_Pixel->ul_Value == (ULONG) _pst_GO )
                    {
                        e = (pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_ElementMask) >> SOFT_Cul_PBQF_ElementShift;
			            t = (pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_TriangleMask) >> SOFT_Cul_PBQF_TriangleShift;
                        if ( !(pc_Sel[ aul_ElementOffset[ e ] + t ] & 0x82) )
                        {
                            i_Face = aul_ElementOffset[ e ] + t; 
                            pc_Sel[ i_Face ] |= 2;
                            i_Result++;
                        }
                    }
                    ul_PrevValue = pst_Pixel->ul_ValueExt;
                }
                x++;
                pst_Pixel++;
            }
            y++;
        }
    }
    else
    {
        y = (int) A->y;
        while ( y <= B->y)
        {
            x = (int) A->x;
            pst_Pixel = _pst_PB->dst_Pixel + ( y * _pst_PB->l_Width + x );
            while( x <= B->x )
            {
                if (pst_Pixel->ul_ValueExt != ul_PrevValue )
                {
                    if (pst_Pixel->ul_SelMask == (ULONG) _pst_GO )
                    {
                        e = (pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_ElementMask) >> SOFT_Cul_PBQF_ElementShift;
			            t = (pst_Pixel->ul_ValueExt & SOFT_Cul_PBQF_TriangleMask) >> SOFT_Cul_PBQF_TriangleShift;
                        if ( _pst_Obj->dst_Element[e].l_MaterialId == _i_Id)
                        {
                            if ( !(pc_Sel[ aul_ElementOffset[ e ] + t ] & 0x82) )
                            {
                                i_Face = aul_ElementOffset[ e ] + t; 
                                pc_Sel[ i_Face ] |= 2;
                                i_Result++;
                            }
                        }
                    }
                    ul_PrevValue = pst_Pixel->ul_ValueExt;
                }
                x++;
                pst_Pixel++;
            }
            y++;
        }
    }

    return i_SendFace ? i_Face : i_Result;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_BuildFaceData(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject					*pst_SO;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Triangle;
	LONG								l_Triangle, l_Elem;
	GEO_tdst_SubObject_Face				*pst_Face;
    ULONG                               ul_UsedIndex;
    GEO_Vertex                          *V;
    MATH_tdst_Vector					V1, V2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pst_SO = _pst_Obj->pst_SubObject;
    if (pst_SO->dst_PointTransformed)
        V = pst_SO->dst_PointTransformed;
    else
        V = _pst_Obj->dst_Point;

	l_Triangle = GEO_l_GetNumberOfTriangles(_pst_Obj);
	if((ULONG) l_Triangle != pst_SO->ul_NbFaces)
	{
		if(pst_SO->ul_NbFaces)
		{
			L_free(pst_SO->dst_Faces);
			L_free(pst_SO->dc_FSel);
		}

		pst_SO->ul_NbFaces = l_Triangle;
		pst_SO->dst_Faces = (GEO_tdst_SubObject_Face *) L_malloc(pst_SO->ul_NbFaces * sizeof(GEO_tdst_SubObject_Face));
		pst_SO->dc_FSel = (char *) L_malloc(pst_SO->ul_NbFaces);
		L_memset(pst_SO->dc_FSel, 0, pst_SO->ul_NbFaces);
	}

	pst_Face = pst_SO->dst_Faces;
	pst_Element = _pst_Obj->dst_Element;
    ul_UsedIndex = 0;
	for(l_Elem = 0; l_Elem < _pst_Obj->l_NbElements; l_Elem++, pst_Element++)
	{
        ul_UsedIndex |= (ULONG) pst_Element->pus_ListOfUsedIndex;
		pst_Triangle = pst_Element->dst_Triangle;
		for(l_Triangle = 0; l_Triangle < pst_Element->l_NbTriangles; l_Triangle++, pst_Triangle++)
		{
			MATH_CopyVector(&pst_Face->st_Center, VCast(V + pst_Triangle->auw_Index[0]));
			MATH_AddEqualVector(&pst_Face->st_Center, VCast(V + pst_Triangle->auw_Index[1]));
			MATH_AddEqualVector(&pst_Face->st_Center, VCast(V + pst_Triangle->auw_Index[2]));
			MATH_ScaleEqualVector(&pst_Face->st_Center, 0.333333333f);
			
			MATH_SubVector( &V1, VCast(V + pst_Triangle->auw_Index[1]), VCast(V + pst_Triangle->auw_Index[0]) );
			MATH_SubVector( &V2, VCast(V + pst_Triangle->auw_Index[2]), VCast(V + pst_Triangle->auw_Index[0]) );
			MATH_CrossProduct(&pst_Face->st_Normal, &V1, &V2);
            MATH_NormalizeAnyVector(&pst_Face->st_Normal, &pst_Face->st_Normal );

			pst_Face->uw_Element = (USHORT) l_Elem;
			pst_Face->uw_Index = (USHORT) l_Triangle;
			pst_Face++;
		}
	}

    if (ul_UsedIndex)
        GEO_MRM_ComputeUsedIndex( _pst_Obj );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_HideUpdate(GEO_tdst_Object *_pst_Obj, BOOL hide)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject_Face		*pst_Face, *pst_FLast;
    GEO_tdst_SubObject_Edge     *pst_Edge, *pst_ELast;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	char						*pc_Sel, *pc_ESel;
	ULONG						i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pst_FLast = pst_Face + _pst_Obj->pst_SubObject->ul_NbFaces;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;

	if(!hide)
	{
		for(i = 0; i < (ULONG) _pst_Obj->l_NbPoints; i++) _pst_Obj->pst_SubObject->dc_VSel[i] &= ~0x80;
		for(i = 0; i < _pst_Obj->pst_SubObject->ul_NbEdges; i++) _pst_Obj->pst_SubObject->dc_ESel[i] &= ~0x80;
        for(i = 0; i < _pst_Obj->pst_SubObject->ul_NbFaces; i++) _pst_Obj->pst_SubObject->dc_FSel[i] &= ~0x80;
		return;
	}

	/* Compute unhidden points */
	for(i = 0; i < (ULONG) _pst_Obj->l_NbPoints; i++) _pst_Obj->pst_SubObject->dc_VSel[i] |= 0x80;
	for(; pst_Face < pst_FLast; pst_Face++, pc_Sel++)
	{
		pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
		if(!(pst_Triangle->ul_MaxFlags & 0x80000000))
		{
			_pst_Obj->pst_SubObject->dc_VSel[pst_Triangle->auw_Index[0]] &= ~0x80;
			_pst_Obj->pst_SubObject->dc_VSel[pst_Triangle->auw_Index[1]] &= ~0x80;
			_pst_Obj->pst_SubObject->dc_VSel[pst_Triangle->auw_Index[2]] &= ~0x80;
            *pc_Sel &= 0x7F;
		}
        else
            *pc_Sel |= 0x80;
	}

    pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
    pst_ELast = pst_Edge + _pst_Obj->pst_SubObject->ul_NbEdges;
    pc_ESel = _pst_Obj->pst_SubObject->dc_ESel;
    pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;

    for ( ; pst_Edge < pst_ELast; pst_Edge++, pc_ESel++ )
    {

        if(pst_Edge->ul_Triangle[0] == -1) continue;

        *pc_ESel = (*pc_ESel & 0x7F) | (pc_Sel[ pst_Edge->uw_Index[0] ] & 0x80);

        if(pst_Edge->ul_Triangle[1] != -1) 
            *pc_ESel = (*pc_ESel & 0x7F) | (*pc_ESel & (pc_Sel[ pst_Edge->uw_Index[1] ] & 0x80));
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
void GEO_SubObject_HideSelectedFaces(GEO_tdst_Object *_pst_Obj, GRO_tdst_Visu* _pst_Visu, BOOL hide, BOOL unsel )
#else
void GEO_SubObject_HideSelectedFaces(GEO_tdst_Object *_pst_Obj, BOOL hide, BOOL unsel )
#endif
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject_Face		*pst_Face, *pst_Last;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	char						*pc_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (!_pst_Obj || !_pst_Obj->pst_SubObject) return;

	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pst_Last = pst_Face + _pst_Obj->pst_SubObject->ul_NbFaces;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;

  #ifdef _XENON_RENDER
    if (_pst_Visu->l_NbXeElements != _pst_Obj->l_NbElements)
        return;

    LPWORD apoIndexBuffer[256];
    if (GDI_b_IsXenonGraphics() && (_pst_Obj->l_NbElements > 0))
    {
        for (LONG lCurElement = 0; lCurElement < _pst_Obj->l_NbElements; lCurElement++)
        {
            GRO_tdst_XeElement* pst_Element = &_pst_Visu->p_XeElements[lCurElement];

            if (pst_Element->pst_IndexBuffer != NULL)
                apoIndexBuffer[lCurElement] = (LPWORD)pst_Element->pst_IndexBuffer->Lock(_pst_Obj->dst_Element[lCurElement].l_NbTriangles * 3);
            else
                apoIndexBuffer[lCurElement] = NULL;
        }
    }
  #endif

	if(hide)
	{
		if (unsel)
		{
			for(; pst_Face < pst_Last; pst_Face++, pc_Sel++)
			{
				if( (*pc_Sel & 1) ) continue;
				pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
				pst_Triangle->ul_MaxFlags |= 0x80000000;

#ifdef _XENON_RENDER
                if (GDI_b_IsXenonGraphics() && (apoIndexBuffer[pst_Face->uw_Element] != NULL))
                {
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 0] = 0;
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 1] = 0;
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 2] = 0;
                }
#endif

			}
		}
		else
		{
			for(; pst_Face < pst_Last; pst_Face++, pc_Sel++)
			{
				if(!(*pc_Sel & 1)) continue;
				pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
				pst_Triangle->ul_MaxFlags |= 0x80000000;

             #ifdef _XENON_RENDER
                if (GDI_b_IsXenonGraphics() && (apoIndexBuffer[pst_Face->uw_Element] != NULL))
                {
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 0] = 0;
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 1] = 0;
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 2] = 0;
				}
              #endif
			}
			GEO_SubObject_UnselAll(_pst_Obj);
		}
	}
	else
	{
		for(; pst_Face < pst_Last; pst_Face++, pc_Sel++)
		{
			pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
			*pc_Sel = (pst_Triangle->ul_MaxFlags & 0x80000000) ? 1 : 0;
			pst_Triangle->ul_MaxFlags &= ~0x80000000;

          #ifdef _XENON_RENDER
            if (GDI_b_IsXenonGraphics() && (apoIndexBuffer[pst_Face->uw_Element] != NULL))
            {
                if (_pst_Visu->p_XeElements[pst_Face->uw_Element].puw_Indices)
                {
                    USHORT* puw_IndicesEd = _pst_Visu->p_XeElements[pst_Face->uw_Element].puw_Indices;

                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 0] = puw_IndicesEd[pst_Face->uw_Index*3 + 0];
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 1] = puw_IndicesEd[pst_Face->uw_Index*3 + 1];
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 2] = puw_IndicesEd[pst_Face->uw_Index*3 + 2];
                }
                else
                {
                    LONG lIndexCount = 0;
                    for (LONG lCurElement = 0 ; lCurElement < pst_Face->uw_Element; lCurElement++)
                    {
                        lIndexCount += _pst_Obj->dst_Element[lCurElement].l_NbTriangles * 3;
                    }
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 0] = (USHORT)(lIndexCount + pst_Face->uw_Index*3 + 0);
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 1] = (USHORT)(lIndexCount + pst_Face->uw_Index*3 + 1);
                    apoIndexBuffer[pst_Face->uw_Element][pst_Face->uw_Index*3 + 2] = (USHORT)(lIndexCount + pst_Face->uw_Index*3 + 2);
                }
            }
          #endif

		}

		GEO_SubObject_FUpdateVSel(_pst_Obj);
	}

  #ifdef _XENON_RENDER
    if (GDI_b_IsXenonGraphics())
    {
        for (LONG lCurElement = 0; lCurElement < _pst_Obj->l_NbElements; lCurElement++)
        {
            GRO_tdst_XeElement* pst_Element = &_pst_Visu->p_XeElements[lCurElement];

            if (pst_Element->pst_IndexBuffer != NULL)
                pst_Element->pst_IndexBuffer->Unlock();
        }
    }
  #endif


	GEO_SubObject_HideUpdate(_pst_Obj, hide);
}

/*
 =======================================================================================================================
    internal functions:: no check perform, assume it is called by function that do the necessary check.
    restoreselectedfaces as to be called after.
 =======================================================================================================================
 */
void GEO_SubObject_SaveSelectedFaces(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject_Face		*pst_Face, *pst_Last;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	char						*pc_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pst_Last = pst_Face + _pst_Obj->pst_SubObject->ul_NbFaces;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	for(; pst_Face < pst_Last; pst_Face++, pc_Sel++)
	{
		/*
        if(*pc_Sel & 1)
		{
			pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
			pst_Triangle->ul_MaxFlags |= 0x8000000;
		}
        */
        pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
        pst_Triangle->ul_MaxFlags &= 0xFFFFFF;
        pst_Triangle->ul_MaxFlags |= *pc_Sel << 24;

	}
}

/*
 =======================================================================================================================
    internal functions:: no check perform, assume it is called by function that do the necessary check.
 =======================================================================================================================
 */
void GEO_SubObject_RestoreSelectedFaces(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject_Face		*pst_Face, *pst_Last;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	char						*pc_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pst_Last = pst_Face + _pst_Obj->pst_SubObject->ul_NbFaces;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	for(; pst_Face < pst_Last; pst_Face++, pc_Sel++)
	{
		pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
        *pc_Sel = (char) ((pst_Triangle->ul_MaxFlags >> 24) & 0xFF);
        pst_Triangle->ul_MaxFlags &= 0x80FFFFFF;

		/*if(pst_Triangle->ul_MaxFlags & 0x8000000)
		{
			*pc_Sel = 1;
			pst_Triangle->ul_MaxFlags -= 0x8000000;
		}*/
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_FlipNormals(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject_Face		*pst_Face, *pst_Last;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	USHORT						uw_Save;
	char						*pc_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pst_Last = pst_Face + _pst_Obj->pst_SubObject->ul_NbFaces;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	for(; pst_Face < pst_Last; pst_Face++, pc_Sel++)
	{
		if(!(*pc_Sel & 1)) continue;

		pst_Triangle = _pst_Obj->dst_Element[pst_Face->uw_Element & 0x7FFF].dst_Triangle + pst_Face->uw_Index;
		uw_Save = pst_Triangle->auw_Index[1];
		pst_Triangle->auw_Index[1] = pst_Triangle->auw_Index[2];
		pst_Triangle->auw_Index[2] = uw_Save;

		uw_Save = pst_Triangle->auw_UV[1];
		pst_Triangle->auw_UV[1] = pst_Triangle->auw_UV[2];
		pst_Triangle->auw_UV[2] = uw_Save;
		
		MATH_NegEqualVector( &pst_Face->st_Normal );
	}

	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_SortID(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	ElemSwap;
	LONG ElemI,ElemJ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if (_pst_Obj->l_NbElements <= 1) return;


	/* sORT ELEMENTS  */
	for(ElemI = 0; ElemI < _pst_Obj->l_NbElements - 1; ElemI++)
		for(ElemJ = ElemI + 1; ElemJ < _pst_Obj->l_NbElements; ElemJ++)
		{
			if (_pst_Obj->dst_Element[ElemI].l_MaterialId > _pst_Obj->dst_Element[ElemJ].l_MaterialId)
			{
					ElemSwap = _pst_Obj->dst_Element[ElemI];
					_pst_Obj->dst_Element[ElemI] = _pst_Obj->dst_Element[ElemJ];
					_pst_Obj->dst_Element[ElemJ] = ElemSwap ;
			}
		}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_ChangeID(GEO_tdst_Object *_pst_Obj, int _i_NewId, BOOL _b_ColMap)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_SubObject_Face				*pst_Face;
	GEO_tdst_ElementIndexedTriangles	*pst_Elem, *pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Triangle, *pst_T, *pst_LT;
	LONG								*pl_ParseForDelete;
	int									i_Elem, i_Tri, i_TriDel;
	char								*pc_Sel, *pc_Last;
	BOOL								b_strip;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (_i_NewId < 0) return;
	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	if(_pst_Obj->ulStripFlag & GEO_C_Strip_DataValid)
	{
		b_strip = TRUE;
		GEO_STRIP_Delete(_pst_Obj);
	}
	else
		b_strip = FALSE;

	/* look if there is at leat one selected face */
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	pc_Last = pc_Sel + _pst_Obj->pst_SubObject->ul_NbFaces;
	for(; pc_Sel < pc_Last; pc_Sel++)
		if(*pc_Sel & 1) break;

	if(pc_Sel == pc_Last)
        return;
	
	GEO_SubObject_SaveSelectedFaces(_pst_Obj);

	/* search element with given id */
	pst_Element = _pst_Obj->dst_Element;
	for(i_Elem = 0; i_Elem < _pst_Obj->l_NbElements; i_Elem++, pst_Element++)
	{
	    if(pst_Element->l_MaterialId == _i_NewId) break;
	}

	/* if no element with given id is found add one element */
	if(i_Elem == _pst_Obj->l_NbElements)
	{
	    GEO_AddElement(_pst_Obj, _i_NewId, 0);
		pst_Element = _pst_Obj->dst_Element + (_pst_Obj->l_NbElements - 1);
    }

	pl_ParseForDelete = (LONG*)L_malloc(4 * _pst_Obj->l_NbElements);
	L_memset(pl_ParseForDelete, 0, 4 * _pst_Obj->l_NbElements);

	/* scan faces */
	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	for(; pc_Sel < pc_Last; pst_Face++, pc_Sel++)
	{
	    if(!(*pc_Sel & 1)) continue;

		pst_Elem = _pst_Obj->dst_Element + pst_Face->uw_Element;
		if(pst_Elem == pst_Element) continue;
		pst_Triangle = pst_Elem->dst_Triangle + pst_Face->uw_Index;

		GEO_AddTriangle(pst_Element, pst_Triangle);

		/* mark element to know there's tri to delete within */
		if(pl_ParseForDelete[pst_Face->uw_Element] == 0)
		{
		    pl_ParseForDelete[pst_Face->uw_Element] = 1;
			pst_T = pst_Elem->dst_Triangle;
			pst_LT = pst_T + pst_Elem->l_NbTriangles;
			for(; pst_T < pst_LT; pst_T++)
			{
				pst_T->ul_MaxFlags &= ~0x40000000;
			}
        }

		/* set to indicate a triangle to be deleted */
		pst_Triangle->ul_MaxFlags |= 0x40000000;

		pst_Face->uw_Element = i_Elem;
		pst_Face->uw_Index = (unsigned short)(pst_Element->l_NbTriangles - 1);
    }

	/* scan triangles to delete moved triangles */
	for(i_Elem = 0; i_Elem < _pst_Obj->l_NbElements; i_Elem++)
	{
	    if(pl_ParseForDelete[i_Elem] == 0) continue;
		pst_Elem = _pst_Obj->dst_Element + i_Elem;

		for(i_TriDel = 0, i_Tri = 0; i_Tri < pst_Elem->l_NbTriangles;)
		{
			if(pst_Elem->dst_Triangle[i_Tri].ul_MaxFlags & 0x40000000)
			{
				i_TriDel++;
				if(i_Tri < pst_Elem->l_NbTriangles - 1)
				{
					L_memcpy
					(
						pst_Elem->dst_Triangle + i_Tri,
						pst_Elem->dst_Triangle + (pst_Elem->l_NbTriangles - 1),
						sizeof(GEO_tdst_IndexedTriangle)
					);
				}
				pst_Elem->l_NbTriangles--;
			}
			else
			{
				i_Tri++;
			}
		}

		if(i_TriDel)
		{
			if(pst_Elem->l_NbTriangles)
			{
				pst_Elem->dst_Triangle = (GEO_tdst_IndexedTriangle*)MEM_p_Realloc
					(
						pst_Elem->dst_Triangle,
						sizeof(GEO_tdst_IndexedTriangle) * pst_Elem->l_NbTriangles
					);
			}
			else
			{
				MEM_Free(pst_Elem->dst_Triangle);
				pst_Elem->dst_Triangle = NULL;
			}
		}
	}

	L_free(pl_ParseForDelete);

	GEO_SubObject_SortID( _pst_Obj );
	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_BuildFaceData(_pst_Obj);

    L_memset(_pst_Obj->pst_SubObject->dc_FSel, 0, _pst_Obj->pst_SubObject->ul_NbFaces);
	GEO_SubObject_RestoreSelectedFaces(_pst_Obj);
    GEO_SubObject_HideUpdate( _pst_Obj , TRUE );

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_FSelBad(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*pst_Tri;
	int									i, i_Elem, i_Result;
	char								*pc_Sel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return 0;
	if(!_pst_Obj->pst_SubObject) return 0;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return 0;

	i_Result = 0;

	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	L_memset(pc_Sel, 0, _pst_Obj->pst_SubObject->ul_NbFaces);

	pst_Elem = _pst_Obj->dst_Element;
	for(i_Elem = 0; i_Elem < _pst_Obj->l_NbElements; i_Elem++, pst_Elem++)
	{
		pst_Tri = pst_Elem->dst_Triangle;
		for(i = 0; i < pst_Elem->l_NbTriangles; i++, pst_Tri++, pc_Sel++)
		{
			if(pst_Tri->auw_Index[0] >= _pst_Obj->l_NbPoints) goto FSelBad_Sel;
			if(pst_Tri->auw_Index[1] >= _pst_Obj->l_NbPoints) goto FSelBad_Sel;
			if(pst_Tri->auw_Index[2] >= _pst_Obj->l_NbPoints) goto FSelBad_Sel;
			if(pst_Tri->auw_Index[0] == pst_Tri->auw_Index[1]) goto FSelBad_Sel;
			if(pst_Tri->auw_Index[0] == pst_Tri->auw_Index[2]) goto FSelBad_Sel;
			if(pst_Tri->auw_Index[1] == pst_Tri->auw_Index[2]) goto FSelBad_Sel;
			continue;
FSelBad_Sel:
			*pc_Sel = 1;
			i_Result = 1;
		}
	}

	return i_Result;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_FDelSel(GEO_tdst_Object *_pst_Obj, BOOL _b_DelVertex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*pst_Tri;
	int									i, i_Elem;
	char								*pc_Sel, *pc_NextSel, *pc_SaveSel;
	LONG								l_Last;
	BOOL								b_strip;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return;
	if(!_pst_Obj->pst_SubObject) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;

	if(_pst_Obj->ulStripFlag & GEO_C_Strip_DataValid)
	{
		b_strip = TRUE;
		GEO_STRIP_Delete(_pst_Obj);
	}
	else
		b_strip = FALSE;

	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;

	pst_Elem = _pst_Obj->dst_Element;
	for(i_Elem = 0; i_Elem < _pst_Obj->l_NbElements; i_Elem++, pst_Elem++)
	{
		pst_Tri = pst_Elem->dst_Triangle;
		pc_NextSel = pc_Sel + pst_Elem->l_NbTriangles;
		pc_SaveSel = pc_Sel;

		l_Last = pst_Elem->l_NbTriangles - 1;

		for(i = 0; i <= l_Last; i++, pst_Tri++, pc_Sel++)
		{
			if(!(*pc_Sel & 1)) continue;

			while((l_Last > i) && (pc_SaveSel[l_Last] & 1)) l_Last--;

			if(i != l_Last) L_memcpy(pst_Tri, pst_Elem->dst_Triangle + l_Last, sizeof(GEO_tdst_IndexedTriangle));
			l_Last--;
		}

		pst_Elem->l_NbTriangles = l_Last + 1;
		pc_Sel = pc_NextSel;
	}
	
	/* loop through element to delete empty element */
	pst_Elem = _pst_Obj->dst_Element;
	for(i_Elem = 0; i_Elem < _pst_Obj->l_NbElements; i_Elem++, pst_Elem++)
	{
		if(pst_Elem->l_NbTriangles) continue;
		if(i_Elem < _pst_Obj->l_NbElements - 1)
		{
			L_memcpy
			(
				pst_Elem,
				_pst_Obj->dst_Element + _pst_Obj->l_NbElements - 1,
				sizeof(GEO_tdst_ElementIndexedTriangles)
			);
		}

		_pst_Obj->l_NbElements--;
	}
	
	/* select and then delete not more used vertices */
	if(_b_DelVertex)
	{
		GEO_SubObject_BuildEdgeData(_pst_Obj);
		GEO_SubObject_BuildFaceData(_pst_Obj);
		GEO_i_SubObject_VSelUnused(_pst_Obj);
		GEO_SubObject_VDelSel(_pst_Obj, NULL);
	}

	/* rebuild data */
	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_SubObject_HideUpdate( _pst_Obj, TRUE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *GEO_SubObject_FGetVContour(GEO_tdst_Object *_pst_Obj, BOOL _b_Border)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*pc_Sel;
	char						*dc_Contour;
	GEO_tdst_SubObject_Edge		*pst_Edge, *pst_Last;
	GEO_tdst_SubObject_Face		*pst_Face, *pst_FLast;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	int							i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Obj) return NULL;
	if(!_pst_Obj->pst_SubObject) return NULL;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return NULL;

	/* alloc vertex selection buffer */
	dc_Contour = (char *) L_malloc(_pst_Obj->l_NbPoints);
	L_memset(dc_Contour, 0, _pst_Obj->l_NbPoints);

	if(_b_Border)
	{
		/*
		 * select all vertex of border edge (edge with only one face) and with a selected
		 * face
		 */
		pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
		pst_Last = pst_Edge + _pst_Obj->pst_SubObject->ul_NbEdges;
		pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
		for(i = 0; pst_Edge < pst_Last; pst_Edge++, i++)
		{
			if(pst_Edge->uw_Index[1] != (USHORT) - 1) continue;

			/* if(!(pc_Sel[pst_Edge->uw_Index[0]] & 1)) continue; */
			dc_Contour[pst_Edge->ul_Point[0]] = 1;
			dc_Contour[pst_Edge->ul_Point[1]] = 1;
		}
	}

	/* parse face */
	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pst_FLast = pst_Face + _pst_Obj->pst_SubObject->ul_NbFaces;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	for(; pst_Face < pst_FLast; pst_Face++, pc_Sel++)
	{
		/* if(*pc_Sel & 1) continue; */
		pst_Triangle = _pst_Obj->dst_Element[pst_Face->uw_Element & 0x7FFF].dst_Triangle + pst_Face->uw_Index;
		dc_Contour[pst_Triangle->auw_Index[0]] |= (*pc_Sel & 1) + 1;
		dc_Contour[pst_Triangle->auw_Index[1]] |= (*pc_Sel & 1) + 1;
		dc_Contour[pst_Triangle->auw_Index[2]] |= (*pc_Sel & 1) + 1;
	}

	for(i = 0; i < _pst_Obj->l_NbPoints; i++)
		if(dc_Contour[i] != 3) dc_Contour[i] = 0;

	return dc_Contour;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_FDetach(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*dc_Contour, *pc_Sel;
	int							*di_New;
	GEO_tdst_SubObject_Face		*pst_Face;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	int							i, i_Count, i_Nb;
	WOR_tdst_Update_RLI			st_UpdateRLIData;
	ULONG						*pul_RLI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	dc_Contour = GEO_SubObject_FGetVContour(_pst_Obj, FALSE);
	if(!dc_Contour) return;

	i_Nb = _pst_Obj->l_NbPoints;
	di_New = (int *) L_malloc(sizeof(int) * (i_Nb + 1));
	i_Count = 0;
	for(i = 0; i < i_Nb; i++)
	{
		di_New[i] = i;
		if(dc_Contour[i] == 3) di_New[i] = _pst_Obj->l_NbPoints + i_Count++;
	}

	di_New[i_Nb] = -1;

	if(i_Count == 0) goto FDetach_End;

	/* add point */
	GEO_AddSeveralPoint(_pst_Obj, i_Count);

	/* init point */
	i_Count = 0;
	for(i = 0; i < i_Nb; i++)
	{
		if(dc_Contour[i] != 3) continue;
		MATH_CopyVector(VCast(&_pst_Obj->dst_Point[i_Nb + i_Count++]), VCast(&_pst_Obj->dst_Point[i]));
	}

	if(pul_RLI = _pst_Obj->dul_PointColors)
	{
		i_Count = 0;
		for(i = 0; i < i_Nb; i++)
		{
			if(dc_Contour[i] != 3) continue;
			pul_RLI[1 + i_Nb + i_Count++] = pul_RLI[1 + i];
		}
	}

	/* update RLI of game object */
	st_UpdateRLIData.p_Geo = _pst_Obj;
	st_UpdateRLIData.l_Op = WOR_Update_RLI_AddSome;
	st_UpdateRLIData.l_NbAdded = i_Count;
	st_UpdateRLIData.l_OldNbPoints = i_Nb;
	st_UpdateRLIData.l_Ind1 = (LONG) di_New;
	WOR_Universe_Update_RLI(&st_UpdateRLIData);

	/* update eventually skin */
	GEO_SKN_UpdateAfterVertexEdition(_pst_Obj, &st_UpdateRLIData);

	/* detach faces */
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pc_Sel++, pst_Face++)
	{
		if(*pc_Sel & 1)
		{
			pst_Triangle = _pst_Obj->dst_Element[pst_Face->uw_Element & 0x7FFF].dst_Triangle + pst_Face->uw_Index;
			pst_Triangle->auw_Index[0] = di_New[pst_Triangle->auw_Index[0]];
			pst_Triangle->auw_Index[1] = di_New[pst_Triangle->auw_Index[1]];
			pst_Triangle->auw_Index[2] = di_New[pst_Triangle->auw_Index[2]];
		}
	}

	/* recompute */
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
	for(i = 0; i < i_Nb; i++)
	{
		if(dc_Contour[i] == 3) _pst_Obj->pst_SubObject->dc_VSel[i] = 0;
	}

	L_memset(_pst_Obj->pst_SubObject->dc_VSel + i_Nb, 1, i_Count);
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);

	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_AllocBuffers(_pst_Obj);
	GEO_SubObject_VUpdateFSel(_pst_Obj);
    GEO_SubObject_HideUpdate( _pst_Obj, TRUE );

FDetach_End:
	L_free(dc_Contour);
	L_free(di_New);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG F3D_ulGetCopyMask(BOOL);

void GEO_SubObject_FAttach(GEO_tdst_Object *_pst_Tgt, GEO_tdst_Object *_pst_Src, ULONG *_pul_SrcRLI, MATH_tdst_Matrix *M, OBJ_tdst_GameObject *_pst_GO_Dst, OBJ_tdst_GameObject *_pst_GO_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*pc_Sel, *pc_Last;
	int							*di_New, *di_UV;
	GEO_tdst_SubObject_Face		*pst_Face;
	GEO_tdst_IndexedTriangle	st_Triangle, *pst_Triangle;
	int							i, j, i_Count, i_UVNb, i_Nb, id, iAddedPointNb;
	WOR_tdst_Update_RLI			st_UpdateRLIData;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

    if (!_pst_Src->pst_SubObject)
    {
        ERR_X_ErrorAssert(0, "GEO_SubObject_FAttach: Copied sub object is not valid", "");
        return;
    }

	pc_Sel = _pst_Src->pst_SubObject->dc_VSel;
	pc_Last = pc_Sel + _pst_Src->l_NbPoints;
	i_Count = 0;
	while(pc_Sel < pc_Last)
		if(*pc_Sel++ &1) i_Count++;

    iAddedPointNb = i_Count;

	if(i_Count == 0) return;

	/* add point */
	i_Nb = _pst_Tgt->l_NbPoints;
	di_New = (int *) L_malloc(_pst_Src->l_NbPoints * 4);
	L_memset(di_New, -1, _pst_Src->l_NbPoints * 4);
    i_UVNb = _pst_Src->l_NbUVs;
	if (i_UVNb)
	{
		di_UV = (int *) L_malloc(i_UVNb * 4);
		L_memset(di_UV, -1, i_UVNb * 4);
	}
	else
		di_UV = NULL;

	GEO_AddSeveralPoint(_pst_Tgt, i_Count);

	/* init point */
	pc_Sel = _pst_Src->pst_SubObject->dc_VSel;
	i_Count = i_Nb;
	for(i = 0; i < _pst_Src->l_NbPoints; i++)
	{
		if(pc_Sel[i] & 1)
		{
			MATH_TransformVertex(_pst_Tgt->dst_Point + i_Count, M, &_pst_Src->dst_Point[i]);
			MATH_TransformVector(_pst_Tgt->dst_PointNormal + i_Count, M, &_pst_Src->dst_PointNormal[i]);
			di_New[i] = i_Count;
			i_Count++;
		}
	}

	/* update RLI of game object */
	st_UpdateRLIData.p_Geo = _pst_Tgt;
	st_UpdateRLIData.l_Op = WOR_Update_RLI_AddSome;
	st_UpdateRLIData.l_NbAdded = i_Count;
	st_UpdateRLIData.l_OldNbPoints = i_Nb;
	st_UpdateRLIData.l_Ind1 = NULL;

    // Copy ponderation and rli only if the whole _pst_Src is copied.
    if (_pst_Src->l_NbPoints == iAddedPointNb )
    {
        BOOL bIsSkinned = GEO_SKN_IsSkinned(_pst_Src);
        BOOL bHasSPG2 = FALSE;
        ULONG ulCopyMask;

        if (bIsSkinned && _pst_GO_Src->pst_Extended && _pst_GO_Src->pst_Extended->pst_Modifiers)
        {
            struct MDF_tdst_Modifier_ *pstModifierSrc = _pst_GO_Src->pst_Extended->pst_Modifiers;
            while (pstModifierSrc)
            {
                if (pstModifierSrc->i->ul_Type == MDF_C_Modifier_SpriteMapper2)
                {
                    bHasSPG2 = TRUE;
                    break;
                }
                pstModifierSrc = pstModifierSrc->pst_Next;
            }
        }

        ulCopyMask = F3D_ulGetCopyMask(bHasSPG2);

        if (ulCopyMask & 1)
            st_UpdateRLIData.aul_AddedRLI = (_pul_SrcRLI? _pul_SrcRLI : _pst_Src->dul_PointColors);

        if (ulCopyMask & 2)
        {
            GEO_SKN_Expand(_pst_Src);
            st_UpdateRLIData.p_AddedSKN_Objectponderation = _pst_Src->p_SKN_Objectponderation;
            st_UpdateRLIData.pst_GoSrc = _pst_GO_Src;
            st_UpdateRLIData.pst_GoDst = _pst_GO_Dst;
        }
    }
	WOR_Universe_Update_RLI(&st_UpdateRLIData);

	/* update eventually skin */
	GEO_SKN_UpdateAfterVertexEdition(_pst_Tgt, &st_UpdateRLIData);

	/* add UVs */
	pc_Sel = _pst_Src->pst_SubObject->dc_FSel;
	for(i = 0; i < (int) _pst_Src->pst_SubObject->ul_NbFaces; i++)
	{
		if(!(pc_Sel[i] & 1)) continue;
		pst_Face = &_pst_Src->pst_SubObject->dst_Faces[i];
		pst_Triangle = _pst_Src->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;

		if (di_UV)
		{
			di_UV[pst_Triangle->auw_UV[0]] = 1;
			di_UV[pst_Triangle->auw_UV[1]] = 1;
			di_UV[pst_Triangle->auw_UV[2]] = 1;
		}
	}

	for(i = 0; i < i_UVNb; i++)
	{
		if(di_UV[i] != 1) continue;
		di_UV[i] = _pst_Tgt->l_NbUVs;
		GEO_AddUV(_pst_Tgt, _pst_Src->dst_UV[i].fU, _pst_Src->dst_UV[i].fV);
	}

	/* add faces */
	pc_Sel = _pst_Src->pst_SubObject->dc_FSel;
	for(i = 0; i < (int) _pst_Src->pst_SubObject->ul_NbFaces; i++)
	{
		if(!(pc_Sel[i] & 1)) continue;

		pst_Face = &_pst_Src->pst_SubObject->dst_Faces[i];

		/* cherche l'élément avec même ID */
		id = _pst_Src->dst_Element[pst_Face->uw_Element].l_MaterialId;

		for(j = 0; j < _pst_Tgt->l_NbElements; j++)
		{
			if(_pst_Tgt->dst_Element[j].l_MaterialId == id) break;
		}

		if(j == _pst_Tgt->l_NbElements) GEO_AddElement(_pst_Tgt, id, 0);

		pst_Triangle = _pst_Src->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
		st_Triangle.auw_Index[0] = di_New[pst_Triangle->auw_Index[0]];
		st_Triangle.auw_Index[1] = di_New[pst_Triangle->auw_Index[1]];
		st_Triangle.auw_Index[2] = di_New[pst_Triangle->auw_Index[2]];
		if (di_UV)
		{
			st_Triangle.auw_UV[0] = di_UV[pst_Triangle->auw_UV[0]];
			st_Triangle.auw_UV[1] = di_UV[pst_Triangle->auw_UV[1]];
			st_Triangle.auw_UV[2] = di_UV[pst_Triangle->auw_UV[2]];
		}
		else
		{
			st_Triangle.auw_UV[0] = 0;
			st_Triangle.auw_UV[1] = 0;
			st_Triangle.auw_UV[2] = 0;
		}
		GEO_AddTriangle(&_pst_Tgt->dst_Element[j], &st_Triangle);
	}

	_pst_Tgt->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildFaceData(_pst_Tgt);
	GEO_SubObject_BuildEdgeData(_pst_Tgt);
	GEO_SubObject_AllocBuffers(_pst_Tgt);
    GEO_SubObject_HideUpdate( _pst_Tgt, TRUE );

	if ( di_UV )
		L_free(di_UV);
	L_free(di_New);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_FDuplicate(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			st_NewTri, *pst_Triangle;
	GEO_tdst_SubObject_Face				*pst_Face, *pst_Last;
	int									*pi_New;
	char								*pc_Sel;
	int									i_Nb, i_Last;
	WOR_tdst_Update_RLI					st_UpdateRLIData;
	int									i;
	ULONG								*pul_RLI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	if(!_pst_Obj) return;
	if(_pst_Obj->st_Id.i->ul_Type != GRO_Geometric) return;
	if(!_pst_Obj->pst_SubObject) return;

	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	i_Last = _pst_Obj->l_NbPoints;
	pi_New = (int *) L_malloc(4 * (i_Last + 1));
	pi_New[i_Last] = -1;

	/* first count selected vertices */
	for(i = 0, i_Nb = 0; i < i_Last; i++)
	{
		pi_New[i] = i;
		if(pc_Sel[i] & 1)
		{
			pi_New[i] = i_Last + i_Nb;
			i_Nb++;
		}
	}

	/* test number : no vertex selected => nothing to do */
	if(i_Nb == 0)
	{
		L_free(pi_New);
		return;
	}

	/* add new point */
	GEO_AddSeveralPoint(_pst_Obj, i_Nb);

	pc_Sel = _pst_Obj->pst_SubObject->dc_VSel;
	for(i = 0, i_Nb = 0; i < i_Last; i++)
	{
		if(pc_Sel[i] & 1)
		{
			MATH_CopyVector(VCast(&_pst_Obj->dst_Point[i_Last + i_Nb++]), VCast(&_pst_Obj->dst_Point[i]));
		}
	}

	if(pul_RLI = _pst_Obj->dul_PointColors)
	{
		for(i = 0, i_Nb = 0; i < i_Last; i++)
		{
			if(!(pc_Sel[i] & 1)) continue;
			pul_RLI[1 + i_Last + i_Nb++] = pul_RLI[1 + i];
		}
	}

	/* update eventually RLI */
	st_UpdateRLIData.p_Geo = _pst_Obj;
	st_UpdateRLIData.l_Op = WOR_Update_RLI_AddSome;
	st_UpdateRLIData.l_NbAdded = i_Nb;
	st_UpdateRLIData.l_OldNbPoints = i_Last;
	st_UpdateRLIData.l_Ind1 = (LONG) pi_New;
	WOR_Universe_Update_RLI(&st_UpdateRLIData);

	/* update eventually skin */
	GEO_SKN_UpdateAfterVertexEdition(_pst_Obj, &st_UpdateRLIData);

	/* create one new triangle for each selected triangle */
	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pst_Last = pst_Face + _pst_Obj->pst_SubObject->ul_NbFaces;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	for(; pst_Face < pst_Last; pst_Face++, pc_Sel++)
	{
		if(*pc_Sel & 1)
		{
			pst_Element = _pst_Obj->dst_Element + pst_Face->uw_Element;
			pst_Triangle = pst_Element->dst_Triangle + pst_Face->uw_Index;

			st_NewTri.auw_Index[0] = (USHORT) pi_New[pst_Triangle->auw_Index[0]];
			st_NewTri.auw_Index[1] = (USHORT) pi_New[pst_Triangle->auw_Index[1]];
			st_NewTri.auw_Index[2] = (USHORT) pi_New[pst_Triangle->auw_Index[2]];
			st_NewTri.auw_UV[0] = pst_Triangle->auw_UV[0];
			st_NewTri.auw_UV[1] = pst_Triangle->auw_UV[1];
			st_NewTri.auw_UV[2] = pst_Triangle->auw_UV[2];

			GEO_AddTriangle(pst_Element, &st_NewTri);
		}
	}

	L_free(pi_New);

	/* update selection */
	L_memset(_pst_Obj->pst_SubObject->dc_VSel, 0, i_Last);
	L_memset(_pst_Obj->pst_SubObject->dc_VSel + i_Last, 1, i_Nb);
	
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);

	/* update sub object data */
	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_VUpdateFSel(_pst_Obj);
	GEO_SubObject_AllocBuffers(_pst_Obj);
    GEO_SubObject_HideUpdate( _pst_Obj, TRUE );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_FExtrude(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char								*dc_Contour, *pc_Sel;
	char								*dc_EdgeExtrude;
	int									*di_New, *di_NewUV;
	GEO_tdst_UV							*pst_UV;
	GEO_tdst_SubObject_Face				*pst_Face;
	GEO_tdst_SubObject_Edge				*pst_Edge;
	GEO_tdst_ElementIndexedTriangles	*pst_Element;
	GEO_tdst_IndexedTriangle			*pst_Triangle, st_NewTri;
	int									i, j, k, l, i_Count, i_Nb, i_NbUVs, uv[2];
	WOR_tdst_Update_RLI					st_UpdateRLIData;
	ULONG								*pul_RLI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	dc_Contour = GEO_SubObject_FGetVContour(_pst_Obj, TRUE);
	if(!dc_Contour) return;

	i_Nb = _pst_Obj->l_NbPoints;
	di_New = (int *) L_malloc(sizeof(int) * (i_Nb + 1));
	i_Count = 0;
	for(i = 0; i < i_Nb; i++)
	{
		di_New[i] = i;
		if(dc_Contour[i]) di_New[i] = _pst_Obj->l_NbPoints + i_Count++;
	}

	di_New[i_Nb] = -1;

	i_NbUVs = _pst_Obj->l_NbUVs;
	di_NewUV = (int *) L_malloc(sizeof(int) * (i_NbUVs + 6 * i_Count));
	for(i = 0; i < _pst_Obj->l_NbUVs; i++) di_NewUV[i] = -1;

	if(i_Count == 0) goto FExtrude_End;

	/* add point */
	GEO_AddSeveralPoint(_pst_Obj, i_Count);

	/* init point */
	i_Count = 0;
	for(i = 0; i < i_Nb; i++)
	{
		if(!dc_Contour[i]) continue;
		MATH_CopyVector(VCast(&_pst_Obj->dst_Point[i_Nb + i_Count++]), VCast(&_pst_Obj->dst_Point[i]));
	}

	if(pul_RLI = _pst_Obj->dul_PointColors)
	{
		i_Count = 0;
		for(i = 0; i < i_Nb; i++)
		{
			if(!dc_Contour[i]) continue;
			pul_RLI[1 + i_Nb + i_Count++] = pul_RLI[i + 1];
		}
	}

	/* update RLI of game object */
	st_UpdateRLIData.p_Geo = _pst_Obj;
	st_UpdateRLIData.l_Op = WOR_Update_RLI_AddSome;
	st_UpdateRLIData.l_NbAdded = i_Count;
	st_UpdateRLIData.l_OldNbPoints = i_Nb;
	st_UpdateRLIData.l_Ind1 = (LONG) di_New;
	WOR_Universe_Update_RLI(&st_UpdateRLIData);

	/* update eventually skin */
	GEO_SKN_UpdateAfterVertexEdition(_pst_Obj, &st_UpdateRLIData);

	/* detach faces */
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pc_Sel++, pst_Face++)
	{
		if(*pc_Sel & 1)
		{
			pst_Triangle = _pst_Obj->dst_Element[pst_Face->uw_Element & 0x7FFF].dst_Triangle + pst_Face->uw_Index;
			pst_Triangle->auw_Index[0] = di_New[pst_Triangle->auw_Index[0]];
			pst_Triangle->auw_Index[1] = di_New[pst_Triangle->auw_Index[1]];
			pst_Triangle->auw_Index[2] = di_New[pst_Triangle->auw_Index[2]];
		}
	}

	/* search for edges that are on contour and build new faces */
	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;
	pc_Sel = _pst_Obj->pst_SubObject->dc_FSel;
	dc_EdgeExtrude = (char *) L_malloc(_pst_Obj->pst_SubObject->ul_NbEdges);
	L_memset(dc_EdgeExtrude, 0, _pst_Obj->pst_SubObject->ul_NbEdges);

	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbEdges; i++, pst_Edge++)
	{
		if((dc_Contour[pst_Edge->ul_Point[0]] & 1) && (dc_Contour[pst_Edge->ul_Point[1]] & 1))
		{
			if(pst_Edge->uw_Index[1] == (USHORT) - 1)
				dc_EdgeExtrude[i] = 1;
			else if((pc_Sel[pst_Edge->uw_Index[0]] & 1) ^ (pc_Sel[pst_Edge->uw_Index[1]] & 1)) dc_EdgeExtrude[i] = 1;
		}
	}

	pst_Element = _pst_Obj->dst_Element;
	pst_Edge = _pst_Obj->pst_SubObject->dst_Edge;

	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbEdges; i++, pst_Edge++)
	{
		if(dc_EdgeExtrude[i])
		{
			/* search index of triangle used by edge extruded */
			for(k = 0; k < 2; k++)
			{
				if(pst_Edge->ul_Triangle[k] == (ULONG) - 1) break;
				pst_Element = _pst_Obj->dst_Element + ((pst_Edge->ul_Triangle[k] >> 16) & 0xFFF);
				pst_Triangle = pst_Element->dst_Triangle + (pst_Edge->ul_Triangle[k] & 0xFFFF);
				for(l = 0; l < 3; l++)
				{
					if(di_New[pst_Edge->ul_Point[0]] == (int) pst_Triangle->auw_Index[l])
					{
						uv[0] = pst_Triangle->auw_UV[l];
						break;
					}
				}

				if(l < 3) break;
			}

			j = (di_New[pst_Edge->ul_Point[1]] == pst_Triangle->auw_Index[(l + 1) % 3]) ? 0 : 1;
			k = 1 - j;
			uv[1] = pst_Triangle->auw_UV[(l + j + 1) % 3];

			/* create new uv */
			for(l = 0; l < 2; l++)
			{
				if(uv[l] >= i_NbUVs) continue;
				if(di_NewUV[uv[l]] != -1) continue;
				di_NewUV[uv[l]] = _pst_Obj->l_NbUVs;
				di_NewUV[_pst_Obj->l_NbUVs] = _pst_Obj->l_NbUVs;
				pst_UV = _pst_Obj->dst_UV + uv[l];
				GEO_AddUV(_pst_Obj, pst_UV->fU, pst_UV->fV);
			}

			for(l = 0; l < 3; l++)
			{
				if(pst_Triangle->auw_UV[l] >= i_NbUVs) continue;
				if(di_NewUV[pst_Triangle->auw_UV[l]] != -1)
					pst_Triangle->auw_UV[l] = di_NewUV[pst_Triangle->auw_UV[l]];
			}

			st_NewTri.auw_Index[0] = (USHORT) pst_Edge->ul_Point[j];
			st_NewTri.auw_Index[1] = (USHORT) pst_Edge->ul_Point[k];
			st_NewTri.auw_Index[2] = (USHORT) di_New[pst_Edge->ul_Point[k]];
			// ColMap have no UVs
			if(_pst_Obj->l_NbUVs)
			{
				st_NewTri.auw_UV[0] = uv[j];
				st_NewTri.auw_UV[1] = uv[k];
				st_NewTri.auw_UV[2] = di_NewUV[uv[k]];
			}
			GEO_AddTriangle(pst_Element, &st_NewTri);

			st_NewTri.auw_Index[0] = (USHORT) di_New[pst_Edge->ul_Point[k]];
			st_NewTri.auw_Index[1] = (USHORT) di_New[pst_Edge->ul_Point[j]];
			st_NewTri.auw_Index[2] = (USHORT) pst_Edge->ul_Point[j];
			if(_pst_Obj->l_NbUVs)
			{
				st_NewTri.auw_UV[0] = di_NewUV[uv[k]];
				st_NewTri.auw_UV[1] = di_NewUV[uv[j]];
				st_NewTri.auw_UV[2] = uv[j];
			}
			GEO_AddTriangle(pst_Element, &st_NewTri);
		}
	}

	L_free(dc_EdgeExtrude);

	/* recompute */
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);
	for(i = 0; i < i_Nb; i++)
	{
		if(dc_Contour[i]) _pst_Obj->pst_SubObject->dc_VSel[i] = 0;
	}

	L_memset(_pst_Obj->pst_SubObject->dc_VSel + i_Nb, 1, i_Count);
	GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel);

	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildFaceData(_pst_Obj);
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_VUpdateFSel(_pst_Obj);
	GEO_SubObject_AllocBuffers(_pst_Obj);
    GEO_SubObject_HideUpdate( _pst_Obj, TRUE );

FExtrude_End:
	L_free(dc_Contour);
	L_free(di_New);
	L_free(di_NewUV);
}

/*
 =======================================================================================================================
    Cut face
 =======================================================================================================================
 */
int GEO_i_SubObject_FCut
(
	GEO_tdst_Object		*_pst_Obj,
	MATH_tdst_Matrix	*M,
	MATH_tdst_Vector	*A,
	MATH_tdst_Vector	*B,
	int					elem,
	int					triangle
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_IndexedTriangle	*T, st_NewTri1, st_NewTri2;
	MATH_tdst_Vector			V[3], W[2], N, D, H;
	float						a, b, f, x, y;
	WOR_tdst_Update_RLI			st_UpdateRLIData;
	ULONG						ul_C0, ul_C1, ul_C2;
    GEO_Vertex                  *Vertex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    L_memset(&st_UpdateRLIData,0,sizeof(WOR_tdst_Update_RLI));

	if(!_pst_Obj) return 0;
	if((elem < 0) || (elem >= _pst_Obj->l_NbElements)) return 0;
	if((triangle < 0) || (triangle >= _pst_Obj->dst_Element[elem].l_NbTriangles)) return 0;

    /* get source coordinates */
    if (_pst_Obj->pst_SubObject->dst_PointTransformed)
        Vertex = _pst_Obj->pst_SubObject->dst_PointTransformed;
    else
        Vertex = _pst_Obj->dst_Point;

	/* Get 3D coordinates of triangles vertices */
	T = _pst_Obj->dst_Element[elem].dst_Triangle + triangle;

	MATH_TransformVertex(V + 0, M, Vertex + T->auw_Index[0]);
	MATH_TransformVertex(V + 1, M, Vertex + T->auw_Index[1]);
	MATH_TransformVertex(V + 2, M, Vertex + T->auw_Index[2]);

	/* compute plane vector */
	MATH_SubVector(W + 0, V + 1, V);
	MATH_SubVector(W + 1, V + 2, V);

	/* compute and test plane normal */
	MATH_CrossProduct(&N, W, W + 1);
	if(MATH_b_NulVectorWithEpsilon(&N, 0.000001f)) return 0;
    MATH_NormalizeEqualVector( &N );

	/* compute H : intersection of line and plan */
	MATH_SubVector(&D, B, A);
	MATH_SubVector(&H, V, A);
	f = MATH_f_DotProduct(&D, &N);
	if(fNulWithEpsilon(f, 0.0001f)) return 0;

	f = MATH_f_DotProduct(&H, &N) / f;
	MATH_AddScaleVector(&H, A, &D, f);

	/* compute coordinates of H in V, W, W + 1 system axis */
	MATH_SubVector(&D, &H, V);

	f = MATH_f_DotProduct(W, W + 1);
	if(fNulWithEpsilon(f, 0.000001f))
	{
		x = MATH_f_DotProduct(&D, W) / MATH_f_DotProduct(W, W);
	}
	else
	{
		b = MATH_f_SqrVector(W + 1);
		a = MATH_f_DotProduct(&D, W + 1) / b;
		b = f / b;

		a = MATH_f_DotProduct(&D, W) - a * f;
		b = MATH_f_DotProduct(W, W) - b * f;

		x = a / b;
	}

	y = (MATH_f_DotProduct(&D, W + 1) - x * f) / MATH_f_SqrVector(W + 1);

	/* Add a point */
	MATH_SubVector(W, _pst_Obj->dst_Point + T->auw_Index[1], _pst_Obj->dst_Point + T->auw_Index[0]);
	MATH_SubVector(W + 1, _pst_Obj->dst_Point + T->auw_Index[2], _pst_Obj->dst_Point + T->auw_Index[0]);

	MATH_AddScaleVector(&H, VCast(_pst_Obj->dst_Point + T->auw_Index[0]), W, x);
	MATH_AddScaleVector(&H, &H, W + 1, y);

	MATH_CrossProduct(&N, W, W + 1);
	MATH_NormalizeEqualVector(&N);

	GEO_AddPoint(_pst_Obj, H.x, H.y, H.z, N.x, N.y, N.z);

	/* Update eventually RLI */
	if(_pst_Obj->dul_PointColors)
	{
		ul_C0 = _pst_Obj->dul_PointColors[1 + T->auw_Index[0]];
		ul_C1 = _pst_Obj->dul_PointColors[1 + T->auw_Index[1]];
		ul_C2 = _pst_Obj->dul_PointColors[1 + T->auw_Index[2]];
		_pst_Obj->dul_PointColors[_pst_Obj->l_NbPoints] = COLOR_ul_Blend3(ul_C0, ul_C1, ul_C2, 1 - x - y, x, y);
	}

	/* update RLI of GameObject that used this object */
	st_UpdateRLIData.p_Geo = _pst_Obj;
	st_UpdateRLIData.l_Op = WOR_Update_RLI_Add | WOR_Update_RLI_Barycentre;
	st_UpdateRLIData.l_Ind0 = T->auw_Index[0];
	st_UpdateRLIData.l_Ind1 = T->auw_Index[1];
	st_UpdateRLIData.l_Ind2 = T->auw_Index[2];
	st_UpdateRLIData.f0 = 1 - x - y;
	st_UpdateRLIData.f1 = x;
	st_UpdateRLIData.f2 = y;
	WOR_Universe_Update_RLI(&st_UpdateRLIData);

	/* update eventually skin */
	GEO_SKN_UpdateAfterVertexEdition(_pst_Obj, &st_UpdateRLIData);

	if(_pst_Obj->dst_UV)
	{
		/* add one UV */
		MATH_ScaleVector(&H, (MATH_tdst_Vector *) &_pst_Obj->dst_UV[T->auw_UV[0]], (1 - x - y));
		MATH_AddScaleVector(&H, &H, (MATH_tdst_Vector *) &_pst_Obj->dst_UV[T->auw_UV[1]], x);
		MATH_AddScaleVector(&H, &H, (MATH_tdst_Vector *) &_pst_Obj->dst_UV[T->auw_UV[2]], y);
		GEO_AddUV(_pst_Obj, H.x, H.y);
	}

	/* update triangles */
	st_NewTri1 = *T;
	st_NewTri2 = *T;

	T->auw_Index[2] = (unsigned short)(_pst_Obj->l_NbPoints - 1);
	if(_pst_Obj->dst_UV)
		T->auw_UV[2] = (unsigned short)(_pst_Obj->l_NbUVs - 1);
	else
		T->auw_UV[2] = 0;

	st_NewTri1.auw_Index[0] = (unsigned short)(_pst_Obj->l_NbPoints - 1);
	if(_pst_Obj->dst_UV)
		st_NewTri1.auw_UV[0] = (unsigned short)(_pst_Obj->l_NbUVs - 1);
	else
		st_NewTri1.auw_UV[0] = 0;
	GEO_AddTriangle(&_pst_Obj->dst_Element[elem], &st_NewTri1);

	st_NewTri2.auw_Index[1] = (unsigned short)(_pst_Obj->l_NbPoints - 1);
	if(_pst_Obj->dst_UV)
		st_NewTri2.auw_UV[1] = (unsigned short)(_pst_Obj->l_NbUVs - 1);
	else
		st_NewTri2.auw_UV[1] = 0;
	GEO_AddTriangle(&_pst_Obj->dst_Element[elem], &st_NewTri2);

	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_BuildFaceData(_pst_Obj);

	/* GEO_ComputeNormalsOfSelected(_pst_Obj, _pst_Obj->pst_SubObject->dc_VSel); */
	GEO_SubObject_AllocBuffers(_pst_Obj);
    GEO_SubObject_HideUpdate( _pst_Obj, TRUE );
	return 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_FSelectExt(GEO_tdst_Object *_pst_Obj, int _i_IDLocked )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, Loop, Sel;
	char						*pc_FSel, *pc_VSel;
	GEO_tdst_SubObject_Face		*pst_Face;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GEO_SubObject_FUpdateVSel(_pst_Obj);

	pc_VSel = _pst_Obj->pst_SubObject->dc_VSel;
	Loop = 0;
	Sel = 0;

	while(1)
	{
		pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
		pc_FSel = _pst_Obj->pst_SubObject->dc_FSel;

		for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pc_FSel++, pst_Face++)
		{
			if(*pc_FSel & 0x81) continue;
			
			if ( (_i_IDLocked != -1) && (_pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId != _i_IDLocked ) )
				continue;

			pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
			if
			(
				(pc_VSel[pst_Triangle->auw_Index[0]] & 1)
			||	(pc_VSel[pst_Triangle->auw_Index[1]] & 1)
			||	(pc_VSel[pst_Triangle->auw_Index[2]] & 1)
			)
			{
				pc_FSel[0] = 1;
				pc_VSel[pst_Triangle->auw_Index[0]] = 1;
				pc_VSel[pst_Triangle->auw_Index[1]] = 1;
				pc_VSel[pst_Triangle->auw_Index[2]] = 1;
				Loop = Sel = 1;
			}
		}

		if(Loop == 0) return Sel;
		Loop = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_FSelectExtUV(GEO_tdst_Object *_pst_Obj )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, j, Loop, LoopAll, Sel, i_ID;
	char						*pc_FSel, *pc_VSel;
	GEO_tdst_SubObject_Face		*pst_Face;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	USHORT						*puw_UVIndex, uv;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* alloc */
	puw_UVIndex = (USHORT *) L_malloc( sizeof( USHORT ) * _pst_Obj->l_NbPoints );
	pc_VSel = (char *) L_malloc( _pst_Obj->l_NbPoints );
	
	/* init */
GEO_i_SubObject_FSelectExtUV_LoopAll:
	//pc_VSel = _pst_Obj->pst_SubObject->dc_VSel;
	Loop = 0;
	Sel = 0;
	L_memset( puw_UVIndex, 0xFF, sizeof( USHORT ) * _pst_Obj->l_NbPoints );
	L_memset( pc_VSel, 0, _pst_Obj->l_NbPoints );
	
	pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pc_FSel = _pst_Obj->pst_SubObject->dc_FSel;
	LoopAll = 0;
	i_ID = -1;
	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pc_FSel++, pst_Face++)
	{
		if(!(*pc_FSel  & 1)) continue;
		if (*pc_FSel & 0x10) continue;
		
		if (i_ID == -1)
			i_ID = _pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId;
			
		if (i_ID == _pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId)
		{
			pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
			
			for (j = 0; j < 3; j++)
			{
				uv = puw_UVIndex[ pst_Triangle->auw_Index[ j ] ];
				if ( (uv != 0xFFFF) && (uv != pst_Triangle->auw_UV[ j ] ) )
					break;
			}
			if (j != 3)
				LoopAll = 1;
			else
			{
				*pc_FSel |= 0x10;
				puw_UVIndex[ pst_Triangle->auw_Index[ 0 ] ] = pst_Triangle->auw_UV[ 0 ];
				puw_UVIndex[ pst_Triangle->auw_Index[ 1 ] ] = pst_Triangle->auw_UV[ 1 ];
				puw_UVIndex[ pst_Triangle->auw_Index[ 2 ] ] = pst_Triangle->auw_UV[ 2 ];
				pc_VSel[ pst_Triangle->auw_Index[ 0 ] ] = 1;
				pc_VSel[ pst_Triangle->auw_Index[ 1 ] ] = 1;
				pc_VSel[ pst_Triangle->auw_Index[ 2 ] ] = 1;
			}
		}
		else
			LoopAll = 1;
	}
	GEO_SubObject_FUpdateVSel(_pst_Obj);
	
	if (i_ID == -1)
		goto GEO_i_SubObject_FSelectExtUV_end;

	/* loop */
	while(1)
	{
		pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
		pc_FSel = _pst_Obj->pst_SubObject->dc_FSel;

		for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pc_FSel++, pst_Face++)
		{
			if(*pc_FSel & 0x81) continue;
			
			if ( _pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId != i_ID )
				continue;

			pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
			if
			(
				(pc_VSel[pst_Triangle->auw_Index[0]] & 1)
			||	(pc_VSel[pst_Triangle->auw_Index[1]] & 1)
			||	(pc_VSel[pst_Triangle->auw_Index[2]] & 1)
			)
			{
				for (j = 0; j < 3; j++)
				{
					uv = puw_UVIndex[ pst_Triangle->auw_Index[ j ] ];
					if ( (uv != 0xFFFF) && (uv != pst_Triangle->auw_UV[ j ] ) )
						break;
				}
				if (j == 3)
				{
					pc_FSel[0] = 0x11;
					pc_VSel[pst_Triangle->auw_Index[0]] = 1;
					pc_VSel[pst_Triangle->auw_Index[1]] = 1;
					pc_VSel[pst_Triangle->auw_Index[2]] = 1;
					Loop = Sel = 1;
					puw_UVIndex[ pst_Triangle->auw_Index[ 0 ] ] = pst_Triangle->auw_UV[ 0 ];
					puw_UVIndex[ pst_Triangle->auw_Index[ 1 ] ] = pst_Triangle->auw_UV[ 1 ];
					puw_UVIndex[ pst_Triangle->auw_Index[ 2 ] ] = pst_Triangle->auw_UV[ 2 ];
				}
			}
		}

		if(Loop == 0) goto GEO_i_SubObject_FSelectExtUV_end;
		Loop = 0;
	}
	

GEO_i_SubObject_FSelectExtUV_end:

	if (LoopAll)
		goto GEO_i_SubObject_FSelectExtUV_LoopAll;
	
	GEO_SubObject_FUpdateVSel(_pst_Obj);
	pc_FSel = _pst_Obj->pst_SubObject->dc_FSel;
	for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pc_FSel++)
		*pc_FSel &= ~0x10;
	L_free( puw_UVIndex );
	L_free( pc_VSel );
	return Sel;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_i_SubObject_FSelectNear(GEO_tdst_Object *_pst_Obj, int _i_ByEdge, int _i_IDLocked )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, Sel, Count;
	char						*pc_FSel, *pc_VSel;
	GEO_tdst_SubObject_Face		*pst_Face;
	GEO_tdst_IndexedTriangle	*pst_Triangle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GEO_SubObject_FUpdateVSel(_pst_Obj);

	pc_VSel = _pst_Obj->pst_SubObject->dc_VSel;
	Sel = 0;

    pst_Face = _pst_Obj->pst_SubObject->dst_Faces;
	pc_FSel = _pst_Obj->pst_SubObject->dc_FSel;

    if (_i_ByEdge)
    {
        for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pc_FSel++, pst_Face++)
	    {
	        if(*pc_FSel & 0x81) continue;
	        
	        if ( (_i_IDLocked != -1) && (_pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId != _i_IDLocked ) )
				continue;

		    pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
            Count = (pc_VSel[pst_Triangle->auw_Index[0]] & 1);
            Count += (pc_VSel[pst_Triangle->auw_Index[1]] & 1);
            Count += (pc_VSel[pst_Triangle->auw_Index[2]] & 1);
		    if (Count >= 2)
            {
			    pc_FSel[0] = 1;
			    Sel = 1;
		    }
        }
    }
    else
    {
	    for(i = 0; i < (int) _pst_Obj->pst_SubObject->ul_NbFaces; i++, pc_FSel++, pst_Face++)
	    {
	        if(*pc_FSel & 0x81) continue;
	        
   	        if ( (_i_IDLocked != -1) && (_pst_Obj->dst_Element[pst_Face->uw_Element].l_MaterialId != _i_IDLocked ) )
				continue;

		    pst_Triangle = &_pst_Obj->dst_Element[pst_Face->uw_Element].dst_Triangle[pst_Face->uw_Index];
		    if
		    (
		        (pc_VSel[pst_Triangle->auw_Index[0]] & 1)
		    ||	(pc_VSel[pst_Triangle->auw_Index[1]] & 1)
		    ||	(pc_VSel[pst_Triangle->auw_Index[2]] & 1)
		    )
		    {
			    pc_FSel[0] = 1;
			    Sel = 1;
		    }
        }
    }

    GEO_SubObject_FUpdateVSel( _pst_Obj );
	return Sel;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_ReorderVertex(GEO_tdst_Object *_pst_Obj, int *_pi_NewIndex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j, k, index;
	GEO_Vertex			*V;
	MATH_tdst_Vector	*N;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* reorder coord and normals */
	V = (GEO_Vertex *) MEM_p_Alloc(_pst_Obj->l_NbPoints * sizeof(GEO_Vertex));
	N = (MATH_tdst_Vector *) MEM_p_Alloc(_pst_Obj->l_NbPoints * sizeof(MATH_tdst_Vector));

	for(i = 0; i < _pst_Obj->l_NbPoints; i++)
	{
		MATH_CopyVector(VCast(&V[_pi_NewIndex[i]]), VCast(&_pst_Obj->dst_Point[i]));
		MATH_CopyVector(&N[_pi_NewIndex[i]], &_pst_Obj->dst_PointNormal[i]);
	}

	MEM_Free(_pst_Obj->dst_Point);
	_pst_Obj->dst_Point = V;
	MEM_Free(_pst_Obj->dst_PointNormal);
	_pst_Obj->dst_PointNormal = N;

	/* change index of triangles */
	for(i = 0; i < _pst_Obj->l_NbElements; i++)
	{
		for(j = 0; j < _pst_Obj->dst_Element[i].l_NbTriangles; j++)
		{
			for(k = 0; k < 3; k++)
			{
				index = _pst_Obj->dst_Element[i].dst_Triangle[j].auw_Index[k];
				_pst_Obj->dst_Element[i].dst_Triangle[j].auw_Index[k] = _pi_NewIndex[index];
			}
		}
	}

	_pst_Obj->pst_SubObject->ul_Flags |= GEO_Cul_SOF_TopologyChange;
	GEO_SubObject_BuildEdgeData(_pst_Obj);
	GEO_SubObject_BuildFaceData(_pst_Obj);
}

/*
 =======================================================================================================================
    Harmonization les index des vertex
 =======================================================================================================================
 */
void GEO_i_SubObject_FHarmonizeIndex(struct GEO_tdst_Object_ *_pst_Tgt, struct GEO_tdst_Object_ *_pst_Src)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, j, i_Loop, index, search, pt[2], count;
	char						*pc_FSel, *pc_LastSel;
	int							i_FTgt, i_FSrc;
	int							*pi_NewIndex, *pi_UsedIndex;
	GEO_tdst_SubObject_Face		*FTgt, *FTgtLast, *FSrc, *FSrcLast;
	GEO_tdst_IndexedTriangle	*TTgt, *TSrc;
	float						fDist[6];
	GEO_Vertex					*PTgt;
	char						*FTouch;
	char						aCorres[6][3] = { { 0, 1, 2 }, { 0, 2, 1 }, { 1, 0, 2 }, { 1, 2, 0 }, { 2, 0, 1 }, { 2, 1, 0 } };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* les objets doivent avoir le même nombre de points */
	if(_pst_Tgt->l_NbPoints != _pst_Src->l_NbPoints) return;

	if(_pst_Tgt->pst_SubObject->ul_Flags & GEO_Cul_SOF_NonMagnifold) return;
	if(_pst_Src->pst_SubObject->ul_Flags & GEO_Cul_SOF_NonMagnifold) return;

	/* les objets doivent avoir une seule face sélectionnée */
	pc_FSel = _pst_Tgt->pst_SubObject->dc_FSel;
	pc_LastSel = pc_FSel + _pst_Tgt->pst_SubObject->ul_NbFaces;
	count = 0;
	while(pc_FSel < pc_LastSel)
	{
		if(*pc_FSel & 1)
		{
			i_FTgt = pc_FSel - _pst_Tgt->pst_SubObject->dc_FSel;
			count++;
		}

		pc_FSel++;
	}

	if(count != 1) return;
	FTgt = &_pst_Tgt->pst_SubObject->dst_Faces[i_FTgt];
	TTgt = &_pst_Tgt->dst_Element[FTgt->uw_Element].dst_Triangle[FTgt->uw_Index];

	pc_FSel = _pst_Src->pst_SubObject->dc_FSel;
	pc_LastSel = pc_FSel + _pst_Src->pst_SubObject->ul_NbFaces;
	count = 0;
	while(pc_FSel < pc_LastSel)
	{
		if(*pc_FSel & 1)
		{
			i_FSrc = pc_FSel - _pst_Src->pst_SubObject->dc_FSel;
			count++;
		}

		pc_FSel++;
	}

	if(count != 1) return;
	FSrc = &_pst_Src->pst_SubObject->dst_Faces[i_FSrc];
	TSrc = &_pst_Src->dst_Element[FSrc->uw_Element].dst_Triangle[FSrc->uw_Index];

	/* data */
	pi_NewIndex = (int *) L_malloc(4 * _pst_Tgt->l_NbPoints);
	L_memset(pi_NewIndex, -1, 4 * _pst_Tgt->l_NbPoints);

	pi_UsedIndex = (int *) L_malloc(4 * _pst_Tgt->l_NbPoints);
	L_memset(pi_UsedIndex, -1, 4 * _pst_Tgt->l_NbPoints);

	FTouch = (char *) L_malloc(_pst_Tgt->pst_SubObject->ul_NbFaces);
	L_memset(FTouch, 0, _pst_Tgt->pst_SubObject->ul_NbFaces);
	FTouch[i_FTgt] = 1;

	/*
	 * { static int test = 1; if ( test ) { for (i = 0; i < _pst_Tgt->l_NbPoints; i++)
	 * { pi_NewIndex[ i ] = _pst_Tgt->l_NbPoints - 1 - i; }
	 * GEO_SubObject_ReorderVertex( _pst_Tgt, pi_NewIndex ); test = 0; return; } }
	 */

	/* trouve la correspondance entre les 3 points des deux triangles */

	/*$F
    for(i = 0; i < 3; i++)
	{
		PTgt = &_pst_Tgt->dst_Point[TTgt->auw_Index[i]];
		fmin = Cf_Infinit;
		for(j = 0; j < 3; j++)
		{
			if(pi_UsedIndex[TSrc->auw_Index[j]] != -1) continue;
			f = MATH_f_Distance(VCast(PTgt), &_pst_Src->dst_Point[TSrc->auw_Index[j]]);
			if(f < fmin)
			{
				index = j;
				fmin = f;
			}
		}

		pi_UsedIndex[TSrc->auw_Index[index]] = TTgt->auw_Index[i];
		pi_NewIndex[TTgt->auw_Index[i]] = TSrc->auw_Index[index];
	}
    */
	L_memset(fDist, 0, 6 * 4);
	for(i = 0; i < 3; i++)
	{
		PTgt = &_pst_Tgt->dst_Point[TTgt->auw_Index[i]];
		for(j = 0; j < 6; j++)
		{
			fDist[j] += MATH_f_Distance(VCast(PTgt), &_pst_Src->dst_Point[TSrc->auw_Index[aCorres[j][i]]]);
		}
	}

	index = 0;
	for(i = 1; i < 6; i++)
	{
		if(fDist[i] < fDist[index]) index = i;
	}

	for(i = 0; i < 3; i++)
	{
		pi_UsedIndex[TSrc->auw_Index[aCorres[index][i]]] = TTgt->auw_Index[i];
		pi_NewIndex[TTgt->auw_Index[i]] = TSrc->auw_Index[aCorres[index][i]];
	}

FHarmonizeIndex_loop:
	i_Loop = 0;
	FTgt = _pst_Tgt->pst_SubObject->dst_Faces;
	FTgtLast = FTgt + _pst_Tgt->pst_SubObject->ul_NbFaces;

	while(FTgt < FTgtLast)
	{
		TTgt = &_pst_Tgt->dst_Element[FTgt->uw_Element].dst_Triangle[FTgt->uw_Index];
		for(count = 0, i = 0, j = 0; i < 3; i++)
		{
			if(pi_NewIndex[TTgt->auw_Index[i]] == -1)
			{
				search = TTgt->auw_Index[i];
				count++;
			}
			else
				pt[j++] = pi_NewIndex[TTgt->auw_Index[i]];
		}

		if(count != 1)
		{
			if(count == 0) FTouch[FTgt - _pst_Tgt->pst_SubObject->dst_Faces] = 1;
			FTgt++;
			continue;
		}

		FSrc = _pst_Src->pst_SubObject->dst_Faces;
		FSrcLast = FSrc + _pst_Src->pst_SubObject->ul_NbFaces;
		while(FSrc < FSrcLast)
		{
			TSrc = &_pst_Src->dst_Element[FSrc->uw_Element].dst_Triangle[FSrc->uw_Index];
			for(count = 0, i = 0; i < 3; i++)
			{
				j = TSrc->auw_Index[i];
				if((j == pt[0]) || (j == pt[1]))
					count++;
				else
					index = j;
			}

			if((count == 2) && (pi_UsedIndex[index] == -1)) break;
			FSrc++;
		}

		if(FSrc >= FSrcLast) goto FHarmonizeIndex_end;

		FTouch[FTgt - _pst_Tgt->pst_SubObject->dst_Faces] = 1;
		i_Loop = 1;
		pi_UsedIndex[index] = search;
		pi_NewIndex[search] = index;

		FTgt++;
	}

	if(i_Loop) goto FHarmonizeIndex_loop;

	/* cherche les index non retriés */
	for(j = 0, i = 0; i < (int) _pst_Tgt->l_NbPoints; i++)
	{
		if(pi_NewIndex[i] == -1)
		{
			while(pi_UsedIndex[j] != -1) j++;
			pi_UsedIndex[j] = i;
			pi_NewIndex[i] = j;
		}
	}

	/* reorder */
	GEO_SubObject_ReorderVertex(_pst_Tgt, pi_NewIndex);

	/* selectionne les faces qui ont été bien retriées */
	L_memcpy(_pst_Tgt->pst_SubObject->dc_FSel, FTouch, _pst_Tgt->pst_SubObject->ul_NbFaces);

FHarmonizeIndex_end:
	L_free(pi_UsedIndex);
	L_free(pi_NewIndex);
	L_free(FTouch);
}

/*$4
 ***********************************************************************************************************************
    Simplify functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_RemoveBadFaces(GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*T;
	int									i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < _pst_Obj->l_NbElements; i++)
	{
		pst_Elem = _pst_Obj->dst_Element + i;
		T = pst_Elem->dst_Triangle;
		for(j = 0; j < pst_Elem->l_NbTriangles; )
		{
			if
			(
				(T->auw_Index[0] == T->auw_Index[1])
			||	(T->auw_Index[0] == T->auw_Index[2])
			||	(T->auw_Index[1] == T->auw_Index[2])
			||	(T->auw_Index[0] > _pst_Obj->l_NbPoints)
			||	(T->auw_Index[1] > _pst_Obj->l_NbPoints)
			||	(T->auw_Index[2] > _pst_Obj->l_NbPoints)
			)
			{
				L_memcpy(T, T + 1, (pst_Elem->l_NbTriangles - j - 1) * sizeof(GEO_tdst_IndexedTriangle));
				pst_Elem->l_NbTriangles--;
			}
			else
			{
				T++;
				j++;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_RemoveIsolatedVertices(GEO_tdst_Object *_pst_Object)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG								*dl_Vertex;
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*pst_Tri;
	int									i, j;
	char								sz_Text[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Object) return;
	if(_pst_Object->st_Id.i->ul_Type != GRO_Geometric) return;

	dl_Vertex = (LONG *) L_malloc(_pst_Object->l_NbPoints * 4);
	L_memset(dl_Vertex, 0, _pst_Object->l_NbPoints * 4);

	for(i = 0; i < _pst_Object->l_NbElements; i++)
	{
		pst_Elem = _pst_Object->dst_Element + i;
		pst_Tri = pst_Elem->dst_Triangle;
		for(j = 0; j < pst_Elem->l_NbTriangles; j++, pst_Tri++)
		{
			dl_Vertex[pst_Tri->auw_Index[0]]++;
			dl_Vertex[pst_Tri->auw_Index[1]]++;
			dl_Vertex[pst_Tri->auw_Index[2]]++;
		}
	}

	j = 0;
	for(i = 0; i < _pst_Object->l_NbPoints; i++)
	{
		if(dl_Vertex[i])
		{
			dl_Vertex[i] = j++;
			if(i != j)
			{
				L_memcpy(_pst_Object->dst_Point + j, _pst_Object->dst_Point + i, sizeof(MATH_tdst_Vector));
				L_memcpy(_pst_Object->dst_PointNormal + j, _pst_Object->dst_PointNormal + i, sizeof(MATH_tdst_Vector));
			}
		}
		else
			dl_Vertex[i] = -1;
	}

	sprintf(sz_Text, "<Simplify> found %d isolated vertices", _pst_Object->l_NbPoints - j);
	LINK_PrintStatusMsg(sz_Text);
	if(j == _pst_Object->l_NbPoints) return;

	for(i = 0; i < _pst_Object->l_NbElements; i++)
	{
		pst_Elem = _pst_Object->dst_Element + i;
		pst_Tri = pst_Elem->dst_Triangle;
		for(j = 0; j < pst_Elem->l_NbTriangles; j++, pst_Tri++)
		{
			pst_Tri->auw_Index[0] = (unsigned short) dl_Vertex[pst_Tri->auw_Index[0]];
			pst_Tri->auw_Index[1] = (unsigned short) dl_Vertex[pst_Tri->auw_Index[1]];
			pst_Tri->auw_Index[2] = (unsigned short) dl_Vertex[pst_Tri->auw_Index[2]];
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SubObject_UVClean(GEO_tdst_Object *_pst_Object)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*pst_Tri;
	int									*uv, i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Object) return;
	if(_pst_Object->st_Id.i->ul_Type != GRO_Geometric) return;
	if(_pst_Object->l_NbUVs == 0) return;

	uv = (int *) L_malloc(_pst_Object->l_NbUVs * sizeof(int));
	L_memset(uv, 0, _pst_Object->l_NbUVs * sizeof(int));

	for(i = 0; i < _pst_Object->l_NbElements; i++)
	{
		pst_Elem = _pst_Object->dst_Element + i;
		pst_Tri = pst_Elem->dst_Triangle;
		for(j = 0; j < pst_Elem->l_NbTriangles; j++, pst_Tri++)
		{
			if ( pst_Tri->auw_UV[0] >= _pst_Object->l_NbUVs )
				pst_Tri->auw_UV[0] = 0;
			if ( pst_Tri->auw_UV[1] >= _pst_Object->l_NbUVs )
				pst_Tri->auw_UV[1] = 0;
			if ( pst_Tri->auw_UV[2] >= _pst_Object->l_NbUVs )
				pst_Tri->auw_UV[2] = 0;
				
			uv[pst_Tri->auw_UV[0]]++;
			uv[pst_Tri->auw_UV[1]]++;
			uv[pst_Tri->auw_UV[2]]++;
		}
	}

	for(i = 0, j = 0; i < _pst_Object->l_NbUVs; i++)
	{
		if(uv[i] == 0) continue;
		L_memcpy(&_pst_Object->dst_UV[j], &_pst_Object->dst_UV[i], sizeof(GEO_tdst_UV));
		uv[i] = j++;
	}

	_pst_Object->l_NbUVs = j;

	for(i = 0; i < _pst_Object->l_NbElements; i++)
	{
		pst_Elem = _pst_Object->dst_Element + i;
		pst_Tri = pst_Elem->dst_Triangle;
		for(j = 0; j < pst_Elem->l_NbTriangles; j++, pst_Tri++)
		{
			pst_Tri->auw_UV[0] = uv[pst_Tri->auw_UV[0]];
			pst_Tri->auw_UV[1] = uv[pst_Tri->auw_UV[1]];
			pst_Tri->auw_UV[2] = uv[pst_Tri->auw_UV[2]];
		}
	}

	L_free(uv);
}

/*
 =======================================================================================================================
    recenter UV to avoid big tiling
 =======================================================================================================================
 */
void GEO_SubObject_UVCenter(GEO_tdst_Object *_pst_Object)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*pst_Tri;
	GEO_tdst_UV							*pst_UV;
	int									i, j, k, loop, onemoreloop;
	int									*uv, *cur, *last;
	int									*uvdetach, *uvindex;
	int									detach, nodetach;
	float								umin, umax, ucenter, vmin, vmax, vcenter;
	char								sz_Text[256];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	#define MAXTILE	15	//31
	#define	MINUV	-7	// -15
	#define	MAXUV	8	// 16
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	if(!_pst_Object) return;
	if(_pst_Object->st_Id.i->ul_Type != GRO_Geometric) return;
	if(_pst_Object->l_NbUVs == 0) return;

	uv = (int *) L_malloc(_pst_Object->l_NbUVs * 3 * sizeof(int));
	uvdetach = (int *) L_malloc(_pst_Object->l_NbUVs * 3 * sizeof(int));
	uvindex = (int *) L_malloc(_pst_Object->l_NbUVs * 3 * sizeof(int));

UVCenter_tryagainafterdetach:
	loop = 0;
	L_memset(uv, 0, _pst_Object->l_NbUVs * sizeof(int));
	last = uv + _pst_Object->l_NbUVs;

UVCenter_tryagain:
	loop++;
	cur = uv;
	while(cur < last)
	{
		if(*cur == 0) break;
		cur++;
	}

	if(cur == last) goto UVCenter_end;

	*cur = loop;
	pst_UV = _pst_Object->dst_UV + (cur - uv);
	umin = umax = pst_UV->fU;
	vmin = vmax = pst_UV->fV;

	/* mark all UV attached */
	do
	{
		onemoreloop = 0;
		for(i = 0; i < _pst_Object->l_NbElements; i++)
		{
			pst_Elem = _pst_Object->dst_Element + i;
			pst_Tri = pst_Elem->dst_Triangle;
			for(j = 0; j < pst_Elem->l_NbTriangles; j++, pst_Tri++)
			{
				k = (uv[pst_Tri->auw_UV[0]] == loop) ? 1 : 0;
				k += (uv[pst_Tri->auw_UV[1]] == loop) ? 1 : 0;
				k += (uv[pst_Tri->auw_UV[2]] == loop) ? 1 : 0;

				if(k == 0) continue;
				if(k == 3) continue;

				onemoreloop = 1;
				uv[pst_Tri->auw_UV[0]] = loop;
				uv[pst_Tri->auw_UV[1]] = loop;
				uv[pst_Tri->auw_UV[2]] = loop;
			}
		}
	} while(onemoreloop);

	/* calcule la boite englobante des uvs marqués */
	cur = uv;
	while(cur < last)
	{
		if(*cur == loop)
		{
			pst_UV = _pst_Object->dst_UV + (cur - uv);
			if(pst_UV->fU < umin) umin = pst_UV->fU;
			if(pst_UV->fU > umax) umax = pst_UV->fU;
			if(pst_UV->fV < vmin) vmin = pst_UV->fV;
			if(pst_UV->fV > vmax) vmax = pst_UV->fV;
		}

		cur++;
	}

	sprintf(sz_Text, "centrage des UV de %s [group %d]", _pst_Object->st_Id.sz_Name, loop);
	LINK_PrintStatusMsg(sz_Text);
	sprintf(sz_Text, ".   u  [ %.3f à %.3f ]; v [ %.3f à %.3f ]", umin, umax, vmin, vmax);
	LINK_PrintStatusMsg(sz_Text);

	/* si la boite est comprise dans un carré de 32 centré pas de modification */
	if((umin >= MINUV) && (umax <= MAXUV) && (vmin >= MINUV) && (vmax <= MAXUV))
	{
		LINK_PrintStatusMsg(".   ces uvs sont nickels");
		goto UVCenter_tryagain;
	}

	/* centrage + warning si après centrage ca sort encore du carré 16, 16 */
	if(((umax - umin) > MAXTILE) || (vmax - vmin) > MAXTILE) 
	{
		L_memset( uvdetach, 0, _pst_Object->l_NbUVs * sizeof(int));
		L_memset( uvindex, 0xFF, _pst_Object->l_NbUVs * sizeof(int));
		cur = uv;
		/* mark all uv that are in the limit */
		if ((umax - umin) > MAXTILE)
		{
			while(cur < last)
			{
				if(*cur == loop)
				{
					pst_UV = _pst_Object->dst_UV + (cur - uv);
					if ( (pst_UV->fU - umin) <= MAXTILE)
					{
						uvdetach[ (cur - uv) ] = 1;
					}
				}
				cur++;
			}
		}
		else
		{
			while(cur < last)
			{
				if(*cur == loop)
				{
					pst_UV = _pst_Object->dst_UV + (cur - uv);
					if ( (pst_UV->fV - vmin) <= MAXTILE)
					{
						uvdetach[ (cur - uv) ] = 1;
					}
				}
				cur++;
			}
		}

		/* cuont face that will be detached */
		detach = nodetach = 0;
		for(i = 0; i < _pst_Object->l_NbElements; i++)
		{
			pst_Elem = _pst_Object->dst_Element + i;
			pst_Tri = pst_Elem->dst_Triangle;
			for(j = 0; j < pst_Elem->l_NbTriangles; j++, pst_Tri++)
			{
				k = (uv[pst_Tri->auw_UV[0]] == loop) ? 1 : 0;
				k += (uv[pst_Tri->auw_UV[1]] == loop) ? 1 : 0;
				k += (uv[pst_Tri->auw_UV[2]] == loop) ? 1 : 0;

				if (k != 3) 
					continue;

				k = uvdetach[pst_Tri->auw_UV[0]] + uvdetach[pst_Tri->auw_UV[1]] + uvdetach[pst_Tri->auw_UV[2]];

				if ((k == 0) || (k == 3))
				{
					nodetach = 1;
					if (detach) break;
					continue;
				}

				detach = 1;
				if (nodetach) break;
			}
		}

		if (!detach || !nodetach)
		{
			LINK_gul_ColorTxt = 0xFF;
			LINK_PrintStatusMsg( ".    Error : UV intraitables, sans doute face énorme" );
			LINK_gul_ColorTxt = 0;
			goto UVCenter_tryagain;
		}

		/* now detach UV that are in BV but used by a triangle out of BV */
		for(i = 0; i < _pst_Object->l_NbElements; i++)
		{
			pst_Elem = _pst_Object->dst_Element + i;
			pst_Tri = pst_Elem->dst_Triangle;
			for(j = 0; j < pst_Elem->l_NbTriangles; j++, pst_Tri++)
			{
				k = uvdetach[pst_Tri->auw_UV[0]] + uvdetach[pst_Tri->auw_UV[1]] + uvdetach[pst_Tri->auw_UV[2]];

				if(k == 0) continue;
				if(k == 3) continue;

				for (k = 0; k < 3; k++)
				{
					if (uvdetach[ pst_Tri->auw_UV[ k ] ] )
					{
						if (uvindex[ pst_Tri->auw_UV[ k ] ] == -1)
						{
							GEO_AddUV( _pst_Object, _pst_Object->dst_UV[ pst_Tri->auw_UV[ k ] ].fU, _pst_Object->dst_UV[ pst_Tri->auw_UV[ k ] ].fV );
							uvindex[ pst_Tri->auw_UV[ k ] ] = _pst_Object->l_NbUVs - 1;
						}
						pst_Tri->auw_UV[ k ] = uvindex[ pst_Tri->auw_UV[ k ] ];
					}
				}
			}
		}
		// recommence tout
		LINK_PrintStatusMsg(".   UV détachés, c'est reparti pour un tour");
		goto UVCenter_tryagainafterdetach;
	}

	ucenter = (float) ((int) ((umin + umax + 1) / 2));
	vcenter = (float) ((int) ((vmin + vmax + 1) / 2));
	if((ucenter == 0) && (vcenter == 0))
	{
		LINK_PrintStatusMsg(".   Déja centré, pas de modification");
		goto UVCenter_tryagain;
	}
	

	sprintf(sz_Text, ".   Deplacement des UV de (%.0f, %.0f)", ucenter, vcenter);
	LINK_gul_ColorTxt = 0xFF;
	LINK_PrintStatusMsg(sz_Text);
	LINK_gul_ColorTxt = 0;

	/* recentre les UVs */
	cur = uv;
	while(cur < last)
	{
		if(*cur == loop)
		{
			pst_UV = _pst_Object->dst_UV + (cur - uv);
			pst_UV->fU -= ucenter;
			pst_UV->fV -= vcenter;		
		}

		cur++;
	}

	goto UVCenter_tryagain;

UVCenter_end:
	L_free(uv);
	L_free(uvdetach);
	L_free(uvindex);
}

/*
 =======================================================================================================================
    recenter UV to avoid big tiling
 =======================================================================================================================
 */
void GEO_SubObject_UVCenterWithMaterial(GEO_tdst_Object *_pst_Object, MAT_tdst_Material *_pst_Mat )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
	GEO_tdst_IndexedTriangle			*pst_Tri;
	GEO_tdst_UV							*pst_SrcUV, *pst_UV, *pst_UVStart, *pst_UVEnd;
	MAT_tdst_Material					*pst_Mat;
	MAT_tdst_MultiTexture				*pst_MTMat;
	MAT_tdst_MTLevel					*pst_MTLevel, *pst_MTLevelBest;
	MAT_tdst_Decompressed_UVMatrix		st_UVTrans;
	MAT_tdst_Decompressed_UVMatrix		*UVMat, *UVInv;
	int									*UVstart, *UVlast;
	char								*uvmark;
	int									i_Elem, i;
	int									i_NbFaces, i_NbUV, i_UVCur, i_Face;
	int									*uv;
	float								fVal, fBestVal, fu, fv;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Object) return;
	if(_pst_Object->st_Id.i->ul_Type != GRO_Geometric) return;
	if(_pst_Object->l_NbUVs == 0) return;
	if(!_pst_Mat ) GEO_SubObject_UVCenter( _pst_Object );
	pst_SrcUV = _pst_Object->dst_UV;

	/* count number of faces */
	for (i_NbFaces = 0, i_Elem = 0; i_Elem < _pst_Object->l_NbElements; i_Elem++)
	{
		pst_Elem = &_pst_Object->dst_Element[ i_Elem ];
		i_NbFaces += pst_Elem->l_NbTriangles;
	}
	if (!i_NbFaces) return;

	i_NbUV = 3 * i_NbFaces;
	if (i_NbUV < _pst_Object->l_NbUVs) i_NbUV = _pst_Object->l_NbUVs;
	pst_UV = (GEO_tdst_UV *) MEM_p_Alloc( i_NbUV * sizeof(GEO_tdst_UV ) );
	uv = (int*) L_malloc( sizeof( int ) * i_NbUV );
	i_UVCur = 0;

	UVMat = (MAT_tdst_Decompressed_UVMatrix *) L_malloc( sizeof(MAT_tdst_Decompressed_UVMatrix) * _pst_Object->l_NbElements );
	UVInv = (MAT_tdst_Decompressed_UVMatrix *) L_malloc( sizeof(MAT_tdst_Decompressed_UVMatrix) * _pst_Object->l_NbElements );
	UVstart = (int *) L_malloc( sizeof( int) * _pst_Object->l_NbElements );
	UVlast = (int *) L_malloc( sizeof( int) * _pst_Object->l_NbElements );

	for (i_Elem = 0; i_Elem < _pst_Object->l_NbElements; i_Elem++)
	{
		pst_Elem  = &_pst_Object->dst_Element[ i_Elem ];
		
		/* copy all UV used by this element in new array of UV */
		UVstart[ i_Elem ] = i_UVCur;
		L_memset( uv, 0xFF, sizeof( int ) * i_NbUV );
		for (pst_Tri = pst_Elem->dst_Triangle, i_Face = 0; i_Face < pst_Elem->l_NbTriangles; i_Face++, pst_Tri++ )
		{
			for (i = 0; i < 3; i++)
			{
				if (uv[ pst_Tri->auw_UV[ i ] ] == -1)
				{
					pst_UV[ i_UVCur ].fU = pst_SrcUV[ pst_Tri->auw_UV[ i ] ].fU;
					pst_UV[ i_UVCur ].fV = pst_SrcUV[ pst_Tri->auw_UV[ i ] ].fV;
					uv[ pst_Tri->auw_UV[ i ] ] = i_UVCur++;
				}
				pst_Tri->auw_UV[ i ] = uv[ pst_Tri->auw_UV[ i ] ];
			}
		}
		UVlast[ i_Elem ] = i_UVCur;

		/* choose good material : material with biggest scale */
		pst_Mat = GEO_GetMaterial( _pst_Mat, pst_Elem->l_MaterialId );
		pst_MTLevelBest = NULL;

		if (pst_Mat->st_Id.i->ul_Type == GRO_MaterialMultiTexture)
		{
			pst_MTMat = (MAT_tdst_MultiTexture *) pst_Mat;
			pst_MTLevel = pst_MTMat->pst_FirstLevel;
			pst_MTLevelBest = NULL;
			while ( pst_MTLevel )
			{
				if ( MAT_GET_UVSource( pst_MTLevel->ul_Flags ) != MAT_Cc_UV_Object1 )
				{
					pst_MTLevelBest = NULL;
					break;
				}
#ifdef JADEFUSION
				extern BOOL MAT_VUISIdentity_i(MAT_tdst_MTLevel *p_tdstLevel);
#endif
				if(!MAT_VUISIdentity_i(pst_MTLevel))
				{
					MAT_VUDecompress(pst_MTLevel, &st_UVTrans);
					for (fVal = 0, i=0; i < 4; i++ )
						fVal += (float) fabs( st_UVTrans.UVMatrix[ i ] );
				}
				else
					fVal = 2.0f;

				if ( ( !pst_MTLevelBest ) || (fBestVal < fVal ) )
				{
					fBestVal = fVal;
					pst_MTLevelBest = pst_MTLevel;
				}
				pst_MTLevel = pst_MTLevel->pst_NextLevel;
			}
		}
#ifdef JADEFUSION
		extern BOOL MAT_VUISIdentity_i(MAT_tdst_MTLevel *p_tdstLevel);
#endif
		if ( !pst_MTLevelBest || MAT_VUISIdentity_i(pst_MTLevelBest))
		{
			L_memset( &UVMat[ i_Elem ], 0, sizeof(MAT_tdst_Decompressed_UVMatrix) );
			UVMat[ i_Elem ].UVMatrix[ 0 ] = UVMat[ i_Elem ].UVMatrix[ 3 ] = 1.0f;
		}
		else
			MAT_VUDecompress(pst_MTLevelBest, &UVMat[ i_Elem ]);
		
		MAT_UVMatrix_Invert( &UVMat[ i_Elem ], &UVInv[ i_Elem ] );

		pst_UVStart = pst_UV + UVstart[ i_Elem ];
		pst_UVEnd = pst_UV + UVlast[ i_Elem ];;
		while (pst_UVStart < pst_UVEnd )
		{
			fu = UVMat[ i_Elem ].UVMatrix[0] * pst_UVStart->fU + UVMat[ i_Elem ].UVMatrix[2] * pst_UVStart->fV + UVMat[ i_Elem ].AddU; 
            fv = UVMat[ i_Elem ].UVMatrix[1] * pst_UVStart->fU + UVMat[ i_Elem ].UVMatrix[3] * pst_UVStart->fV + UVMat[ i_Elem ].AddV;
			pst_UVStart->fU = fu;
			pst_UVStart->fV = fv;
			pst_UVStart++;
		}
	}

	MEM_Free( _pst_Object->dst_UV );
	_pst_Object->dst_UV = pst_UV;
	_pst_Object->l_NbUVs = i_UVCur;

	// center transformed UV
	GEO_SubObject_UVCenter( _pst_Object );
	

	// reinvert UV
	pst_UVStart = _pst_Object->dst_UV;
	uvmark = (char*)L_malloc( _pst_Object->l_NbUVs );
	L_memset( uvmark, 0, _pst_Object->l_NbUVs );
	for (i_Elem = 0; i_Elem < _pst_Object->l_NbElements; i_Elem++)
	{
		L_memset( uvmark, 0, _pst_Object->l_NbUVs );
		pst_Elem  = &_pst_Object->dst_Element[ i_Elem ];

		/* mark all uv used by this element */
		for (pst_Tri = pst_Elem->dst_Triangle, i_Face = 0; i_Face < pst_Elem->l_NbTriangles; i_Face++, pst_Tri++ )
		{
			for (i = 0; i < 3; i++)
				uvmark[ pst_Tri->auw_UV[ i ] ] = 1;
		}

		pst_UV = pst_UVStart;
		for (i = 0; i < _pst_Object->l_NbUVs; i++, pst_UV++)
		{
			if (!uvmark[ i ] ) continue;
			fu = UVInv[ i_Elem ].UVMatrix[0] * pst_UV->fU + UVInv[ i_Elem ].UVMatrix[2] * pst_UV->fV + UVInv[ i_Elem ].AddU;
			fv = UVInv[ i_Elem ].UVMatrix[1] * pst_UV->fU + UVInv[ i_Elem ].UVMatrix[3] * pst_UV->fV + UVInv[ i_Elem ].AddV;
			pst_UV->fU = fu;
			pst_UV->fV = fv;
		}
	}
}

/*
 =======================================================================================================================
    recenter UV to avoid big tiling
 =======================================================================================================================
 */
void GEO_SubObject_SetUVfor3DText( GEO_tdst_Object *_pst_Object, int i_Element )
{
	int									*UVReorder;
	GEO_tdst_ElementIndexedTriangles	*pst_Elem, st_Elem;
    GEO_tdst_IndexedTriangle			*pst_Tri;
	GEO_tdst_UV							*pst_NewUV;
    int									i, uv, newnbuv, elem;

    if (!_pst_Object) return;
	if (_pst_Object->st_Id.i->ul_Type != GRO_Geometric) return;
    if (_pst_Object->l_NbPoints == 0) return;
	if (_pst_Object->l_NbElements == 0) return;
    if (_pst_Object->dst_Element->l_NbTriangles == 0) return;

	// reorder element : the one with 3DText UV have to be at the beginning
	if (i_Element)
	{
		L_memcpy( &st_Elem, _pst_Object->dst_Element, sizeof( GEO_tdst_ElementIndexedTriangles ) );
		L_memcpy( _pst_Object->dst_Element, _pst_Object->dst_Element + i_Element, sizeof( GEO_tdst_ElementIndexedTriangles ) );
		L_memcpy( _pst_Object->dst_Element + i_Element, &st_Elem, sizeof( GEO_tdst_ElementIndexedTriangles ) );
	}

	// get elem
	pst_Elem = _pst_Object->dst_Element;

	// alloc reorder buffer
	UVReorder = (int *) L_malloc( _pst_Object->l_NbUVs * 4 );
	L_memset( UVReorder, 0xFF, _pst_Object->l_NbUVs * 4 );

	// reserve uv for i_Element
	uv = pst_Elem->l_NbTriangles * 2;

	// reorder other UV
	for ( elem = 1; elem < _pst_Object->l_NbElements; elem++)
	{
		for ( i = 0; i < _pst_Object->dst_Element[elem].l_NbTriangles; i++)
		{
			UVReorder[ _pst_Object->dst_Element[elem].dst_Triangle[i].auw_UV[0] ] = 1;
			UVReorder[ _pst_Object->dst_Element[elem].dst_Triangle[i].auw_UV[1] ] = 1;
			UVReorder[ _pst_Object->dst_Element[elem].dst_Triangle[i].auw_UV[2] ] = 1;
		}
	}

	// count used UVs
	newnbuv = pst_Elem->l_NbTriangles * 2;
	for ( i = 0; i < _pst_Object->l_NbUVs; i++)
	{
		if ( UVReorder[ i ] == 1 )
			UVReorder[ i ] = newnbuv++;
	}

	// alloc new data
	pst_NewUV = (GEO_tdst_UV *) MEM_p_Alloc( newnbuv * sizeof (GEO_tdst_UV ) );

	// set UV for elem 3D Text
	uv = 0;
	for (i = 0; i < pst_Elem->l_NbTriangles; i += 2, uv+= 4 )
	{
		pst_NewUV[ uv + 0].fU = pst_NewUV[ uv + 0].fV = (uv + 0) * 0.01f;
		pst_NewUV[ uv + 1].fU = pst_NewUV[ uv + 1].fV = (uv + 1) * 0.01f;
		pst_NewUV[ uv + 2].fU = pst_NewUV[ uv + 2].fV = (uv + 2) * 0.01f;
		pst_NewUV[ uv + 3].fU = pst_NewUV[ uv + 3].fV = (uv + 3) * 0.01f;
		
		pst_Elem->dst_Triangle[i].auw_Index[ 0 ] = pst_Elem->dst_Triangle[i].auw_UV[ 0 ] = uv;
		pst_Elem->dst_Triangle[i].auw_Index[ 1 ] = pst_Elem->dst_Triangle[i].auw_UV[ 1 ] = uv+1;
		pst_Elem->dst_Triangle[i].auw_Index[ 2 ] = pst_Elem->dst_Triangle[i].auw_UV[ 2 ] = uv+2;

		pst_Elem->dst_Triangle[i + 1].auw_Index[ 0 ] = pst_Elem->dst_Triangle[i + 1].auw_UV[ 0 ] = uv+2;
		pst_Elem->dst_Triangle[i + 1].auw_Index[ 1 ] = pst_Elem->dst_Triangle[i + 1].auw_UV[ 1 ] = uv+1;
		pst_Elem->dst_Triangle[i + 1].auw_Index[ 2 ] = pst_Elem->dst_Triangle[i + 1].auw_UV[ 2 ] = uv+3;
	}

	// reserve uv for i_Element
	uv = pst_Elem->l_NbTriangles * 2;
	for (i = 0; i < _pst_Object->l_NbUVs; i++ )
	{
		if (UVReorder[ i ] == -1 ) continue;
		L_memcpy( pst_NewUV + UVReorder[ i ], _pst_Object->dst_UV + i, sizeof( GEO_tdst_UV ) );
	}

	// reorder other UV
	for ( elem = 1; elem < _pst_Object->l_NbElements; elem++)
	{
		for ( i = 0; i < _pst_Object->dst_Element[elem].l_NbTriangles; i++)
		{
			pst_Tri = _pst_Object->dst_Element[ elem ].dst_Triangle + i;
			pst_Tri->auw_UV[0] = UVReorder[ pst_Tri->auw_UV[0] ];
			pst_Tri->auw_UV[1] = UVReorder[ pst_Tri->auw_UV[1] ];
			pst_Tri->auw_UV[2] = UVReorder[ pst_Tri->auw_UV[2] ];			
		}
	}

	MEM_Free( _pst_Object->dst_UV );
	_pst_Object->dst_UV = pst_NewUV;
	_pst_Object->l_NbUVs = newnbuv;

	/*
    while ( _pst_Object->l_NbUVs != _pst_Object->l_NbPoints )
    {
        GEO_AddUV( _pst_Object, 0, 0 );
    }

    for (i = 0; i < _pst_Object->dst_Element->l_NbTriangles; i++)
    {
        pst_Tri = _pst_Object->dst_Element->dst_Triangle + i;
        pst_Tri->auw_UV[0] = pst_Tri->auw_Index[0];
        pst_Tri->auw_UV[1] = pst_Tri->auw_Index[1];
        pst_Tri->auw_UV[2] = pst_Tri->auw_Index[2];
    }
	*/

    _pst_Object->ul_EditorFlags |= GEO_CEF_GeomFor3DText;
}

/*
 =======================================================================================================================
	Pick UV from another object
 =======================================================================================================================
 */
void GEO_PickUV_FindBestUV( GEO_Vertex *V, int index, GEO_tdst_Object *_pst_Src, GEO_tdst_UV *UV )
{
	int									i, j0, j1, j2, k1, k2;
	GEO_tdst_SubObject_Face				*F;
	GEO_tdst_ElementIndexedTriangles	*pst_Elem;
    GEO_tdst_IndexedTriangle			*pst_Tri;
    MATH_tdst_Vector					v, v1, v2, n, vtemp;
    float								t, a, b, f, x, y; 
    
	
	F = _pst_Src->pst_SubObject->dst_Faces;
	for ( i = 0; i < (int) _pst_Src->pst_SubObject->ul_NbFaces; i++, F++)
	{
		pst_Elem = _pst_Src->dst_Element + _pst_Src->pst_SubObject->dst_Faces[ i ].uw_Element;
		pst_Tri = pst_Elem->dst_Triangle + _pst_Src->pst_SubObject->dst_Faces[ i ].uw_Index;
		
		for ( j0 = 0; j0 < 3; j0++)
			if (pst_Tri->auw_Index[ j0 ] == index )
				break;
		if (j0 == 3) continue;
		
		j1 = (j0 + 1) % 3; k1 = pst_Tri->auw_Index[ j1 ];
		j2 = (j0 + 2) % 3; k2 = pst_Tri->auw_Index[ j2 ];
		
		MATH_SubVector( &v1, &_pst_Src->dst_Point[ k1 ], &_pst_Src->dst_Point[ index ] );
		MATH_SubVector( &v2, &_pst_Src->dst_Point[ k2 ], &_pst_Src->dst_Point[ index ] );
		
		MATH_CrossProduct( &n, &v1, &v2 );
		
		MATH_SubVector( &v, V, &_pst_Src->dst_Point[ index ] );
		
		t = MATH_f_DotProduct( &n, &n );
		if ( t == 0 ) 
			continue;
		t = MATH_f_DotProduct( &v, &n ) / t;
		MATH_ScaleVector( &vtemp, &n, t );
		MATH_SubEqualVector( &v, &vtemp );
		
		//a = MATH_f_DotProduct( &v, &v1 ) / MATH_f_DotProduct( &v1, &v1 );
		//b = MATH_f_DotProduct( &v, &v2 ) / MATH_f_DotProduct( &v2, &v2 );
		
		/* compute coordinates of v in V, v1, v2 system axis */
		f = MATH_f_DotProduct( &v1, &v2);
		if(fNulWithEpsilon(f, 0.000001f))
		{
			x = MATH_f_DotProduct( &v, &v1 ) / MATH_f_DotProduct( &v1, &v1 );
		}
		else
		{
			b = MATH_f_SqrVector( &v2 );
			a = MATH_f_DotProduct( &v, &v2) / b;
			b = f / b;

			a = MATH_f_DotProduct(&v, &v1) - a * f;
			b = MATH_f_DotProduct(&v1, &v1) - b * f;
			x = a / b;
		}

		y = (MATH_f_DotProduct(&v, &v2) - x * f) / MATH_f_SqrVector( &v2 );

		if ( !((x >= 0) && (x <= 1) && (y >= 0) && (y <= 1)))
			continue;
		
		UV->fU = _pst_Src->dst_UV[ pst_Tri->auw_UV[j0] ].fU;
		UV->fV = _pst_Src->dst_UV[ pst_Tri->auw_UV[j0] ].fV;
		UV->fU += x * (_pst_Src->dst_UV[ pst_Tri->auw_UV[j1] ].fU - _pst_Src->dst_UV[ pst_Tri->auw_UV[j0] ].fU );
		UV->fV += x * (_pst_Src->dst_UV[ pst_Tri->auw_UV[j1] ].fV - _pst_Src->dst_UV[ pst_Tri->auw_UV[j0] ].fV );
		UV->fU += y * (_pst_Src->dst_UV[ pst_Tri->auw_UV[j2] ].fU - _pst_Src->dst_UV[ pst_Tri->auw_UV[j0] ].fU );
		UV->fV += y * (_pst_Src->dst_UV[ pst_Tri->auw_UV[j2] ].fV - _pst_Src->dst_UV[ pst_Tri->auw_UV[j0] ].fV );
		break;
	}
}
/**/
void GEO_PickUV( GEO_tdst_Object *_pst_Tgt, GEO_tdst_Object *_pst_Src, char _c_Sel )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    ULONG                           ul_Nearest;
    float                           delta, f_Cur, f_Dist;
    int                             i, j, vertex, face;
    GEO_Vertex                      *pst_SrcPt, *pst_TgtPt;
    char                            *dc_Sel, *pc_Sel;
    int								*di_VertexTable;
    int								i_SrcSubBuild;
    GEO_tdst_ElementIndexedTriangles	*pst_Elem;
    GEO_tdst_IndexedTriangle			*pst_Tri;
    GEO_tdst_UV							st_UV;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    
    if ( !_pst_Src || !_pst_Tgt ) return;
    if (!_pst_Tgt->pst_SubObject ) return;
    
    i_SrcSubBuild = 0;
    if (!_pst_Src->pst_SubObject )
    {
		GEO_SubObject_Create( _pst_Src );
		i_SrcSubBuild = 1;
    }
    
    if (_c_Sel) 
    {
        dc_Sel = NULL;
        pc_Sel = _pst_Tgt->pst_SubObject->dc_FSel;
    }
    else
    {
         dc_Sel = (char* )L_malloc( _pst_Tgt->pst_SubObject->ul_NbFaces);
       L_memset( dc_Sel, 1, _pst_Tgt->pst_SubObject->ul_NbFaces );
        pc_Sel = dc_Sel;
    }
    
    /* look for all target src the nearest src vertex - build table */
    di_VertexTable = (int *) L_malloc( _pst_Tgt->l_NbPoints * sizeof(int ) );
    L_memset( di_VertexTable, 0xFF, _pst_Tgt->l_NbPoints * sizeof(int ) );
    pst_TgtPt = _pst_Tgt->dst_Point;
    for (vertex = 0; vertex < _pst_Tgt->l_NbPoints; vertex++, pst_TgtPt++)
    {
		if (!_pst_Tgt->pst_SubObject->dc_VSel[ vertex ] ) continue;
		
		f_Dist = Cf_Infinit;
		pst_SrcPt = _pst_Src->dst_Point;
		for ( j = 0; j < _pst_Src->l_NbPoints; j++, pst_SrcPt++ )
        {
            delta = pst_SrcPt->x - pst_TgtPt->x;
            delta *= delta;
            if (delta < f_Dist)
            {
                f_Cur = pst_SrcPt->y - pst_TgtPt->y;
                f_Cur *= f_Cur;
                f_Cur += delta;

                if (delta < f_Dist)
                {
                    delta = pst_SrcPt->z - pst_TgtPt->z;
                    delta *= delta;
                    f_Cur += delta;
                    if (f_Cur < f_Dist)
                    {
                        f_Dist = f_Cur;
                        ul_Nearest = j;
                    }
                }
            }
        }
        di_VertexTable[ vertex ] = ul_Nearest;
    }
    
    /* for each face */
    for ( face = 0; face < (int) _pst_Tgt->pst_SubObject->ul_NbFaces; face++, pc_Sel++)
    {
		if (!(*pc_Sel & 1)) 
			continue;
			
		pst_Elem = _pst_Tgt->dst_Element + _pst_Tgt->pst_SubObject->dst_Faces[ face ].uw_Element;
		pst_Tri = pst_Elem->dst_Triangle + _pst_Tgt->pst_SubObject->dst_Faces[ face ].uw_Index;
			
		for (i = 0; i < 3; i++ )
		{
			GEO_PickUV_FindBestUV
			(
				&_pst_Tgt->dst_Point[ pst_Tri->auw_Index[ i ] ],
				di_VertexTable[ pst_Tri->auw_Index[ i ] ],
				_pst_Src,
				&st_UV
			);
			
			GEO_AddUV( _pst_Tgt, st_UV.fU, st_UV.fV );
			pst_Tri->auw_UV[ i ] = (USHORT) _pst_Tgt->l_NbUVs - 1;
		}
    }

    if (dc_Sel)
        L_free( dc_Sel );
	if (i_SrcSubBuild)
		GEO_SubObject_Free( _pst_Src );
	
       

    //GEO_DestroyRLI( _pst_Tgt );
}

#endif /* ACTIVE_EDITORS */
