/*$T GEOcreateobject.c GC! 1.081 05/25/01 14:30:26 */


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
#include "GEOmetric/GEOcreateobject.h"
#include "MATHs/MATH.h"
#include "GDInterface/GDInterface.h"
#include "SOFT/SOFTlinear.h"
#include "SOFT/SOFThelper.h"
#include "GEOmetric/GEOsubobject.h"
#include "GEOmetric/GEOobject.h"
#include "ENGine/sources/WORld/WORupdate.h"
#include "ENGine/sources/WORld/WORuniverse.h"
#include "ENGine/sources/OBJects/OBJorient.h"

#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLinit.h"
#include "ENGine/Sources/COLlision/COLsave.h"

#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/SAVing/SAVdefs.h"

#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"





/*$4
 ***********************************************************************************************************************
    Macros
 ***********************************************************************************************************************
 */

#define GEOCO_SetVertex(_x_, _y_, _z_) \
	{ \
		MATH_InitVector(V, _x_, _y_, _z_); \
		V++; \
	}

#define GEOCO_CopyVertex( _VSrc_ )\
    { \
		MATH_CopyVector(V, _VSrc_); \
		V++; \
	}

#define GEOCO_SetTriangleIndex(_i0_, _i1_, _i2_) \
	{ \
        T->ul_MaxFlags = 0x53;\
		T->auw_Index[0] = _i0_; \
		T->auw_Index[1] = _i1_; \
		T->auw_Index[2] = _i2_; \
		T++; \
	}

#define GEOCO_SetTriangleIndexAndUV(_i0_, _i1_, _i2_) \
	{ \
        T->ul_MaxFlags = 0x53;\
		T->auw_Index[0] = _i0_; \
		T->auw_Index[1] = _i1_; \
		T->auw_Index[2] = _i2_; \
        T->auw_UV[0] = _i0_; \
        T->auw_UV[1] = _i1_; \
        T->auw_UV[2] = _i2_; \
		T++; \
	}

#define COLCO_SetTriangleIndex(_i0_, _i1_, _i2_) \
	{ \
		T->auw_Index[0] = _i0_; \
		T->auw_Index[1] = _i1_; \
		T->auw_Index[2] = _i2_; \
		T++; \
	}




