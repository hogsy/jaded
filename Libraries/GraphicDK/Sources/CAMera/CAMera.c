/*$T CAMera.c GC! 1.081 06/06/00 14:42:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "CAMera/CAMera.h"
#include "BASe/BAStypes.h"
#include "GDInterface/GDInterface.h"
#include "LINks/LINKmsg.h"

/*$4
 ***********************************************************************************************************************
    Globals
 ***********************************************************************************************************************
 */

CAM_tdst_Camera CAM_gst_IsoNoClip;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_Engine_ComputePlans(CAM_tdst_Camera *_pst_Cam)
{
	if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
	{
		_pst_Cam->st_NormPlaneLeft.x = fOptCos(fHalf(_pst_Cam->f_FieldOfVision));
		_pst_Cam->st_NormPlaneRight.x = -_pst_Cam->st_NormPlaneLeft.x;
		_pst_Cam->st_NormPlaneLeft.y = 0.0f;
		_pst_Cam->st_NormPlaneRight.y = 0.0f;
		_pst_Cam->st_NormPlaneLeft.z = fOptSin(fHalf(_pst_Cam->f_FieldOfVision));
		_pst_Cam->st_NormPlaneRight.z = _pst_Cam->st_NormPlaneLeft.z;

		_pst_Cam->st_NormPlaneUp.x = 0.0;
		_pst_Cam->st_NormPlaneDown.x = 0.0;
		_pst_Cam->st_NormPlaneUp.y = fOptCos(fHalf(_pst_Cam->f_FieldOfVision));
		_pst_Cam->st_NormPlaneDown.y = -_pst_Cam->st_NormPlaneUp.y;
		_pst_Cam->st_NormPlaneUp.z = fOptSin(fHalf(_pst_Cam->f_FieldOfVision));
		_pst_Cam->st_NormPlaneDown.z = _pst_Cam->st_NormPlaneUp.z;
	}
	else	/* Isometric Camera */
	{
		_pst_Cam->st_NormPlaneLeft.x = 1;
		_pst_Cam->st_NormPlaneLeft.y = 0;
		_pst_Cam->st_NormPlaneLeft.z = 0;

		_pst_Cam->st_NormPlaneRight.x = -1;
		_pst_Cam->st_NormPlaneRight.y = 0;
		_pst_Cam->st_NormPlaneRight.z = 0;

		_pst_Cam->st_NormPlaneUp.x = 0;
		_pst_Cam->st_NormPlaneUp.y = 1;
		_pst_Cam->st_NormPlaneUp.z = 0;

		_pst_Cam->st_NormPlaneDown.x = 0;
		_pst_Cam->st_NormPlaneDown.y = -1;
		_pst_Cam->st_NormPlaneDown.z = 0;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_Engine_Init(CAM_tdst_Camera *_pst_Cam)
{
	MATH_SetIdentityMatrix(&_pst_Cam->st_Matrix);
	MATH_SetIdentityMatrix(&_pst_Cam->st_InverseMatrix);
	MATH_SetIdentityMatrix(&_pst_Cam->st_ProjectionMatrix);

	_pst_Cam->st_Matrix.Jy = 0.0f;
	_pst_Cam->st_Matrix.Jz = -1.0f;
	_pst_Cam->st_Matrix.Ky = 1.0f;
	_pst_Cam->st_Matrix.Kz = 0.0f;
	MATH_SetTranslationType(&_pst_Cam->st_Matrix);
	MATH_SetRotationType(&_pst_Cam->st_Matrix);

	_pst_Cam->st_Matrix.T.y = -20.0f;
	_pst_Cam->st_Matrix.T.z = 2.0f;

	_pst_Cam->ul_Flags = CAM_Cul_Flags_Perspective;

	_pst_Cam->f_NearPlane = 1.0f;
	_pst_Cam->f_FarPlane = 500000.0f;

#ifdef ACTIVE_EDITORS
    _pst_Cam->f_FarPlane = 2000000.0f;
#endif
	_pst_Cam->f_FieldOfVision = Cf_PiBy2;
	_pst_Cam->f_FactorX = 1.0f / fNormalTan(_pst_Cam->f_FieldOfVision / 2);
	_pst_Cam->f_FactorY = _pst_Cam->f_FactorX;
	_pst_Cam->f_YoverX = 1.0f;

	_pst_Cam->f_IsoFactor = 10.0f;
	_pst_Cam->f_IsoZoom = 1.0f;

	CAM_Engine_ComputePlans(_pst_Cam);
}

/*
 =======================================================================================================================
    Aim:    Assign camera to viewport, change camera parameters to fit with viewport size
 =======================================================================================================================
 */
void CAM_AssignCameraToDevice(GDI_tdst_ScreenFormat *_pst_SF, CAM_tdst_Camera *_pst_Cam, LONG _l_Width, LONG _l_Height)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	LONG	w, h, W, H;
	float	f, f_ScreenRatio;
	/*~~~~~~~~~~~~~~~~~~~~~*/

    /* compute screen ratio */
    if (!(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective) )
        f_ScreenRatio = GDI_gaf_ScreenRation[ GDI_Cul_SRC_Square ];
    else
    {
	    if((_pst_SF->l_ScreenRatioConst <= 0) || (_pst_SF->l_ScreenRatioConst >= GDI_Cul_SRC_Number))
		    f_ScreenRatio = _pst_SF->f_ScreenYoverX;
	    else
		    f_ScreenRatio = GDI_gaf_ScreenRation[_pst_SF->l_ScreenRatioConst];
    }

	f = _pst_Cam->f_YoverX * _pst_SF->f_PixelYoverX * f_ScreenRatio;

	/* compute height and width of screen */
	w = _l_Width;
	h = _l_Height;

	if(_pst_SF->ul_Flags & GDI_Cul_SFF_ReferenceIsY)
	{
		H = h;
		W = (LONG) (h / f);

		if((_pst_SF->ul_Flags & GDI_Cul_SFF_CropToWindow) && (W > w))
		{
			H = (LONG) (w * f);
			W = w;
		}
		else if((_pst_SF->ul_Flags & GDI_Cul_SFF_OccupyAll) && (W < w))
		{
			H = (LONG) (w * f);
			W = w;
		}
	}
	else
	{
		H = (LONG) (w * f);
		W = w;

		if((_pst_SF->ul_Flags & GDI_Cul_SFF_CropToWindow) && (H > h))
		{
			W = (LONG) (h / f);
			H = h;
		}
		else if((_pst_SF->ul_Flags & GDI_Cul_SFF_OccupyAll) && (H < h))
		{
			W = (LONG) (h / f);
			H = h;
		}
	}

	h = (int) (_pst_Cam->f_ViewportHeight * H) / 2;
	w = (int) (_pst_Cam->f_ViewportWidth * W) / 2;
	_pst_Cam->f_Width = (float) w * 2;
	_pst_Cam->f_Height = (float) h * 2;

	if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
	{
		if(_pst_SF->ul_Flags & GDI_Cul_SFF_ReferenceIsY)
		{
			_pst_Cam->f_FactorX = (w * f_ScreenRatio) / fNormalTan(_pst_Cam->f_FieldOfVision / 2);
			_pst_Cam->f_FactorY = h / fNormalTan(_pst_Cam->f_FieldOfVision / 2);
		}
		else
		{
			_pst_Cam->f_FactorX = w / fNormalTan(_pst_Cam->f_FieldOfVision / 2);
			_pst_Cam->f_FactorY = h / (fNormalTan(_pst_Cam->f_FieldOfVision / 2) * f_ScreenRatio);
		}
	}
	else
	{
		if(_pst_SF->ul_Flags & GDI_Cul_SFF_ReferenceIsY)
		{
			_pst_Cam->f_FactorX = (w * f_ScreenRatio) / (_pst_Cam->f_IsoFactor * _pst_Cam->f_IsoZoom);
			_pst_Cam->f_FactorY = h / (_pst_Cam->f_IsoFactor * _pst_Cam->f_IsoZoom);
		}
		else
		{
			_pst_Cam->f_FactorX = w / (_pst_Cam->f_IsoFactor * _pst_Cam->f_IsoZoom);
			_pst_Cam->f_FactorY = h / ((_pst_Cam->f_IsoFactor * _pst_Cam->f_IsoZoom) * f_ScreenRatio);
		}
	}

	_pst_Cam->f_CenterX = (float) (_l_Width / 2);
	_pst_Cam->f_CenterY = (float) (_l_Height / 2);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_Move(CAM_tdst_Camera *_pst_Cam, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*m;
	/*~~~~~~~~~~~~~~~~~~~*/

	m = &_pst_Cam->st_Matrix;

	if(v->x)
	{
		m->T.x += m->Ix * v->x;
		m->T.y += m->Iy * v->x;
		m->T.z += m->Iz * v->x;
	}

	if(v->y)
	{
		m->T.x += m->Jx * v->y;
		m->T.y += m->Jy * v->y;
		m->T.z += m->Jz * v->y;
	}

	if(v->z)
	{
		m->T.x += m->Kx * v->z;
		m->T.y += m->Ky * v->z;
		m->T.z += m->Kz * v->z;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_Rotate(CAM_tdst_Camera *_pst_Cam, MATH_tdst_Vector *v)
{
	if(v->x) MATH_RotateMatrix_AroundLocalYAxis(&_pst_Cam->st_Matrix, v->x);
	if(v->y) MATH_RotateMatrix_AroundLocalXAxis(&_pst_Cam->st_Matrix, v->y);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_RotateAroundTarget(CAM_tdst_Camera *_pst_Cam, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*M;
	MATH_tdst_Matrix	Mrot, Mbuffer;
	MATH_tdst_Vector	*pst_Target;
	MATH_tdst_Vector	*XAxis, *YAxis, *ZAxis;
	float				fAngle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M = &_pst_Cam->st_Matrix;
	pst_Target = &_pst_Cam->st_Target;

	/* Compute a rotation matrix around one axis (defined by a point and a vector) */
	//MATH_MakeRotationMatrix_PointAxisAngle(&Mrot, pst_Target, MATH_pst_GetYAxis(M), v->x, 0, 1);
    MATH_MakeRotationMatrix_PointAxisAngle(&Mrot, pst_Target, &MATH_gst_BaseVectorK, -v->x, 0, 1);
	M->Sx = M->Sy = M->Sz = 1.0f;
	Mrot.lType = MATH_Ci_Complex;
	Mrot.Sx = Mrot.Sy = Mrot.Sz = 1.0f;
	MATH_SetIdentityMatrix(&Mbuffer);

	MATH_MulMatrixMatrix(&Mbuffer, M, &Mrot);
	Mbuffer.Sx = Mbuffer.Sy = Mbuffer.Sz = 1.0f;
	MATH_CopyMatrix(M, &Mbuffer);

    MATH_MakeRotationMatrix_PointAxisAngle(&Mrot, pst_Target, MATH_pst_GetXAxis(M), v->y, 0, 1);
	Mrot.Sx = Mrot.Sy = Mrot.Sz = 1.0f;
	Mrot.lType = MATH_Ci_Complex;
	MATH_MulMatrixMatrix(&Mbuffer, M, &Mrot);
	Mbuffer.Sx = Mbuffer.Sy = Mbuffer.Sz = 1.0f;
    if (Mbuffer.Jz < 0)
        MATH_CopyMatrix(M, &Mbuffer);

	/* Make the camera horizontal (that is the X axis is into the (z=0) plane) */
	XAxis = MATH_pst_GetXAxis(M);
	YAxis = MATH_pst_GetYAxis(M);
	ZAxis = MATH_pst_GetZAxis(M);

	if(fAbs(XAxis->z) > .01f)
	{
		fAngle = fAtan2(-XAxis->z, YAxis->z);

		if(fAbs(fAngle) > Cf_PiBy2) fAngle += Cf_Pi;

		MATH_MakeRotationMatrix_PointAxisAngle(&Mrot, pst_Target, ZAxis, fAngle, 0, 1);
		Mrot.Sx = Mrot.Sy = Mrot.Sz = 1.0f;
		Mrot.lType = MATH_Ci_Complex;
		MATH_MulMatrixMatrix(&Mbuffer, M, &Mrot);
		MATH_CopyMatrix(M, &Mbuffer);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_Inverse(CAM_tdst_Camera *_pst_Cam)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*m, *mi;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	m = &_pst_Cam->st_Matrix;
	mi = &_pst_Cam->st_InverseMatrix;

	mi->Sx = mi->Sy = mi->Sz = 0.0f;
	MATH_Transp33MatrixWithoutBuffer(mi, m);

	mi->T.x = -m->T.x;
	mi->T.y = -m->T.y;
	mi->T.z = -m->T.z;

	MATH_TransformVector(&mi->T, mi, &mi->T);

	mi->lType = MATH_Ci_Translation | MATH_Ci_Rotation;
	mi->w = 1.0f;// MATRIX W!
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_SetObjectMatrixFromCam(MATH_tdst_Matrix *_pst_DstMatrix, MATH_tdst_Matrix *_pst_Matrix)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Axis;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_GetRotationMatrix(_pst_DstMatrix, _pst_Matrix);
	MATH_SetTranslation(_pst_DstMatrix, MATH_pst_GetTranslation(_pst_Matrix));

	/* Z is Y */
	MATH_CopyVector(&st_Axis, MATH_pst_GetZAxis(_pst_DstMatrix));
	MATH_CopyVector(MATH_pst_GetZAxis(_pst_DstMatrix), MATH_pst_GetYAxis(_pst_DstMatrix));
	MATH_NegEqualVector(MATH_pst_GetZAxis(_pst_DstMatrix));
	MATH_CopyVector(MATH_pst_GetYAxis(_pst_DstMatrix), &st_Axis);
	MATH_NegEqualVector(MATH_pst_GetXAxis(_pst_DstMatrix));
#ifndef _GAMECUBE	
	MATH_NegEqualVector(MATH_pst_GetYAxis(_pst_DstMatrix));
#endif	

	/* MATH_SetType(_pst_DstMatrix, MATH_Ci_Complex); */
	MATH_l_CalculateCorrectType(_pst_DstMatrix);
}

#ifdef ACTIVE_EDITORS
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_SetCamMatrixFromObject(MATH_tdst_Matrix *_pst_DstMatrix, MATH_tdst_Matrix *_pst_Matrix)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Axis;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_GetRotationMatrix(_pst_DstMatrix, _pst_Matrix);
	MATH_SetTranslation(_pst_DstMatrix, MATH_pst_GetTranslation(_pst_Matrix));

	/* Z is Y */
	MATH_CopyVector(&st_Axis, MATH_pst_GetZAxis(_pst_DstMatrix));
	MATH_CopyVector(MATH_pst_GetZAxis(_pst_DstMatrix), MATH_pst_GetYAxis(_pst_DstMatrix));
	MATH_CopyVector(MATH_pst_GetYAxis(_pst_DstMatrix), &st_Axis);
	MATH_NegEqualVector(MATH_pst_GetYAxis(_pst_DstMatrix));
	MATH_NegEqualVector(MATH_pst_GetXAxis(_pst_DstMatrix));
#ifndef _GAMECUBE	
	MATH_NegEqualVector(MATH_pst_GetZAxis(_pst_DstMatrix));
#endif	

	/* MATH_SetType(_pst_DstMatrix, MATH_Ci_Complex); */
	MATH_l_CalculateCorrectType(_pst_DstMatrix);
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_SetCameraMatrix(CAM_tdst_Camera *_pst_Cam, MATH_tdst_Matrix *_pst_Matrix)
{
	CAM_SetObjectMatrixFromCam(&_pst_Cam->st_Matrix, _pst_Matrix);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_SetCameraPos(CAM_tdst_Camera *_pst_Cam, MATH_tdst_Vector *_pst_Pos)
{
	MATH_CopyVector(&_pst_Cam->st_Matrix.T, _pst_Pos);
}

/*
 =======================================================================================================================
    Aim:    Compute 3D point from 2D point. x, y of 2D point are screen coordinates z of 2D point is depth of 3D point.
 =======================================================================================================================
 */
void CAM_2Dto3D(CAM_tdst_Camera *_pst_Cam, MATH_tdst_Vector *_pst_3DVector, MATH_tdst_Vector *_pst_2DVector)
{
	/*~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~*/

	if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
	{
		v.x = (_pst_2DVector->x - _pst_Cam->f_CenterX) * (_pst_2DVector->z / _pst_Cam->f_FactorX);
		v.y = (_pst_2DVector->y - _pst_Cam->f_CenterY) * (_pst_2DVector->z / _pst_Cam->f_FactorY);
		v.z = _pst_2DVector->z;
	}
	else
	{
		v.x = (_pst_2DVector->x - _pst_Cam->f_CenterX) / _pst_Cam->f_FactorX;
		v.y = (_pst_2DVector->y - _pst_Cam->f_CenterY) / _pst_Cam->f_FactorY;
		v.z = _pst_2DVector->z;
	}

	MATH_TransformVertex(_pst_3DVector, &_pst_Cam->st_Matrix, &v);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_2Dto3DCamera(CAM_tdst_Camera *_pst_Cam, MATH_tdst_Vector *_pst_3DVector, MATH_tdst_Vector *_pst_2DVector)
{
#ifdef PSX2_TARGETXXX
	_pst_Cam->f_Width = (float)GDI_gpst_CurDD->st_Device.l_Width;
	_pst_Cam->f_Height = (float)GDI_gpst_CurDD->st_Device.l_Height;
	_pst_Cam->f_CenterX = _pst_Cam->f_Width * 0.5f;
	_pst_Cam->f_CenterY = _pst_Cam->f_Height * 0.5f;
	_pst_3DVector->x = (_pst_2DVector->x - _pst_Cam->f_CenterX) * (_pst_2DVector->z / _pst_Cam->f_FactorX);
	_pst_3DVector->y = (_pst_Cam->f_CenterY - _pst_2DVector->y) * (_pst_2DVector->z / _pst_Cam->f_FactorY);
	_pst_3DVector->z = _pst_2DVector->z;
#else
	if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
	{
		_pst_3DVector->x = (_pst_2DVector->x - _pst_Cam->f_CenterX) * (_pst_2DVector->z / _pst_Cam->f_FactorX);
		_pst_3DVector->y = (_pst_Cam->f_CenterY - _pst_2DVector->y) * (_pst_2DVector->z / _pst_Cam->f_FactorY);
		_pst_3DVector->z = _pst_2DVector->z;
	}
	else
	{
		_pst_3DVector->x = (_pst_2DVector->x - _pst_Cam->f_CenterX) / _pst_Cam->f_FactorX;

		/* MODIF_Leroy 10/5/00 */
		_pst_3DVector->y = (_pst_Cam->f_CenterY - _pst_2DVector->y) / _pst_Cam->f_FactorY;

		/*
		 * _pst_3DVector->y = (_pst_2DVector->y - _pst_Cam->f_CenterY) /
		 * _pst_Cam->f_FactorY;
		 */
		_pst_3DVector->z = _pst_2DVector->z;
	}
#endif		
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_2Dto3DCamera2(CAM_tdst_Camera *_pst_Cam, MATH_tdst_Vector *_pst_3D, const MATH_tdst_Vector *_pst_2D)
{
#ifdef PSX2_TARGETXXX
	_pst_Cam->f_Width = (float)GDI_gpst_CurDD->st_Device.l_Width;
	_pst_Cam->f_Height = (float)GDI_gpst_CurDD->st_Device.l_Height;
	_pst_Cam->f_CenterX = _pst_Cam->f_Width * 0.5f;
	_pst_Cam->f_CenterY = _pst_Cam->f_Height * 0.5f;
   _pst_Cam->f_FactorX = _pst_Cam->f_CenterX * 1.0f / fNormalTan(_pst_Cam->f_FieldOfVision / 2.0f);
   _pst_Cam->f_FactorY = _pst_Cam->f_CenterY * (1.0f / fNormalTan(_pst_Cam->f_FieldOfVision / 2.0f)) * (5.0f/4.0f);
	
	_pst_3D->x = (_pst_2D->x - _pst_Cam->f_CenterX) * (_pst_2D->z / _pst_Cam->f_FactorX);
	_pst_3D->y = (_pst_Cam->f_CenterY - _pst_2D->y) * (_pst_2D->z / _pst_Cam->f_FactorY);
	_pst_3D->z = _pst_2D->z;
#else
	if(_pst_Cam->ul_Flags & CAM_Cul_Flags_Perspective)
	{
		_pst_3D->x = (_pst_2D->x + _pst_Cam->l_ViewportRealLeft - _pst_Cam->f_CenterX) * (_pst_2D->z / _pst_Cam->f_FactorX);
		_pst_3D->y = (_pst_Cam->f_CenterY - (_pst_2D->y + _pst_Cam->l_ViewportRealTop) ) * (_pst_2D->z / _pst_Cam->f_FactorY);
		_pst_3D->z = _pst_2D->z;
	}
	else
	{
		_pst_3D->x = (_pst_2D->x - _pst_Cam->f_CenterX) / _pst_Cam->f_FactorX;
		_pst_3D->y = (_pst_Cam->f_CenterY - _pst_2D->y) / _pst_Cam->f_FactorY;
		_pst_3D->z = _pst_2D->z;
	}
#endif	
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void CAM_UpdateIsometricFactor(CAM_tdst_Camera *_pst_Cam, MATH_tdst_Vector *_pst_Move, char c_UseZ )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	fNewIsoFactorZoom, fIsoFactorZoom;
#ifdef ACTIVE_EDITORS
//	char	buf[80];
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	fIsoFactorZoom = _pst_Cam->f_IsoFactor * _pst_Cam->f_IsoZoom;

    if (c_UseZ)
    {
        if(_pst_Move->z < 0)
		    fNewIsoFactorZoom = fIsoFactorZoom * 1.02f;
	    else
		    fNewIsoFactorZoom = fIsoFactorZoom / 1.02f;
    }
    else
    {
	    if(_pst_Move->z < 0)
		    fNewIsoFactorZoom = fIsoFactorZoom * 1.2f;
	    else
		    fNewIsoFactorZoom = fIsoFactorZoom / 1.2f;
    }


	_pst_Cam->f_FactorX = _pst_Cam->f_FactorX * fIsoFactorZoom / fNewIsoFactorZoom;
	_pst_Cam->f_FactorY = _pst_Cam->f_FactorY * fIsoFactorZoom / fNewIsoFactorZoom;
	_pst_Cam->f_IsoZoom = fNewIsoFactorZoom / _pst_Cam->f_IsoFactor;

#ifdef ACTIVE_EDITORS
	
//	sprintf(buf, "newIsofactorZoom %.2f, near %.2f", fNewIsoFactorZoom, _pst_Cam->f_NearPlane + (_pst_Cam->f_IsoFactor / _pst_Cam->f_IsoZoom));
//	LINK_PrintStatusMsg(buf);
	
#endif
}

