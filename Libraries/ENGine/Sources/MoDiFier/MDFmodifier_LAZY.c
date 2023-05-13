/*$T MDFmodifier_Lazy.c GC! 1.100 08/28/01 17:06:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"

#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJInit.h"
#include "ENGine/Sources/OBJects/OBJConst.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "GDInterface/GDIrequest.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_Lazy.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTlinear.h"

#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "GraphicDK/Sources/GRObject/GROrender.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGfat.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern ULONG		ENG_gp_CameraCutHasBeenDetected;
static GAO_tdst_ModifierLazy *p_FirsToUnapply;
/*$4
 ***********************************************************************************************************************
    Lazy modifier:: Modifier apply Lazy on world
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Lazy_FirstInit(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	static ULONG	bFirst = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(bFirst) return;
	bFirst = 1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLazy_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierLazy	*p_Lazy;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Lazy_FirstInit();
#if defined(_XBOX) || defined(_XENON)
	_pst_Mod->p_Data = MEM_p_AllocAlign(sizeof(GAO_tdst_ModifierLazy),16);
#else
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierLazy));
#endif
	p_Lazy = (GAO_tdst_ModifierLazy *) _pst_Mod->p_Data;
	if(p_Data == NULL)
	{
		/* First init */
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
		L_memset(_pst_Mod->p_Data, 0, sizeof(GAO_tdst_ModifierLazy));
		p_Lazy->LazyFactor = 0.9f;
		p_Lazy->ulFlags = 0;
		MATH_InitVector(&p_Lazy->stMinBV , -0.1f, -0.1f, -0.1f);
		MATH_InitVector(&p_Lazy->stMaxBV ,  0.1f,  0.1f,  0.1f);

	}
	else
	{
		/* duplicate */
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierLazy));
	}
	GAO_ModifierLazy_Reinit(_pst_Mod);

#ifdef ACTIVE_EDITORS
	p_Lazy->ulCodeKey = 0xC0DE2001;