/*$4
 ***********************************************************************************************************************
    Simple functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ul_ChangeVertexNumber(GEO_tdst_Object *_pst_Obj, LONG _l_Nb)
{
	/*~~~~~~~~~~~~*/
	ULONG	ul_Size;
	/*~~~~~~~~~~~~*/

	if(!_l_Nb) return;
	if(_l_Nb == _pst_Obj->l_NbPoints) return;

	if(_l_Nb > _pst_Obj->l_NbPoints)
	{
		ul_Size = _l_Nb * sizeof(GEO_Vertex);
		_pst_Obj->dst_Point = (GEO_Vertex *) MEM_p_Realloc(_pst_Obj->dst_Point, ul_Size);
		GEO_UseNormals(_pst_Obj);
		_pst_Obj->dst_PointNormal = (GEO_Vertex *) MEM_p_Realloc(_pst_Obj->dst_PointNormal, ul_Size);
	}

	if(_pst_Obj->dul_PointColors)
	{
		ul_Size = _l_Nb * sizeof(ULONG) + 1;
		_pst_Obj->dul_PointColors = (ULONG *) MEM_p_Realloc(_pst_Obj->dul_PointColors, ul_Size);
		*_pst_Obj->dul_PointColors = _l_Nb;
	}

	_pst_Obj->l_NbPoints = _l_Nb;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ul_ChangeUVNumber(GEO_tdst_Object *_pst_Obj, LONG _l_Nb)
{
	if(!_l_Nb) return;
	if(_l_Nb == _pst_Obj->l_NbUVs) return;

	if(_l_Nb > _pst_Obj->l_NbUVs)
		_pst_Obj->dst_UV = (GEO_tdst_UV *) MEM_p_Realloc(_pst_Obj->dst_UV, _l_Nb * sizeof(GEO_tdst_UV));
	_pst_Obj->l_NbUVs = _l_Nb;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
extern void GEO_FreeElementContent(GEO_tdst_ElementIndexedTriangles *);

void GEO_ul_ChangeElemNumber( GEO_tdst_Object *_pst_Obj, LONG _l_Nb)
{
    int i;
	if(!_l_Nb) return;
	if(_l_Nb == _pst_Obj->l_NbElements) return;

	if(_l_Nb > _pst_Obj->l_NbElements)
    {
		_pst_Obj->dst_Element = (GEO_tdst_ElementIndexedTriangles *) MEM_p_Realloc(_pst_Obj->dst_Element, _l_Nb * sizeof(GEO_tdst_ElementIndexedTriangles));
        for (i = _pst_Obj->l_NbElements; i < _l_Nb; i++)
            L_memset( &_pst_Obj->dst_Element[ i ], 0, sizeof( GEO_tdst_ElementIndexedTriangles ) );
    }
    else
    {
        for(  i = _l_Nb; i < _pst_Obj->l_NbElements; i++)
            GEO_FreeElementContent( _pst_Obj->dst_Element + i );
    }
	_pst_Obj->l_NbElements = _l_Nb;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_ul_ChangeTriangleNumber(GEO_tdst_Object *_pst_Obj, int _i_Index, LONG _l_Nb)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ElementIndexedTriangles	*E;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	E = _pst_Obj->dst_Element + _i_Index;
	if(_l_Nb == E->l_NbTriangles) return;

    if (E->l_NbTriangles == 0)
        E->dst_Triangle = (GEO_tdst_IndexedTriangle*)MEM_p_AllocAlign(_l_Nb * sizeof(GEO_tdst_IndexedTriangle), 16);
	else if(_l_Nb > E->l_NbTriangles)
    	E->dst_Triangle = (GEO_tdst_IndexedTriangle*)MEM_p_ReallocAlign(E->dst_Triangle, _l_Nb * sizeof(GEO_tdst_IndexedTriangle), 16);
	E->l_NbTriangles = _l_Nb;
}

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_CO_Text(GEO_tdst_Object **_ppst_Obj, char *sz_Path, char *sz_Name, GEO_tdst_CO_Text *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i;
	float						w, h, x, angle, dangle;
	GEO_Vertex					*V;
	GEO_tdst_Object				*pst_Obj;
	GEO_tdst_IndexedTriangle	*T;
	MATH_tdst_Vector			dir, normal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(*_ppst_Obj == NULL)
	{
		pst_Obj = GEO_pst_Create(_pst_Data->i_NbChar * 4, _pst_Data->i_NbChar * 4, 1, 0);
		L_memset(&pst_Obj->dst_Element[0], 0, sizeof(GEO_tdst_ElementIndexedTriangles));
		pst_Obj->dst_Element[0].l_NbTriangles = _pst_Data->i_NbChar * 2;
		GEO_AllocElementContent(pst_Obj->dst_Element);
		*_ppst_Obj = pst_Obj;
		GRO_Struct_SetName(&pst_Obj->st_Id, sz_Name);
	}
	else
	{
		pst_Obj = *_ppst_Obj;
		GEO_ul_ChangeVertexNumber(pst_Obj, _pst_Data->i_NbChar * 4);
		GEO_ul_ChangeUVNumber(pst_Obj, _pst_Data->i_NbChar * 4);
		GEO_ul_ChangeTriangleNumber(pst_Obj, 0, _pst_Data->i_NbChar * 2);
	}

	if(_pst_Data->f_Radius == 0)
	{
		w = _pst_Data->i_NbChar * _pst_Data->f_Width + (_pst_Data->i_NbChar - 1) * _pst_Data->f_Espacement;
		h = _pst_Data->f_Height / 2;
		x = -w / 2;
		V = pst_Obj->dst_Point;

		for(i = 0; i < _pst_Data->i_NbChar; i++)
		{
			GEOCO_SetVertex(x, h, 0);
			GEOCO_SetVertex(x, -h, 0);
			x += _pst_Data->f_Width;

			GEOCO_SetVertex(x, h, 0);
			GEOCO_SetVertex(x, -h, 0);
			x += _pst_Data->f_Espacement;
		}
	}
	else	/* circular texte */
	{
		angle = 0;
		dangle = (_pst_Data->f_Width + _pst_Data->f_Espacement) / _pst_Data->f_Radius;
		V = pst_Obj->dst_Point;

		for(i = 0; i < _pst_Data->i_NbChar; i++, angle -= dangle)
		{
			MATH_InitVector(&dir, fCos(angle), fSin(angle), 0);
			MATH_InitVector(&normal, -dir.y, dir.x, 0);

			MATH_ScaleVector(V, &dir, _pst_Data->f_Height + _pst_Data->f_Radius);
			MATH_AddScaleVector(V, V, &normal, _pst_Data->f_Width / 2);
			V++;
			MATH_ScaleVector(V, &dir, _pst_Data->f_Radius);
			MATH_AddScaleVector(V, V, &normal, _pst_Data->f_Width / 2);
			V++;

			MATH_ScaleVector(V, &dir, _pst_Data->f_Height + _pst_Data->f_Radius);
			MATH_AddScaleVector(V, V, &normal, -_pst_Data->f_Width / 2);
			V++;
			MATH_ScaleVector(V, &dir, _pst_Data->f_Radius);
			MATH_AddScaleVector(V, V, &normal, -_pst_Data->f_Width / 2);
			V++;
		}
	}

	T = pst_Obj->dst_Element[0].dst_Triangle;
	for(i = 0; i < _pst_Data->i_NbChar; i++)
	{
		T->auw_Index[0] = T->auw_UV[0] = (i * 4) + 0;
		T->auw_Index[1] = T->auw_UV[1] = (i * 4) + 1;
		T->auw_Index[2] = T->auw_UV[2] = (i * 4) + 2;
		T->ul_MaxFlags = 0;
		T++;

		T->auw_Index[0] = T->auw_UV[0] = (i * 4) + 2;
		T->auw_Index[1] = T->auw_UV[1] = (i * 4) + 1;
		T->auw_Index[2] = T->auw_UV[2] = (i * 4) + 3;
		T->ul_MaxFlags = 0;
		T++;
	}

	GEO_ComputeNormals(pst_Obj);
    pst_Obj->ul_EditorFlags = GEO_CEF_GeomFor3DText;

	return GRO_ul_Struct_FullSave((GRO_tdst_Struct *) pst_Obj, sz_Path, sz_Name, &TEX_gst_GlobalList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_CO_Plane(GEO_tdst_Object **_ppst_Obj, char *sz_Path, char *sz_Name, GEO_tdst_CO_Plane *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, j, k, dk, NbV, NbT;
	float						x, y, dx, dy, u, v, du, dv;
	GEO_Vertex					*V;
	GEO_tdst_UV					*UV;
	GEO_tdst_Object				*pst_Obj;
	GEO_tdst_IndexedTriangle	*T;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	NbV = (_pst_Data->i_NbX + 1) * (_pst_Data->i_NbY + 1);
	NbT = (_pst_Data->i_NbX) * (_pst_Data->i_NbY) * 2;

	if(*_ppst_Obj == NULL)
	{
		pst_Obj = GEO_pst_Create(NbV, NbV, 1, 0);
		L_memset(&pst_Obj->dst_Element[0], 0, sizeof(GEO_tdst_ElementIndexedTriangles));
		pst_Obj->dst_Element[0].l_NbTriangles = NbT;
		GEO_AllocElementContent(pst_Obj->dst_Element);
		*_ppst_Obj = pst_Obj;
		GRO_Struct_SetName(&pst_Obj->st_Id, sz_Name);
	}
	else
	{
		pst_Obj = *_ppst_Obj;
		GEO_ul_ChangeVertexNumber(pst_Obj, NbV);
		GEO_ul_ChangeUVNumber(pst_Obj, NbV);
		GEO_ul_ChangeTriangleNumber(pst_Obj, 0, NbT);
	}

	V = pst_Obj->dst_Point;
	UV = pst_Obj->dst_UV;

	du = 1.0f / _pst_Data->i_NbX;
	dv = 1.0f / _pst_Data->i_NbY;
	dx = _pst_Data->x / _pst_Data->i_NbX;
	dy = _pst_Data->y / _pst_Data->i_NbY;
	y = _pst_Data->y / 2;
	v = 0;

	for(j = 0; j <= _pst_Data->i_NbY; j++, y -= dy, v += dv)
	{
		x = -_pst_Data->x / 2;
		u = 0;
		for(i = 0; i <= _pst_Data->i_NbX; i++, x += dx, u += du)
		{
			GEOCO_SetVertex(x, y, 0);
			UV->fU = u;
			UV->fV = v;
			UV++;
		}
	}

	T = pst_Obj->dst_Element[0].dst_Triangle;
	k = 0;
	dk = _pst_Data->i_NbX + 1;

	for(j = 0; j < _pst_Data->i_NbY; j++, k += dk)
	{
		for(i = 0; i < _pst_Data->i_NbX; i++)
		{
			T->auw_Index[0] = T->auw_UV[0] = k + i;
			T->auw_Index[1] = T->auw_UV[1] = k + dk + i;
			T->auw_Index[2] = T->auw_UV[2] = k + i + 1;
#ifdef JADEFUSION
			T->ul_MaxFlags = 0x05;
#else
			T->ul_MaxFlags = 0;
#endif
			T++;

			T->auw_Index[0] = T->auw_UV[0] = k + i + 1;
			T->auw_Index[1] = T->auw_UV[1] = k + i + dk;
			T->auw_Index[2] = T->auw_UV[2] = k + i + 1 + dk;
#ifdef JADEFUSION
			T->ul_MaxFlags = 0x06;
#else
			T->ul_MaxFlags = 0;
#endif			
			T++;
		}
	}

	GEO_ComputeNormals(pst_Obj);

	return GRO_ul_Struct_FullSave((GRO_tdst_Struct *) pst_Obj, sz_Path, sz_Name, &TEX_gst_GlobalList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_CO_Box(GEO_tdst_Object **_ppst_Obj, char *sz_Path, char *sz_Name, GEO_tdst_CO_Box *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, j, k, nk, di, dj, dk, NbV, NbUV, p, i_Elem;
	float						x, y, z, dx, dy, dz, du, dv, dw;
	GEO_Vertex					*V;
	GEO_tdst_UV					*UV;
	GEO_tdst_Object				*pst_Obj;
	GEO_tdst_IndexedTriangle	*T;
    int                         NbElem, ai_NbT[ 6 ], ai_NbTUsed[ 6 ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	di = _pst_Data->i_NbX + 1;
	dj = _pst_Data->i_NbY + 1;
	dk = _pst_Data->i_NbZ + 1;

	NbV = di * dj * dk;
	NbV -= (_pst_Data->i_NbX - 1) * (_pst_Data->i_NbY - 1) * (_pst_Data->i_NbZ - 1);

	NbUV = di * dj + di * dk + dj * dk;

    NbElem = (_pst_Data->i_NbElem < 1) ? 1 : ((_pst_Data->i_NbElem > 6) ? 6 : _pst_Data->i_NbElem); 
    L_memset( ai_NbT, 0, sizeof( ai_NbT ) );
    L_memset( ai_NbTUsed, 0, sizeof( ai_NbTUsed ) );
    ai_NbT[ 0 ] += 2 * _pst_Data->i_NbX * _pst_Data->i_NbY;
    ai_NbT[ 1 % NbElem ] += 2 * _pst_Data->i_NbX * _pst_Data->i_NbY;
    ai_NbT[ 2 % NbElem ] += 2 * _pst_Data->i_NbY * _pst_Data->i_NbZ;
    ai_NbT[ 3 % NbElem ] += 2 * _pst_Data->i_NbY * _pst_Data->i_NbZ;
    ai_NbT[ 4 % NbElem ] += 2 * _pst_Data->i_NbX * _pst_Data->i_NbZ;
    ai_NbT[ 5 % NbElem ] += 2 * _pst_Data->i_NbX * _pst_Data->i_NbZ;

	if(*_ppst_Obj == NULL)
	{
		pst_Obj = GEO_pst_Create(NbV, NbUV, NbElem, 0);
		*_ppst_Obj = pst_Obj;
		GRO_Struct_SetName(&pst_Obj->st_Id, sz_Name);

		for(i = 0; i < NbElem; i++)
		{
			L_memset(&pst_Obj->dst_Element[i], 0, sizeof(GEO_tdst_ElementIndexedTriangles));
			pst_Obj->dst_Element[i].l_NbTriangles = ai_NbT[ i ];
			GEO_AllocElementContent(pst_Obj->dst_Element + i);
            pst_Obj->dst_Element[i].l_MaterialId = i;
		}
	}
	else
	{
		pst_Obj = *_ppst_Obj;
		GEO_ul_ChangeVertexNumber(pst_Obj, NbV);
		GEO_ul_ChangeUVNumber(pst_Obj, NbUV);
        GEO_ul_ChangeElemNumber( pst_Obj, NbElem );
		for(i = 0; i < NbElem; i++)
		{
			GEO_ul_ChangeTriangleNumber(pst_Obj, i, ai_NbT[ i ]);
            pst_Obj->dst_Element[i].l_MaterialId = i;
		}
	}

	du = 1.0f / _pst_Data->i_NbX;
	dv = 1.0f / _pst_Data->i_NbY;
	dw = 1.0f / _pst_Data->i_NbZ;
	dx = _pst_Data->x / _pst_Data->i_NbX;
	dy = _pst_Data->y / _pst_Data->i_NbY;
	dz = _pst_Data->z / _pst_Data->i_NbZ;
	p = 2 * (_pst_Data->i_NbX + _pst_Data->i_NbY);

	/*$1- Vertex ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	V = pst_Obj->dst_Point;

	/* down face and up face vertex */
	z = 0;
	for(k = 0; k < 2; k++)
	{
		y = -_pst_Data->y / 2;
		for(j = 0; j <= _pst_Data->i_NbY; j++, y += dy)
		{
			x = -_pst_Data->x / 2;
			for(i = 0; i <= _pst_Data->i_NbX; i++, x += dx) GEOCO_SetVertex(x, y, z);
		}

		z = _pst_Data->z;
	}

	/* other vertices */
	z = dz;
	for(k = 1; k < _pst_Data->i_NbZ; k++, z += dz)
	{
		y = -_pst_Data->y / 2;
		x = -_pst_Data->x / 2;
		for(i = 0; i < _pst_Data->i_NbX; i++, x += dx) GEOCO_SetVertex(x, y, z);
		for(j = 0; j < _pst_Data->i_NbY; j++, y += dy) GEOCO_SetVertex(x, y, z);
		for(i = 0; i < _pst_Data->i_NbX; i++, x -= dx) GEOCO_SetVertex(x, y, z);
		for(j = 0; j < _pst_Data->i_NbY; j++, y -= dy) GEOCO_SetVertex(x, y, z);
	}

	/*$1- UV ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	UV = pst_Obj->dst_UV;

	/* left and right UV */
    for(k = 0; k < dk; k++)
	    for(j = 0; j < dj; j++)
		{
			UV->fU = dv * j;
			UV->fV = dw * k;
			UV++;
		}

	/* front and rear UV */
	for(k = 0; k < dk; k++)
        for(i = 0; i < di; i++)
		{
			UV->fU = du * i;
			UV->fV = dw * k;
			UV++;
		}

	/* up and down UV */
	for(j = 0; j < dj; j++)
        for(i = 0; i < di; i++)
		{
			UV->fU = du * i;
			UV->fV = dv * j;
			UV++;
		}

	/*$1- Triangles ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* down face */
    i_Elem = 1 % NbElem;
	T = pst_Obj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
	for(j = 0, k = 0; j < _pst_Data->i_NbY; j++, k += di)
	{
		for(i = 0; i < _pst_Data->i_NbX; i++)
        {
			GEOCO_SetTriangleIndex(k + i, k + i + di, k + i + di + 1);
			GEOCO_SetTriangleIndex(k + i + di + 1, k + i + 1, k + i);
		}
	}
    ai_NbTUsed[ i_Elem ] = T - pst_Obj->dst_Element[ i_Elem ].dst_Triangle;

	/* up face */
    i_Elem = 0;
	T = pst_Obj->dst_Element[0].dst_Triangle + ai_NbTUsed[ 0 ];
	for(j = 0, k = di * dj; j < _pst_Data->i_NbY; j++, k += di)
	{
        for(i = 0; i < _pst_Data->i_NbX; i++)
		{
			GEOCO_SetTriangleIndex(k + i, k + i + 1, k + i + di + 1);
			GEOCO_SetTriangleIndex(k + i + di + 1, k + i + di, k + i);
		}
	}
    ai_NbTUsed[ 0 ] = T - pst_Obj->dst_Element[ 0 ].dst_Triangle;

	/* front face */
    i_Elem = 4 % NbElem;
	T = pst_Obj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
	for(j = 0, k = 0, nk = 2 * di * dj; j < _pst_Data->i_NbZ; j++, k = nk, nk += p)
	{
		if(nk >= NbV) nk = di * dj;

		for(i = 0; i < _pst_Data->i_NbX; i++)
        {
			GEOCO_SetTriangleIndex(k + i, k + i + 1, nk + i + 1);
			GEOCO_SetTriangleIndex(nk + i + 1, nk + i, k + i);
		}
	}
    ai_NbTUsed[ i_Elem ] = T - pst_Obj->dst_Element[ i_Elem ].dst_Triangle;

	/* right face */
    i_Elem = 3 % NbElem;
    T = pst_Obj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
    //T = pst_Obj->dst_Element[3].dst_Triangle;
    if (_pst_Data->i_NbZ == 1)
    {
        for(k = di - 1, nk = di * dj + (di - 1), i = 0, j = 0; i < _pst_Data->i_NbY; i++, j += di ) 
	    {
		    GEOCO_SetTriangleIndex(k + j, k + j + di, nk + j + di);
		    GEOCO_SetTriangleIndex(nk + j + di, nk + j, k + j);
	    }
    }
    else
    {
	    for(k = di - 1, nk = 2 * di * dj + (di - 1), i = 0, j = 0; i < _pst_Data->i_NbY; i++, j += di)
	    {
		    GEOCO_SetTriangleIndex(k + j, k + j + di, nk + i + 1);
		    GEOCO_SetTriangleIndex(nk + i + 1, nk + i, k + j);
	    }

	    for(j = 1, k = nk, nk += p; j < _pst_Data->i_NbZ - 1; j++, k = nk, nk += p)
	    {
		    for(i = 0; i < _pst_Data->i_NbY; i++)
		    {
			    GEOCO_SetTriangleIndex(k + i, k + i + 1, nk + i + 1);
			    GEOCO_SetTriangleIndex(nk + i + 1, nk + i, k + i);
		    }
	    }

	    for(nk = di * dj + (di - 1), i = 0, j = 0; i < _pst_Data->i_NbY; i++, j += di)
	    {
		    GEOCO_SetTriangleIndex(k + i, k + i + 1, nk + j + di);
		    GEOCO_SetTriangleIndex(nk + j + di, nk + j, k + i);
	    }
    }
    ai_NbTUsed[ i_Elem ] = T - pst_Obj->dst_Element[ i_Elem ].dst_Triangle;

	/* back face */
    i_Elem = 5 % NbElem;
	T = pst_Obj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
    //T = pst_Obj->dst_Element[5].dst_Triangle;
    if (_pst_Data->i_NbZ == 1)
    {
        for(k = di * dj - 1, nk = 2 * di * dj - 1, i = 0, j = 0; i < _pst_Data->i_NbX; i++, j-- ) 
	    {
		    GEOCO_SetTriangleIndex(k + j, k + j - 1, nk + j - 1);
		    GEOCO_SetTriangleIndex(nk + j - 1, nk + j, k + j);
	    }
    }
    else
    {
	    for(k = di * dj - di, nk = 2 * di * dj + (di + dj - 2), i = 0, j = di - 1; i < _pst_Data->i_NbX; i++, j--)
	    {
		    GEOCO_SetTriangleIndex(k + j, k + j - 1, nk + i + 1);
		    GEOCO_SetTriangleIndex(nk + i + 1, nk + i, k + j);
	    }

	    for(j = 1, k = nk, nk += p; j < _pst_Data->i_NbZ - 1; j++, k = nk, nk += p)
	    {
		    for(i = 0; i < _pst_Data->i_NbX; i++)
		    {
			    GEOCO_SetTriangleIndex(k + i, k + i + 1, nk + i + 1);
			    GEOCO_SetTriangleIndex(nk + i + 1, nk + i, k + i);
		    }
	    }

	    for(nk = 2 * di * dj - di, i = 0, j = di - 1; i < _pst_Data->i_NbX; i++, j--)
	    {
		    GEOCO_SetTriangleIndex(k + i, k + i + 1, nk + j - 1);
		    GEOCO_SetTriangleIndex(nk + j - 1, nk + j, k + i);
	    }
    }
    ai_NbTUsed[ i_Elem ] = T - pst_Obj->dst_Element[ i_Elem ].dst_Triangle;

	/* left face */
    i_Elem = 2 % NbElem;
	T = pst_Obj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
    //T = pst_Obj->dst_Element[2].dst_Triangle;
    if (_pst_Data->i_NbZ == 1)
    {
        for(k = di * dj - di, nk = 2 * di * dj - di, i = 0, j = 0; i < _pst_Data->i_NbY - 1; i++, j -= di)
	    {
		    GEOCO_SetTriangleIndex(k + j, k + j - di, nk + j - di);
		    GEOCO_SetTriangleIndex(nk + j - di, nk + j, k + j);
	    }
        GEOCO_SetTriangleIndex(k + j, k + j - di, nk + j - di);
	    GEOCO_SetTriangleIndex(nk + j - di, nk + j, k + j);
    }
    else
    {
	    for(k = di * dj - di, nk = 2 * di * dj + p - (dj - 1), i = 0, j = 0; i < _pst_Data->i_NbY - 1; i++, j -= di)
	    {
		    GEOCO_SetTriangleIndex(k + j, k + j - di, nk + i + 1);
		    GEOCO_SetTriangleIndex(nk + i + 1, nk + i, k + j);
	    }

	    GEOCO_SetTriangleIndex(k + j, k + j - di, 2 * di * dj);
	    GEOCO_SetTriangleIndex(2 * di * dj, nk + i, k + j);

	    for(j = 1, k = nk, nk += p; j < _pst_Data->i_NbZ - 1; j++, k = nk, nk += p)
	    {
		    for(i = 0; i < _pst_Data->i_NbY - 1; i++)
		    {
			    GEOCO_SetTriangleIndex(k + i, k + i + 1, nk + i + 1);
			    GEOCO_SetTriangleIndex(nk + i + 1, nk + i, k + i);
		    }

		    GEOCO_SetTriangleIndex(k + i, k + i + 1 - p, nk + i + 1 - p);
		    GEOCO_SetTriangleIndex(nk + i + 1 - p, nk + i, k + i);
	    }

	    for(nk = 2 * di * dj - di, i = 0, j = 0; i < _pst_Data->i_NbY - 1; i++, j -= di)
	    {
		    GEOCO_SetTriangleIndex(k + i, k + i + 1, nk + j - di);
		    GEOCO_SetTriangleIndex(nk + j - di, nk + j, k + i);
	    }

	    GEOCO_SetTriangleIndex(k + i, k + i + 1 - p, nk + j - di);
	    GEOCO_SetTriangleIndex(nk + j - di, nk + j, k + i);
    }
    ai_NbTUsed[ i_Elem ] = T - pst_Obj->dst_Element[ i_Elem ].dst_Triangle;

    /* UV of faces */
    L_memset( ai_NbTUsed, 0, sizeof( ai_NbTUsed ) );

    /* UV top faces */
    i_Elem = 1 % NbElem;
    T = pst_Obj->dst_Element[i_Elem].dst_Triangle;
    for(k = di * dk + dj * dk, j = 0; j < dj - 1; j++, k+= di)
	    for(i = di - 1; i > 0; i--)
		{
            T->auw_UV[0] = k + i;
            T->auw_UV[1] = k + i + di;
            T->auw_UV[2] = k + i + di - 1;
            T->ul_MaxFlags = 0;
			T++;

            T->auw_UV[0] = k + i + di - 1;
            T->auw_UV[1] = k + i -1;
            T->auw_UV[2] = k + i;
            T->ul_MaxFlags = 0;
			T++;
		}
    ai_NbTUsed[ i_Elem ] = T - pst_Obj->dst_Element[ i_Elem ].dst_Triangle;

    /* UV down faces */
    T = pst_Obj->dst_Element[0].dst_Triangle + ai_NbTUsed[ 0 ] ;
    for(k = di * dk + dj * dk, j = 0; j < dj - 1; j++, k+= di)
	    for(i = 0; i < di - 1; i++)
		{
            T->auw_UV[0] = k + i;
            T->auw_UV[1] = k + i + 1;
            T->auw_UV[2] = k + i + di + 1;
            T->ul_MaxFlags = 0;
			T++;

            T->auw_UV[0] = k + i + di + 1;
            T->auw_UV[1] = k + i + di;
            T->auw_UV[2] = k + i;
            T->ul_MaxFlags = 0;
			T++;
		}
    ai_NbTUsed[0] = T - pst_Obj->dst_Element[ 0 ].dst_Triangle;

    /* UV for front and rear faces */
    i_Elem = 4 % NbElem;
    T = pst_Obj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
    for(k = dj * dk, j = 0; j < dk - 1; j++, k+= di)
	    for(i = 0; i < di - 1; i++)
		{
            T->auw_UV[0] = k + i;
            T->auw_UV[1] = k + i + 1;
            T->auw_UV[2] = k + i + di + 1;
            T->ul_MaxFlags = 0;
			T++;

            T->auw_UV[0] = k + i + di + 1;
            T->auw_UV[1] = k + i + di;
            T->auw_UV[2] = k + i;
            T->ul_MaxFlags = 0;
			T++;
		}
    ai_NbTUsed[ i_Elem ] = T - pst_Obj->dst_Element[ i_Elem ].dst_Triangle;
    

    /* UV for left and right faces */
    i_Elem = 3 % NbElem;
    T = pst_Obj->dst_Element[i_Elem].dst_Triangle + ai_NbTUsed[ i_Elem ];
    for(k = 0, j = 0; j < dk - 1; j++, k+= dj)
	    for(i = 0; i < dj - 1; i++)
		{
            T->auw_UV[0] = k + i;
            T->auw_UV[1] = k + i + 1;
            T->auw_UV[2] = k + i + dj + 1;
            T->ul_MaxFlags = 0;
			T++;

            T->auw_UV[0] = k + i + dj + 1;
            T->auw_UV[1] = k + i + dj;
            T->auw_UV[2] = k + i;
            T->ul_MaxFlags = 0;
			T++;
		}
    ai_NbTUsed[ i_Elem ] = T - pst_Obj->dst_Element[ i_Elem ].dst_Triangle;

    /* UV for front and rear faces */
    i_Elem = 5 % NbElem;
    T = pst_Obj->dst_Element[i_Elem].dst_Triangle + ai_NbTUsed[ i_Elem ];
    for(k = dj * dk, j = 0; j < dk - 1; j++, k+= di)
	    for(i = 0; i < di - 1; i++)
		{
            T->auw_UV[0] = k + i;
            T->auw_UV[1] = k + i + 1;
            T->auw_UV[2] = k + i + di + 1;
            T->ul_MaxFlags = 0;
			T++;

            T->auw_UV[0] = k + i + di + 1;
            T->auw_UV[1] = k + i + di;
            T->auw_UV[2] = k + i;
            T->ul_MaxFlags = 0;
			T++;
		}
    ai_NbTUsed[ i_Elem ] = T - pst_Obj->dst_Element[ i_Elem ].dst_Triangle;
    
    /* UV for left and right faces */
    i_Elem = 2 % NbElem;
    T = pst_Obj->dst_Element[i_Elem].dst_Triangle + ai_NbTUsed[ i_Elem ];
    for(k = 0, j = 0; j < dk - 1; j++, k+= dj)
	    for(i = 0; i < dj - 1; i++)
		{
            T->auw_UV[0] = k + i;
            T->auw_UV[1] = k + i + 1;
            T->auw_UV[2] = k + i + dj + 1;
            T->ul_MaxFlags = 0;
			T++;

            T->auw_UV[0] = k + i + dj + 1;
            T->auw_UV[1] = k + i + dj;
            T->auw_UV[2] = k + i;
            T->ul_MaxFlags = 0;
			T++;
		}
    ai_NbTUsed[ i_Elem ] = T - pst_Obj->dst_Element[ i_Elem ].dst_Triangle;

	GEO_ComputeNormals(pst_Obj);
	return GRO_ul_Struct_FullSave((GRO_tdst_Struct *) pst_Obj, sz_Path, sz_Name, &TEX_gst_GlobalList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_CO_Cylindre(GEO_tdst_Object **_ppst_Obj, char *sz_Path, char *sz_Name, GEO_tdst_CO_Cylindre *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							l, dl, i, j, k, NbV, NbT, NbUV;
	float						R, z, angle, dangle;
	GEO_Vertex					*V;
   	GEO_tdst_UV					*UV;
	GEO_tdst_Object				*pst_Obj;
	GEO_tdst_IndexedTriangle	*T;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    dl = _pst_Data->i_NbSides;
	NbV = 2 + ( ( 2 * _pst_Data->i_NbCapSegs ) + _pst_Data->i_NbZSegs - 1 ) * dl;
    NbUV = NbV + 2 * dl + _pst_Data->i_NbZSegs + 1;
    NbT = dl * ( 2 * (2 * _pst_Data->i_NbCapSegs - 1) + _pst_Data->i_NbZSegs * 2 );
    dl = _pst_Data->i_NbSides;

    if(*_ppst_Obj == NULL)
	{
		pst_Obj = GEO_pst_Create(NbV, NbUV, 1, 0);
		*_ppst_Obj = pst_Obj;
		GRO_Struct_SetName(&pst_Obj->st_Id, sz_Name);
		L_memset(&pst_Obj->dst_Element[0], 0, sizeof(GEO_tdst_ElementIndexedTriangles));
		pst_Obj->dst_Element[0].l_NbTriangles = NbT;
		GEO_AllocElementContent(pst_Obj->dst_Element);
	}
	else
	{
		pst_Obj = *_ppst_Obj;
		GEO_ul_ChangeVertexNumber(pst_Obj, NbV);
		GEO_ul_ChangeUVNumber(pst_Obj, NbUV);
        GEO_ul_ChangeTriangleNumber(pst_Obj, 0, NbT);
	}


    /*$1- vertex */
    V = pst_Obj->dst_Point;
    l = ( ( 2 * _pst_Data->i_NbCapSegs ) + _pst_Data->i_NbZSegs - 1 );
    dangle = Cf_2Pi / dl;

    GEOCO_SetVertex(0, 0, 0);
    
    for (j = 0; j < l; j++)
    {
        R = _pst_Data->f_Radius;
        if (j < _pst_Data->i_NbCapSegs - 1)
        {
            R *= ((float) (j + 1)) / ((float) _pst_Data->i_NbCapSegs);
            z = 0;
        }
        else if (l - 1 - j < _pst_Data->i_NbCapSegs - 1)
        {
            R *= ((float) (l - j)) / ((float) _pst_Data->i_NbCapSegs);
            z = _pst_Data->z;
        }
        else
        {
            z = (_pst_Data->z * (j - _pst_Data->i_NbCapSegs + 1) ) / (float) _pst_Data->i_NbZSegs;
        }

        angle = 0;
		for(i = 0; i < dl; i++, angle += dangle) 
            GEOCO_SetVertex(R * fCos(angle), R * fSin(angle), z);
    }
	GEOCO_SetVertex(0, 0, _pst_Data->z );

    /*$1- UV */
    UV = pst_Obj->dst_UV;
    
    /* planar for bottom */
    UV->fU = UV->fV = 0.5;
    UV++;
    for (j = 0; j < _pst_Data->i_NbCapSegs; j++)
    {
        R = 0.5f * ((float) (j+1)) / (float) _pst_Data->i_NbCapSegs;
        angle = 0;
        for(i = 0; i < dl; i++, angle += dangle) 
        {
            UV->fU = 0.5f - R * fCos(angle);
            UV->fV = 0.5f + R * fSin(angle);
            UV++;
        }
    }

    /* planar for top */
    for (j = _pst_Data->i_NbCapSegs - 1; j >= 0; j--)
    {
        R = 0.5f * ((float) (j+1)) / (float) _pst_Data->i_NbCapSegs;
        angle = 0;
        for(i = 0; i < dl; i++, angle += dangle) 
        {
            UV->fU = 0.5f - R * fSin(angle);
            UV->fV = 0.5f + R * fCos(angle);
            UV++;
        }
    }
    UV->fU = UV->fV = 0.5;
    UV++;

    /* cylindrique for border */
    for ( j = 0; j <= _pst_Data->i_NbZSegs; j++)
    {
        R = (float) j / (float) _pst_Data->i_NbZSegs;
        for (i = 0; i <= dl; i++)
        {
            UV->fU = (float) i / (float) dl;
            UV->fV = R;
            UV++;
        }
    }

    i = UV - pst_Obj->dst_UV;

    /*$1 Triangles */
    T = pst_Obj->dst_Element[0].dst_Triangle;

	/* bottom  */
    for ( i = 1; i < dl; i++ )
        GEOCO_SetTriangleIndex( 0, i+1, i );
    GEOCO_SetTriangleIndex( 0, 1, i );

    for ( k = 1, j = 1; j < _pst_Data->i_NbCapSegs; j++, k += dl)
    {
	    for(i = 0; i < dl - 1; i++)
	    {
            GEOCO_SetTriangleIndex( k + i, k + i + dl + 1, k + i + dl );
            GEOCO_SetTriangleIndex( k + i, k + i + 1, k + i + dl + 1);
	    }
        GEOCO_SetTriangleIndex( k + i, k + i + 1, k + i + dl );
        GEOCO_SetTriangleIndex( k + i, k , k + i + 1);
    }

    /* top */
    for ( k = NbV - 1 - dl * _pst_Data->i_NbCapSegs, j = 1; j < _pst_Data->i_NbCapSegs; j++, k += dl)
    {
	    for(i = 0; i < dl - 1; i++)
	    {
            GEOCO_SetTriangleIndex( k + i, k + i + dl + 1, k + i + dl );
            GEOCO_SetTriangleIndex( k + i, k + i + 1, k + i + dl + 1);
	    }
        GEOCO_SetTriangleIndex( k + i, k + i + 1, k + i + dl );
        GEOCO_SetTriangleIndex( k + i, k , k + i + 1);
    }

    for ( k = NbV - dl - 1, i = 0; i < dl - 1; i ++ )
        GEOCO_SetTriangleIndex( NbV - 1, k+i, k+i+1 );
    GEOCO_SetTriangleIndex( NbV - 1, k+i, k+i+1-dl );

    /* border */
    for ( k = 1 + dl * (_pst_Data->i_NbCapSegs - 1) , j = 0; j < _pst_Data->i_NbZSegs; j++, k+=dl )
    {
        for (i = 0; i < dl; i++)
        {
            GEOCO_SetTriangleIndex( k + i, k + i + dl + 1, k + i + dl );
            GEOCO_SetTriangleIndex( k + i, k + i + 1, k + i + dl + 1);
        }
        T -= 2;
        i--;
        GEOCO_SetTriangleIndex( k + i, k + i + 1, k + i + dl );
        GEOCO_SetTriangleIndex( k + i, k + i + 1 - dl, k + i + 1);
    }
    

	GEO_ComputeNormals(pst_Obj);
	return GRO_ul_Struct_FullSave((GRO_tdst_Struct *) pst_Obj, sz_Path, sz_Name, &TEX_gst_GlobalList);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GEO_ul_CO_Sphere(GEO_tdst_Object **_ppst_Obj, char *sz_Path, char *sz_Name, GEO_tdst_CO_Sphere *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							l, dl, i, j, k, uv, NbV, NbT, NbUV, NbSlice;
	float						R, z, d, angle, dangle;
	GEO_Vertex					*V;
   	GEO_tdst_UV					*UV;
	GEO_tdst_Object				*pst_Obj;
	GEO_tdst_IndexedTriangle	*T;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	NbSlice = _pst_Data->i_NbSeg >> 1;
	NbV = 2 + (_pst_Data->i_NbSeg) * (NbSlice - 1);
	NbT = 2 * (_pst_Data->i_NbSeg) * (NbSlice - 1);
    NbUV = (_pst_Data->i_NbSeg + 1) * (NbSlice + 1);

	if(*_ppst_Obj == NULL)
	{
		pst_Obj = GEO_pst_Create(NbV, NbUV, 1, 0);
		L_memset(&pst_Obj->dst_Element[0], 0, sizeof(GEO_tdst_ElementIndexedTriangles));
		pst_Obj->dst_Element[0].l_NbTriangles = NbT;
		GEO_AllocElementContent(pst_Obj->dst_Element);
		*_ppst_Obj = pst_Obj;
		GRO_Struct_SetName(&pst_Obj->st_Id, sz_Name);
	}
	else
	{
		pst_Obj = *_ppst_Obj;
		GEO_ul_ChangeVertexNumber(pst_Obj, NbV);
		GEO_ul_ChangeUVNumber(pst_Obj, NbUV);
		GEO_ul_ChangeTriangleNumber(pst_Obj, 0, NbT);
	}

    /*$1- vertex */
	dangle = Cf_2Pi / _pst_Data->i_NbSeg;
	R = _pst_Data->f_Radius;
	V = pst_Obj->dst_Point;

	GEOCO_SetVertex(0, 0, R);

	for(k = 0; k < NbSlice - 1; k++)
	{
		angle = Cf_PiBy2 - ((k + 1) * Cf_Pi / NbSlice);
		z = R * fSin(angle);
		d = R * fCos(angle);

		angle = 0;
		for(i = 0; i < _pst_Data->i_NbSeg; i++, angle -= dangle) GEOCO_SetVertex(d * fSin(angle), d * fCos(angle), z);
	}

	GEOCO_SetVertex(0, 0, -R);

    /*$1- UV */
    UV = pst_Obj->dst_UV;
    for (j = 0; j <= NbSlice; j++)
    {
        R = 1.0f - ((float) j / (float) NbSlice);
        for (i = 0; i <= _pst_Data->i_NbSeg; i++)
        {
            UV->fU = ((float) i)/ ((float) _pst_Data->i_NbSeg);
            UV->fV = R;
            UV++;
        }
    }

    /*$1 Triangles */

	T = pst_Obj->dst_Element[0].dst_Triangle;

	/* top triangles */
	for(i = 0; i < _pst_Data->i_NbSeg; i++)
	{
        T->auw_UV[0] = i;
		T->auw_UV[1] = i + _pst_Data->i_NbSeg + 1;
		T->auw_UV[2] = T->auw_UV[1] + 1;
        GEOCO_SetTriangleIndex( 0, i + 1, 1 + ((1 + i) % (_pst_Data->i_NbSeg)) );
	}

	/* slice triangles */
	l = 1;
	dl = _pst_Data->i_NbSeg;
    uv = dl + 1;

	for(k = 0; k < NbSlice - 2; k++)
	{
		for(i = 0; i < dl; i++)
		{
            j = ((1 + i) % _pst_Data->i_NbSeg);
			T->auw_UV[0] = uv;
			T->auw_UV[1] = uv + dl + 1;
			T->auw_UV[2] = uv + dl + 2;
            GEOCO_SetTriangleIndex( i + l, i + l + dl, j + l + dl );

			T->auw_UV[0] = uv;
			T->auw_UV[1] = uv + dl + 2;
			T->auw_UV[2] = uv + 1;
			GEOCO_SetTriangleIndex( i + l, j + l + dl, j + l );
            uv++;
		}
		l += dl;
        uv++;
	}

	/* bottom triangles */
	for(i = 0; i < _pst_Data->i_NbSeg; i++)
	{
		T->auw_UV[0] = uv + dl + 1;
		T->auw_UV[1] = uv + 1;
		T->auw_UV[2] = uv++;
        GEOCO_SetTriangleIndex( NbV - 1, ((i + 1) % _pst_Data->i_NbSeg) + l, i + l );
	}

	GEO_ComputeNormals(pst_Obj);
	return GRO_ul_Struct_FullSave((GRO_tdst_Struct *) pst_Obj, sz_Path, sz_Name, &TEX_gst_GlobalList);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int GEO_ul_CO_Geosphere_SphericalInterpolate( MATH_tdst_Vector *V1, MATH_tdst_Vector *V2, MATH_tdst_Vector *V, int Nb)
{
	int i;
	float theta, theta1, theta2, sn, cs, rad;
    MATH_tdst_Vector VTemp;

	if (Nb < 2) 
        return 0;

	rad = MATH_f_DotProduct( V1, V1 );
	if ( rad==0 ) 
    {
        for (i = 1; i < Nb; i++)
            GEOCO_CopyVertex( V1 );
		return Nb;
	}
	cs = MATH_f_DotProduct( V1, V2) / rad;
    theta = (float) (cs <= -1 ) ? Cf_Pi : (cs >= 1) ? 0 : fAcos( cs );
    sn = fInv( fSin( theta ) );
    
	for (i=1; i < Nb; i++) 
    {
		theta1 = (theta*i) / Nb;
		theta2 = (theta * (Nb-i)) / Nb;
        MATH_ScaleVector( &VTemp, V1, fSin( theta2 ) * sn);
        MATH_AddScaleVector( &VTemp, &VTemp, V2, fSin( theta1) * sn);
        GEOCO_CopyVertex( &VTemp );
	}
    return Nb - 1;
}
/**/
int GEO_ul_CO_Geosphere_FindVertex(int s, int f, int r, int c)
{						// segs, face, row, column.

    if (r==0) return (f<4) ? 0 : 5;
    

	if (r == s) 
    {
		if (((f<4)&&(c==0)) || ((f>3)&&(c==r))) return f%4+1;
		if (((f>3)&&(c==0)) || ((f<4)&&(c==r))) return (f+1)%4+1;
		if (f<4) return 6+(s-1)*(8+f)+c-1;
		return 6+(s-1)*(4+f)+s-1-c;
	}

    // r is between 0 and s.
	if (c==0) 
    {  
		if (f<4) return 6+(s-1)*f + r-1;
		return 6+(s-1)*((f+1)%4+4) + r-1;
	}
	
	if (c==r) 
    {
		if (f<4) return 6+(s-1)*((f+1)%4) + r-1;
		return 6+(s-1)*f + r-1;
	}

	return 6 + (s-1)*12 + f*(s-1)*(s-2)/2 + (r-1)*(r-2)/2 + c-1;
}

/**/
ULONG GEO_ul_CO_Geosphere(GEO_tdst_Object **_ppst_Obj, char *sz_Path, char *sz_Name, GEO_tdst_CO_Geosphere *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							p0, p1, p2, i, j, k, NbV, NbT, NbUV, NbSlice;
	float						R, f;
	GEO_Vertex					*V0, *V1, *V;
   	GEO_tdst_UV					*UV;
	GEO_tdst_Object				*pst_Obj;
	GEO_tdst_IndexedTriangle	*T;
    char                        ac_EdgeIndex[ 12 ] = { 1, 2, 3, 4, 0x51, 0x52, 0x53, 0x54, 0x12, 0x23, 0x34, 0x41 };
    short                       aw_FaceIndex[ 8 ] = { 0x012, 0x023, 0x034, 0x041, 0x521, 0x532, 0x0534,0x0514 };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	NbSlice = _pst_Data->i_NbSeg - 1;
	NbT = (NbSlice + 1) * (NbSlice + 1) * 8;
    NbV = 2 + (NbT / 2);
    //NbUV = NbV + 1 + 2 * NbSlice;
    NbUV = NbV;

	if(*_ppst_Obj == NULL)
	{
		pst_Obj = GEO_pst_Create(NbV, NbUV, 1, 0);
		L_memset(&pst_Obj->dst_Element[0], 0, sizeof(GEO_tdst_ElementIndexedTriangles));
		pst_Obj->dst_Element[0].l_NbTriangles = NbT;
		GEO_AllocElementContent(pst_Obj->dst_Element);
		*_ppst_Obj = pst_Obj;
		GRO_Struct_SetName(&pst_Obj->st_Id, sz_Name);
	}
	else
	{
		pst_Obj = *_ppst_Obj;
		GEO_ul_ChangeVertexNumber(pst_Obj, NbV);
		GEO_ul_ChangeUVNumber(pst_Obj, NbUV);
		GEO_ul_ChangeTriangleNumber(pst_Obj, 0, NbT);
	}

    V = pst_Obj->dst_Point;
    R = _pst_Data->f_Radius;

    /*$1- vertex sommets */
    GEOCO_SetVertex(0, 0, R);
    GEOCO_SetVertex(R, 0, 0);
    GEOCO_SetVertex(0, R, 0);
    GEOCO_SetVertex(-R, 0, 0);
    GEOCO_SetVertex(0, -R, 0);
    GEOCO_SetVertex(0, 0, -R);

    /*$1- vertex sur les 12 edges principaux */
    f = Cf_PiBy2 / (float) (_pst_Data->i_NbSeg);
    for (i = 0; i < 12; i++)
    {
        V0 = &pst_Obj->dst_Point[ ac_EdgeIndex[ i ] >> 4 ];
        V1 = &pst_Obj->dst_Point[ ac_EdgeIndex[ i ] & 0xF ];
        V += GEO_ul_CO_Geosphere_SphericalInterpolate( V0, V1, V, _pst_Data->i_NbSeg );
    }

    /*$1- vertex sur les 8 faces principales */
    for ( j = 0; j < 4; j++) 
    {
	    for (i=1; i < NbSlice; i++) 
        {
            V0 = &pst_Obj->dst_Point[ 6 + j * NbSlice + i ];
            V1 = &pst_Obj->dst_Point[ 6 + ((j+1)%4) * NbSlice + i ];
            V += GEO_ul_CO_Geosphere_SphericalInterpolate( V0, V1, V, i + 1 );
        }
    }
    for ( j = 0; j < 4; j++) 
    {
	    for (i=1; i < NbSlice; i++) 
        {
            V0 = &pst_Obj->dst_Point[ 6 + ((j+1)%4+4) * NbSlice + i ];
            V1 = &pst_Obj->dst_Point[ 6 + (j+4) * NbSlice + i ];
            V += GEO_ul_CO_Geosphere_SphericalInterpolate( V0, V1, V, i + 1 );
        }
    }

    /*$1- UV */
    V = pst_Obj->dst_Point;
    UV = pst_Obj->dst_UV;
    for (i = 0; i < NbV; i++, UV++, V++)
    {
        f = V->z / R;
        f = (f > 1) ? Cf_PiBy2 : ( f < -1) ? -Cf_PiBy2 : fAsin(f);
        UV->fV = (f / Cf_Pi) + 0.5f;
        f = V->x / (R * fCos( f ) );
        f = ( f > 1 ) ? 0 : ( f < -1 ) ? Cf_Pi : fAcos( f );
        if (V->y < 0) f = Cf_2Pi - f;
        UV->fU = f / Cf_2Pi;
    }
    
    /*$1 Triangles */

	T = pst_Obj->dst_Element[0].dst_Triangle;
    for ( i = 0; i < 8; i++) 
    {
	    for (j = 0; j < _pst_Data->i_NbSeg; j++) 
        {
		    for (k = 0; k <= j; k++) 
            {
				p0 = GEO_ul_CO_Geosphere_FindVertex(_pst_Data->i_NbSeg, i, j, k);
				p1 = GEO_ul_CO_Geosphere_FindVertex(_pst_Data->i_NbSeg, i, j + 1, k);
				p2 = GEO_ul_CO_Geosphere_FindVertex(_pst_Data->i_NbSeg, i, j + 1, k + 1);
                GEOCO_SetTriangleIndexAndUV( p0, p1, p2 );
                
				if ( k < j ) 
                {
					p1 = GEO_ul_CO_Geosphere_FindVertex (_pst_Data->i_NbSeg, i, j, k + 1);
                    GEOCO_SetTriangleIndexAndUV( p0, p2, p1 );
				}
			}
		}
	}

	GEO_ComputeNormals(pst_Obj);
	return GRO_ul_Struct_FullSave((GRO_tdst_Struct *) pst_Obj, sz_Path, sz_Name, &TEX_gst_GlobalList);
}


/* code pour charger un .asc et tester avec les données générées */

/*$F
{
    char sz_Line[1024], *pc_Cur;
    L_FILE  x_File;
    GEO_Vertex                  TestV[ 2202 ];
    int                         TestT[ 4400 ][3];


    x_File = L_fopen( "d:/utils/3DSMax/meshes/box3.asc", "rt" );
    while( !feof( x_File ) )
    {
        fgets( sz_Line, 1024, x_File );
        if ( (*sz_Line == 'V') && (sz_Line[ 7 ] != 'l') )
        {
            while ( pc_Cur = strchr( sz_Line, ',') )
                *pc_Cur = '.';
            sscanf( sz_Line, "Vertex %d: X: %f Y: %f Z: %f", &i, &x, &y, &z );
            MATH_InitVector( TestV + i, x, y, z );
        }
        else if ( (*sz_Line == 'F') && (sz_Line[5] != 'l') )
        {
            sscanf( sz_Line, "Face %d:    A:%d B:%d C:%d", &i, &j, &k, &l );
            TestT[i][0] = j;
            TestT[i][1] = k;
            TestT[i][2] = l;
        }
    }
    fclose( x_File );

    for (i = 0; i < 2202; i++)
    {
        if( !MATH_b_EqVectorWithEpsilon( pst_Obj->dst_Point + i, TestV + i, 0.001f ) )
            z = 0;
    }   

    T = pst_Obj->dst_Element[0].dst_Triangle;
    for (i = 0; i < 4400; i++)
    {
        if ( T[i].auw_Index[0] != TestT[i][0] )
            z = 0;
        if ( T[i].auw_Index[1] != TestT[i][1] )
            z = 0;
        if ( T[i].auw_Index[2] != TestT[i][2] )
            z = 0;
    }
}
*/

/*********************************************************************************************************************


										COLMAPS


*********************************************************************************************************************/

void COL_AllocateAndComputeNormals(COL_tdst_IndexedTriangles *pst_CobObj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	
	COL_tdst_ElementIndexedTriangles	*pst_CobElement, *pst_CobLastElement;
	COL_tdst_IndexedTriangle			*pst_CobTriangle, *pst_CobLastTriangle;
	MATH_tdst_Vector					st_VectT1T2, st_VectT1T3, st_Norm;
	ULONG								ul_Triangle, ul_T1, ul_T2, ul_T3;
	MATH_tdst_Vector					*pst_T1, *pst_T2, *pst_T3;
	int									i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	

	if(!pst_CobObj || !pst_CobObj->dst_Element) return;

	if(pst_CobObj->dst_FaceNormal)
		MEM_Free(pst_CobObj->dst_FaceNormal);

	pst_CobObj->dst_FaceNormal = (MATH_tdst_Vector *) MEM_p_Alloc(sizeof(MATH_tdst_Vector) * pst_CobObj->l_NbFaces);

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

				/* We get the triangles points. */
				pst_T1 = VCast(&pst_CobObj->dst_Point[ul_T1]);
				pst_T2 = VCast(&pst_CobObj->dst_Point[ul_T2]);
				pst_T3 = VCast(&pst_CobObj->dst_Point[ul_T3]);

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
/**/
COL_tdst_Cob *COL_pst_AllocateGeometricCob(int NbT, int NbV)
{
	COL_tdst_Cob						*pst_Cob;
	COL_tdst_IndexedTriangles			*pst_CobObj;
	COL_tdst_ElementIndexedTriangles	*pst_CobElement;

	pst_Cob = (COL_tdst_Cob *) MEM_p_Alloc(sizeof(COL_tdst_Cob));
	pst_Cob->uw_NbOfInstances = 1;
	pst_Cob->pst_GMatList = NULL;

	pst_Cob->uc_Type = COL_C_Zone_Triangles;
	pst_Cob->pst_TriangleCob = (COL_tdst_IndexedTriangles *) MEM_p_Alloc(sizeof(COL_tdst_IndexedTriangles));
	L_memset(pst_Cob->pst_TriangleCob, 0, sizeof(COL_tdst_IndexedTriangles));

	pst_CobObj = (COL_tdst_IndexedTriangles *) pst_Cob->pst_TriangleCob;
	pst_CobObj->pst_OK3 = NULL;

	pst_CobObj->l_NbPoints = NbV;			
	pst_CobObj->dst_Point = (MATH_tdst_Vector *) MEM_p_Alloc(sizeof(MATH_tdst_Vector) * pst_CobObj->l_NbPoints);

	pst_CobObj->l_NbElements = 1;
	pst_CobObj->dst_Element = (COL_tdst_ElementIndexedTriangles *) MEM_p_Alloc(sizeof(COL_tdst_ElementIndexedTriangles) * pst_CobObj->l_NbElements);
	L_memset(&pst_CobObj->dst_Element[0], 0, sizeof(COL_tdst_ElementIndexedTriangles));

	pst_CobObj->l_NbFaces = NbT;
	pst_CobObj->dst_Element[0].uw_NbTriangles = (USHORT) NbT;

	pst_CobElement = pst_CobObj->dst_Element;

	pst_CobElement->uc_Flag = 0;
	pst_CobElement->l_MaterialId = 0;
	pst_CobElement->uw_NbTriangles = (USHORT) NbT;

	pst_CobElement->dst_Triangle = (COL_tdst_IndexedTriangle *) MEM_p_Alloc(pst_CobElement->uw_NbTriangles * sizeof(COL_tdst_IndexedTriangle));
	pst_CobElement->pst_Cob = pst_Cob;

	pst_Cob->ul_EditedElement = 0xFFFFFFFF;

	/*$F--------- Dummy Name -------------- */
	pst_Cob->sz_GMatName = (char *) MEM_p_Alloc(5);
	pst_Cob->pst_Itself = pst_Cob;
	pst_Cob->p_GeoCob = NULL;
	pst_Cob->uc_Flag = 0;
	strcpy(pst_Cob->sz_GMatName, "None");

	return pst_Cob;
}
/**/
ULONG COL_ul_SaveCob(COL_tdst_Cob *_pst_Cob, char *_psz_Path, char *_psz_Name)
{
	ULONG			ul_Dummy;
	BIG_INDEX		ul_CobIndex;

	SAV_Begin(_psz_Path, _psz_Name);
	ul_Dummy = 0;
	SAV_Buffer(&ul_Dummy, sizeof(ULONG));
	ul_CobIndex = SAV_ul_End();

	LOA_AddAddress(ul_CobIndex, _pst_Cob);
	COL_SaveCob(_pst_Cob, BIG_C_InvalidIndex);

	return ul_CobIndex;
}
/**/
ULONG COL_ul_CO_Plane(COL_tdst_Cob **_ppst_Cob, char *sz_Path, char *sz_Name, GEO_tdst_CO_Plane *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, j, k, dk, NbV, NbT;
	float						x, y, dx, dy, u, v, du, dv;
	GEO_Vertex					*V;
	COL_tdst_Cob				*pst_Cob;
	COL_tdst_IndexedTriangles	*pst_CobObj;
	COL_tdst_IndexedTriangle	*T;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	NbV = (_pst_Data->i_NbX + 1) * (_pst_Data->i_NbY + 1);
	NbT = (_pst_Data->i_NbX) * (_pst_Data->i_NbY) * 2;

	COL_FreeCob(*_ppst_Cob);
	pst_Cob = COL_pst_AllocateGeometricCob(NbT, NbV);
	*_ppst_Cob = pst_Cob;

	LINK_RegisterPointer(pst_Cob, LINK_C_Cob, sz_Name, sz_Path);
	pst_CobObj = (COL_tdst_IndexedTriangles *) pst_Cob->pst_TriangleCob;

	V = pst_CobObj->dst_Point;

	du = 1.0f / _pst_Data->i_NbX;
	dv = 1.0f / _pst_Data->i_NbY;
	dx = _pst_Data->x / _pst_Data->i_NbX;
	dy = _pst_Data->y / _pst_Data->i_NbY;
	y = _pst_Data->y / 2;
	v = 0;

	for(j = 0; j <= _pst_Data->i_NbY; j++, y -= dy, v += dv)
	{
		x = -_pst_Data->x / 2;
		u = 0;
		for(i = 0; i <= _pst_Data->i_NbX; i++, x += dx, u += du)
		{
			GEOCO_SetVertex(x, y, 0);
		}
	}

	T = pst_CobObj->dst_Element[0].dst_Triangle;
	k = 0;
	dk = _pst_Data->i_NbX + 1;

	for(j = 0; j < _pst_Data->i_NbY; j++, k += dk)
	{
		for(i = 0; i < _pst_Data->i_NbX; i++)
		{
			T->auw_Index[0] = k + i;
			T->auw_Index[1] = k + dk + i;
			T->auw_Index[2] = k + i + 1;
			T++;

			T->auw_Index[0] = k + i + 1;
			T->auw_Index[1] = k + i + dk;
			T->auw_Index[2] = k + i + 1 + dk;
			T++;
		}
	}

	COL_AllocateAndComputeNormals(pst_CobObj);
	return (COL_ul_SaveCob(pst_Cob, sz_Path, sz_Name)); 
}
/**/
ULONG COL_ul_CO_Box(COL_tdst_Cob **_ppst_Cob, char *sz_Path, char *sz_Name, GEO_tdst_CO_Box *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							i, j, k, nk, di, dj, dk, NbV, NbUV, p, i_Elem;
	float						x, y, z, dx, dy, dz, du, dv, dw;
	GEO_Vertex					*V;
	COL_tdst_Cob				*pst_Cob;
	COL_tdst_IndexedTriangles	*pst_CobObj;
	COL_tdst_IndexedTriangle	*T;
    int                         NbElem, ai_NbT[ 6 ], ai_NbTUsed[ 6 ];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	di = _pst_Data->i_NbX + 1;
	dj = _pst_Data->i_NbY + 1;
	dk = _pst_Data->i_NbZ + 1;

	NbV = di * dj * dk;
	NbV -= (_pst_Data->i_NbX - 1) * (_pst_Data->i_NbY - 1) * (_pst_Data->i_NbZ - 1);

	NbUV = di * dj + di * dk + dj * dk;

    NbElem = (_pst_Data->i_NbElem < 1) ? 1 : ((_pst_Data->i_NbElem > 6) ? 6 : _pst_Data->i_NbElem); 
    L_memset( ai_NbT, 0, sizeof( ai_NbT ) );
    L_memset( ai_NbTUsed, 0, sizeof( ai_NbTUsed ) );
    ai_NbT[ 0 ] += 2 * _pst_Data->i_NbX * _pst_Data->i_NbY;
    ai_NbT[ 1 % NbElem ] += 2 * _pst_Data->i_NbX * _pst_Data->i_NbY;
    ai_NbT[ 2 % NbElem ] += 2 * _pst_Data->i_NbY * _pst_Data->i_NbZ;
    ai_NbT[ 3 % NbElem ] += 2 * _pst_Data->i_NbY * _pst_Data->i_NbZ;
    ai_NbT[ 4 % NbElem ] += 2 * _pst_Data->i_NbX * _pst_Data->i_NbZ;
    ai_NbT[ 5 % NbElem ] += 2 * _pst_Data->i_NbX * _pst_Data->i_NbZ;

	COL_FreeCob(*_ppst_Cob);
	pst_Cob = COL_pst_AllocateGeometricCob(ai_NbT[1 % NbElem], NbV);
	*_ppst_Cob = pst_Cob;

	LINK_RegisterPointer(pst_Cob, LINK_C_Cob, sz_Name, sz_Path);
	pst_CobObj = (COL_tdst_IndexedTriangles *) pst_Cob->pst_TriangleCob;

	du = 1.0f / _pst_Data->i_NbX;
	dv = 1.0f / _pst_Data->i_NbY;
	dw = 1.0f / _pst_Data->i_NbZ;
	dx = _pst_Data->x / _pst_Data->i_NbX;
	dy = _pst_Data->y / _pst_Data->i_NbY;
	dz = _pst_Data->z / _pst_Data->i_NbZ;
	p = 2 * (_pst_Data->i_NbX + _pst_Data->i_NbY);

	/*$1- Vertex ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	V = pst_CobObj->dst_Point;

	/* down face and up face vertex */
	z = 0;
	for(k = 0; k < 2; k++)
	{
		y = -_pst_Data->y / 2;
		for(j = 0; j <= _pst_Data->i_NbY; j++, y += dy)
		{
			x = -_pst_Data->x / 2;
			for(i = 0; i <= _pst_Data->i_NbX; i++, x += dx) GEOCO_SetVertex(x, y, z);
		}

		z = _pst_Data->z;
	}

	/* other vertices */
	z = dz;
	for(k = 1; k < _pst_Data->i_NbZ; k++, z += dz)
	{
		y = -_pst_Data->y / 2;
		x = -_pst_Data->x / 2;
		for(i = 0; i < _pst_Data->i_NbX; i++, x += dx) GEOCO_SetVertex(x, y, z);
		for(j = 0; j < _pst_Data->i_NbY; j++, y += dy) GEOCO_SetVertex(x, y, z);
		for(i = 0; i < _pst_Data->i_NbX; i++, x -= dx) GEOCO_SetVertex(x, y, z);
		for(j = 0; j < _pst_Data->i_NbY; j++, y -= dy) GEOCO_SetVertex(x, y, z);
	}

	/*$1- Triangles ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* down face */
    i_Elem = 1 % NbElem;
	T = pst_CobObj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
	for(j = 0, k = 0; j < _pst_Data->i_NbY; j++, k += di)
	{
		for(i = 0; i < _pst_Data->i_NbX; i++)
        {
			COLCO_SetTriangleIndex(k + i, k + i + di, k + i + di + 1);
			COLCO_SetTriangleIndex(k + i + di + 1, k + i + 1, k + i);
		}
	}
    ai_NbTUsed[ i_Elem ] = T - pst_CobObj->dst_Element[ i_Elem ].dst_Triangle;

	/* up face */
    i_Elem = 0;
	T = pst_CobObj->dst_Element[0].dst_Triangle + ai_NbTUsed[ 0 ];
	for(j = 0, k = di * dj; j < _pst_Data->i_NbY; j++, k += di)
	{
        for(i = 0; i < _pst_Data->i_NbX; i++)
		{
			COLCO_SetTriangleIndex(k + i, k + i + 1, k + i + di + 1);
			COLCO_SetTriangleIndex(k + i + di + 1, k + i + di, k + i);
		}
	}
    ai_NbTUsed[ 0 ] = T - pst_CobObj->dst_Element[ 0 ].dst_Triangle;

	/* front face */
    i_Elem = 4 % NbElem;
	T = pst_CobObj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
	for(j = 0, k = 0, nk = 2 * di * dj; j < _pst_Data->i_NbZ; j++, k = nk, nk += p)
	{
		if(nk >= NbV) nk = di * dj;

		for(i = 0; i < _pst_Data->i_NbX; i++)
        {
			COLCO_SetTriangleIndex(k + i, k + i + 1, nk + i + 1);
			COLCO_SetTriangleIndex(nk + i + 1, nk + i, k + i);
		}
	}
    ai_NbTUsed[ i_Elem ] = T - pst_CobObj->dst_Element[ i_Elem ].dst_Triangle;

	/* right face */
    i_Elem = 3 % NbElem;
    T = pst_CobObj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
    //T = pst_CobObj->dst_Element[3].dst_Triangle;
    if (_pst_Data->i_NbZ == 1)
    {
        for(k = di - 1, nk = di * dj + (di - 1), i = 0, j = 0; i < _pst_Data->i_NbY; i++, j += di ) 
	    {
		    COLCO_SetTriangleIndex(k + j, k + j + di, nk + j + di);
		    COLCO_SetTriangleIndex(nk + j + di, nk + j, k + j);
	    }
    }
    else
    {
	    for(k = di - 1, nk = 2 * di * dj + (di - 1), i = 0, j = 0; i < _pst_Data->i_NbY; i++, j += di)
	    {
		    COLCO_SetTriangleIndex(k + j, k + j + di, nk + i + 1);
		    COLCO_SetTriangleIndex(nk + i + 1, nk + i, k + j);
	    }

	    for(j = 1, k = nk, nk += p; j < _pst_Data->i_NbZ - 1; j++, k = nk, nk += p)
	    {
		    for(i = 0; i < _pst_Data->i_NbY; i++)
		    {
			    COLCO_SetTriangleIndex(k + i, k + i + 1, nk + i + 1);
			    COLCO_SetTriangleIndex(nk + i + 1, nk + i, k + i);
		    }
	    }

	    for(nk = di * dj + (di - 1), i = 0, j = 0; i < _pst_Data->i_NbY; i++, j += di)
	    {
		    COLCO_SetTriangleIndex(k + i, k + i + 1, nk + j + di);
		    COLCO_SetTriangleIndex(nk + j + di, nk + j, k + i);
	    }
    }
    ai_NbTUsed[ i_Elem ] = T - pst_CobObj->dst_Element[ i_Elem ].dst_Triangle;

	/* back face */
    i_Elem = 5 % NbElem;
	T = pst_CobObj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
    //T = pst_CobObj->dst_Element[5].dst_Triangle;
    if (_pst_Data->i_NbZ == 1)
    {
        for(k = di * dj - 1, nk = 2 * di * dj - 1, i = 0, j = 0; i < _pst_Data->i_NbX; i++, j-- ) 
	    {
		    COLCO_SetTriangleIndex(k + j, k + j - 1, nk + j - 1);
		    COLCO_SetTriangleIndex(nk + j - 1, nk + j, k + j);
	    }
    }
    else
    {
	    for(k = di * dj - di, nk = 2 * di * dj + (di + dj - 2), i = 0, j = di - 1; i < _pst_Data->i_NbX; i++, j--)
	    {
		    COLCO_SetTriangleIndex(k + j, k + j - 1, nk + i + 1);
		    COLCO_SetTriangleIndex(nk + i + 1, nk + i, k + j);
	    }

	    for(j = 1, k = nk, nk += p; j < _pst_Data->i_NbZ - 1; j++, k = nk, nk += p)
	    {
		    for(i = 0; i < _pst_Data->i_NbX; i++)
		    {
			    COLCO_SetTriangleIndex(k + i, k + i + 1, nk + i + 1);
			    COLCO_SetTriangleIndex(nk + i + 1, nk + i, k + i);
		    }
	    }

	    for(nk = 2 * di * dj - di, i = 0, j = di - 1; i < _pst_Data->i_NbX; i++, j--)
	    {
		    COLCO_SetTriangleIndex(k + i, k + i + 1, nk + j - 1);
		    COLCO_SetTriangleIndex(nk + j - 1, nk + j, k + i);
	    }
    }
    ai_NbTUsed[ i_Elem ] = T - pst_CobObj->dst_Element[ i_Elem ].dst_Triangle;

	/* left face */
    i_Elem = 2 % NbElem;
	T = pst_CobObj->dst_Element[ i_Elem ].dst_Triangle + ai_NbTUsed[ i_Elem ];
    //T = pst_CobObj->dst_Element[2].dst_Triangle;
    if (_pst_Data->i_NbZ == 1)
    {
        for(k = di * dj - di, nk = 2 * di * dj - di, i = 0, j = 0; i < _pst_Data->i_NbY - 1; i++, j -= di)
	    {
		    COLCO_SetTriangleIndex(k + j, k + j - di, nk + j - di);
		    COLCO_SetTriangleIndex(nk + j - di, nk + j, k + j);
	    }
        COLCO_SetTriangleIndex(k + j, k + j - di, nk + j - di);
	    COLCO_SetTriangleIndex(nk + j - di, nk + j, k + j);
    }
    else
    {
	    for(k = di * dj - di, nk = 2 * di * dj + p - (dj - 1), i = 0, j = 0; i < _pst_Data->i_NbY - 1; i++, j -= di)
	    {
		    COLCO_SetTriangleIndex(k + j, k + j - di, nk + i + 1);
		    COLCO_SetTriangleIndex(nk + i + 1, nk + i, k + j);
	    }

	    COLCO_SetTriangleIndex(k + j, k + j - di, 2 * di * dj);
	    COLCO_SetTriangleIndex(2 * di * dj, nk + i, k + j);

	    for(j = 1, k = nk, nk += p; j < _pst_Data->i_NbZ - 1; j++, k = nk, nk += p)
	    {
		    for(i = 0; i < _pst_Data->i_NbY - 1; i++)
		    {
			    COLCO_SetTriangleIndex(k + i, k + i + 1, nk + i + 1);
			    COLCO_SetTriangleIndex(nk + i + 1, nk + i, k + i);
		    }

		    COLCO_SetTriangleIndex(k + i, k + i + 1 - p, nk + i + 1 - p);
		    COLCO_SetTriangleIndex(nk + i + 1 - p, nk + i, k + i);
	    }

	    for(nk = 2 * di * dj - di, i = 0, j = 0; i < _pst_Data->i_NbY - 1; i++, j -= di)
	    {
		    COLCO_SetTriangleIndex(k + i, k + i + 1, nk + j - di);
		    COLCO_SetTriangleIndex(nk + j - di, nk + j, k + i);
	    }

	    COLCO_SetTriangleIndex(k + i, k + i + 1 - p, nk + j - di);
	    COLCO_SetTriangleIndex(nk + j - di, nk + j, k + i);
    }
    ai_NbTUsed[ i_Elem ] = T - pst_CobObj->dst_Element[ i_Elem ].dst_Triangle;


	COL_AllocateAndComputeNormals(pst_CobObj);
	return (COL_ul_SaveCob(pst_Cob, sz_Path, sz_Name)); 
}
/**/
ULONG COL_ul_CO_Sphere(COL_tdst_Cob **_ppst_Cob, char *sz_Path, char *sz_Name, GEO_tdst_CO_Sphere *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							l, dl, i, j, k, uv, NbV, NbT, NbSlice;
	float						R, z, d, angle, dangle;
	GEO_Vertex					*V;
	COL_tdst_Cob				*pst_Cob;
	COL_tdst_IndexedTriangles	*pst_CobObj;
	COL_tdst_IndexedTriangle	*T;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	NbSlice = _pst_Data->i_NbSeg >> 1;
	NbV = 2 + (_pst_Data->i_NbSeg) * (NbSlice - 1);
	NbT = 2 * (_pst_Data->i_NbSeg) * (NbSlice - 1);

	COL_FreeCob(*_ppst_Cob);
	pst_Cob = COL_pst_AllocateGeometricCob(NbT, NbV);
	*_ppst_Cob = pst_Cob;

	LINK_RegisterPointer(pst_Cob, LINK_C_Cob, sz_Name, sz_Path);
	pst_CobObj = (COL_tdst_IndexedTriangles *) pst_Cob->pst_TriangleCob;

    /*$1- vertex */
	dangle = Cf_2Pi / _pst_Data->i_NbSeg;
	R = _pst_Data->f_Radius;
	V = pst_CobObj->dst_Point;

	GEOCO_SetVertex(0, 0, R);

	for(k = 0; k < NbSlice - 1; k++)
	{
		angle = Cf_PiBy2 - ((k + 1) * Cf_Pi / NbSlice);
		z = R * fSin(angle);
		d = R * fCos(angle);      

		angle = 0;
		for(i = 0; i < _pst_Data->i_NbSeg; i++, angle -= dangle) GEOCO_SetVertex(d * fSin(angle), d * fCos(angle), z);
	}

	GEOCO_SetVertex(0, 0, -R);

    /*$1 Triangles */

	T = pst_CobObj->dst_Element[0].dst_Triangle;

	/* top triangles */
	for(i = 0; i < _pst_Data->i_NbSeg; i++)
	{
			T->auw_Index[0] = 0;
			T->auw_Index[1] = i + 1;
			T->auw_Index[2] = 1 + ((1 + i) % (_pst_Data->i_NbSeg)) ;
			T++;
	}

	/* slice triangles */
	l = 1;
	dl = _pst_Data->i_NbSeg;
    uv = dl + 1;

	for(k = 0; k < NbSlice - 2; k++)
	{
		for(i = 0; i < dl; i++)
		{
			j = ((1 + i) % _pst_Data->i_NbSeg);
			T->auw_Index[0] = i + l;
			T->auw_Index[1] = i + l + dl;
			T->auw_Index[2] = j + l + dl;
			T++;

			T->auw_Index[0] = i + l;
			T->auw_Index[1] = j + l + dl;
			T->auw_Index[2] = j + l;
			T++;
		}
		l += dl;
	}

	/* bottom triangles */
	for(i = 0; i < _pst_Data->i_NbSeg; i++)
	{
		T->auw_Index[0] = NbV - 1;
		T->auw_Index[1] = ((i + 1) % _pst_Data->i_NbSeg) + l;
		T->auw_Index[2] = i + l;
		T++;
	}

	COL_AllocateAndComputeNormals(pst_CobObj);
	return (COL_ul_SaveCob(pst_Cob, sz_Path, sz_Name)); 
}

/**/
ULONG COL_ul_CO_Cylindre(COL_tdst_Cob **_ppst_Cob, char *sz_Path, char *sz_Name, GEO_tdst_CO_Cylindre *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							l, dl, i, j, k, NbV, NbT;
	float						R, z, angle, dangle;
	GEO_Vertex					*V;
	COL_tdst_Cob				*pst_Cob;
	COL_tdst_IndexedTriangles	*pst_CobObj;
	COL_tdst_IndexedTriangle	*T;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    dl = _pst_Data->i_NbSides;
	NbV = 2 + ( ( 2 * _pst_Data->i_NbCapSegs ) + _pst_Data->i_NbZSegs - 1 ) * dl;
    NbT = dl * ( 2 * (2 * _pst_Data->i_NbCapSegs - 1) + _pst_Data->i_NbZSegs * 2 );
    dl = _pst_Data->i_NbSides;

	COL_FreeCob(*_ppst_Cob);
	pst_Cob = COL_pst_AllocateGeometricCob(NbT, NbV);
	*_ppst_Cob = pst_Cob;

	LINK_RegisterPointer(pst_Cob, LINK_C_Cob, sz_Name, sz_Path);
	pst_CobObj = (COL_tdst_IndexedTriangles *) pst_Cob->pst_TriangleCob;

    /*$1- vertex */
    V = pst_CobObj->dst_Point;
    l = ( ( 2 * _pst_Data->i_NbCapSegs ) + _pst_Data->i_NbZSegs - 1 );
    dangle = Cf_2Pi / dl;

    GEOCO_SetVertex(0, 0, 0);
    
    for (j = 0; j < l; j++)
    {
        R = _pst_Data->f_Radius;
        if (j < _pst_Data->i_NbCapSegs - 1)
        {
            R *= ((float) (j + 1)) / ((float) _pst_Data->i_NbCapSegs);
            z = 0;
        }
        else if (l - 1 - j < _pst_Data->i_NbCapSegs - 1)
        {
            R *= ((float) (l - j)) / ((float) _pst_Data->i_NbCapSegs);
            z = _pst_Data->z;
        }
        else
        {
            z = (_pst_Data->z * (j - _pst_Data->i_NbCapSegs + 1) ) / (float) _pst_Data->i_NbZSegs;
        }

        angle = 0;
		for(i = 0; i < dl; i++, angle += dangle) 
            GEOCO_SetVertex(R * fCos(angle), R * fSin(angle), z);
    }
	GEOCO_SetVertex(0, 0, _pst_Data->z );

    
    /*$1 Triangles */
    T = pst_CobObj->dst_Element[0].dst_Triangle;

	/* bottom  */
    for ( i = 1; i < dl; i++ )
	{
		T->auw_Index[0] = 0;
		T->auw_Index[1] = i+1;
		T->auw_Index[2] = i;
		T++;
	}

	T->auw_Index[0] = 0;
	T->auw_Index[1] = 1;
	T->auw_Index[2] = i;
	T++;


    for ( k = 1, j = 1; j < _pst_Data->i_NbCapSegs; j++, k += dl)
    {
	    for(i = 0; i < dl - 1; i++)
		{
			T->auw_Index[0] = k + i;
			T->auw_Index[1] = k + i + dl + 1;
			T->auw_Index[2] = k + i + dl;
			T++;

			T->auw_Index[0] = k + i;
			T->auw_Index[1] = k + i + 1;
			T->auw_Index[2] = k + i + dl + 1;
			T++;

	    }

	T->auw_Index[0] = k + i;
	T->auw_Index[1] = k + i + 1;
	T->auw_Index[2] = k + i + dl;
	T++;

	T->auw_Index[0] = k + i;
	T->auw_Index[1] = k;
	T->auw_Index[2] = k + i + 1;
	T++;
    }

    /* top */
    for ( k = NbV - 1 - dl * _pst_Data->i_NbCapSegs, j = 1; j < _pst_Data->i_NbCapSegs; j++, k += dl)
    {
	    for(i = 0; i < dl - 1; i++)
	    {
            COLCO_SetTriangleIndex( k + i, k + i + dl + 1, k + i + dl );
            COLCO_SetTriangleIndex( k + i, k + i + 1, k + i + dl + 1);
	    }
        COLCO_SetTriangleIndex( k + i, k + i + 1, k + i + dl );
        COLCO_SetTriangleIndex( k + i, k , k + i + 1);
    }

    for ( k = NbV - dl - 1, i = 0; i < dl - 1; i ++ )
        COLCO_SetTriangleIndex( NbV - 1, k+i, k+i+1 );
    COLCO_SetTriangleIndex( NbV - 1, k+i, k+i+1-dl );

    /* border */
    for ( k = 1 + dl * (_pst_Data->i_NbCapSegs - 1) , j = 0; j < _pst_Data->i_NbZSegs; j++, k+=dl)
    {
        for (i = 0; i < dl; i++)
        {
            COLCO_SetTriangleIndex( k + i, k + i + dl + 1, k + i + dl );
            COLCO_SetTriangleIndex( k + i, k + i + 1, k + i + dl + 1);
        }
        T -= 2;
        i--;
        COLCO_SetTriangleIndex( k + i, k + i + 1, k + i + dl );
        COLCO_SetTriangleIndex( k + i, k + i + 1 - dl, k + i + 1);
    }
    
	COL_AllocateAndComputeNormals(pst_CobObj);
	return (COL_ul_SaveCob(pst_Cob, sz_Path, sz_Name)); 

}
/**/
ULONG COL_ul_CO_Geosphere(COL_tdst_Cob **_ppst_Cob, char *sz_Path, char *sz_Name, GEO_tdst_CO_Geosphere *_pst_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int							p0, p1, p2, i, j, k, NbV, NbT, NbSlice;
	float						R, f;
	GEO_Vertex					*V0, *V1, *V;
	COL_tdst_Cob				*pst_Cob;
	COL_tdst_IndexedTriangles	*pst_CobObj;
	COL_tdst_IndexedTriangle	*T;
    char                        ac_EdgeIndex[ 12 ] = { 1, 2, 3, 4, 0x51, 0x52, 0x53, 0x54, 0x12, 0x23, 0x34, 0x41 };
    short                       aw_FaceIndex[ 8 ] = { 0x012, 0x023, 0x034, 0x041, 0x521, 0x532, 0x0534,0x0514 };
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	NbSlice = _pst_Data->i_NbSeg - 1;
	NbT = (NbSlice + 1) * (NbSlice + 1) * 8;
    NbV = 2 + (NbT / 2);

	COL_FreeCob(*_ppst_Cob);
	pst_Cob = COL_pst_AllocateGeometricCob(NbT, NbV);
	*_ppst_Cob = pst_Cob;

	LINK_RegisterPointer(pst_Cob, LINK_C_Cob, sz_Name, sz_Path);

	pst_CobObj = (COL_tdst_IndexedTriangles *) pst_Cob->pst_TriangleCob;
	V = pst_CobObj->dst_Point;
	R = _pst_Data->f_Radius;

    /*$1- vertex sommets */
    GEOCO_SetVertex(0, 0, R);
    GEOCO_SetVertex(R, 0, 0);
    GEOCO_SetVertex(0, R, 0);
    GEOCO_SetVertex(-R, 0, 0);
    GEOCO_SetVertex(0, -R, 0);
    GEOCO_SetVertex(0, 0, -R);

    /*$1- vertex sur les 12 edges principaux */
    f = Cf_PiBy2 / (float) (_pst_Data->i_NbSeg);
    for (i = 0; i < 12; i++)
    {
        V0 = &pst_CobObj->dst_Point[ ac_EdgeIndex[ i ] >> 4 ];
        V1 = &pst_CobObj->dst_Point[ ac_EdgeIndex[ i ] & 0xF ];
        V += GEO_ul_CO_Geosphere_SphericalInterpolate( V0, V1, V, _pst_Data->i_NbSeg );
    }

    /*$1- vertex sur les 8 faces principales */
    for ( j = 0; j < 4; j++) 
    {
	    for (i=1; i < NbSlice; i++) 
        {
            V0 = &pst_CobObj->dst_Point[ 6 + j * NbSlice + i ];
            V1 = &pst_CobObj->dst_Point[ 6 + ((j+1)%4) * NbSlice + i ];
            V += GEO_ul_CO_Geosphere_SphericalInterpolate( V0, V1, V, i + 1 );
        }
    }
    for ( j = 0; j < 4; j++) 
    {
	    for (i=1; i < NbSlice; i++) 
        {
            V0 = &pst_CobObj->dst_Point[ 6 + ((j+1)%4+4) * NbSlice + i ];
            V1 = &pst_CobObj->dst_Point[ 6 + (j+4) * NbSlice + i ];
            V += GEO_ul_CO_Geosphere_SphericalInterpolate( V0, V1, V, i + 1 );
        }
    }
    
    /*$1 Triangles */

	T = pst_CobObj->dst_Element[0].dst_Triangle;
    for ( i = 0; i < 8; i++) 
    {
	    for (j = 0; j < _pst_Data->i_NbSeg; j++) 
        {
		    for (k = 0; k <= j; k++) 
            {
				p0 = GEO_ul_CO_Geosphere_FindVertex(_pst_Data->i_NbSeg, i, j, k);
				p1 = GEO_ul_CO_Geosphere_FindVertex(_pst_Data->i_NbSeg, i, j + 1, k);
				p2 = GEO_ul_CO_Geosphere_FindVertex(_pst_Data->i_NbSeg, i, j + 1, k + 1);

				T->auw_Index[0] = p0;
				T->auw_Index[1] = p1;
				T->auw_Index[2] = p2;
				T++;

				if ( k < j ) 
                {
					p1 = GEO_ul_CO_Geosphere_FindVertex (_pst_Data->i_NbSeg, i, j, k + 1);

					T->auw_Index[0] = p0;
					T->auw_Index[1] = p2;
					T->auw_Index[2] = p1;
					T++;
				}
			}
		}
	}

	COL_AllocateAndComputeNormals(pst_CobObj);
	return (COL_ul_SaveCob(pst_Cob, sz_Path, sz_Name)); 
}


#endif /* ACTIVE_EDITORS */
