/*$T MATHdebug.c GC!1.32 05/07/99 11:00:34 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"

#include "math.h" /* our math module */
#include "stdio.h"
#include "BASe/ERRors/ERRasser.h"

#ifdef _DEBUG
#ifdef PSX2_TARGET
#define _isnan	isnanf
#endif

#ifdef _GAMECUBE
#define _isnan	isnan
#endif

void MATH_CheckMatrix(MATH_tdst_Matrix *_pst_Matrix, char *psz_Text)
{
	if(!_pst_Matrix) return;
	if
	(
		(_isnan(_pst_Matrix->Ix))
	||	(_isnan(_pst_Matrix->Iy))
	||	(_isnan(_pst_Matrix->Iz))
	||	(_isnan(_pst_Matrix->Jx))
	||	(_isnan(_pst_Matrix->Jy))
	||	(_isnan(_pst_Matrix->Jz))
	||	(_isnan(_pst_Matrix->Kx))
	||	(_isnan(_pst_Matrix->Ky))
	||	(_isnan(_pst_Matrix->Kz))
	||	(_isnan(_pst_Matrix->T.x))
	||	(_isnan(_pst_Matrix->T.y))
	||	(_isnan(_pst_Matrix->T.z))
	)
	{
		ERR_X_Warning(0, "Invalid Matrix", psz_Text);

		MATH_SetIdentityMatrix(_pst_Matrix);

	}
}

void MATH_CheckVector(MATH_tdst_Vector *_pst_Vector, char *psz_Text)
{
	if(!_pst_Vector) return;
	if((_isnan(_pst_Vector->x)) || (_isnan(_pst_Vector->y)) || (_isnan(_pst_Vector->z)))
	{
		ERR_X_Warning(0, "Invalid Vector", psz_Text);

		_pst_Vector->x = 0.0f;
		_pst_Vector->y = 0.0f;
		_pst_Vector->z = 0.0f;

	}
}

void MATH_CheckFloat(float *_pf, char *psz_Text)
{
	if(_isnan(*_pf))
	{
		ERR_X_Warning(0, "Invalid Float", psz_Text);

		*_pf = 0.0f;
		
	}
}

void MATH_CheckQuaternion(MATH_tdst_Quaternion *_pst_Quaternion, char *psz_Text)
{
	if(!_pst_Quaternion) return;
	if
	(
		(_isnan(_pst_Quaternion->x))
	||	(_isnan(_pst_Quaternion->y))
	||	(_isnan(_pst_Quaternion->z))
	||	(_isnan(_pst_Quaternion->w))
	) 
	ERR_X_Warning(0, "Invalid Quaternion", psz_Text);
}

/*void MATH_CheckNormalizedQuaternion(MATH_tdst_Quaternion *_pst_Quaternion)
{
	if(!_pst_Quaternion)
		OSReport("No Quaternion");
	else if (
		(_isnan(_pst_Quaternion->x))
	||	(_isnan(_pst_Quaternion->y))
	||	(_isnan(_pst_Quaternion->z))
	||	(_isnan(_pst_Quaternion->w))) 
		OSReport("Invalid Quaternion");
	else
	{
		float fNorm = fSqr(_pst_Quaternion->x) + fSqr(_pst_Quaternion->y) + fSqr(_pst_Quaternion->z) + fSqr(_pst_Quaternion->w);
		if ((fNorm > 1.1f) || (fNorm < 0.9f))
			OSReport("Quaternion not normalized");
	}		
}
*/


#endif // _DEBUG

