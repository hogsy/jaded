/*$T GEOobjectaccess.c GC!1.68 01/05/00 14:42:57 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
#include "Precomp.h"
#ifndef PSX2_TARGET
#include <fcntl.h>
#endif


#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIfile.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOsubobject.h"



#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif



#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SetPoint
(
    GEO_tdst_Object *_pst_Object,
    LONG            _l_Point,
    float           _fX,
    float           _fY,
    float           _fZ,
    float           _fNX,
    float           _fNY,
    float           _fNZ
)
{
    if((_pst_Object == NULL) || (_l_Point >= _pst_Object->l_NbPoints))
        return;
    _pst_Object->dst_Point[_l_Point].x = _fX;
    _pst_Object->dst_Point[_l_Point].y = _fY;
    _pst_Object->dst_Point[_l_Point].z = _fZ;
    _pst_Object->dst_PointNormal[_l_Point].x = _fNX;
    _pst_Object->dst_PointNormal[_l_Point].y = _fNY;
    _pst_Object->dst_PointNormal[_l_Point].z = _fNZ;
}

/*
 =======================================================================================================================
    Add a point to an object
 =======================================================================================================================
 */
void GEO_AddPoint(GEO_tdst_Object *_pst_Object, float _fX, float _fY, float _fZ, float _fNX, float _fNY, float _fNZ)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_SubObject  *pst_SO;
    LONG                l_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_Object == NULL) return;

    if(_pst_Object->l_NbPoints == 0)
    {
        _pst_Object->dst_Point = (GEO_Vertex *) MEM_p_Alloc(sizeof(GEO_Vertex));
        _pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_p_Alloc(sizeof(MATH_tdst_Vector));
        if(_pst_Object->pst_SubObject)
        {
            pst_SO = _pst_Object->pst_SubObject;
            pst_SO->dc_VSel = (char *) L_malloc( 1 );
            pst_SO->dst_PointTransformed = (GEO_Vertex *) L_malloc( sizeof( GEO_Vertex ) );
        }
    }
    else
    {
        _pst_Object->dst_Point = (GEO_Vertex *) MEM_p_Realloc
            (
                _pst_Object->dst_Point,
                sizeof(GEO_Vertex) * (_pst_Object->l_NbPoints + 1)
            );
        _pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_p_Realloc
            (
                _pst_Object->dst_PointNormal,
                sizeof(MATH_tdst_Vector) * (_pst_Object->l_NbPoints + 1)
            );
            
        if ( _pst_Object->pBiNormales )
        {
			MEM_FreeAlign( _pst_Object->pBiNormales );
			_pst_Object->pBiNormales = NULL;
        }

        if (_pst_Object->dul_PointColors)
        {
            l_Size = (_pst_Object->l_NbPoints + 2) * sizeof( ULONG );
            _pst_Object->dul_PointColors = (ULONG*)MEM_p_Realloc( _pst_Object->dul_PointColors, l_Size );
            _pst_Object->dul_PointColors[0] = _pst_Object->l_NbPoints + 1;
        }

        if(_pst_Object->pst_SubObject)
        {
            pst_SO = _pst_Object->pst_SubObject;
            pst_SO->dc_VSel = (char *) L_realloc( pst_SO->dc_VSel, _pst_Object->l_NbPoints + 1 );
            pst_SO->dc_VSel[ _pst_Object->l_NbPoints ] = 0;
			if (pst_SO->dst_PointTransformed)
			{
				L_free( pst_SO->dst_PointTransformed );
				pst_SO->dst_PointTransformed = NULL;
			}
        }
    }

    GEO_SetPoint(_pst_Object, _pst_Object->l_NbPoints++, _fX, _fY, _fZ, _fNX, _fNY, _fNZ);
}

/*
 =======================================================================================================================
    Add a point to an object (added point are not initialized)
 =======================================================================================================================
 */
