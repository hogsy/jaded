/*$T DYNSolid.c GC!1.52 02/02/00 10:51:55 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    Aim:    Handles the functions related to solid objects
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "ENGine/Sources/DYNamics/DYNinit.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "ENGine/Sources/DYNamics/DYNsolid.h"
#include "MATHs/MATH.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

/*
 ===================================================================================================
    Aim:    Create the solid structure
 ===================================================================================================
 */
DYN_tdst_Solid *DYN_pst_SolidCreate(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DYN_tdst_Solid  *pst_Solid;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Solid = (DYN_tdst_Solid *) MEM_p_Alloc(sizeof(DYN_tdst_Solid));

    /* L_memset(pst_Solid, 0, sizeof(DYN_tdst_Solid)); */
    return(pst_Solid);
}

/*
 ===================================================================================================
    Aim:    Init the solid structure
 ===================================================================================================
 */
void DYN_SolidInit
(
    DYN_tdst_Solid      *_pst_Solid,
    MATH_tdst_Vector    *_pst_w,
    MATH_tdst_Vector    *_pst_T,
    MATH_tdst_Vector    *_pst_X,
    float               _f_wLimit
)
{
	_pst_Solid->f_Factor = 0.5f;
    _pst_Solid->f_wLimit = _f_wLimit;
	MATH_CopyVector(&_pst_Solid->st_OldPosition, _pst_X);
    MATH_CopyVector(&_pst_Solid->st_w, _pst_w);
    MATH_CopyVector(&_pst_Solid->st_T, _pst_T);

}

/*
 ===================================================================================================
    Aim:    Reinit the solid structure with default values
 ===================================================================================================
 */
void DYN_SolidReinit(DYN_tdst_Solid *_pst_Solid)
{
    DYN_SolidInit(_pst_Solid, &MATH_gst_NulVector, &MATH_gst_NulVector, &MATH_gst_NulVector, Cf_Infinit);
}

/*
 ===================================================================================================
 ===================================================================================================
 */
void DYN_SolidClose(DYN_tdst_Solid *_pst_Solid)
{
    MEM_Free(_pst_Solid);
}

/*
 ===================================================================================================
    Aim:    Calculate rotation Speed, using the couple
 ===================================================================================================
 */
void DYN_CalcRotationSpeed(DYN_tdst_Dyna *_pst_Dyna, float _f_dt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    *pst_W;
    MATH_tdst_Vector    st_dW;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_W = DYN_pst_WGet(_pst_Dyna->pst_Solid);

    /* First, we calculate the angular momentum L= T*dt (where T is the torque) */

    /* We know that L = I*w, (I = Inertia tensor matrix) */

    /* (for the moment I=identity matrix * mass, so we just divide by the mass) */

    /* So a simple scale by _f_dt*invmass is enough to find the roation speed */
    MATH_ScaleVector
    (
        &st_dW,
        DYN_pst_TGet(_pst_Dyna->pst_Solid),
        _f_dt * DYN_f_GetInvMass(_pst_Dyna)
    );
    MATH_AddEqualVector(pst_W, &st_dW);
}

/*
 ===================================================================================================
    Apply the rotation Speed limit
 ===================================================================================================
 */
void DYN_ApplyRotationSpeedLimit(DYN_tdst_Dyna *_pst_Dyna)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    float               f_NormW;
    float               f_Limit;
    MATH_tdst_Vector    *pst_W;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_W = DYN_pst_WGet(_pst_Dyna->pst_Solid);
    f_NormW = MATH_f_NormVector(pst_W);

    f_Limit = DYN_f_wLimitGet(_pst_Dyna->pst_Solid);
    if(f_NormW > f_Limit)
        MATH_SetNormVector(pst_W, pst_W, f_Limit);
}

/*
 ===================================================================================================
    Apply the final rotation speed
 ===================================================================================================
 */
void DYN_ApplyRotationSpeed(DYN_tdst_Dyna *_pst_Dyna, float _f_dt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    *pst_W;
    MATH_tdst_Vector    st_Axis;
    MATH_tdst_Matrix    st_RotMat, st_Tmp;
    MATH_tdst_Matrix    *pst_M;
    MATH_tdst_Vector    *pst_Center;
    float               f_Angle;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* We make the rotation matrix using the angular speed vector pst_W */
    pst_W = DYN_pst_WGet(_pst_Dyna->pst_Solid);

    f_Angle = MATH_f_NormVector(pst_W) * _f_dt;
    if(f_Angle > 0.001f)
    {
        pst_M = DYN_pst_GetGlobalMatrix(_pst_Dyna);
        pst_Center = DYN_pst_GetPosition(_pst_Dyna);
        MATH_NormalizeVector(&st_Axis, pst_W);

        /* MATH_MakeRotationMatrix_AxisAngle(&st_RotMat, &st_Axis, f_Angle, 0, 1); */
        MATH_MakeRotationMatrix_PointAxisAngle(&st_RotMat, pst_Center, &st_Axis, f_Angle, 0, 1);

        /* We rotate the object */
        MATH_MulMatrixMatrix(&st_Tmp, pst_M, &st_RotMat);

        MATH_CopyMatrix(pst_M, &st_Tmp);

		/* Orthonormalize Matrix */
		MATH_NormalizeEqualVector(MATH_pst_GetXAxis(pst_M));
		MATH_NormalizeEqualVector(MATH_pst_GetYAxis(pst_M));
		MATH_NormalizeEqualVector(MATH_pst_GetZAxis(pst_M));
    }
}

/*
 ===================================================================================================
    Aim:    Make the rotation matrix from an angular velocity and dt
	Note:	Uses the relation dR/dt = W*R(t) where W=[(0,wz,-wy),(-wz,0,wx),(wy,-wx,0)]
 ===================================================================================================
 */
void DYN_MakeWMatrix(MATH_tdst_Matrix *_pst_WMat, MATH_tdst_Vector *_pst_W, float _f_dt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    st_Wdt;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_ScaleVector(&st_Wdt, _pst_W, _f_dt);
    MATH_SetIdentityMatrix(_pst_WMat);
    _pst_WMat->Iy = st_Wdt.z;
    _pst_WMat->Iz = -st_Wdt.y;
    _pst_WMat->Jx = -st_Wdt.z;
    _pst_WMat->Jz = st_Wdt.x;
    _pst_WMat->Kx = st_Wdt.y;
    _pst_WMat->Ky = -st_Wdt.x;
	MATH_NormalizeEqualVector(MATH_pst_GetXAxis(_pst_WMat));
	MATH_NormalizeEqualVector(MATH_pst_GetYAxis(_pst_WMat));
	MATH_NormalizeEqualVector(MATH_pst_GetZAxis(_pst_WMat));
}

/*
 ===================================================================================================
    Aim:    Apply the angular velocity to a dynamic object
 ===================================================================================================
 */
void DYN_ApplyAngularVelocity(DYN_tdst_Dyna *_pst_Dyna, float _f_dt)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector    *pst_W;
    MATH_tdst_Matrix    st_RotMat, st_Tmp;
    MATH_tdst_Matrix    *pst_M;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    /* We make the rotation matrix using the angular speed vector pst_W */
    pst_W = DYN_pst_WGet(_pst_Dyna->pst_Solid);

    DYN_MakeWMatrix(&st_RotMat, pst_W, _f_dt);
    pst_M = DYN_pst_GetGlobalMatrix(_pst_Dyna);
    MATH_MulMatrixMatrix(&st_Tmp, &st_RotMat,pst_M);
    MATH_CopyMatrix(pst_M, &st_Tmp);

	MATH_Orthonormalize(pst_M);
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
