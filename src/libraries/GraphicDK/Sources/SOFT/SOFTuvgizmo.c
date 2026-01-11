/*$T SOFTuvgizmo.c GC! 1.081 01/16/01 10:30:22 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"

#ifdef ACTIVE_EDITORS
#include "BASe/MEMory/MEM.h"
#include "SOFT/SOFTuvgizmo.h"
#include "SELection/SELection.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "GEOmetric/GEODebugObject.h"
#include "GEOmetric/GEOsubobject.h"
#include "LINks/LINKtoed.h"
#include "GDInterface/GDIrequest.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_UVGizmo_Init( SOFT_tdst_UVGizmo *_pst_UVGizmo )
{
    _pst_UVGizmo->pst_GO = NULL;
    _pst_UVGizmo->c_On = 0;
    _pst_UVGizmo->c_Type = 0;
    _pst_UVGizmo->c_XYZ = 0;
    _pst_UVGizmo->x = 1.0f;
    _pst_UVGizmo->y = 1.0f;
    _pst_UVGizmo->z = 1.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_UVGizmo_Set(  SOFT_tdst_UVGizmo *_pst_UVGizmo, OBJ_tdst_GameObject *_pst_GO, int _c_Type )
{
    GEO_tdst_Object *pst_Geo;
    GEO_Vertex      VMin, VMax, *V, *VLast; 

    if (!_pst_GO) return;
    _pst_UVGizmo->pst_GO = _pst_GO;
    _pst_UVGizmo->c_On = 1;
    _pst_UVGizmo->c_XYZ = SOFT_Cl_UVGizmo_Z;
    _pst_UVGizmo->c_Type = _c_Type;

    MATH_SetIdentityMatrix( &_pst_UVGizmo->st_Matrix);
    MATH_GetRotationMatrix( &_pst_UVGizmo->st_Matrix, OBJ_pst_GetAbsoluteMatrix( _pst_GO ) );
    MATH_SetTranslation( &_pst_UVGizmo->st_Matrix, OBJ_pst_GetAbsolutePosition( _pst_GO ) );
    MATH_SetRotationType( &_pst_UVGizmo->st_Matrix );

    if ( ( _c_Type == SOFT_Cl_UVGizmo_Cylindre ) || (_c_Type == SOFT_Cl_UVGizmo_Box) ) 
    {
        pst_Geo = (GEO_tdst_Object*)OBJ_p_GetCurrentGeo( _pst_GO );
        if ( !pst_Geo || !pst_Geo->l_NbPoints )
        {
            _pst_UVGizmo->x = _pst_UVGizmo->y = _pst_UVGizmo->z = 1.0f;
            return;
        }
        V = pst_Geo->dst_Point;
        VLast = V + pst_Geo->l_NbPoints;
        MATH_InitVector( &VMin, Cf_Infinit, Cf_Infinit, Cf_Infinit );
        MATH_NegVector( &VMax, &VMin );
        for ( ; V < VLast; V++)
        {
            if (V->x < VMin.x) VMin.x = V->x;
            if (V->x > VMax.x) VMax.x = V->x;
            if (V->y < VMin.y) VMin.y = V->y;
            if (V->y > VMax.y) VMax.y = V->y;
            if (V->z < VMin.z) VMin.z = V->z;
            if (V->z > VMax.z) VMax.z = V->z;
        }
        _pst_UVGizmo->x = (VMin.x == VMax.x) ? 1 : ((VMax.x - VMin.x) / 2);
        _pst_UVGizmo->y = (VMin.y == VMax.y) ? 1 : ((VMax.y - VMin.y) / 2);
        _pst_UVGizmo->z = (VMin.z == VMax.z) ? 1 : ((VMax.z - VMin.z) / 2);

        MATH_AddEqualVector( &VMin, &VMax );
        MATH_ScaleEqualVector( &VMin, 0.5f );
        MATH_TransformVertex( &VMax, OBJ_pst_GetAbsoluteMatrix( _pst_GO), &VMin );
        MATH_SetTranslation( &_pst_UVGizmo->st_Matrix, &VMax );
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_UVGizmo_Reset( SOFT_tdst_UVGizmo *_pst_UVGizmo )
{
    _pst_UVGizmo->c_On = 0;
}

/*
 =======================================================================================================================
    move the gizmo
 =======================================================================================================================
 */