#endif

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLazy_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	if(_pst_Mod->p_Data)
	{
#if defined(_XBOX) || defined(_XENON)
		MEM_FreeAlign(_pst_Mod->p_Data);
#else
		MEM_Free(_pst_Mod->p_Data);
#endif
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLazy_Inherit(GAO_tdst_ModifierLazy *p_Lazy , OBJ_tdst_GameObject *pst_Father , OBJ_tdst_GameObject *pst_Son)
{
	if(pst_Father->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) 
	{
		OBJ_tdst_Hierarchy	*pst_Hie;
		OBJ_tdst_Hierarchy	*pst_HieS;
		pst_Hie = pst_Father->pst_Base->pst_Hierarchy;
		pst_HieS= pst_Son->pst_Base->pst_Hierarchy;
		if (p_Lazy ->ulFlags & GAO_FLG_Lazy_Inherit_Trans)
		{
			pst_HieS->st_LocalMatrix.T = pst_Hie->st_LocalMatrix.T;
		}
		if (p_Lazy ->ulFlags & GAO_FLG_Lazy_Inherit_Rotation)
		{
			*MATH_pst_GetXAxis(&pst_HieS->st_LocalMatrix) = *MATH_pst_GetXAxis(&pst_Hie->st_LocalMatrix);
			*MATH_pst_GetYAxis(&pst_HieS->st_LocalMatrix) = *MATH_pst_GetYAxis(&pst_Hie->st_LocalMatrix);
			*MATH_pst_GetZAxis(&pst_HieS->st_LocalMatrix) = *MATH_pst_GetZAxis(&pst_Hie->st_LocalMatrix);
		}
		if (p_Lazy ->ulFlags & GAO_FLG_Lazy_Inherit_Scale)
		{
			if (MATH_b_TestScaleType(&pst_Hie->st_LocalMatrix))
			{
				MATH_SetScaleType(&pst_HieS->st_LocalMatrix);
				pst_HieS->st_LocalMatrix.Sx = pst_Hie->st_LocalMatrix.Sx;
				pst_HieS->st_LocalMatrix.Sy = pst_Hie->st_LocalMatrix.Sy;
				pst_HieS->st_LocalMatrix.Sz = pst_Hie->st_LocalMatrix.Sz;
			} else 
				MATH_ClearScaleType(&pst_HieS->st_LocalMatrix);
		}
	} else
	{
		OBJ_tdst_Hierarchy	*pst_HieS;
		pst_HieS= pst_Son->pst_Base->pst_Hierarchy;
		if (p_Lazy ->ulFlags & GAO_FLG_Lazy_Inherit_Rotation)
		{
			*MATH_pst_GetXAxis(&pst_HieS->st_LocalMatrix) = *MATH_pst_GetXAxis(pst_Father->pst_GlobalMatrix);
			*MATH_pst_GetYAxis(&pst_HieS->st_LocalMatrix) = *MATH_pst_GetYAxis(pst_Father->pst_GlobalMatrix);
			*MATH_pst_GetZAxis(&pst_HieS->st_LocalMatrix) = *MATH_pst_GetZAxis(pst_Father->pst_GlobalMatrix);
		}
		if (p_Lazy ->ulFlags & GAO_FLG_Lazy_Inherit_Scale)
		{
			if (MATH_b_TestScaleType(pst_Father->pst_GlobalMatrix))
			{
				MATH_SetScaleType(&pst_HieS->st_LocalMatrix);
				pst_HieS->st_LocalMatrix.Sx = pst_Father->pst_GlobalMatrix->Sx;
				pst_HieS->st_LocalMatrix.Sy = pst_Father->pst_GlobalMatrix->Sy;
				pst_HieS->st_LocalMatrix.Sz = pst_Father->pst_GlobalMatrix->Sz;
			} else 
				MATH_ClearScaleType(&pst_HieS->st_LocalMatrix);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLazy_Compute_SemiMecanical_Model(GAO_tdst_ModifierLazy *p_Lazy , MATH_tdst_Matrix *p_NewGlobalMatrix , MATH_tdst_Matrix *p_NewFatherGlobalMatrix)
{
	MATH_tdst_Vector Trans;
	MATH_tdst_Vector Trans2;
	MATH_tdst_Vector OriginalPos;
	MATH_tdst_Matrix MGlobalSavedLocalMatrix;
	float D1,D2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	if (p_NewFatherGlobalMatrix == NULL)	return;
	//TIM_gf_dt

	/* Compute mecanic begin */
	OriginalPos = p_Lazy->stCurrentMatrix.T;
	MATH_MulMatrixMatrix(&MGlobalSavedLocalMatrix,&p_Lazy->stSaveMatrix,p_NewFatherGlobalMatrix);
	/* Forces */
	MATH_SubVector(&Trans , &MGlobalSavedLocalMatrix.T , &p_Lazy->stCurrentMatrix.T);
	MATH_AddScaleVector(&p_Lazy->stCurrentSpeed , &p_Lazy->stCurrentSpeed , &Trans , p_Lazy->LazyFactor );

/*	MATH_InitVector(&GrvityT,0,0,p_Lazy->Gravity);
	MATH_TransformVector(&GrvityT , &p_Lazy->stCurrentMatrix , &GrvityT);
	MATH_AddVector(&p_Lazy->stCurrentSpeed,&p_Lazy->stCurrentSpeed,&GrvityT);//*/
	p_Lazy->stCurrentSpeed.z += p_Lazy->Gravity;//*/

	MATH_ScaleEqualVector(&p_Lazy->stCurrentSpeed , p_Lazy->Friction);

	MATH_AddScaleVector(&p_Lazy->stCurrentMatrix.T , &p_Lazy->stCurrentMatrix.T , &p_Lazy->stCurrentSpeed , TIM_gf_dt);
	/* Compute mecanic end */

	/* Compute BV Constraint Begin */
	if (p_Lazy ->ulFlags & GAO_FLG_Lazy_UseBV)
	{
		float DX,DY,DZ;
		MATH_tdst_Vector Local;
		MATH_SubVector(&Local , &p_Lazy->stCurrentMatrix.T , &MGlobalSavedLocalMatrix.T);
		DX = MATH_f_DotProduct(&Local , MATH_pst_GetXAxis(&MGlobalSavedLocalMatrix));
		DY = MATH_f_DotProduct(&Local , MATH_pst_GetYAxis(&MGlobalSavedLocalMatrix));
		DZ = MATH_f_DotProduct(&Local , MATH_pst_GetZAxis(&MGlobalSavedLocalMatrix));
		if (DX < p_Lazy->stMinBV.x) DX = p_Lazy->stMinBV.x;
		if (DY < p_Lazy->stMinBV.y) DY = p_Lazy->stMinBV.y;
		if (DZ < p_Lazy->stMinBV.z) DZ = p_Lazy->stMinBV.z;
		if (DX > p_Lazy->stMaxBV.x) DX = p_Lazy->stMaxBV.x;
		if (DY > p_Lazy->stMaxBV.y) DY = p_Lazy->stMaxBV.y;
		if (DZ > p_Lazy->stMaxBV.z) DZ = p_Lazy->stMaxBV.z;
		MATH_ScaleVector(&Local , MATH_pst_GetXAxis(&MGlobalSavedLocalMatrix) , DX);
		MATH_AddScaleVector(&Local , &Local , MATH_pst_GetYAxis(&MGlobalSavedLocalMatrix) , DY);
		MATH_AddScaleVector(&Local , &Local , MATH_pst_GetZAxis(&MGlobalSavedLocalMatrix) , DZ);
		MATH_AddVector(&p_Lazy->stCurrentMatrix.T , &Local , &MGlobalSavedLocalMatrix.T);
	}
	/* Compute BV Constraint end */
	
	/* Compute matrix + normalization to father begin */
	MATH_SubVector(&Trans2 , &p_NewGlobalMatrix->T , &p_NewFatherGlobalMatrix->T);
	MATH_SubVector(&Trans , &p_Lazy->stCurrentMatrix.T , &p_NewFatherGlobalMatrix->T);
	D1 = MATH_f_NormVector(&Trans2);
	D2 = MATH_f_NormVector(&Trans);
	if (D2 != 0.0f)
	{
		MATH_AddScaleVector(&p_Lazy->stCurrentMatrix.T , &p_NewFatherGlobalMatrix->T  , &Trans  , D1 / D2);
		if (D1 != 0.0f)
		{
			MATH_tdst_Quaternion Quat;
			MATH_tdst_Matrix MDst,M2;
			MATH_ScaleEqualVector(&Trans2 , 1.0f / D1);
			MATH_ScaleEqualVector(&Trans , 1.0f / D2);
			MATH_AddEqualVector(&Trans , &Trans2 );
			MATH_NormalizeVector(&Trans , &Trans );
			MATH_CrossProduct((MATH_tdst_Vector *)&Quat , &Trans2 , &Trans);
			Quat.w = MATH_f_DotProduct(&Trans2 , &Trans);
			MATH_ConvertQuaternionToMatrix(&MDst, &Quat);
			MATH_CopyMatrix(&M2,p_NewGlobalMatrix);
			MATH_Mul33MatrixMatrix(p_NewGlobalMatrix, &M2, &MDst, 1);
		}
	}
	/* Compute matrix + normalization to father end */

	/* Deduct Speed */
	MATH_SubVector(&p_Lazy->stCurrentSpeed , &p_Lazy->stCurrentMatrix.T , &OriginalPos);
	MATH_ScaleEqualVector(&p_Lazy->stCurrentSpeed , 1.0f / TIM_gf_dt);
	p_NewGlobalMatrix->T = p_Lazy->stCurrentMatrix.T;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLazy_Compute(GAO_tdst_ModifierLazy *p_Lazy , MATH_tdst_Matrix *p_NewGlobalMatrix , MATH_tdst_Matrix *p_NewFatherGlobalMatrix)
{
	float FactorCorrected,FactorRest,inc;
	ULONG ulFlags;
#define Correct_DT (1.0f/300.0f)
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* This modifier work only if a hierarchy is present */
	if (p_Lazy ->ulFlags & GAO_FLG_Lazy_UseSemiMecanicalModel)
	{
		GAO_ModifierLazy_Compute_SemiMecanical_Model(p_Lazy , p_NewGlobalMatrix , p_NewFatherGlobalMatrix);
		return;
	}
	FactorCorrected = 1.0f;
	FactorRest = 1.0f - p_Lazy->LazyFactor;
	if (FactorRest < 0.0f) FactorRest = 0.0f;
	FactorRest *= FactorRest;
	FactorRest = 1.0f - FactorRest ;
	inc = 0.0f;
	
	ulFlags = p_Lazy ->ulFlags;
	if (p_NewFatherGlobalMatrix == NULL)	ulFlags &= ~GAO_FLG_Lazy_Hierarchy;
	if (ulFlags & GAO_FLG_Lazy_Hierarchy)	ulFlags &= ~GAO_FLG_Lazy_Rotation;

	while (inc < TIM_gf_dt)
	{
		FactorCorrected *= FactorRest;
		inc += Correct_DT;
	}
	FactorCorrected = 1.0f - FactorCorrected;

	if (ulFlags & (GAO_FLG_Lazy_Trans|GAO_FLG_Lazy_Hierarchy))
	{
		MATH_tdst_Vector Trans;
		MATH_SubVector(&Trans , &p_NewGlobalMatrix->T , &p_Lazy->stCurrentMatrix.T);
		MATH_AddScaleVector(&p_Lazy->stCurrentMatrix.T , &p_Lazy->stCurrentMatrix.T  , &Trans  , FactorCorrected);
		if (ulFlags & GAO_FLG_Lazy_Hierarchy)
		{
			MATH_tdst_Vector Trans2;
			float D1,D2;
			p_Lazy->stCurrentMatrix.T.z += p_Lazy->Gravity * FactorCorrected;
			/* Compute BV Constraint */
			if (ulFlags & GAO_FLG_Lazy_UseBV)
			{
				MATH_tdst_Matrix MGlobalSavedLocalMatrix;
				float DX,DY,DZ;
				MATH_tdst_Vector Local;
				MATH_MulMatrixMatrix(&MGlobalSavedLocalMatrix,&p_Lazy->stSaveMatrix,p_NewFatherGlobalMatrix);
				MATH_SubVector(&Local , &p_Lazy->stCurrentMatrix.T , &MGlobalSavedLocalMatrix.T);
				DX = MATH_f_DotProduct(&Local , MATH_pst_GetXAxis(&MGlobalSavedLocalMatrix));
				DY = MATH_f_DotProduct(&Local , MATH_pst_GetYAxis(&MGlobalSavedLocalMatrix));
				DZ = MATH_f_DotProduct(&Local , MATH_pst_GetZAxis(&MGlobalSavedLocalMatrix));
				if (DX < p_Lazy->stMinBV.x) DX = p_Lazy->stMinBV.x;
				if (DY < p_Lazy->stMinBV.y) DY = p_Lazy->stMinBV.y;
				if (DZ < p_Lazy->stMinBV.z) DZ = p_Lazy->stMinBV.z;
				if (DX > p_Lazy->stMaxBV.x) DX = p_Lazy->stMaxBV.x;
				if (DY > p_Lazy->stMaxBV.y) DY = p_Lazy->stMaxBV.y;
				if (DZ > p_Lazy->stMaxBV.z) DZ = p_Lazy->stMaxBV.z;
				MATH_ScaleVector(&Local , MATH_pst_GetXAxis(&MGlobalSavedLocalMatrix) , DX);
				MATH_AddScaleVector(&Local , &Local , MATH_pst_GetYAxis(&MGlobalSavedLocalMatrix) , DY);
				MATH_AddScaleVector(&Local , &Local , MATH_pst_GetZAxis(&MGlobalSavedLocalMatrix) , DZ);
				MATH_AddVector(&p_Lazy->stCurrentMatrix.T , &Local , &MGlobalSavedLocalMatrix.T);
			}
			

			MATH_SubVector(&Trans2 , &p_NewGlobalMatrix->T , &p_NewFatherGlobalMatrix->T);
			MATH_SubVector(&Trans , &p_Lazy->stCurrentMatrix.T , &p_NewFatherGlobalMatrix->T);
			D1 = MATH_f_NormVector(&Trans2);
			D2 = MATH_f_NormVector(&Trans);
			if (D2 != 0.0f)
			{
				MATH_AddScaleVector(&p_Lazy->stCurrentMatrix.T , &p_NewFatherGlobalMatrix->T  , &Trans  , D1 / D2);
				/* Reorient matrixt according father Move */
				if (D1 != 0.0f)
				{
					MATH_tdst_Quaternion Quat;
					MATH_tdst_Matrix MDst,M2;
					MATH_ScaleEqualVector(&Trans2 , 1.0f / D1);
					MATH_ScaleEqualVector(&Trans , 1.0f / D2);
					MATH_AddEqualVector(&Trans , &Trans2 );
					MATH_NormalizeVector(&Trans , &Trans );
					MATH_CrossProduct((MATH_tdst_Vector *)&Quat , &Trans2 , &Trans);
					Quat.w = MATH_f_DotProduct(&Trans2 , &Trans);
					//MATH_NormalizeQuaternion(&Quat, &Quat);//not needed
					MATH_ConvertQuaternionToMatrix(&MDst, &Quat);
					MATH_CopyMatrix(&M2,p_NewGlobalMatrix);
					MATH_Mul33MatrixMatrix(p_NewGlobalMatrix, &M2, &MDst, 1);
				}
			}
		}
		p_NewGlobalMatrix->T = p_Lazy->stCurrentMatrix.T;
	}
	if (ulFlags & GAO_FLG_Lazy_Rotation)
	{
		MATH_tdst_Vector Trans;
		MATH_SubVector(&Trans , MATH_pst_GetXAxis(p_NewGlobalMatrix) , MATH_pst_GetXAxis(&p_Lazy->stCurrentMatrix));
		MATH_AddScaleVector(MATH_pst_GetXAxis(&p_Lazy->stCurrentMatrix) , MATH_pst_GetXAxis(&p_Lazy->stCurrentMatrix)  , &Trans  , FactorCorrected);
		MATH_SubVector(&Trans , MATH_pst_GetYAxis(p_NewGlobalMatrix) , MATH_pst_GetYAxis(&p_Lazy->stCurrentMatrix));
		MATH_AddScaleVector(MATH_pst_GetYAxis(&p_Lazy->stCurrentMatrix) , MATH_pst_GetYAxis(&p_Lazy->stCurrentMatrix)  , &Trans  , FactorCorrected);
		MATH_SubVector(&Trans , MATH_pst_GetZAxis(p_NewGlobalMatrix) , MATH_pst_GetZAxis(&p_Lazy->stCurrentMatrix));
		MATH_AddScaleVector(MATH_pst_GetZAxis(&p_Lazy->stCurrentMatrix) , MATH_pst_GetZAxis(&p_Lazy->stCurrentMatrix)  , &Trans  , FactorCorrected);
		MATH_Orthonormalize(&p_Lazy->stCurrentMatrix);
		*MATH_pst_GetXAxis(p_NewGlobalMatrix) = *MATH_pst_GetXAxis(&p_Lazy->stCurrentMatrix);
		*MATH_pst_GetYAxis(p_NewGlobalMatrix) = *MATH_pst_GetYAxis(&p_Lazy->stCurrentMatrix);
		*MATH_pst_GetZAxis(p_NewGlobalMatrix) = *MATH_pst_GetZAxis(&p_Lazy->stCurrentMatrix);
	}
	if (ulFlags & GAO_FLG_Lazy_Scale)
	{
		MATH_tdst_Vector Trans;
		MATH_tdst_Vector Scale0, Scale1;
		Scale0.x = Scale0.y = Scale0.z = 1.0f;
		Scale1.x = Scale1.y = Scale1.z = 1.0f;
		if (MATH_b_TestScaleType(p_NewGlobalMatrix))
		{
			Scale0.x = p_NewGlobalMatrix->Sx;
			Scale0.y = p_NewGlobalMatrix->Sy;
			Scale0.z = p_NewGlobalMatrix->Sz;
			if (MATH_b_TestScaleType(&p_Lazy->stCurrentMatrix))
			{
				Scale1.x = p_Lazy->stCurrentMatrix.Sx;
				Scale1.y = p_Lazy->stCurrentMatrix.Sy;
				Scale1.z = p_Lazy->stCurrentMatrix.Sz;
			} else 
				MATH_SetScaleType(&p_Lazy->stCurrentMatrix);

			MATH_SubVector(&Trans , &Scale0 , &Scale1);
			MATH_AddScaleVector(&Scale1 , &Scale1  , &Trans  , FactorCorrected);

			p_Lazy->stCurrentMatrix.Sx = p_NewGlobalMatrix->Sx = Scale1.x;
			p_Lazy->stCurrentMatrix.Sy = p_NewGlobalMatrix->Sy = Scale1.y;
			p_Lazy->stCurrentMatrix.Sz = p_NewGlobalMatrix->Sz = Scale1.z;
		} 
	}

		
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
static float UpperFactor;
static float UpperFriction;
static float UpperGravity;
static ULONG UpperFlag;
static MATH_tdst_Vector UpperMin;
static MATH_tdst_Vector UpperMax;
void GAO_ModifierLazy_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierLazy *p_Lazy;
	MATH_tdst_Matrix Mi;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//#ifdef USE_DOUBLE_RENDERING	
	if (ENG_gp_CameraCutHasBeenDetected) // CameraCut & cinematic
		GAO_ModifierLazy_Reinit(_pst_Mod);
//#endif	
	p_Lazy = (GAO_tdst_ModifierLazy*)_pst_Mod->p_Data;
	p_Lazy -> SaveFriction  = UpperFriction	= p_Lazy -> Friction;
	p_Lazy -> SaveGravity	= UpperGravity	= p_Lazy ->Gravity;
	p_Lazy -> SaveFactor	= UpperFactor	= p_Lazy ->LazyFactor;
	p_Lazy -> SaveFlags		= UpperFlag		= p_Lazy ->ulFlags ;
	p_Lazy -> SavedstMaxBV	= UpperMax		= p_Lazy ->stMaxBV;
	p_Lazy -> SavedstMinBV	= UpperMin		= p_Lazy ->stMinBV;
	if (p_Lazy ->ulFlags & GAO_FLG_Lazy_IsComputed) return;
	if(_pst_Mod->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		OBJ_tdst_GameObject *pst_Father;
		OBJ_tdst_Hierarchy	*pst_Hie;
		pst_Hie = _pst_Mod->pst_GO->pst_Base->pst_Hierarchy;
		pst_Father = pst_Hie->pst_Father;

		if (pst_Father)
		{
			if ((pst_Father ->ul_IdentityFlags & OBJ_C_IdentityFlag_ExtendedObject) && (pst_Father ->pst_Extended))
			{
				MDF_tdst_Modifier		*pst_Modifier;
				pst_Modifier = pst_Father->pst_Extended->pst_Modifiers;
				while(pst_Modifier)
				{
					if	(	(pst_Modifier->i->ul_Type == MDF_C_Modifier_Lazy) &&
							(!(pst_Modifier->ul_Flags & (MDF_C_Modifier_Inactive|MDF_C_Modifier_NoApply))) &&
							(pst_Modifier->ul_Flags & MDF_C_Modifier_ApplyGao))
					{
						GAO_ModifierLazy_Apply(pst_Modifier, NULL);
					}
					pst_Modifier = pst_Modifier->pst_Next;
				}
			}
			if (p_Lazy->ulFlags & GAO_FLG_Lazy_Inherit_Flags)
					p_Lazy->ulFlags = UpperFlag;
			if (p_Lazy->ulFlags & GAO_FLG_Lazy_Inherit_Factor)
			{
					p_Lazy->LazyFactor = UpperFactor;
					p_Lazy->Gravity = UpperGravity;
					p_Lazy -> Friction  = UpperFriction;
			}
			if (p_Lazy->ulFlags & GAO_FLG_Lazy_Inherit_BV)
			{
					p_Lazy -> stMaxBV	= UpperMax;
					p_Lazy -> stMinBV	= UpperMin;
			}

			if (p_Lazy ->ulFlags & GAO_FLG_Lazy_Inherit_MASK) GAO_ModifierLazy_Inherit(p_Lazy , pst_Father , _pst_Mod->pst_GO);
			OBJ_ComputeGlobalWhenHie(_pst_Mod->pst_GO);

			MATH_CopyMatrix(&p_Lazy->stSaveMatrix , &pst_Hie->st_LocalMatrix );

#ifdef ACTIVE_EDITORS
			if (ENG_gb_EngineRunning) 
#endif
				GAO_ModifierLazy_Compute(p_Lazy , _pst_Mod->pst_GO->pst_GlobalMatrix , pst_Father->pst_GlobalMatrix );

			MATH_InvertMatrix(&Mi, OBJ_pst_GetAbsoluteMatrix(pst_Father));
			MATH_MulMatrixMatrix(&pst_Hie->st_LocalMatrix, _pst_Mod->pst_GO->pst_GlobalMatrix , &Mi);

		}
	} else
	{
		MATH_CopyMatrix(&p_Lazy->stSaveMatrix, _pst_Mod->pst_GO->pst_GlobalMatrix);
#ifdef ACTIVE_EDITORS
		if (ENG_gb_EngineRunning) 
#endif
			GAO_ModifierLazy_Compute(p_Lazy , _pst_Mod->pst_GO->pst_GlobalMatrix , NULL);
	}

	p_Lazy ->ulFlags |= GAO_FLG_Lazy_IsComputed;

	p_Lazy ->pst_GO = _pst_Mod->pst_GO;
	p_Lazy ->p_NextModifierLazy = p_FirsToUnapply;
	p_FirsToUnapply = p_Lazy ;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLazy_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
void GAO_ModifierLazy_DrawInfo(GAO_tdst_ModifierLazy *p_LazyLst)
{
	extern void OGL_DrawLineEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawLineEx *_pst_Data);
	GDI_tdst_Request_DrawLineEx stDLE;
	ULONG Counter,Counter2;
	MATH_tdst_Vector A[8];
	MATH_tdst_Vector A2,B2;
	for (Counter = 0 ; Counter < 4 ; Counter ++)
	{
		MATH_tdst_Vector *pMinMax;
		MATH_AddScaleVector(&A[Counter * 2 + 0],&p_LazyLst->stSaveMatrix.T,MATH_pst_GetXAxis(&p_LazyLst->stSaveMatrix),p_LazyLst->stMaxBV.x);
		MATH_AddScaleVector(&A[Counter * 2 + 1],&p_LazyLst->stSaveMatrix.T,MATH_pst_GetXAxis(&p_LazyLst->stSaveMatrix),p_LazyLst->stMinBV.x);
		pMinMax = &p_LazyLst->stMinBV;
		if (Counter & 1) pMinMax = &p_LazyLst->stMaxBV;
		MATH_AddScaleVector(&A[Counter * 2 + 0],&A[Counter * 2 + 0],MATH_pst_GetYAxis(&p_LazyLst->stSaveMatrix),pMinMax->y);
		MATH_AddScaleVector(&A[Counter * 2 + 1],&A[Counter * 2 + 1],MATH_pst_GetYAxis(&p_LazyLst->stSaveMatrix),pMinMax->y);
		pMinMax = &p_LazyLst->stMinBV;
		if (Counter & 2) pMinMax = &p_LazyLst->stMaxBV;
		MATH_AddScaleVector(&A[Counter * 2 + 0],&A[Counter * 2 + 0],MATH_pst_GetZAxis(&p_LazyLst->stSaveMatrix),pMinMax->z);
		MATH_AddScaleVector(&A[Counter * 2 + 1],&A[Counter * 2 + 1],MATH_pst_GetZAxis(&p_LazyLst->stSaveMatrix),pMinMax->z);
	}


	for (Counter = 0 ; Counter < 8 - 1; Counter ++)
		for (Counter2 = Counter + 1; Counter2 < 8 ; Counter2 ++)
		{
			ULONG Test;
			Test = Counter ^ Counter2;
			if (!((Test != 1) && (Test != 2) && (Test != 4)))
			{
				stDLE.A = &A[Counter];
				stDLE.B = &A[Counter2];
				stDLE.f_Width = 1.0f;
				stDLE.ul_Color = 0xffffff;
				if(p_LazyLst->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
				{
					OBJ_tdst_Hierarchy	*pst_Hie;
					pst_Hie = p_LazyLst->pst_GO->pst_Base->pst_Hierarchy;
					MATH_TransformVertex(&A2,pst_Hie->pst_Father->pst_GlobalMatrix,stDLE.A);
					MATH_TransformVertex(&B2,pst_Hie->pst_Father->pst_GlobalMatrix,stDLE.B);
					stDLE.A = &A2;
					stDLE.B = &B2;
				}
				OGL_DrawLineEx(GDI_gpst_CurDD, &stDLE);
			}
	}
}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLazy_UnApply_Local(GAO_tdst_ModifierLazy *p_LazyLst)
{
#ifdef ACTIVE_EDITORS
	if (p_LazyLst ->ulFlags & GAO_FLG_Lazy_DrawInfo)
		GAO_ModifierLazy_DrawInfo(p_LazyLst);
#endif
	p_LazyLst -> Gravity		= p_LazyLst ->SaveGravity;
	p_LazyLst -> Friction		= p_LazyLst ->SaveFriction;
	p_LazyLst -> LazyFactor		= p_LazyLst ->SaveFactor;
	p_LazyLst -> ulFlags		= p_LazyLst ->SaveFlags ;
	p_LazyLst -> stMaxBV		= p_LazyLst ->SavedstMaxBV;
	p_LazyLst -> stMinBV		= p_LazyLst ->SavedstMinBV;
	p_LazyLst -> ulFlags &= ~GAO_FLG_Lazy_IsComputed;
	if(p_LazyLst->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		OBJ_tdst_Hierarchy	*pst_Hie;
		pst_Hie = p_LazyLst->pst_GO->pst_Base->pst_Hierarchy;
		MATH_CopyMatrix(&pst_Hie->st_LocalMatrix , &p_LazyLst->stSaveMatrix);
	} else
	{
		MATH_CopyMatrix(p_LazyLst->pst_GO->pst_GlobalMatrix , &p_LazyLst->stSaveMatrix);
	}
}
 
void GAO_ModifierLazy_UnapplyAll()
{
	GAO_tdst_ModifierLazy *p_LazyLst;
	GDI_gpst_CurDD->st_GDI.pfnv_SetProjectionMatrix(&GDI_gpst_CurDD->st_Camera);
	CAM_Inverse(&GDI_gpst_CurDD->st_Camera);
	GDI_SetViewMatrix((*GDI_gpst_CurDD), &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
//	SOFT_MatrixStack_Reset(&GDI_gpst_CurDD->st_MatrixStack, &GDI_gpst_CurDD->st_Camera.st_InverseMatrix);
	p_LazyLst = p_FirsToUnapply;
	while (p_LazyLst)
	{
		GAO_ModifierLazy_UnApply_Local(p_LazyLst);
		p_LazyLst = p_LazyLst->p_NextModifierLazy;
	}
	p_FirsToUnapply = NULL;

}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLazy_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierLazy *p_Lazy;
	OBJ_tdst_GameObject *pst_Father;
	OBJ_tdst_Hierarchy	*pst_Hie;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	/* This modifier work only if a hierarchy is present */
	p_Lazy = (GAO_tdst_ModifierLazy*)_pst_Mod->p_Data;
	if(_pst_Mod->pst_GO)
	{
		if(_pst_Mod->pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		{
			pst_Hie = _pst_Mod->pst_GO->pst_Base->pst_Hierarchy;
			pst_Father = pst_Hie->pst_Father;

			if (pst_Father)
			{
				OBJ_ComputeGlobalWhenHie(_pst_Mod->pst_GO);
				MATH_CopyMatrix(&p_Lazy->stCurrentMatrix, _pst_Mod->pst_GO->pst_GlobalMatrix);
			}
		} else
		{
			MATH_CopyMatrix(&p_Lazy->stCurrentMatrix, _pst_Mod->pst_GO->pst_GlobalMatrix);
		}
	}
}

#ifdef USE_DOUBLE_RENDERING	
void 	GAO_ModifierLazy_Interpolate
		(
			struct MDF_tdst_Modifier_ * _pst_Mod, 
			u_int mode , 
			float fInterpolatedValue
		)
{
/*	GAO_tdst_ModifierLazy *p_Lazy;
	if (ENG_gp_CameraCutHasBeenDetected) // CameraCut & cinematic
	{
		GAO_ModifierLazy_Reinit(_pst_Mod);
			p_Lazy = _pst_Mod->p_Data;
			p_Lazy->ulFlags |= GAO_FLG_Lazy_MustReinit;
	}*/
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GAO_ul_ModifierLazy_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierLazy	*p_Lazy;
	ULONG					ulVersion;
	char *					_pc_BufferSave;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Lazy = (GAO_tdst_ModifierLazy*)_pst_Mod->p_Data;
	_pc_BufferSave = _pc_Buffer;

	/* size */
	LOA_ReadLong_Ed(&_pc_Buffer, NULL); // skip size

	/* version */
	ulVersion = LOA_ReadULong(&_pc_Buffer);

	p_Lazy->LazyFactor = LOA_ReadFloat(&_pc_Buffer);
	p_Lazy->ulFlags = LOA_ReadULong(&_pc_Buffer);
	if (ulVersion > 0)
	{
		p_Lazy->stMinBV.x = LOA_ReadFloat(&_pc_Buffer);
		p_Lazy->stMinBV.y = LOA_ReadFloat(&_pc_Buffer);
		p_Lazy->stMinBV.z = LOA_ReadFloat(&_pc_Buffer);
		p_Lazy->stMaxBV.x = LOA_ReadFloat(&_pc_Buffer);
		p_Lazy->stMaxBV.y = LOA_ReadFloat(&_pc_Buffer);
		p_Lazy->stMaxBV.z = LOA_ReadFloat(&_pc_Buffer);
		if (ulVersion > 1)
		{
			p_Lazy->Gravity = LOA_ReadFloat(&_pc_Buffer);
			if (ulVersion > 2)
			{
				p_Lazy->Friction = LOA_ReadFloat(&_pc_Buffer);
			}
		}

	} else
	{
		MATH_InitVector(&p_Lazy->stMinBV , -1.0f, -1.0f, -1.0f);
		MATH_InitVector(&p_Lazy->stMaxBV ,  1.0f,  1.0f,  1.0f);
	}

#ifdef ACTIVE_EDITORS
	p_Lazy->ulCodeKey = 0xC0DE2001;
#endif

	return _pc_Buffer - _pc_BufferSave;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GAO_ModifierLazy_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierLazy *p_Lazy;
	ULONG					ulSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_Lazy = (GAO_tdst_ModifierLazy*)_pst_Mod->p_Data;

	/* Save Size */
	ulSize = 44;
	SAV_Buffer(&ulSize, 4);

	/* Save version */
	ulSize = 3;
	SAV_Buffer(&ulSize, 4);
	SAV_Buffer(&p_Lazy->LazyFactor, 4);
	SAV_Buffer(&p_Lazy->ulFlags, 4);
	SAV_Buffer(&p_Lazy->stMinBV.x, 4);
	SAV_Buffer(&p_Lazy->stMinBV.y, 4);
	SAV_Buffer(&p_Lazy->stMinBV.z, 4);
	SAV_Buffer(&p_Lazy->stMaxBV.x, 4);
	SAV_Buffer(&p_Lazy->stMaxBV.y, 4);
	SAV_Buffer(&p_Lazy->stMaxBV.z, 4);
	SAV_Buffer(&p_Lazy->Gravity, 4);
	SAV_Buffer(&p_Lazy->Friction, 4);
	
}

#endif

/**********************************************************************************************/
/**********************************************************************************************/
/***********   Bones refine modifier	    ***************************************************/
/**********************************************************************************************/
/**********************************************************************************************/
#ifdef ACTIVE_EDITORS
#define SAV_LOAD_Buffer(ValueA , TypeC)	ulSize += sizeof(ValueA); if (Flags & 1) SAV_Buffer(&ValueA , sizeof(ValueA)); if (Flags & 2) ValueA = LOA_Read##TypeC(_pc_ReadBuffer);
#else
#define SAV_LOAD_Buffer(ValueA , TypeC)	ulSize += sizeof(ValueA); if (Flags & 2) ValueA = LOA_Read##TypeC(_pc_ReadBuffer);
#endif



#ifdef USE_DOUBLE_RENDERING	
void GAO_ModifierBoneRefineInterpolate(struct MDF_tdst_Modifier_ * p_Mod, u32 mode , float fInterpolatedValue) {};
#endif
void GAO_ModifierBoneRefineCreate(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierBoneRefine));

	if (p_Data == NULL)
	{
		/* First init */
		L_memset(_pst_Mod->p_Data , 0 , sizeof(GAO_tdst_ModifierBoneRefine));
		_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
	}
	else
	{
		GAO_tdst_ModifierBoneRefine *p_FOGDY;
		_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
		p_FOGDY = (GAO_tdst_ModifierBoneRefine *)_pst_Mod->p_Data;
		L_memcpy( _pst_Mod->p_Data, p_Data, sizeof (GAO_tdst_ModifierBoneRefine) );
	}
	((GAO_tdst_ModifierBoneRefine *)_pst_Mod->p_Data) ->fInterpolValue = 0.5f;
#ifdef ACTIVE_EDITORS
	((GAO_tdst_ModifierBoneRefine *)_pst_Mod->p_Data) ->ulCodeKey = 0xC0DE2002;
#endif
}
void GAO_ModifierBoneRefineDestroy(MDF_tdst_Modifier *_pst_Mod)
{
	if (_pst_Mod->p_Data)
	{
		MEM_Free(_pst_Mod->p_Data);
	}
}
void GAO_ModifierBoneRefineApply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj) 
{
	OBJ_tdst_GameObject *GA,*GB,*GC;
	MATH_tdst_Matrix *A,*B,*C;
	GAO_tdst_ModifierBoneRefine *p_BR;
	p_BR = (GAO_tdst_ModifierBoneRefine *) _pst_Mod->p_Data;
	A = B = C = NULL;
	GC = _pst_Mod->pst_GO;
	if((GC->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && GC->pst_Base && GC->pst_Base->pst_Hierarchy) 
	{
		GB = GC->pst_Base->pst_Hierarchy->pst_Father;
		if((GB->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && GB->pst_Base && GB->pst_Base->pst_Hierarchy) 
		{
			GA = GB->pst_Base->pst_Hierarchy->pst_Father;
			OBJ_ComputeGlobalWhenHie(GA);
			OBJ_ComputeGlobalWhenHie(GB);
			OBJ_ComputeGlobalWhenHie(GC);
			A = GA ->pst_GlobalMatrix;
			B = GB ->pst_GlobalMatrix;
			C = GC ->pst_GlobalMatrix;
		}
	}
	if (A && B && C)
	{
		MATH_SetIdentityMatrix(C);
		if (p_BR->ulMode)
		{
			MATH_tdst_Quaternion QA,QAi,QB,QC,QCt;
			MATH_ConvertMatrixToQuaternion(&QA, A);
			MATH_ConvertMatrixToQuaternion(&QB, B);
			QAi .x = -QA .x ;
			QAi .y = -QA .y ;
			QAi .z = -QA .z ;
			QAi .w =  QA .w ;
			MATH_MulQuaternion(&QC,&QAi,&QB);
			QC.x = QC.y = 0.0f;
			QC.z *= p_BR->fInterpolValue;
			if (QC.w < 0.0f)
				QC.z = -QC.z;

			QC.w = fSqrt(1.0f - MATH_f_SqrNormVector((MATH_tdst_Vector *)&QC));
			MATH_MulQuaternion(&QCt,&QA,&QC);
			MATH_ConvertQuaternionToMatrix(C,&QCt);
			C->T = A->T;
		} else
		{
			MATH_MatrixBlend(C,A,B,p_BR->fInterpolValue,1);
			C->T = B->T;
		}
		
		OBJ_ComputeLocalWhenHie(GC);
	}
};
void GAO_ModifierBoneRefineUnapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj) 
{
};		
void GAO_ModifierBoneRefineInit(GAO_tdst_ModifierBoneRefine *p_FOGDY) {};
void GAO_ModifierBoneRefineReinit(MDF_tdst_Modifier *_pst_Mod) {};
#if defined(XML_CONV_TOOL)
ULONG gBoneRefineMdfVersion;
ULONG gBoneRefineMdfDummy1;
ULONG gBoneRefineMdfDummy2;
ULONG gBoneRefineMdfDummy3;
ULONG gBoneRefineMdfSize;
#endif
ULONG BoneRefineModifier_SaveLoad_Parrams(GAO_tdst_ModifierBoneRefine *_pst_Modifier, ULONG Flags, char **_pc_ReadBuffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							ulSize,ulVersion,DUMMY;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ulSize = 0;
	if (Flags & 1) // Save
	{
#if defined(XML_CONV_TOOL)
		ulVersion = gBoneRefineMdfVersion;
#else
		ulVersion = 3; // Increment Last version Here
#endif
	}
	if (Flags & 2) // Read
	{
		GAO_ModifierBoneRefineInit(_pst_Modifier);
	}
	/* SaveLoad Version */
	SAV_LOAD_Buffer(ulVersion, Long);
#if defined(XML_CONV_TOOL)
	if (Flags & 2) // Read
	{
		gBoneRefineMdfVersion = ulVersion;
	}
#endif
	if (ulVersion < 2)
	{
#if defined(XML_CONV_TOOL)
		SAV_LOAD_Buffer(gBoneRefineMdfDummy1, Long);
		SAV_LOAD_Buffer(gBoneRefineMdfDummy2, Long);
		SAV_LOAD_Buffer(gBoneRefineMdfDummy3, Long);
#else
		SAV_LOAD_Buffer(DUMMY, Long);
		SAV_LOAD_Buffer(DUMMY, Long);
		SAV_LOAD_Buffer(DUMMY, Long);
#endif
	}
	if (ulVersion >= 1)
	{
		SAV_LOAD_Buffer(_pst_Modifier->ulMode, Long);
		if (ulVersion >= 3)
		{
			SAV_LOAD_Buffer(_pst_Modifier->fInterpolValue, Float);
		} else
			_pst_Modifier->fInterpolValue = 0.5f;

	} 

#ifdef ACTIVE_EDITORS
	_pst_Modifier->ulCodeKey = 0xC0DE2002;
#endif
	return ulSize;
}

#ifdef ACTIVE_EDITORS
int BoneRefineModifier_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierBoneRefine	*Src, *Dst ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Src = (GAO_tdst_ModifierBoneRefine *) _pst_Src->p_Data;
	Dst = (GAO_tdst_ModifierBoneRefine *) _pst_Dst->p_Data;
	memcpy(Dst , Src , sizeof(GAO_tdst_ModifierBoneRefine));
	return 1;
}
void BoneRefineModifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	ULONG								ulSize;
	ulSize = BoneRefineModifier_SaveLoad_Parrams((GAO_tdst_ModifierBoneRefine *) _pst_Mod->p_Data, 0 , NULL);	/* Save Size */
#if defined(XML_CONV_TOOL)
	ulSize = gBoneRefineMdfSize;
#endif
	SAV_Buffer(&ulSize, 4);
	BoneRefineModifier_SaveLoad_Parrams((GAO_tdst_ModifierBoneRefine *) _pst_Mod->p_Data, 1 , NULL);
}
#endif
ULONG BoneRefineul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	char *pc_Cur;
	pc_Cur = _pc_Buffer;
#if defined( XML_CONV_TOOL )
	gBoneRefineMdfSize = LOA_ReadLong( &pc_Cur );
#else
	LOA_ReadLong( &pc_Cur ); /* Skip size */
#endif
	BoneRefineModifier_SaveLoad_Parrams( ( GAO_tdst_ModifierBoneRefine * ) _pst_Mod->p_Data, 2, &pc_Cur );
	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
	return pc_Cur - _pc_Buffer;
}

/**********************************************************************************************/
/**********************************************************************************************/
/***********   Bones Meca modifier	    ***************************************************/
/**********************************************************************************************/
/**********************************************************************************************/

void GAO_ModifierBoneMecaInterpolate(struct MDF_tdst_Modifier_ * p_Mod, u32 mode , float fInterpolatedValue) {};
void GAO_ModifierBoneMecaInit(GAO_tdst_ModifierBoneMeca *p_FOGDY) {};
void GAO_ModifierBoneMecaReinit(MDF_tdst_Modifier *_pst_Mod) {};
void BoneMecaModifier_ValidateEditor(MDF_tdst_Modifier *_pst_Mod)
{
#ifdef ACTIVE_EDITORS
	((GAO_tdst_ModifierBoneMeca *) (_pst_Mod->p_Data) )->ulCodeKey = 0xC0DE2002;
	((GAO_tdst_ModifierBoneMeca *) (_pst_Mod->p_Data) )->_pst_GO = 	_pst_Mod->pst_GO;
#endif
	((GAO_tdst_ModifierBoneMeca *) (_pst_Mod->p_Data) )->LastRenderNum = -1;
}

void GAO_ModifierBoneMecaCreate(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierBoneMeca));

	if (p_Data == NULL)
	{
		/* First init */
		L_memset(_pst_Mod->p_Data , 0 , sizeof(GAO_tdst_ModifierBoneMeca));
		_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
	}
	else
	{
		GAO_tdst_ModifierBoneMeca *p_FOGDY;
		_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;
		p_FOGDY = (GAO_tdst_ModifierBoneMeca *)_pst_Mod->p_Data;
		L_memcpy( _pst_Mod->p_Data, p_Data, sizeof (GAO_tdst_ModifierBoneMeca) );
	}
	BoneMecaModifier_ValidateEditor(_pst_Mod);
}
void GAO_ModifierBoneMecaDestroy(MDF_tdst_Modifier *_pst_Mod)
{
	if (_pst_Mod->p_Data)
	{
		MEM_Free(_pst_Mod->p_Data);
	}
}
#ifdef ACTIVE_EDITORS
extern void GAO_ModifierFOGDY_DrawDebugSphere(MATH_tdst_Matrix	*p_stSphereMatrix, MATH_tdst_Vector *pCenter , float Radius , u32 Color);
#endif
void BoneMeca_GetBonePos(OBJ_tdst_GameObject *_pst_GO , MATH_tdst_Matrix	*p_stSphereMatrix, u32 ul_MatrixNum)
{
	if (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix)
	{
		if ((u32)_pst_GO->pst_Base->pst_AddMatrix->l_Number > ul_MatrixNum) 
		{
			if (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AddMatArePointer)
			{
				if (_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ul_MatrixNum].pst_GO)
				{
					*p_stSphereMatrix = *_pst_GO->pst_Base->pst_AddMatrix->dst_GizmoPtr[ul_MatrixNum].pst_GO->pst_GlobalMatrix;
				}
			}
			else
			{
 				MATH_MulMatrixMatrix(p_stSphereMatrix , &_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo[ul_MatrixNum].st_Matrix,_pst_GO->pst_GlobalMatrix);
			}
		}
		else
		{
			*p_stSphereMatrix = *_pst_GO->pst_GlobalMatrix;
		}
	}
}