void GEO_AddSeveralPoint(GEO_tdst_Object *_pst_Object, int _i_Nb )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    GEO_tdst_SubObject  *pst_SO;
    LONG                l_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_Object == NULL) return;

    if(_pst_Object->l_NbPoints == 0)
    {
        _pst_Object->dst_Point = (GEO_Vertex *) MEM_p_Alloc(sizeof(GEO_Vertex) * _i_Nb);
        _pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_p_Alloc(sizeof(MATH_tdst_Vector) *_i_Nb);

        if(_pst_Object->pst_SubObject)
        {
            pst_SO = _pst_Object->pst_SubObject;
            pst_SO->dc_VSel = (char *) L_malloc( _i_Nb );
            pst_SO->dst_PointTransformed = NULL;
        }
    }
    else
    {
        l_Size = sizeof(GEO_Vertex) * (_pst_Object->l_NbPoints + _i_Nb);
        _pst_Object->dst_Point = (GEO_Vertex *) MEM_p_Realloc( _pst_Object->dst_Point, l_Size );
        _pst_Object->dst_PointNormal = (MATH_tdst_Vector *) MEM_p_Realloc( _pst_Object->dst_PointNormal, l_Size );

        if (_pst_Object->dul_PointColors)
        {
            l_Size = _pst_Object->dul_PointColors[0];
            l_Size = (_pst_Object->l_NbPoints + 1 + _i_Nb) * sizeof( ULONG );
            _pst_Object->dul_PointColors = (ULONG*)MEM_p_Realloc( _pst_Object->dul_PointColors, l_Size );
            _pst_Object->dul_PointColors[0] += _i_Nb;
        }
        
        if ( _pst_Object->pBiNormales )
        {
			MEM_FreeAlign( _pst_Object->pBiNormales );
			_pst_Object->pBiNormales = NULL;
        }
        
        if(_pst_Object->pst_SubObject)
        {
            pst_SO = _pst_Object->pst_SubObject;
            pst_SO->dc_VSel = (char *) L_realloc( pst_SO->dc_VSel, _pst_Object->l_NbPoints + _i_Nb);
			L_memset( pst_SO->dc_VSel + _pst_Object->l_NbPoints, 0, _i_Nb );
			L_free( pst_SO->dst_PointTransformed );
			pst_SO->dst_PointTransformed = NULL;
        }
    }

    _pst_Object->l_NbPoints += _i_Nb;
}

/*
 =======================================================================================================================
    del a point to an object
 =======================================================================================================================
 */