void SOFT_UVGizmo_Move(SOFT_tdst_UVGizmo *_pst_Gizmo, MATH_tdst_Matrix *pst_Cam2World, MATH_tdst_Vector *V )
{
    MATH_tdst_Vector Move;

    MATH_TransformVector(&Move, pst_Cam2World, V );
    MATH_AddEqualVector( &_pst_Gizmo->st_Matrix.T, &Move );

    /* apply and refresh */
    SOFT_UVGizmo_Apply( _pst_Gizmo);
    LINK_Refresh();
}

/*
 =======================================================================================================================
    rotate the gizmo
 =======================================================================================================================
 */
void SOFT_UVGizmo_Rotate(SOFT_tdst_UVGizmo *_pst_Gizmo, MATH_tdst_Matrix *pst_Cam2World, MATH_tdst_Vector *pst_Axis, MATH_tdst_Vector *V )
{
    float               f_Angle;
    MATH_tdst_Matrix    st_TempMatrix, st_Rotation;
    MATH_tdst_Vector    st_Temp, st_Axis;
	
    f_Angle = (V->x) ? V->x : ((V->y) ? V->y : V->z);
	if(f_Angle == 0)
        return;
	
	/* Transform axis into globalaxis of object */
	MATH_TransformVector(&st_Axis, pst_Cam2World, pst_Axis);
	MATH_NormalizeVector(&st_Axis, &st_Axis);

    MATH_CopyMatrix( &st_TempMatrix, &_pst_Gizmo->st_Matrix );
	MATH_TranspEq33Matrix(&st_TempMatrix);
    
    MATH_CopyVector( &st_Temp, &st_Axis );
    MATH_TransformVector(&st_Axis, &st_TempMatrix, &st_Temp);
	MATH_NormalizeVector(&st_Axis, &st_Axis);

	/* Compute the matrix for the rotation around that axis */
	MATH_MakeRotationMatrix_AxisAngle(&st_Rotation, &st_Axis, f_Angle, NULL, 1);

	/* Make the rotation */
	MATH_CopyMatrix( &st_TempMatrix, &_pst_Gizmo->st_Matrix);
	MATH_Mul33MatrixMatrix(&_pst_Gizmo->st_Matrix, &st_Rotation, &st_TempMatrix, 1);

    /* apply and refresh */
    SOFT_UVGizmo_Apply( _pst_Gizmo );
    LINK_Refresh();

}

/*
 =======================================================================================================================
    rotate the gizmo
 =======================================================================================================================
 */