OBJ_tdst_GameObject *BoneMeca_GetGO_From_Skeleton(OBJ_tdst_GameObject *_pst_GO , u32 ul_GoNum)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	OBJ_tdst_Group			*pst_Skeleton;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)) return NULL;
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	pst_Skeleton = pst_GOAnim->pst_Skeleton;

	if(!pst_Skeleton) return NULL;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);

	if (ul_GoNum > (u32)(pst_EndBone - pst_CurrentBone)) return NULL;

	return (OBJ_tdst_GameObject *) pst_CurrentBone[ul_GoNum].p_Pointer;
}

void BoneMeca_ResetSF(OBJ_tdst_GameObject *_pst_GO )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	OBJ_tdst_Group			*pst_Skeleton;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)) return;
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	pst_Skeleton = pst_GOAnim->pst_Skeleton;

	if(!pst_Skeleton) return;

	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
	while (pst_CurrentBone < pst_EndBone)
	{
		OBJ_tdst_GameObject *_pst_GO;
		_pst_GO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;

		OBJ_GameObject_CreateExtendedIfNot(_pst_GO );
		_pst_GO ->pst_Extended->uw_ExtraFlags &= ~OBJ_C_ExtraFlag_DontReinitPos;
		pst_CurrentBone++;
	}
}