void GEO_DelPoint(GEO_tdst_Object *_pst_Object, int _i_Pt )
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    LONG                l_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(_pst_Object == NULL) return;
    if(_i_Pt >= _pst_Object->l_NbPoints) return;

    if (_i_Pt == _pst_Object->l_NbPoints - 1)
    {
        if (_pst_Object->dul_PointColors)
        {
            l_Size = _pst_Object->dul_PointColors[0];
            _pst_Object->dul_PointColors[0] = _pst_Object->l_NbPoints - 1;
        }
        _pst_Object->l_NbPoints--;
    }
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SetUV(GEO_tdst_Object *_pst_Object, LONG _lUV, float _fU, float _fV)
{
    if((_pst_Object == NULL) || (_lUV >= _pst_Object->l_NbUVs))
        return;
    _pst_Object->dst_UV[_lUV].fU = _fU;
    _pst_Object->dst_UV[_lUV].fV = _fV;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_AddUV(GEO_tdst_Object *_pst_Object, float _fU, float _fV)
{
    if(_pst_Object == NULL) return;

    if(_pst_Object->l_NbUVs == 0)
        _pst_Object->dst_UV = (GEO_tdst_UV *) MEM_p_Alloc(sizeof(GEO_tdst_UV));
    else
    {
        _pst_Object->dst_UV = (GEO_tdst_UV *) MEM_p_Realloc
            (
                _pst_Object->dst_UV,
                sizeof(GEO_tdst_UV) * (_pst_Object->l_NbUVs + 1)
            );
    }

    GEO_SetUV(_pst_Object, _pst_Object->l_NbUVs++, _fU, _fV);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DelUV(GEO_tdst_Object *_pst_Object, int _i_UV)
{
    if (_pst_Object == NULL) return;
    if ((_i_UV < 0) || (_i_UV>= _pst_Object->l_NbUVs)) return;

    if (_i_UV == _pst_Object->l_NbUVs - 1)
        _pst_Object->l_NbUVs--;
}


/*
 =======================================================================================================================
    Set an object element
 =======================================================================================================================
 */
void GEO_SetElement(GEO_tdst_Object *_pst_Object, LONG _l_Element, LONG _l_MaterialId, LONG _l_NbTriangles)
{
    if((_pst_Object == NULL) || (_l_Element >= _pst_Object->l_NbElements))
        return;

	
    _pst_Object->dst_Element[_l_Element].l_MaterialId = _l_MaterialId;
    _pst_Object->dst_Element[_l_Element].l_NbTriangles = _l_NbTriangles;

	_pst_Object->dst_Element[_l_Element].ul_NumberOfUsedIndex = 0;
	_pst_Object->dst_Element[_l_Element].p_MrmElementAdditionalInfo = NULL;
    GEO_AllocElementContent(&_pst_Object->dst_Element[_l_Element]);
}

/*
 =======================================================================================================================
    Add an element to object
 =======================================================================================================================
 */
void GEO_AddElement(GEO_tdst_Object *_pst_Object, LONG _l_MaterialId, LONG _l_NbTriangles)
{
    if(_pst_Object == NULL) return;

    if(_pst_Object->l_NbElements == 0)
    {
        _pst_Object->dst_Element = (GEO_tdst_ElementIndexedTriangles *) MEM_p_Alloc(sizeof(GEO_tdst_ElementIndexedTriangles));
        L_memset(_pst_Object->dst_Element, 0, sizeof(GEO_tdst_ElementIndexedTriangles));
    }
    else
    {
        _pst_Object->dst_Element = (GEO_tdst_ElementIndexedTriangles *) MEM_p_Realloc
            (
                _pst_Object->dst_Element,
                sizeof(GEO_tdst_ElementIndexedTriangles) * (_pst_Object->l_NbElements + 1)
            );
        L_memset(_pst_Object->dst_Element+_pst_Object->l_NbElements, 0, sizeof(GEO_tdst_ElementIndexedTriangles));
    }

    GEO_SetElement(_pst_Object, _pst_Object->l_NbElements++, _l_MaterialId, _l_NbTriangles);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_SetTriangle
(
    GEO_tdst_ElementIndexedTriangles    *_pst_Element,
    LONG                                _lTriangle,
    GEO_tdst_IndexedTriangle            *_pst_Triangle
)
{
    if((_pst_Element == NULL) || (_lTriangle >= _pst_Element->l_NbTriangles))
        return;
    L_memcpy(&_pst_Element->dst_Triangle[_lTriangle], _pst_Triangle, sizeof(GEO_tdst_IndexedTriangle));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_AddTriangle(GEO_tdst_ElementIndexedTriangles *_pst_Element, GEO_tdst_IndexedTriangle *_pst_Triangle)
{
    if(_pst_Element == NULL) return;

    if(_pst_Element->l_NbTriangles == 0)
        _pst_Element->dst_Triangle = (GEO_tdst_IndexedTriangle *) MEM_p_Alloc(sizeof(GEO_tdst_IndexedTriangle));
    else
    {
        _pst_Element->dst_Triangle = (GEO_tdst_IndexedTriangle *) MEM_p_Realloc
            (
                _pst_Element->dst_Triangle,
                sizeof(GEO_tdst_IndexedTriangle) * (_pst_Element->l_NbTriangles + 1)
            );
    }

    _pst_Triangle->ul_MaxFlags &= 0xFF000000;
    GEO_SetTriangle(_pst_Element, _pst_Element->l_NbTriangles++, _pst_Triangle);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_DelTriangle(GEO_tdst_ElementIndexedTriangles *_pst_Element, int i_Index )
{
    if(_pst_Element == NULL) return;
    if(_pst_Element->l_NbTriangles <= i_Index) return;

    if(i_Index < _pst_Element->l_NbTriangles - 1)
    {
        L_memcpy( &_pst_Element->dst_Triangle[ i_Index ], &_pst_Element->dst_Triangle[ _pst_Element->l_NbTriangles - 1 ], sizeof( GEO_tdst_IndexedTriangle ) );
    }
    _pst_Element->l_NbTriangles--;
}

#endif


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif