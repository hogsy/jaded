/*$T F3Dview_portal.cpp GC! 1.081 03/25/04 11:59:51 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "EDIapp.h"
#include "F3Dframe/F3Dview.h"
#include "F3Dframe/F3Dstrings.h"
#include "F3Dframe/F3Dview_undo.h"
#include "CAMera/CAMera.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "EDImainframe.h"
#include "EDImsg.h"
#include "INOut/INOkeyboard.h"
#include "LINKs/LINKtoed.h"
#include "LINKs/LINKmsg.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/ENGcall.h"
#include "ENGine/Sources/WORld/WORrender.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJboundingvolume.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"

#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLaccess.h"

#include "GEOmetric/GEOdebugobject.h"
#include "GraphicDK/Sources/CAMera/CAMstruct.h"
#include "SOFT/SOFThelper.h"
#include "SOFT/SOFTpickingbuffer.h"
#include "Res/Res.h"
#include "BASe/MEMory/MEM.h"
#include "EDIpaths.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "MATHs/MATH.h"
#include "EDItors/Sources/OUTput/OUTframe.h"

#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEnt_interpolationkey.h"

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */
 
 void Portal_GetCenter( WOR_tdst_Portal *_pst_Portal, MATH_tdst_Vector *_pst_Center )
 {
	MATH_AddVector( _pst_Center, &_pst_Portal->vA, &_pst_Portal->vB );
	MATH_AddEqualVector( _pst_Center, &_pst_Portal->vC );
	MATH_AddEqualVector( _pst_Center, &_pst_Portal->vD );
	MATH_ScaleEqualVector( _pst_Center, 0.25f );
 }

/*
 =======================================================================================================================
    compute pos of help matrix when working with portal
 =======================================================================================================================
 */
BOOL F3D_cl_View::Portal_b_ComputeHelperMatrix(MATH_tdst_Matrix *_pst_Matrix)
{
	MATH_tdst_Vector	X, Y, Z;
	float				x, y, z;
	
	if ( !mpst_EditedPortal ) return FALSE;
	
	MATH_SubVector( &X, &mpst_EditedPortal->vB, &mpst_EditedPortal->vA );
	x = MATH_f_NormVector( &X );
	MATH_SubVector( &Z, &mpst_EditedPortal->vD, &mpst_EditedPortal->vA );
	z = MATH_f_NormVector( &Z );
	MATH_CrossProduct( &Y, &Z, &X );
	y = MATH_f_NormVector( &Y );
	
	MATH_SetIdentityMatrix( _pst_Matrix );
	if ( ( x != 0 ) && (y != 0 ) && ( z != 0 ) )
	{
		MATH_ScaleEqualVector( &X, fInv( x ) );
		MATH_ScaleEqualVector( &Y, fInv( y ) );
		MATH_CrossProduct( &Z, &X, &Y );
		
		MATH_SetXAxis( _pst_Matrix, &X );
		MATH_SetYAxis( _pst_Matrix, &Y );
		MATH_SetZAxis( _pst_Matrix, &Z );
	}
	
	Portal_GetCenter( mpst_EditedPortal, &_pst_Matrix->T );
	M_F3D_Helpers->ul_Flags |= SOFT_Cul_HF_LocalMode;
	return TRUE;
}

/*
 =======================================================================================================================
    Aim:    Move selected corner of portal
 =======================================================================================================================
 */
void F3D_cl_View::Portal_Move(MATH_tdst_Vector *_pst_Move)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	MATH_tdst_Matrix	*M;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if ( !mpst_EditedPortal ) 
		return;

	/* move is in camera system axis, transform into world SA */
	M = &mst_WinHandles.pst_DisplayData->st_Camera.st_Matrix;
	MATH_TransformVector(&v, M, _pst_Move);
	
	MATH_AddEqualVector( &mpst_EditedPortal->vA, &v );
	MATH_AddEqualVector( &mpst_EditedPortal->vB, &v );
	MATH_AddEqualVector( &mpst_EditedPortal->vC, &v );
	MATH_AddEqualVector( &mpst_EditedPortal->vD, &v );
	
	WOR_ValidatePortal( mst_WinHandles.pst_DisplayData->pst_World, mpst_EditedPortal );

	Refresh();
}

/*
 =======================================================================================================================
    Aim:    rotate portal
 =======================================================================================================================
 */
void F3D_cl_View::Portal_Rotate(MATH_tdst_Vector *_pst_Axe, MATH_tdst_Vector *_pst_Angle)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Angle;
	MATH_tdst_Vector	*Pos[4], v, a, st_Center;
	MATH_tdst_Matrix	Rotate;
	CAM_tdst_Camera		*pst_Camera;
    int                 i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	if ( !mpst_EditedPortal ) 
		return;
	
	/* get angle */
	f_Angle = (_pst_Angle->x) ? _pst_Angle->x : ((_pst_Angle->y) ? _pst_Angle->y : _pst_Angle->z);
	if(!f_Angle) return;
	
	/* move axis into world system axis */
	pst_Camera = (CAM_tdst_Camera *) (&M_F3D_DD->st_Camera);
	MATH_TransformVector(&a, &pst_Camera->st_Matrix, _pst_Axe);
	MATH_NormalizeVector(&a, &a);
	
	MATH_MakeRotationMatrix_AxisAngle(&Rotate, &a, f_Angle, NULL, 1);
	
	Portal_GetCenter( mpst_EditedPortal, &st_Center );
	Pos[ 0 ] = &mpst_EditedPortal->vA;
	Pos[ 1 ] = &mpst_EditedPortal->vB;
	Pos[ 2 ] = &mpst_EditedPortal->vC;
	Pos[ 3 ] = &mpst_EditedPortal->vD;
	
	for ( i = 0; i < 4; i++ )
	{
		MATH_SubVector(&a, Pos[ i ], &st_Center);
		MATH_TransformVector( &v, &Rotate, &a);
		MATH_AddVector( Pos[ i ], &st_Center, &v);
	}
	
	WOR_ValidatePortal( mst_WinHandles.pst_DisplayData->pst_World, mpst_EditedPortal );
	
	Refresh();
}

/*
 =======================================================================================================================
    Aim:    rotate portal
 =======================================================================================================================
 */
void F3D_cl_View::Portal_Scale( MATH_tdst_Vector *_pst_Axe )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	M;
	MATH_tdst_Vector	*Pos[4], a, u, st_Center, *Axe;
    int                 i, i_axe;
    float				t, f_Scale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	if ( !mpst_EditedPortal ) 
		return;
	
	Portal_GetCenter( mpst_EditedPortal, &st_Center );
	Pos[ 0 ] = &mpst_EditedPortal->vA;
	Pos[ 1 ] = &mpst_EditedPortal->vB;
	Pos[ 2 ] = &mpst_EditedPortal->vC;
	Pos[ 3 ] = &mpst_EditedPortal->vD;
	
	Portal_b_ComputeHelperMatrix( &M );
	
		
	for (i_axe = 0; i_axe < 2; i_axe ++ )
	{
		if (i_axe == 0 )
		{
			if (_pst_Axe->x == 1.0f )
				continue;
			f_Scale = _pst_Axe->x;
			Axe = MATH_pst_GetXAxis( &M );
		}
		else 
		{
			if (_pst_Axe->z == 1.0f )
				continue;
			f_Scale = _pst_Axe->z;
			Axe = MATH_pst_GetZAxis( &M );
		}
		
		for ( i = 0; i < 4; i++ )
		{
			MATH_SubVector(&a, Pos[ i ], &st_Center);
			t = MATH_f_DotProduct( &a, Axe );
			MATH_ScaleVector( &u, Axe, t );
			MATH_SubEqualVector( &a, &u );
			MATH_ScaleEqualVector( &u, f_Scale );
			MATH_AddEqualVector( &a, &u );
			MATH_AddVector( Pos[ i ], &st_Center, &a);
		}
	}
	
	WOR_ValidatePortal( mst_WinHandles.pst_DisplayData->pst_World, mpst_EditedPortal );
	
	Refresh();
}

#endif /* ACTIVE_EDITORS */

