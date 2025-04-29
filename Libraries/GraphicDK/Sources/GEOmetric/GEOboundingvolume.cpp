/*$T GEOboundingvolume.c GC! 1.081 01/29/01 11:15:53 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/BASsys.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"

#include "GDInterface/GDInterface.h"
#include "GEOmetric/GEOdebugobject.h"
#include "SELection/SELection.h"

#ifdef JADEFUSION
#include "XenonGraphics/XeMesh.h"
#endif
#define GEO_Cul_BV_DrawMask (GDI_Cul_DM_All - GDI_Cul_DM_NotWired - GDI_Cul_DM_DontForceColor - GDI_Cul_DM_Lighted - GDI_Cul_DM_ReceiveDynSdw - GDI_Cul_DM_Fogged - GDI_Cul_DM_MaterialColor)

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_BoundingVolume_DisplayOBBox(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	void				*pst_BV;
	MATH_tdst_Vector	st_Min, st_Max;
	MATH_tdst_Matrix	*pst_GlobalMatrix;
	ULONG				ul_SaveColor;
	BOOL				b_SelectedBV;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BV = _pst_GO->pst_BV;
	pst_GlobalMatrix = OBJ_pst_GetAbsoluteMatrix(_pst_GO);

	SOFT_l_MatrixStack_Push(&_pst_DD->st_MatrixStack, _pst_GO->pst_GlobalMatrix);

	MATH_CopyVector(&st_Min, OBJ_pst_BV_GetLMin(pst_BV));
	MATH_CopyVector(&st_Max, OBJ_pst_BV_GetLMax(pst_BV));

	GEO_DebugObject_PushBoxMatrix(&_pst_DD->st_MatrixStack, &st_Min, &st_Max);
	GDI_SetViewMatrix((*_pst_DD) , _pst_DD->st_MatrixStack.pst_CurrentMatrix);

	if
	(
		(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
	&&	(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV)
	&&	(_pst_DD->uc_EditBounding)
	)
	{
		ul_SaveColor = _pst_DD->ul_ColorConstant;
		_pst_DD->ul_ColorConstant = 0x000000FF; /* Rouge */
		b_SelectedBV = TRUE;
	}

    GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Box, GEO_Cul_BV_DrawMask, 0, NULL);

	if(b_SelectedBV)
	{
		_pst_DD->ul_ColorConstant = ul_SaveColor;
	}

	GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);
	SOFT_l_MatrixStack_Pop(&_pst_DD->st_MatrixStack);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_BoundingVolume_DisplayAABBox(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Min, st_Max;
	void				*pst_BV;
	ULONG				ul_SaveColor;
	BOOL				b_Selected;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BV = _pst_GO->pst_BV;

	/*
	 * The Min/Max points of the AABBox are in the global coordinate system but
	 * WITHOUT the translation of the object. We just translate the AABBox before
	 * sending it to viewport.
	 */
	if(_pst_DD->uc_EditBounding)
	{
		MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMaxInit(pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
		MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMinInit(pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
	}
	else
	{
		MATH_AddVector(&st_Max, OBJ_pst_BV_GetGMax(pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
		MATH_AddVector(&st_Min, OBJ_pst_BV_GetGMin(pst_BV), OBJ_pst_GetAbsolutePosition(_pst_GO));
	}

	GEO_DebugObject_PushBoxMatrix(&_pst_DD->st_MatrixStack, &st_Min, &st_Max);
	GDI_SetViewMatrix((*_pst_DD) , _pst_DD->st_MatrixStack.pst_CurrentMatrix);
	

	b_Selected =
		(
			(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
		||	(
				_pst_GO
			&&	_pst_GO->pst_Extended
			&&	_pst_GO->pst_Extended->pst_Col
			&&	((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap
			&&	((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob
			&&	SEL_RetrieveItem
				(
					_pst_DD->pst_World->pst_Selection,
					*(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob)
				)
			)
		);


	if
	(
		(b_Selected)
	&&	!(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV)
	&&	(_pst_DD->uc_EditBounding)
	)
	{
		ul_SaveColor = _pst_DD->ul_ColorConstant;
		_pst_DD->ul_ColorConstant = 0x000000FF; /* Rouge */
	    GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Box, GEO_Cul_BV_DrawMask, 0, NULL);
		_pst_DD->ul_ColorConstant = ul_SaveColor;

	}
	else
    {
    	GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Box, GEO_Cul_BV_DrawMask, 0, NULL);
    }

	GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GEO_BoundingVolume_DisplaySphere(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Center;
	void				*pst_BV;
	ULONG				ul_SaveColor;
	BOOL				b_Selected;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BV = _pst_GO->pst_BV;
	MATH_TransformVertexNoScale(&st_Center, OBJ_pst_GetAbsoluteMatrix(_pst_GO), OBJ_pst_BV_GetCenter(pst_BV));

	GEO_DebugObject_PushSphereMatrix(&_pst_DD->st_MatrixStack, &st_Center, OBJ_f_BV_GetRadius(pst_BV));
	GDI_SetViewMatrix((*_pst_DD) , _pst_DD->st_MatrixStack.pst_CurrentMatrix);
	

	b_Selected =
		(
			(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
		||	(
				_pst_GO
			&&	_pst_GO->pst_Extended
			&&	_pst_GO->pst_Extended->pst_Col
			&&	((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap
			&&	((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob
			&&	SEL_RetrieveItem
				(
					_pst_DD->pst_World->pst_Selection,
					*(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob)
				)
			)
		);

	if(b_Selected && (_pst_DD->uc_EditBounding))
	{
		ul_SaveColor = _pst_DD->ul_ColorConstant;
		_pst_DD->ul_ColorConstant = 0x000000FF; /* Rouge */
        GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Sphere, GEO_Cul_BV_DrawMask, 0, NULL);
		_pst_DD->ul_ColorConstant = ul_SaveColor;
	}
	else
	{
    	GEO_DebugObject_Draw(_pst_DD, GEO_DebugObject_Sphere, GEO_Cul_BV_DrawMask, 0, NULL);
	}

	GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);
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
extern ULONG OGL_EditorInfoCorrectMul2X(ULONG Color);

extern "C" void GEO_BoundingVolume_Display( GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_CurrentDrawMask;
	ULONG	ul_SaveForcedColor, ul_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_CurrentDrawMask = _pst_DD->ul_CurrentDrawMask;
	ul_SaveForcedColor = _pst_DD->ul_ColorConstant;

	if
	(
		(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_OBBox))
	&&	(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV)
	) 
	{
		ul_Color = 0x0000FFFF;
		if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) ul_Color = 0x0099FFFF;
	}
	else
	{
		ul_Color = 0xFF8080FF;
		if(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected) ul_Color = 0xFF80FF80;
	}

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_HasInitialPos) && !(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV))
	{
		if((OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy)) && OBJ_pst_GetFather(_pst_GO))
		{
			if
			(
				memcmp
				(
					OBJ_pst_GetLocalMatrix(_pst_GO),
					OBJ_pst_GetInitialAbsoluteMatrix(_pst_GO),
					sizeof(MATH_tdst_Vector)
				)
			) ul_Color |= 0xFF0000;
		}
		else
		{
			if
			(
				memcmp
				(
					OBJ_pst_GetAbsoluteMatrix(_pst_GO),
					OBJ_pst_GetInitialAbsoluteMatrix(_pst_GO),
					sizeof(MATH_tdst_Matrix)
				)
			) ul_Color |= 0xFF0000;
		}
	}

	_pst_DD->ul_ColorConstant = OGL_EditorInfoCorrectMul2X(ul_Color);


	if
	(
		(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_OBBox))
	&&	(_pst_GO->ul_EditorFlags & OBJ_C_EditFlags_ShowAltBV)
	) 
		GEO_BoundingVolume_DisplayOBBox(_pst_DD, _pst_GO);
	else
	{
		_pst_GO->ul_EditorFlags &= ~OBJ_C_EditFlags_ShowAltBV;

		if(OBJ_BV_IsAABBox(_pst_GO->pst_BV))
			GEO_BoundingVolume_DisplayAABBox(_pst_DD, _pst_GO);
		else
			GEO_BoundingVolume_DisplaySphere(_pst_DD, _pst_GO);
	}

	_pst_DD->ul_ColorConstant = ul_SaveForcedColor;
	_pst_DD->ul_CurrentDrawMask = ul_CurrentDrawMask;
}
/*
=======================================================================================================================
=======================================================================================================================
*/
#ifdef _XENON_RENDER
void GEO_BoundingVolume_DisplayElementsBV(GDI_tdst_DisplayData *_pst_DD, OBJ_tdst_GameObject *_pst_GO)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector	st_Center;
    void				*pst_BV;
    ULONG				ul_SaveColor;
    BOOL				b_Selected;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ERR_X_Assert( _pst_GO );

    ULONG ul_backupColor = _pst_DD->ul_ColorConstant;

    if( _pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected ) 
    {
        _pst_DD->ul_ColorConstant = 0xFFFF0000; 
    }
    else
    {
        _pst_DD->ul_ColorConstant = 0xFFFFFF00;
    }

    if( _pst_GO->pst_Base &&
        _pst_GO->pst_Base->pst_Visu &&
        _pst_GO->pst_Base->pst_Visu->p_XeElements )
    {
        int iNbElementsMeshes = _pst_GO->pst_Base->pst_Visu->l_NbXeElements;
        for( int i=0; i<iNbElementsMeshes; ++i )
        {
            XeMesh * pMesh = _pst_GO->pst_Base->pst_Visu->p_XeElements[i].pst_Mesh;
            if( pMesh )
            {
                if( pMesh->HasBoundingVolume() )
                {
                    MATH_tdst_Vector vAABBMin, vAABBMax;
                    pMesh->GetAxisAlignedBoundingVolume( vAABBMin, vAABBMax );

                    //
                    // Draw the box for the sub mesh
                    // The bounding volume is in object space (min, max of all vertices)
                    //

                    SOFT_l_MatrixStack_Push( &_pst_DD->st_MatrixStack, OBJ_pst_GetAbsoluteMatrix(_pst_GO) );
                    GEO_DebugObject_PushBoxMatrix(&_pst_DD->st_MatrixStack, &vAABBMin, &vAABBMax);
                    GDI_SetViewMatrix((*_pst_DD) , _pst_DD->st_MatrixStack.pst_CurrentMatrix);

                    GEO_DebugObject_Draw( _pst_DD, GEO_DebugObject_Box, GEO_Cul_BV_DrawMask, 0, NULL );

                    GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);
                    GEO_DebugObject_PopMatrix(&_pst_DD->st_MatrixStack);
                }
            }
        }
    }

    _pst_DD->ul_ColorConstant = ul_backupColor;
}
#endif
#endif// ACTIVE_EDITORS