#ifdef ACTIVE_EDITORS
u32			stInsides[MAX_BONEMECA_MOVERS];
MATH_tdst_Vector stInsidesEx[MAX_BONEMECA_MOVERS];
extern void OGL_DrawLineEx(GDI_tdst_DisplayData *_pst_DD, GDI_tdst_Request_DrawLineEx *_pst_Data);
MATH_tdst_Vector *MAT_GetVecInd(MATH_tdst_Matrix		*p_stGOMatrix, u32 Index)
{
	if (Index == 0) return MATH_pst_GetXAxis(p_stGOMatrix);
	else
	if (Index == 1) return MATH_pst_GetYAxis(p_stGOMatrix);
	else
		return MATH_pst_GetZAxis(p_stGOMatrix);
}
void GAO_ModifierBoneMeca_DrawDebugSphere(MATH_tdst_Matrix		*p_stOGLSphereMatrix, u32 Color, float ToCame)
{
	MATH_tdst_Vector					stX,stY,stZ,stP;
	MATH_tdst_Vector					stC[2];
	float								I,J,Ip,DCos,DSin;
	u32									Counter,Counter2,Counter3;
	GDI_tdst_Request_DrawLineEx stDLE;
	L_memset(&stDLE,0,sizeof(stDLE));
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);
#define SphereDiscreet 20
	Counter = 3;
	while (Counter--)
	{
		Counter2 = 3;
		while (Counter2--)
		{
			stP = p_stOGLSphereMatrix->T;
			stX = *MAT_GetVecInd(p_stOGLSphereMatrix, Counter % 3);
			stY = *MAT_GetVecInd(p_stOGLSphereMatrix, (Counter + 1) % 3);
			stZ = *MAT_GetVecInd(p_stOGLSphereMatrix, (Counter + 2) % 3);
			if (Counter2)
			{
				MATH_ScaleEqualVector(&stX , 0.86602540378443864676372317075294f);
				MATH_ScaleEqualVector(&stY , 0.86602540378443864676372317075294f);
				MATH_AddScaleVector(&stP,&stP,&stZ,(Counter2 == 1) ? 0.5f : -0.5f);
			}
			DCos = fCos(2.0f * 3.1415927f / SphereDiscreet);
			DSin = fSin(2.0f * 3.1415927f / SphereDiscreet);
			stDLE.A = &stC[0];	
			stDLE.B = &stC[1];
			stDLE.ul_Flags = 0;
			stDLE.f_Width = 2.0f;
			if (ToCame != 0.0f)
			{
				stDLE.ul_Flags = GDI_Request_DrawLine_NoZWrite;
				stDLE.f_Width = 1.0f;
			}
			stDLE.ul_Color = Color;
			if (Color == 0x00ff00)
			{
				stDLE.ul_Color = (0x80 << (Counter << 3) ) | 0x404040;
			}
			I = 1.0f; J = 0.0f;
			Counter3 = SphereDiscreet;
			MATH_AddScaleVector(&stC[1],&stP,&stX,I);
			MATH_AddScaleVector(&stC[1],&stC[1],&stY,J);
			MATH_BlendVector(&stC[1],&stC[1],&GDI_gpst_CurDD->st_Camera.st_Matrix.T,ToCame);
			while (Counter3--)
			{
				stC[0] = stC[1];
				Ip = DCos * I + DSin * J;
				J = -DSin * I + DCos * J;
				I = Ip;
				MATH_AddScaleVector(&stC[1],&stP,&stX,I);
				MATH_AddScaleVector(&stC[1],&stC[1],&stY,J);
				MATH_BlendVector(&stC[1],&stC[1],&GDI_gpst_CurDD->st_Camera.st_Matrix.T,ToCame);
				OGL_DrawLineEx(GDI_gpst_CurDD, &stDLE);
			}
		}
	}
}
void GAO_ModifierBoneMeca_DrawDebugLine(MATH_tdst_Vector *Pa,MATH_tdst_Vector *Pb, u32 Color, float ToCame)
{
	GDI_tdst_Request_DrawLineEx stDLE;
	MATH_tdst_Vector P[2];
	L_memset(&stDLE,0,sizeof(stDLE));
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);
	MATH_BlendVector(&P[0],Pa,&GDI_gpst_CurDD->st_Camera.st_Matrix.T,ToCame);
	MATH_BlendVector(&P[1],Pb,&GDI_gpst_CurDD->st_Camera.st_Matrix.T,ToCame);
	stDLE.A = P;	
	stDLE.B = P + 1;
	stDLE.ul_Flags = 0;
	stDLE.f_Width = 2.0f;
	if (ToCame != 0.0f)
	{
		stDLE.ul_Flags = GDI_Request_DrawLine_NoZWrite;
	}
	stDLE.ul_Color = Color;
	OGL_DrawLineEx(GDI_gpst_CurDD, &stDLE);
}
void GAO_ModifierBoneMeca_DrawDebugSphereHorizon(MATH_tdst_Matrix		*p_stOGLSphereMatrix, u32 Color, float ToCame)
{
	MATH_tdst_Vector					stX,stY,stZ,stP;
	MATH_tdst_Vector					stC[2];
	float								I,J,Ip,DCos,DSin;
	u32									Counter,Counter2,Counter3;
	GDI_tdst_Request_DrawLineEx stDLE;
	L_memset(&stDLE,0,sizeof(stDLE));
	GDI_SetViewMatrix((*GDI_gpst_CurDD) , GDI_gpst_CurDD->st_MatrixStack.pst_CurrentMatrix);
#define SphereDiscreet 20
	Counter = 3;
	while (Counter--)
	{
		Counter2 = 3;
		while (Counter2--)
		{
			stP = p_stOGLSphereMatrix->T;
			stX = *MAT_GetVecInd(p_stOGLSphereMatrix, Counter % 3);
			stY = *MAT_GetVecInd(p_stOGLSphereMatrix, (Counter + 1) % 3);
			stZ = *MAT_GetVecInd(p_stOGLSphereMatrix, (Counter + 2) % 3);
			if (Counter2)
			{
				MATH_ScaleEqualVector(&stX , 0.86602540378443864676372317075294f);
				MATH_ScaleEqualVector(&stY , 0.86602540378443864676372317075294f);
				MATH_AddScaleVector(&stP,&stP,&stZ,(Counter2 == 1) ? 0.5f : -0.5f);
			}
			DCos = fCos(2.0f * 3.1415927f / SphereDiscreet);
			DSin = fSin(2.0f * 3.1415927f / SphereDiscreet);
			stDLE.A = &stC[0];	
			stDLE.B = &stC[1];
			stDLE.ul_Flags = 0;
			stDLE.f_Width = 2.0f;
			if (ToCame != 0.0f)
			{
				stDLE.ul_Flags = GDI_Request_DrawLine_NoZWrite;
				stDLE.f_Width = 1.0f;
			}
			stDLE.ul_Color = Color;
			I = 1.0f; J = 0.0f;
			Counter3 = SphereDiscreet;
			MATH_AddScaleVector(&stC[1],&stP,&stX,I);
			MATH_AddScaleVector(&stC[1],&stC[1],&stY,J);
			MATH_BlendVector(&stC[1],&stC[1],&GDI_gpst_CurDD->st_Camera.st_Matrix.T,ToCame);
			while (Counter3--)
			{
				stC[0] = stC[1];
				Ip = DCos * I + DSin * J;
				J = -DSin * I + DCos * J;
				I = Ip;
				MATH_AddScaleVector(&stC[1],&stP,&stX,I);
				MATH_AddScaleVector(&stC[1],&stC[1],&stY,J);
				MATH_BlendVector(&stC[1],&stC[1],&GDI_gpst_CurDD->st_Camera.st_Matrix.T,ToCame);
				OGL_DrawLineEx(GDI_gpst_CurDD, &stDLE);
			}
		}
	}
}
#endif
void GAO_ModifierBoneMecaApply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj) 
{
	GAO_tdst_ModifierBoneMeca			*_pst_Modifier;
	u32 Counter,Counter2;

	OBJ_tdst_GameObject					*stBP[MAX_BONEMECA_MOVERS];
	OBJ_tdst_GameObject					*stBF[MAX_BONEMECA_MOVERS];
	MATH_tdst_Vector					st_SavedPos[MAX_BONEMECA_MOVERS];
	MATH_tdst_Vector					*pst_BonesPos[MAX_BONEMECA_MOVERS];
	MATH_tdst_Vector					*pst_BonesPosFather[MAX_BONEMECA_MOVERS];
	MATH_tdst_Vector					*pst_BonesPosFather2[MAX_BONEMECA_MOVERS];
	float								st_BonesDistanceToFather[MAX_BONEMECA_MOVERS];
	MATH_tdst_Vector					st_Renormalizators[MAX_BONEMECA_MOVERS];
	MATH_tdst_Vector					st_RenormalizatorsS[MAX_BONEMECA_MOVERS];
	
	float								LocalLocalDT,OOLLDT;

	float								LocalDT,UserDT;

	if (!ENG_gb_EngineRunning) return;

	_pst_Modifier = (GAO_tdst_ModifierBoneMeca *)_pst_Mod->p_Data;

	UserDT = TIM_gf_dt;

	if (_pst_Modifier->LastRenderNum != GDI_gpst_CurDD->ul_RenderingCounter)
		/* Clean */
	{
		L_memset(_pst_Modifier->stBS, 0 , sizeof(_pst_Modifier->stBS));
		UserDT = 0.0f;
	}
	_pst_Modifier->LastRenderNum = GDI_gpst_CurDD->ul_RenderingCounter+1;



	/* 0 Compute Tables */
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		pst_BonesPos[Counter] = NULL;
		pst_BonesPosFather[Counter] = NULL;
		stBF[Counter] = NULL;
		stBP[Counter] = BoneMeca_GetGO_From_Skeleton(_pst_Mod->pst_GO , _pst_Modifier ->stBR[Counter]);
		if (stBP[Counter])
		{
			if((stBP[Counter]->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && stBP[Counter]->pst_Base && stBP[Counter]->pst_Base->pst_Hierarchy) 
			{
				stBF[Counter] = stBP[Counter]->pst_Base->pst_Hierarchy->pst_Father;
				OBJ_ComputeGlobalWhenHie(stBF[Counter]);
				pst_BonesPosFather[Counter] = &stBF[Counter]->pst_GlobalMatrix->T;
				st_BonesDistanceToFather[Counter] = MATH_f_NormVector(&(stBP[Counter]->pst_GlobalMatrix + 1)->T);
			}
			OBJ_ComputeGlobalWhenHie(stBP[Counter]);
			pst_BonesPos[Counter] = &stBP[Counter]->pst_GlobalMatrix->T;
			OBJ_GameObject_CreateExtendedIfNot(stBP[Counter]);
			stBP[Counter]->pst_Extended->uw_ExtraFlags |= OBJ_C_ExtraFlag_DontReinitPos;
		}
	}
	/* Special tag */
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (pst_BonesPos[Counter]) *(u32 *)&pst_BonesPos[Counter]->x &= ~3;
		if (pst_BonesPosFather[Counter]) *(u32 *)&pst_BonesPosFather[Counter]->x &= ~3;
	}
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (pst_BonesPos[Counter]) *(u32 *)&pst_BonesPos[Counter]->x |= 1;
	}
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		pst_BonesPosFather2[Counter] = NULL;
		if (pst_BonesPosFather[Counter] && *(u32 *)&pst_BonesPosFather[Counter]->x & 1)
			pst_BonesPosFather2[Counter] = pst_BonesPosFather[Counter];
	}
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (pst_BonesPos[Counter])
		{
			st_SavedPos[Counter] = *pst_BonesPos[Counter];
		}
	}
#if 1
	LocalDT = UserDT;
	while (LocalDT > 0.0f)
	{

		LocalLocalDT = fMin(1.0f / 200.0f , LocalDT);
		LocalDT -= LocalLocalDT;
		/* 1 compute mecanics Only with Matrix T */
		Counter = _pst_Modifier->ulNumberOfMovers;
		while(Counter--)
		{
			_pst_Modifier->stBS[Counter].z += _pst_Modifier->fGravity;
			MATH_ScaleEqualVector(&_pst_Modifier->stBS[Counter],_pst_Modifier->fFriction);
			if (pst_BonesPos[Counter])
			{
				MATH_AddScaleVector(pst_BonesPos[Counter] , pst_BonesPos[Counter] , &_pst_Modifier->stBS[Counter] , LocalLocalDT);
			}
		}
		/* 2 Compute constraints Only with Matrix T */
		Counter2 = 2;
		while (Counter2--)
		{
			u32   CounterS;
			L_memset(st_Renormalizators,0,sizeof(st_Renormalizators));
			L_memset(st_RenormalizatorsS,0,sizeof(st_RenormalizatorsS));
			CounterS = _pst_Modifier->ulNumberOfExcluders;
			/* Extrude from spheres */
			while (CounterS--)
			{
				MATH_tdst_Matrix	stSphereMatrix;
				MATH_tdst_Vector	stOoRadius;
				BoneMeca_GetBonePos(_pst_Mod->pst_GO , &stSphereMatrix, _pst_Modifier ->stBM[CounterS].ulBoneRef);
				MATH_AddScaleVector(&stSphereMatrix.T,&stSphereMatrix.T,MATH_pst_GetXAxis(&stSphereMatrix),_pst_Modifier ->stBM[CounterS].DeltaPos.x);
				MATH_AddScaleVector(&stSphereMatrix.T,&stSphereMatrix.T,MATH_pst_GetYAxis(&stSphereMatrix),_pst_Modifier ->stBM[CounterS].DeltaPos.y);
				MATH_AddScaleVector(&stSphereMatrix.T,&stSphereMatrix.T,MATH_pst_GetZAxis(&stSphereMatrix),_pst_Modifier ->stBM[CounterS].DeltaPos.z);
				stOoRadius.x = 1.0f / _pst_Modifier ->stBM[CounterS].SphereRadius.x;
				stOoRadius.y = 1.0f / _pst_Modifier ->stBM[CounterS].SphereRadius.y;
				stOoRadius.z = 1.0f / _pst_Modifier ->stBM[CounterS].SphereRadius.z;
				Counter = _pst_Modifier->ulNumberOfMovers;
				while(Counter--)
				{
					if (pst_BonesPos[Counter])
					{
						MATH_tdst_Vector LocalCoord;
						MATH_tdst_Vector Axis;
						MATH_SubVector(&LocalCoord , pst_BonesPos[Counter] , &stSphereMatrix.T);
						Axis.x = MATH_f_DotProduct(&LocalCoord , MATH_pst_GetXAxis(&stSphereMatrix));
						Axis.y = MATH_f_DotProduct(&LocalCoord , MATH_pst_GetYAxis(&stSphereMatrix));
						Axis.z = MATH_f_DotProduct(&LocalCoord , MATH_pst_GetZAxis(&stSphereMatrix));
						LocalCoord = Axis;
						MATH_MulTwoVectors(&Axis , &Axis , &stOoRadius);
						if (MATH_f_SqrNormVector(&Axis) < 0.99999f)
						{
							float K,q,H,B,CosAl,SinAl;
							/* inside */
							K = MATH_f_NormVector(&Axis);
							MATH_MulTwoVectors(&LocalCoord , &Axis , &stOoRadius);
							MATH_MulTwoVectors(&LocalCoord , &LocalCoord , &stOoRadius);
							MATH_ScaleVector(&Axis , &Axis , 1.0f / K);
							MATH_NormalizeVector(&LocalCoord , &LocalCoord );

							SinAl = MATH_f_DotProduct( &LocalCoord , &Axis );
							CosAl = fOptSqrt(1.0f - SinAl * SinAl);

							q = K * SinAl;
							H = K * CosAl;

							B = fOptSqrt(1.0f - H * H) - q;

							MATH_SetNormVector(&LocalCoord , &LocalCoord , B );
							MATH_AddScaleVector(&Axis , &LocalCoord , &Axis , K);


							MATH_MulTwoVectors(&Axis , &Axis , &_pst_Modifier ->stBM[CounterS].SphereRadius);
							LocalCoord  = stSphereMatrix.T;
							MATH_AddScaleVector(&LocalCoord , &LocalCoord , MATH_pst_GetXAxis(&stSphereMatrix) , Axis .x);
							MATH_AddScaleVector(&LocalCoord , &LocalCoord , MATH_pst_GetYAxis(&stSphereMatrix) , Axis .y);
							MATH_AddScaleVector(&LocalCoord , &LocalCoord , MATH_pst_GetZAxis(&stSphereMatrix) , Axis .z);
#ifdef ACTIVE_EDITORS
							if (_pst_Modifier->ulDrawBones)
							{
								stInsides[Counter] = 1;
								stInsidesEx[Counter] = LocalCoord;
							}
#endif
							MATH_SubVector(&LocalCoord , pst_BonesPos[Counter] , &LocalCoord );
 							MATH_AddScaleVector(&st_RenormalizatorsS[Counter],&st_RenormalizatorsS[Counter],&LocalCoord,1.0f);//*/
						}


					}
				}
			}
			/* Bones renorm */
			Counter = _pst_Modifier->ulNumberOfMovers;
			while(Counter--)
			{
				if (stBF[Counter] && stBP[Counter])
				{
					float Distance;
					MATH_tdst_Vector Axis;
					MATH_SubVector(&Axis , pst_BonesPos[Counter] , pst_BonesPosFather[Counter]);
					Distance = MATH_f_NormVector(&Axis);
					MATH_SetNormVector(&Axis,&Axis,(Distance - st_BonesDistanceToFather[Counter]) * 0.5f);
					MATH_AddVector(&st_Renormalizators[Counter],&st_Renormalizators[Counter],&Axis);
					CounterS = _pst_Modifier->ulNumberOfExcluders;
				}
			}

			Counter = _pst_Modifier->ulNumberOfMovers;
			while(Counter--)
			{
				if (pst_BonesPos[Counter])			
				{
					MATH_SubVector(pst_BonesPos[Counter]		, pst_BonesPos[Counter]		 , &st_Renormalizators[Counter]);
					MATH_SubVector(pst_BonesPos[Counter]		, pst_BonesPos[Counter]		 , &st_RenormalizatorsS[Counter]);
				}
				if (pst_BonesPosFather2[Counter])	
					MATH_AddScaleVector(pst_BonesPosFather2[Counter] , pst_BonesPosFather2[Counter], &st_Renormalizators[Counter],_pst_Modifier->fRenormToFatherFactor);
			}
		}//*/

	}