void SOFT_UVGizmo_Scale(SOFT_tdst_UVGizmo *_pst_Gizmo, MATH_tdst_Vector *V )
{
    _pst_Gizmo->x *= V->x;
    _pst_Gizmo->y *= V->y;
    _pst_Gizmo->z *= V->z;

    /* apply and refresh */
    SOFT_UVGizmo_Apply( _pst_Gizmo );
    LINK_Refresh();
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_UVGizmo_Draw(SOFT_tdst_UVGizmo *_pst_Gizmo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GDI_tdst_Request_DrawLineEx st_LineEx[3];
	MATH_tdst_Vector			O0, O1, A, B, C, D, V;
    float                       angle;
    int                         i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if (!_pst_Gizmo->c_On) return;

    SOFT_MatrixStack_Reset(&GDI_gpst_CurDD->st_MatrixStack, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
    SOFT_l_MatrixStack_Push(&GDI_gpst_CurDD->st_MatrixStack, &_pst_Gizmo->st_Matrix );
    GDI_gpst_CurDD->st_Camera.pst_ObjectToCameraMatrix = GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix;
	GDI_SetViewMatrix((*GDI_gpst_CurDD), GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);

	st_LineEx[0].A = &A;
	st_LineEx[0].B = &B;
	st_LineEx[0].f_Width = 3;
	st_LineEx[0].ul_Flags = 0;
	st_LineEx[0].ul_Color = 0xFF007FFF;

    if (_pst_Gizmo->c_Type == SOFT_Cl_UVGizmo_Planar )
    {
        MATH_ScaleVector(&A, &MATH_gst_BaseVectorI, _pst_Gizmo->x);
	    MATH_CopyVector(&B, &A);
	    MATH_AddScaleVector(&A, &A, &MATH_gst_BaseVectorK, _pst_Gizmo->z);
	    MATH_AddScaleVector(&B, &B, &MATH_gst_BaseVectorK, -_pst_Gizmo->z);
	    
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
	    MATH_NegEqualVector(&A);
	    GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
	    MATH_NegEqualVector(&B);
	    GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
	    MATH_NegEqualVector(&A);
	    GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
    }
    else if (_pst_Gizmo->c_Type == SOFT_Cl_UVGizmo_Cylindre )
    {
        st_LineEx[1].A = &A;
        st_LineEx[1].B = &C;
        st_LineEx[1].f_Width = 3;
        st_LineEx[1].ul_Flags = 0;
	    st_LineEx[1].ul_Color = 0xFF007FFF;

        st_LineEx[2].A = &B;
        st_LineEx[2].B = &D;
        st_LineEx[2].f_Width = 3;
        st_LineEx[2].ul_Flags = 0;
	    st_LineEx[2].ul_Color = 0xFF007FFF;

        MATH_ScaleVector(&O0, &MATH_gst_BaseVectorK, _pst_Gizmo->z);
        MATH_NegVector(&O1, &O0);
	    MATH_AddScaleVector(&C, &O0, &MATH_gst_BaseVectorI, _pst_Gizmo->x);
	    MATH_AddScaleVector(&D, &O1, &MATH_gst_BaseVectorI, _pst_Gizmo->x);
        V.z = 0;

        for (i = 1; i <= 16; i++)
        {
            angle = i * Cf_2Pi / 16;

            MATH_CopyVector( &A, &C );
            MATH_CopyVector( &B, &D );

            V.x = _pst_Gizmo->x * fCos( angle );
            V.y = _pst_Gizmo->y * fSin( angle );
            MATH_AddVector( &C, &O0, &V );
            MATH_AddVector( &D, &O1, &V );

            GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
            GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) (st_LineEx + 1) );
            GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) (st_LineEx + 2) );
        }
    }
    else if (_pst_Gizmo->c_Type == SOFT_Cl_UVGizmo_Box )
    {
        MATH_SetVector( &A, _pst_Gizmo->x, _pst_Gizmo->y, _pst_Gizmo->z );
        MATH_SetVector( &B, -_pst_Gizmo->x, _pst_Gizmo->y, _pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
        MATH_CopyVector( &A, &B );
        MATH_SetVector( &B, -_pst_Gizmo->x, -_pst_Gizmo->y, _pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
        MATH_CopyVector( &A, &B );
        MATH_SetVector( &B, _pst_Gizmo->x, -_pst_Gizmo->y, _pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
        MATH_CopyVector( &A, &B );
        MATH_SetVector( &B, _pst_Gizmo->x, _pst_Gizmo->y, _pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);

        MATH_SetVector( &A, _pst_Gizmo->x, _pst_Gizmo->y, -_pst_Gizmo->z );
        MATH_SetVector( &B, -_pst_Gizmo->x, _pst_Gizmo->y, -_pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
        MATH_CopyVector( &A, &B );
        MATH_SetVector( &B, -_pst_Gizmo->x, -_pst_Gizmo->y, -_pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
        MATH_CopyVector( &A, &B );
        MATH_SetVector( &B, _pst_Gizmo->x, -_pst_Gizmo->y, -_pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
        MATH_CopyVector( &A, &B );
        MATH_SetVector( &B, _pst_Gizmo->x, _pst_Gizmo->y, -_pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);

        MATH_SetVector( &A, _pst_Gizmo->x, _pst_Gizmo->y, -_pst_Gizmo->z );
        MATH_SetVector( &B, _pst_Gizmo->x, _pst_Gizmo->y, _pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
        MATH_SetVector( &A, -_pst_Gizmo->x, _pst_Gizmo->y, -_pst_Gizmo->z );
        MATH_SetVector( &B, -_pst_Gizmo->x, _pst_Gizmo->y, _pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
        MATH_SetVector( &A, -_pst_Gizmo->x, -_pst_Gizmo->y, -_pst_Gizmo->z );
        MATH_SetVector( &B, -_pst_Gizmo->x, -_pst_Gizmo->y, _pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
        MATH_SetVector( &A, _pst_Gizmo->x, -_pst_Gizmo->y, -_pst_Gizmo->z );
        MATH_SetVector( &B, _pst_Gizmo->x, -_pst_Gizmo->y, _pst_Gizmo->z );
        GDI_gpst_CurDD->st_GDI.pfnl_Request(GDI_Cul_Request_DrawLineEx, (ULONG) st_LineEx);
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_UVGizmo_Planar(SOFT_tdst_UVGizmo *_pst_Gizmo, MATH_tdst_Matrix *M, GEO_tdst_Object *_pst_Geo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*pc_Sel;
	int							i, nb, first;
	GEO_tdst_IndexedTriangle	*T;
	MATH_tdst_Vector			V;
	GEO_tdst_SubObject_Face		*pst_Face;
	int							*pi_VertexUV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pi_VertexUV = (int *) L_malloc(4 * _pst_Geo->l_NbPoints);
	L_memset(pi_VertexUV, -1, 4 * _pst_Geo->l_NbPoints);

    /* first : count number of points */
    first = _pst_Geo->l_NbUVs;
    pc_Sel = _pst_Geo->pst_SubObject->dc_VSel;
    for (nb = 0, i = 0; i < _pst_Geo->l_NbPoints; i++, pc_Sel++)
    {
        if (*pc_Sel & 1) 
            pi_VertexUV[i] = first + nb++;
    }

    if (nb == 0) return;

    /* add uv */
    first = _pst_Geo->l_NbUVs;
    _pst_Geo->l_NbUVs += nb;
    if( first )
        _pst_Geo->dst_UV = (GEO_tdst_UV *) MEM_p_Realloc(_pst_Geo->dst_UV, (first + nb) * sizeof(GEO_tdst_UV));
	else
		_pst_Geo->dst_UV = (GEO_tdst_UV *) MEM_p_Alloc(nb * sizeof(GEO_tdst_UV));

    /* compute new UV */
	pc_Sel = _pst_Geo->pst_SubObject->dc_VSel;
	for(i = 0; i < _pst_Geo->l_NbPoints; i++, pc_Sel++)
	{
		if( !(*pc_Sel & 1) ) continue;

        if ( (pi_VertexUV[i] < 0) || (pi_VertexUV[i] >= (first + nb) ) )
            continue;

		MATH_TransformVertex(&V, M, &_pst_Geo->dst_Point[i]);
		_pst_Geo->dst_UV[ pi_VertexUV[i] ].fU = (V.x / _pst_Gizmo->x) / 2 + 0.5f;
		_pst_Geo->dst_UV[ pi_VertexUV[i] ].fV = -(V.z / _pst_Gizmo->z) / 2 + 0.5f;
	}

    /* set uv of selected faces */
	pc_Sel = _pst_Geo->pst_SubObject->dc_FSel;
	pst_Face = _pst_Geo->pst_SubObject->dst_Faces;
	for(i = 0; i < (int) _pst_Geo->pst_SubObject->ul_NbFaces; i++, pc_Sel++, pst_Face++)
	{
		if(!(*pc_Sel & 1)) continue;
		T = _pst_Geo->dst_Element[pst_Face->uw_Element].dst_Triangle + pst_Face->uw_Index;
		T->auw_UV[0] = pi_VertexUV[T->auw_Index[0]];
		T->auw_UV[1] = pi_VertexUV[T->auw_Index[1]];
		T->auw_UV[2] = pi_VertexUV[T->auw_Index[2]];
	}

    L_free( pi_VertexUV );

    GEO_SubObject_UVClean( _pst_Geo );

	if(_pst_Geo->pst_SubObject->pfn_UVMapper_Update)
		_pst_Geo->pst_SubObject->pfn_UVMapper_Update(_pst_Geo->pst_SubObject->ul_UVMapper_Param);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_UVGizmo_Cylinder(SOFT_tdst_UVGizmo *_pst_Gizmo, MATH_tdst_Matrix *M, GEO_tdst_Object *_pst_Geo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*pc_Sel;
	int							i, j, zone, nb;
	GEO_tdst_IndexedTriangle	*T, *TLast;
	MATH_tdst_Vector			V, V0, V1;
    float                       n, a, cosa;
    int                         *pai_VUV[3];
    int                         *pi_UsedUV;
    char                        *pc_FaceZone, *pc_CurFaceZone;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* alloc */
    for (i = 0; i < 3; i++)
    {
        pai_VUV[i] = (int *) L_malloc(4 * _pst_Geo->l_NbPoints);
	    L_memset(pai_VUV[i], -1, 4 * _pst_Geo->l_NbPoints);
    }
	if (_pst_Geo->l_NbUVs )
	{
		pi_UsedUV = (int *) L_malloc( 4 * _pst_Geo->l_NbUVs );
		L_memset( pi_UsedUV , 0, 4 * _pst_Geo->l_NbUVs );
	}
	else
		pi_UsedUV = NULL;

    pc_FaceZone = (char *) L_malloc( _pst_Geo->pst_SubObject->ul_NbFaces );
    L_memset( pc_FaceZone, -1, _pst_Geo->pst_SubObject->ul_NbFaces );

    /* compute array of indexes */
    pc_Sel = _pst_Geo->pst_SubObject->dc_FSel;
    pc_CurFaceZone = pc_FaceZone;
    for ( i = 0; i < _pst_Geo->l_NbElements; i++)
    {
        T = _pst_Geo->dst_Element[i].dst_Triangle;
        TLast = T + _pst_Geo->dst_Element[i].l_NbTriangles;
        for ( ; T < TLast; T++, pc_Sel++, pc_CurFaceZone++ )
        {
            if (*pc_Sel & 1)
            {
                MATH_SubVector( &V0, VCast( &_pst_Geo->dst_Point[ T->auw_Index[0] ] ), VCast( &_pst_Geo->dst_Point[ T->auw_Index[1] ] ) );
                MATH_SubVector( &V1, VCast( &_pst_Geo->dst_Point[ T->auw_Index[0] ] ), VCast( &_pst_Geo->dst_Point[ T->auw_Index[2] ] ) );
                MATH_CrossProduct(&V, &V0, &V1 ); 
                MATH_NormalizeEqualVector( &V );
                n = MATH_f_DotProduct( &V, MATH_pst_GetZAxis( M ) );
                zone = ( n > 0.5) ? 0 : (n < -0.5) ? 2 : 1;
                *pc_CurFaceZone = zone;
                for ( j = 0; j < 3; j++)
                {
                    if (pai_VUV[ zone ][ T->auw_Index[j] ] == - 1)
                        pai_VUV[ zone ][ T->auw_Index[j] ] = T->auw_UV[j];
                }
            }
            else
            {
				if (pi_UsedUV)
				{
	                for (j = 0; j < 3; j++)
		                pi_UsedUV[ T->auw_UV[ j ] ]++;
				}
            }
        }
    }

    /* count number of point to add and new indexes */
    nb = _pst_Geo->l_NbUVs;
    for (j = 0; j < 3; j++)
    {
        for (i = 0; i < _pst_Geo->l_NbPoints; i++)
        {
            if (pai_VUV[ j ][ i ] == -1 ) continue;
            if (!pi_UsedUV || (pi_UsedUV[ pai_VUV[j][i] ] == 0)) continue;
            pai_VUV[j][i] = nb++;
        }
    }

    /* add uv */
    if (nb != _pst_Geo->l_NbUVs)
    {
        if (_pst_Geo->l_NbUVs)
            _pst_Geo->dst_UV = (GEO_tdst_UV *) MEM_p_Realloc(_pst_Geo->dst_UV, nb * sizeof(GEO_tdst_UV));
	    else
		    _pst_Geo->dst_UV = (GEO_tdst_UV *) MEM_p_Alloc(nb * sizeof(GEO_tdst_UV));
    }

    /* compute new UV for zone 0 : up of cylinder */
    for (i = 0; i < _pst_Geo->l_NbPoints; i++)
    {
        if (pai_VUV[ 0 ][ i ] == -1 ) continue;

        MATH_TransformVertex(&V, M, &_pst_Geo->dst_Point[i]);
		_pst_Geo->dst_UV[ pai_VUV[0][i] ].fU = (V.x / _pst_Gizmo->x) / 2 + 0.5f;
		_pst_Geo->dst_UV[ pai_VUV[0][i] ].fV = (V.y / _pst_Gizmo->y) / 2 + 0.5f;
    }

    /* compute new uv for zone 1 : on the cylinder */
    for (i = 0; i < _pst_Geo->l_NbPoints; i++)
    {
        if (pai_VUV[ 1 ][ i ] == -1 ) continue;

        MATH_TransformVertex(&V, M, &_pst_Geo->dst_Point[i]);
        _pst_Geo->dst_UV[ pai_VUV[1][i] ].fV = (V.z / _pst_Gizmo->z) / 2 + 0.5f;

        V.z = 0;
        n = MATH_f_NormVector( &V );
        if ( n == 0 )
            _pst_Geo->dst_UV[ pai_VUV[1][i] ].fU = 0;
        else
        {
            cosa = V.x / n;
            a = fAcos( cosa );
            if (V.y < 0)
                a = Cf_2Pi - a;
            _pst_Geo->dst_UV[ pai_VUV[1][i] ].fU = a / Cf_2Pi;
        }
    }

    /* compute new UV for zone 2 : bottom of cylinder */
    for (i = 0; i < _pst_Geo->l_NbPoints; i++)
    {
        if (pai_VUV[ 2 ][ i ] == -1 ) continue;

        MATH_TransformVertex(&V, M, &_pst_Geo->dst_Point[i]);
		_pst_Geo->dst_UV[ pai_VUV[2][i] ].fU = (V.x / _pst_Gizmo->x) / 2 + 0.5f;
		_pst_Geo->dst_UV[ pai_VUV[2][i] ].fV = (V.y / _pst_Gizmo->y) / 2 + 0.5f;
    }

    /* assign new uv to the triangles */
    pc_Sel = _pst_Geo->pst_SubObject->dc_FSel;
    pc_CurFaceZone = pc_FaceZone;
    for ( i = 0; i < _pst_Geo->l_NbElements; i++)
    {
        T = _pst_Geo->dst_Element[i].dst_Triangle;
        TLast = T + _pst_Geo->dst_Element[i].l_NbTriangles;
        for ( ; T < TLast; T++, pc_Sel++, pc_CurFaceZone++ )
        {
            if ( !(*pc_Sel & 1) ) continue;
            for (j = 0; j < 3; j++)
                T->auw_UV[ j ] = pai_VUV[ *pc_CurFaceZone ][ T->auw_Index[ j ] ];
        }
    }

    L_free( pai_VUV[0] );
    L_free( pai_VUV[1] );
    L_free( pai_VUV[2] );
    if (pi_UsedUV) L_free( pi_UsedUV );
    L_free( pc_FaceZone );

    GEO_SubObject_UVClean( _pst_Geo );

	if(_pst_Geo->pst_SubObject->pfn_UVMapper_Update)
		_pst_Geo->pst_SubObject->pfn_UVMapper_Update(_pst_Geo->pst_SubObject->ul_UVMapper_Param);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_UVGizmo_Box(SOFT_tdst_UVGizmo *_pst_Gizmo, MATH_tdst_Matrix *M, GEO_tdst_Object *_pst_Geo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char						*pc_Sel;
	int							i, j, zone, nb;
	GEO_tdst_IndexedTriangle	*T, *TLast;
	MATH_tdst_Vector			V, V0, V1;
    int                         *pai_VUV[6];
    int                         *pi_UsedUV;
    char                        *pc_FaceZone, *pc_CurFaceZone;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* alloc */
    for (i = 0; i < 6; i++)
    {
        pai_VUV[i] = (int *) L_malloc(4 * _pst_Geo->l_NbPoints);
	    L_memset(pai_VUV[i], -1, 4 * _pst_Geo->l_NbPoints);
    }
	if (_pst_Geo->l_NbUVs)
	{
		pi_UsedUV = (int *) L_malloc( 4 * _pst_Geo->l_NbUVs );
		L_memset( pi_UsedUV , 0, 4 * _pst_Geo->l_NbUVs );
	}
	else
		pi_UsedUV = NULL;

    pc_FaceZone = (char *) L_malloc( _pst_Geo->pst_SubObject->ul_NbFaces );
    L_memset( pc_FaceZone, -1, _pst_Geo->pst_SubObject->ul_NbFaces );

    /* compute array of indexes */
    pc_Sel = _pst_Geo->pst_SubObject->dc_FSel;
    pc_CurFaceZone = pc_FaceZone;
    for ( i = 0; i < _pst_Geo->l_NbElements; i++)
    {
        T = _pst_Geo->dst_Element[i].dst_Triangle;
        TLast = T + _pst_Geo->dst_Element[i].l_NbTriangles;
        for ( ; T < TLast; T++, pc_Sel++, pc_CurFaceZone++ )
        {
            if (*pc_Sel & 1)
            {
                MATH_SubVector( &V0, VCast( &_pst_Geo->dst_Point[ T->auw_Index[0] ] ), VCast( &_pst_Geo->dst_Point[ T->auw_Index[1] ] ) );
                MATH_SubVector( &V1, VCast( &_pst_Geo->dst_Point[ T->auw_Index[0] ] ), VCast( &_pst_Geo->dst_Point[ T->auw_Index[2] ] ) );
                MATH_CrossProduct(&V, &V0, &V1 ); 
                MATH_TransformVector(&V0, M, &V);
                if ( (fAbs( V0.x) >= fAbs( V0.y )) && (fAbs(V0.x) >= fAbs( V0.z)) )
                    zone = (V0.x < 0) ? 1 : 0;
                else if (fAbs(V0.y ) > fAbs( V0.z) )
                    zone = (V0.y < 0) ? 3 : 2;
                else
                    zone = (V0.z < 0) ? 5 : 4;
                *pc_CurFaceZone = zone;
                for ( j = 0; j < 3; j++)
                {
                    if (pai_VUV[ zone ][ T->auw_Index[j] ] == - 1)
                        pai_VUV[ zone ][ T->auw_Index[j] ] = T->auw_UV[j];
                }
            }
            else if ( pi_UsedUV )
            {
                for (j = 0; j < 3; j++)
					if ( T->auw_UV[ j ] != (USHORT) -1 )
						pi_UsedUV[ T->auw_UV[ j ] ]++;
            }
        }
    }

    /* count number of point to add and new indexes */
    nb = _pst_Geo->l_NbUVs;
    for (j = 0; j < 6; j++)
    {
        for (i = 0; i < _pst_Geo->l_NbPoints; i++)
        {
            if (pai_VUV[ j ][ i ] == -1 ) continue;
			if (pi_UsedUV && ( pi_UsedUV[ pai_VUV[j][i] ] == 0) )
            {
                pi_UsedUV[ pai_VUV[j][i] ]++;
                continue;
            }
            pai_VUV[j][i] = nb++;
        }
    }

    /* add uv */
    if (nb != _pst_Geo->l_NbUVs)
    {
        if (_pst_Geo->l_NbUVs)
            _pst_Geo->dst_UV = (GEO_tdst_UV *) MEM_p_Realloc(_pst_Geo->dst_UV, nb * sizeof(GEO_tdst_UV));
	    else
		    _pst_Geo->dst_UV = (GEO_tdst_UV *) MEM_p_Alloc(nb * sizeof(GEO_tdst_UV));
        _pst_Geo->l_NbUVs = nb;
    }

    /* compute new UV for zone 0 : x > 0 */
    for (i = 0; i < _pst_Geo->l_NbPoints; i++)
    {
        if (pai_VUV[ 0 ][ i ] == -1 ) continue;

        MATH_TransformVertex(&V, M, &_pst_Geo->dst_Point[i]);
		_pst_Geo->dst_UV[ pai_VUV[0][i] ].fU = (V.y / _pst_Gizmo->y) / 2 + 0.5f;
		_pst_Geo->dst_UV[ pai_VUV[0][i] ].fV = (V.z / _pst_Gizmo->z) / 2 + 0.5f;
    }

    /* compute new UV for zone 1 : x < 0 */
    for (i = 0; i < _pst_Geo->l_NbPoints; i++)
    {
        if (pai_VUV[ 1 ][ i ] == -1 ) continue;

        MATH_TransformVertex(&V, M, &_pst_Geo->dst_Point[i]);
		_pst_Geo->dst_UV[ pai_VUV[1][i] ].fU = (V.y / _pst_Gizmo->y) / 2 + 0.5f;
		_pst_Geo->dst_UV[ pai_VUV[1][i] ].fV = (V.z / _pst_Gizmo->z) / 2 + 0.5f;
    }

    /* compute new UV for zone 2 : y > 0 */
    for (i = 0; i < _pst_Geo->l_NbPoints; i++)
    {
        if (pai_VUV[ 2 ][ i ] == -1 ) continue;

        MATH_TransformVertex(&V, M, &_pst_Geo->dst_Point[i]);
		_pst_Geo->dst_UV[ pai_VUV[2][i] ].fU = (V.x / _pst_Gizmo->x) / 2 + 0.5f;
		_pst_Geo->dst_UV[ pai_VUV[2][i] ].fV = (V.z / _pst_Gizmo->z) / 2 + 0.5f;
    }

    /* compute new UV for zone 3 : y < 0 */
    for (i = 0; i < _pst_Geo->l_NbPoints; i++)
    {
        if (pai_VUV[ 3 ][ i ] == -1 ) continue;

        MATH_TransformVertex(&V, M, &_pst_Geo->dst_Point[i]);
		_pst_Geo->dst_UV[ pai_VUV[3][i] ].fU = (V.x / _pst_Gizmo->x) / 2 + 0.5f;
		_pst_Geo->dst_UV[ pai_VUV[3][i] ].fV = (V.z / _pst_Gizmo->z) / 2 + 0.5f;
    }

    /* compute new UV for zone 4 : z > 0 */
    for (i = 0; i < _pst_Geo->l_NbPoints; i++)
    {
        if (pai_VUV[ 4 ][ i ] == -1 ) continue;

        MATH_TransformVertex(&V, M, &_pst_Geo->dst_Point[i]);
		_pst_Geo->dst_UV[ pai_VUV[4][i] ].fU = (V.x / _pst_Gizmo->x) / 2 + 0.5f;
		_pst_Geo->dst_UV[ pai_VUV[4][i] ].fV = (V.y / _pst_Gizmo->y) / 2 + 0.5f;
    }

    /* compute new UV for zone 1 : z < 0 */
    for (i = 0; i < _pst_Geo->l_NbPoints; i++)
    {
        if (pai_VUV[ 5 ][ i ] == -1 ) continue;

        MATH_TransformVertex(&V, M, &_pst_Geo->dst_Point[i]);
		_pst_Geo->dst_UV[ pai_VUV[5][i] ].fU = (V.x / _pst_Gizmo->x) / 2 + 0.5f;
		_pst_Geo->dst_UV[ pai_VUV[5][i] ].fV = (V.y / _pst_Gizmo->y) / 2 + 0.5f;
    }

    /* assign new uv to the triangles */
    pc_Sel = _pst_Geo->pst_SubObject->dc_FSel;
    pc_CurFaceZone = pc_FaceZone;
    for ( i = 0; i < _pst_Geo->l_NbElements; i++)
    {
        T = _pst_Geo->dst_Element[i].dst_Triangle;
        TLast = T + _pst_Geo->dst_Element[i].l_NbTriangles;
        for ( ; T < TLast; T++, pc_Sel++, pc_CurFaceZone++ )
        {
            if ( !(*pc_Sel & 1) ) continue;
            for (j = 0; j < 3; j++)
                T->auw_UV[ j ] = pai_VUV[ *pc_CurFaceZone ][ T->auw_Index[ j ] ];
        }
    }

    L_free( pai_VUV[0] );
    L_free( pai_VUV[1] );
    L_free( pai_VUV[2] );
    L_free( pai_VUV[3] );
    L_free( pai_VUV[4] );
    L_free( pai_VUV[5] );
    if (pi_UsedUV) L_free( pi_UsedUV );
    L_free( pc_FaceZone );

    GEO_SubObject_UVClean( _pst_Geo );

	if(_pst_Geo->pst_SubObject->pfn_UVMapper_Update)
		_pst_Geo->pst_SubObject->pfn_UVMapper_Update(_pst_Geo->pst_SubObject->ul_UVMapper_Param);
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL SOFT_b_UVGizmo_Apply(SEL_tdst_SelectedItem *_pst_Sel, ULONG _ul_Param1, ULONG _ul_Param2)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	GEO_tdst_Object		*pst_Obj;
	MATH_tdst_Matrix	M;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = (OBJ_tdst_GameObject *) _pst_Sel->p_Content;
	if(!pst_GO) return TRUE;
	pst_Obj = (GEO_tdst_Object *) OBJ_p_GetCurrentGeo(pst_GO);
	if(!pst_Obj) return TRUE;
	if(!pst_Obj->pst_SubObject) return TRUE;

    MATH_MulMatrixMatrix(&M, pst_GO->pst_GlobalMatrix, (MATH_tdst_Matrix *) _ul_Param2);

    switch( ((SOFT_tdst_UVGizmo *) _ul_Param1)->c_Type)
    {
    case SOFT_Cl_UVGizmo_Planar:
	    SOFT_UVGizmo_Planar((SOFT_tdst_UVGizmo *) _ul_Param1, &M, pst_Obj);
        break;
    case SOFT_Cl_UVGizmo_Cylindre:
        SOFT_UVGizmo_Cylinder((SOFT_tdst_UVGizmo *) _ul_Param1, &M, pst_Obj);
        break;
    case SOFT_Cl_UVGizmo_Box:
        SOFT_UVGizmo_Box((SOFT_tdst_UVGizmo *) _ul_Param1, &M, pst_Obj);
        break;
    }
	return TRUE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SOFT_UVGizmo_Apply(SOFT_tdst_UVGizmo *pst_Gizmo)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SEL_tdst_Selection	*pst_Sel;
	MATH_tdst_Matrix	st_Matrix;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    if ( (!pst_Gizmo) || (!pst_Gizmo->pst_GO ) ) return;

	MATH_SetIdentityMatrix(&st_Matrix);
	MATH_InvertMatrix(&st_Matrix, &pst_Gizmo->st_Matrix );

	pst_Sel = pst_Gizmo->pst_GO->pst_World->pst_Selection;
	SEL_EnumItem(pst_Sel, SEL_C_SIF_Object, SOFT_b_UVGizmo_Apply, (ULONG) pst_Gizmo, (ULONG) & st_Matrix);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
#endif /* ACTIVE_EDITORS */ 
 