#endif
		// 3 Rededuct speeds 
	Counter = _pst_Modifier->ulNumberOfMovers;
	if (UserDT)
	{
		OOLLDT = 1.0f / UserDT;
		while(Counter--)
		{
			if (pst_BonesPos[Counter])		 
			{
				MATH_SubVector(&_pst_Modifier->stBS[Counter] , pst_BonesPos[Counter] , &st_SavedPos[Counter]);
				MATH_ScaleEqualVector(&_pst_Modifier->stBS[Counter],OOLLDT);
			}
		}//*/
	}
	/* 3 re-chnuque */
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (stBP[Counter])
		{
            OBJ_ComputeLocalWhenHie(stBP[Counter]);
		}
	}
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (stBP[Counter])
		{
            OBJ_ComputeLocalWhenHie(stBP[Counter]);
		}
	}
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (stBP[Counter])
		{
            OBJ_ComputeLocalWhenHie(stBP[Counter]);
		}
	}
	/* 3 rededuct matrixes via quaternions */
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (stBP[Counter] && stBF[Counter])
		{
			MATH_tdst_Vector stOriginalVector;
			MATH_tdst_Vector stComputedVector;
			MATH_tdst_Quaternion stQuaternion;
			MATH_tdst_Matrix MDst;
			MATH_SetIdentityMatrix(&MDst);

			MATH_NormalizeVector(&stOriginalVector ,&(stBP[Counter]->pst_GlobalMatrix + 1)->T);
			MATH_NormalizeVector(&stComputedVector ,&(stBP[Counter]->pst_Base->pst_Hierarchy->st_LocalMatrix).T);
			MATH_AddVector(&stComputedVector ,&stComputedVector ,&stOriginalVector );
			MATH_NormalizeVector(&stComputedVector ,&stComputedVector );

			MATH_CrossProduct((MATH_tdst_Vector *)&stQuaternion,&stOriginalVector ,&stComputedVector );

			stQuaternion.w = MATH_f_DotProduct(&stComputedVector , &stOriginalVector);
			MATH_ConvertQuaternionToMatrix(&MDst, &stQuaternion);

			MATH_Mul33MatrixMatrix(&stBP[Counter]->pst_Base->pst_Hierarchy->st_LocalMatrix, stBP[Counter]->pst_GlobalMatrix + 1, &MDst, 1);
		}
	}//*/

	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (stBP[Counter])
		{
            OBJ_ComputeGlobalWhenHie(stBP[Counter]);
		}
	}
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (stBP[Counter])
		{
            OBJ_ComputeGlobalWhenHie(stBP[Counter]);
		}
	}
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (stBP[Counter])
		{
            OBJ_ComputeGlobalWhenHie(stBP[Counter]);
		}
	}
	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (stBP[Counter])
		{
            OBJ_ComputeLocalWhenHie(stBP[Counter]);
		}
	}

	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (stBP[Counter])
		{
            OBJ_ComputeLocalWhenHie(stBP[Counter]);
		}
	}

	Counter = _pst_Modifier->ulNumberOfMovers;
	while(Counter--)
	{
		if (stBP[Counter])
		{
            OBJ_ComputeLocalWhenHie(stBP[Counter]);
		}
	}

};

void GAO_ModifierBoneMecaUnapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj) 
{ 
#ifdef ACTIVE_EDITORS

	if (_pst_Mod->pst_GO->ul_EditorFlags & OBJ_C_EditFlags_Selected)
	{
		GAO_tdst_ModifierBoneMeca *_pst_Modifier;
		u32 Counter;
		_pst_Modifier = (GAO_tdst_ModifierBoneMeca *)_pst_Mod->p_Data;
		if (_pst_Modifier->ulDrawBones)
		{
			Counter = _pst_Modifier ->ulNumberOfExcluders;
			while (Counter--)
			{
				MATH_tdst_Matrix	stSphereMatrix;
				stSphereMatrix = *_pst_Mod->pst_GO->pst_GlobalMatrix;
				BoneMeca_GetBonePos(_pst_Mod->pst_GO , &stSphereMatrix, _pst_Modifier ->stBM[Counter].ulBoneRef);
				MATH_MakeOGLMatrix(&stSphereMatrix, &stSphereMatrix);
				MATH_AddScaleVector(&stSphereMatrix.T,&stSphereMatrix.T,MATH_pst_GetXAxis(&stSphereMatrix),_pst_Modifier ->stBM[Counter].DeltaPos.x);
				MATH_AddScaleVector(&stSphereMatrix.T,&stSphereMatrix.T,MATH_pst_GetYAxis(&stSphereMatrix),_pst_Modifier ->stBM[Counter].DeltaPos.y);
				MATH_AddScaleVector(&stSphereMatrix.T,&stSphereMatrix.T,MATH_pst_GetZAxis(&stSphereMatrix),_pst_Modifier ->stBM[Counter].DeltaPos.z);
				MATH_ScaleEqualVector(MATH_pst_GetXAxis(&stSphereMatrix),_pst_Modifier ->stBM[Counter].SphereRadius.x);
				MATH_ScaleEqualVector(MATH_pst_GetYAxis(&stSphereMatrix),_pst_Modifier ->stBM[Counter].SphereRadius.y);
				MATH_ScaleEqualVector(MATH_pst_GetZAxis(&stSphereMatrix),_pst_Modifier ->stBM[Counter].SphereRadius.z);
				GAO_ModifierBoneMeca_DrawDebugSphere(&stSphereMatrix, 0x402020,0.5f);
				GAO_ModifierBoneMeca_DrawDebugSphere(&stSphereMatrix, 0xff0000,0.0f);
			}
			Counter = _pst_Modifier ->ulNumberOfMovers;
			while (Counter--)
			{
				OBJ_tdst_GameObject					*stGO;
				stGO = BoneMeca_GetGO_From_Skeleton(_pst_Mod->pst_GO , _pst_Modifier ->stBR[Counter]);
				if (stGO)
				{
					MATH_tdst_Matrix	stSphereMatrix;
					stSphereMatrix = *stGO->pst_GlobalMatrix;
					MATH_MakeOGLMatrix(&stSphereMatrix, &stSphereMatrix);
					MATH_ScaleEqualVector(MATH_pst_GetXAxis(&stSphereMatrix),0.02f);
					MATH_ScaleEqualVector(MATH_pst_GetYAxis(&stSphereMatrix),0.02f);
					MATH_ScaleEqualVector(MATH_pst_GetZAxis(&stSphereMatrix),0.02f);
					if (stInsides[Counter])
					{
						MATH_tdst_Matrix	stSphereMatrix2;
						stSphereMatrix2 = stSphereMatrix;
						stSphereMatrix2 .T = stInsidesEx[Counter];
						
						GAO_ModifierBoneMeca_DrawDebugSphere(&stSphereMatrix2 , 0x202040,0.5f);
						GAO_ModifierBoneMeca_DrawDebugSphere(&stSphereMatrix2 , 0x0000ff,0.0f);
					} //*/
					GAO_ModifierBoneMeca_DrawDebugSphere(&stSphereMatrix, 0x00ff00,0.5f);
					GAO_ModifierBoneMeca_DrawDebugSphere(&stSphereMatrix, 0x00ff00,0.0f);
					if((stGO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) && stGO->pst_Base && stGO->pst_Base->pst_Hierarchy) 
					{
						GAO_ModifierBoneMeca_DrawDebugLine(&stGO->pst_GlobalMatrix->T,&stGO->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix->T, 0x404020,0.5f);
						GAO_ModifierBoneMeca_DrawDebugLine(&stGO->pst_GlobalMatrix->T,&stGO->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix->T, 0xffFF00,0.0f);
					}

				}
				
			}
		}
	}
	L_memset(stInsides,0,sizeof(stInsides));
	L_memset(stInsidesEx,0,sizeof(stInsidesEx));

#endif
};
ULONG BoneMecaModifier_SaveLoad_Parrams(GAO_tdst_ModifierBoneMeca *_pst_Modifier, ULONG Flags, char **_pc_ReadBuffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG							ulSize,ulVersion;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ulSize = 0;
	ulVersion = 3; // Increment Last version Here
	if (Flags & 2) // Read
	{
		GAO_ModifierBoneMecaInit(_pst_Modifier);
	}
	/* SaveLoad Version */
	SAV_LOAD_Buffer(ulVersion, Long);

	if (ulVersion >= 1)
	{
		u32 Counter;
		SAV_LOAD_Buffer(_pst_Modifier->ulNumberOfExcluders, Long);
		for (Counter = 0 ; Counter < _pst_Modifier->ulNumberOfExcluders ; Counter++)
		{
			SAV_LOAD_Buffer(_pst_Modifier->stBM[lMin(Counter,MAX_BONEMECA_EXCLUDERS - 1)].ulBoneRef		, Long);
			SAV_LOAD_Buffer(_pst_Modifier->stBM[lMin(Counter,MAX_BONEMECA_EXCLUDERS - 1)].SphereRadius.x	, Float);
			SAV_LOAD_Buffer(_pst_Modifier->stBM[lMin(Counter,MAX_BONEMECA_EXCLUDERS - 1)].SphereRadius.y	, Float);
			SAV_LOAD_Buffer(_pst_Modifier->stBM[lMin(Counter,MAX_BONEMECA_EXCLUDERS - 1)].SphereRadius.z	, Float);
			SAV_LOAD_Buffer(_pst_Modifier->stBM[lMin(Counter,MAX_BONEMECA_EXCLUDERS - 1)].DeltaPos.x		, Float);
			SAV_LOAD_Buffer(_pst_Modifier->stBM[lMin(Counter,MAX_BONEMECA_EXCLUDERS - 1)].DeltaPos.y		, Float);
			SAV_LOAD_Buffer(_pst_Modifier->stBM[lMin(Counter,MAX_BONEMECA_EXCLUDERS - 1)].DeltaPos.z		, Float);
		}
		_pst_Modifier->ulNumberOfExcluders = lMin(_pst_Modifier->ulNumberOfExcluders,MAX_BONEMECA_EXCLUDERS);
	

		SAV_LOAD_Buffer(_pst_Modifier->ulNumberOfMovers, Long);
		for (Counter = 0 ; Counter < _pst_Modifier->ulNumberOfMovers ; Counter++)
		{
			SAV_LOAD_Buffer(_pst_Modifier->stBR[lMin(Counter,MAX_BONEMECA_MOVERS - 1)], Long);
		}
		_pst_Modifier->ulNumberOfMovers = lMin(_pst_Modifier->ulNumberOfMovers,MAX_BONEMECA_MOVERS);
		SAV_LOAD_Buffer(_pst_Modifier->fGravity, Float);
		SAV_LOAD_Buffer(_pst_Modifier->fFriction, Float);
		if (ulVersion >= 2)
		{
#ifdef ACTIVE_EDITORS
			SAV_LOAD_Buffer(_pst_Modifier->ulDrawBones, Long);
#else
			u32 Dummy;
			SAV_LOAD_Buffer(Dummy, Long);
#endif
			if (ulVersion >= 3)
			{
				SAV_LOAD_Buffer(_pst_Modifier->fRenormToFatherFactor, Float);
			} else
				_pst_Modifier->fRenormToFatherFactor = 1.0f;

		}

	} else
	{
		_pst_Modifier->ulNumberOfExcluders = 0;
		_pst_Modifier->fGravity = 0.0f;
		_pst_Modifier->fFriction = 0.0f;
	}

	return ulSize;
}

#ifdef ACTIVE_EDITORS
int BoneMecaModifier_Copy(MDF_tdst_Modifier *_pst_Dst, MDF_tdst_Modifier *_pst_Src )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GAO_tdst_ModifierBoneMeca	*Src, *Dst ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	Src = (GAO_tdst_ModifierBoneMeca *) _pst_Src->p_Data;
	Dst = (GAO_tdst_ModifierBoneMeca *) _pst_Dst->p_Data;
	memcpy(Dst , Src , sizeof(GAO_tdst_ModifierBoneMeca));
	BoneMecaModifier_ValidateEditor(_pst_Dst);
	return 1;
}
void BoneMecaModifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	ULONG								ulSize;
	ulSize = BoneMecaModifier_SaveLoad_Parrams((GAO_tdst_ModifierBoneMeca *) _pst_Mod->p_Data, 0 , NULL);	/* Save Size */
	SAV_Buffer(&ulSize, 4);
	BoneMecaModifier_SaveLoad_Parrams((GAO_tdst_ModifierBoneMeca *) _pst_Mod->p_Data, 1 , NULL);
	BoneMecaModifier_ValidateEditor(_pst_Mod);
}
#endif
ULONG BoneMecaul_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	CHAR					*pc_Cur;
	pc_Cur = _pc_Buffer;
	LOA_ReadLong(&pc_Cur); /* Skip size */	
	BoneMecaModifier_SaveLoad_Parrams((GAO_tdst_ModifierBoneMeca *) _pst_Mod->p_Data , 2, &pc_Cur);
	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
	BoneMecaModifier_ValidateEditor(_pst_Mod);
	return pc_Cur - _pc_Buffer ;
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
