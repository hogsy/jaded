/*$T MDFmodifier_ROTR.c GC 1.129 10/09/01 16:44:58 */


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
#include "ENGine/Sources/COLlision/COLray.h"
#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_SNAKE.h"
#include "GEOmetric/GEOobject.h"
#include "GEOmetric/GEOload.h"
#include "SOFT/SOFTzlist.h"
#include "SOFT/SOFTlinear.h"

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

/*
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierSNAKE_UpdatePointers(MDF_tdst_Modifier * _pst_Mod)
{
	extern void OBJ_ChangeIdentityFlags(OBJ_tdst_GameObject *, ULONG, ULONG);
	
	GAO_tdst_ModifierSNAKE	*p_SNAKE;
	int						i;
	MATH_tdst_Vector		v, v_Scale;
	OBJ_tdst_Group			*pst_Group;
	float					f_Zoom;


	p_SNAKE = (GAO_tdst_ModifierSNAKE *) _pst_Mod->p_Data;
	p_SNAKE->pst_Followed = NULL;
	p_SNAKE->f_TimeLeft = 0;
	if(p_SNAKE->i_NumBones < 2) return;
	pst_Group = MDF_Modifier_GetCurGroup(_pst_Mod);
	if(!pst_Group) return;

	for(i = 0; i < p_SNAKE->i_NumBones; i++)
	{
		p_SNAKE->pst_GO[i] = OBJ_pst_GroupGetByRank(pst_Group, p_SNAKE->ai_Bones[i]);
		if(!p_SNAKE->pst_GO[i]) return;
		OBJ_RestoreInitialPos(p_SNAKE->pst_GO[i]);
		OBJ_ComputeGlobalWithLocal(p_SNAKE->pst_GO[i], p_SNAKE->pst_GO[i]->pst_GlobalMatrix, 1);
	}

	p_SNAKE->pst_Followed = p_SNAKE->pst_GO[0];
	for(i = 1; i < p_SNAKE->i_NumBones; i++)
	{
		MATH_SubVector(&v, OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i]), OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i - 1]));
		p_SNAKE->af_GODist[i] = MATH_f_NormVector(&v);
		
		MATH_GetScale(&v_Scale, p_SNAKE->pst_GO[i]->pst_GlobalMatrix);
		f_Zoom = fMax3(v_Scale.x, v_Scale.y, v_Scale.z);
		if ( f_Zoom )
			p_SNAKE->af_GODist[i] /= f_Zoom;
	}
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierSNAKE_Create(OBJ_tdst_GameObject * _pst_GO, MDF_tdst_Modifier * _pst_Mod, void *p_Data)
{
	GAO_tdst_ModifierSNAKE * p_SNAKE;

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(GAO_tdst_ModifierSNAKE));
	p_SNAKE = (GAO_tdst_ModifierSNAKE *) _pst_Mod->p_Data;
	if(p_Data == NULL)
	{
		/* First init */
		L_memset(_pst_Mod->p_Data, 0, sizeof(GAO_tdst_ModifierSNAKE));
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
		GAO_ModifierSNAKE_UpdatePointers(_pst_Mod);
	}
	else
	{
		_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
		if(MDF_gpst_GlobalCreate->ul_Flags & SNAKE_TAKEY) _pst_Mod->ul_Flags |= SNAKE_TAKEY;
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(GAO_tdst_ModifierSNAKE));
	}
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierSNAKE_Destroy (MDF_tdst_Modifier * _pst_Mod)
{
	if(_pst_Mod->p_Data) MEM_Free(_pst_Mod->p_Data);
}

/*
 =======================================================================================================================
    Aim:    Recompute local matrix depending of father
 =======================================================================================================================
 */
void OBJMY_ComputeLocalWhenHie(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Father;
	MATH_tdst_Matrix M	ONLY_PSX2_ALIGNED(16);
	OBJ_tdst_Hierarchy	*pst_Hie;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		pst_Hie = _pst_GO->pst_Base->pst_Hierarchy;
		pst_Father = pst_Hie->pst_Father;
		if(pst_Father)
		{
			MATH_InvertMatrix(&M, OBJ_pst_GetAbsoluteMatrix(pst_Father));
			MATH_MulMatrixMatrix(&pst_Hie->st_LocalMatrix, OBJ_pst_GetAbsoluteMatrix(_pst_GO), &M);
			MATH_Orthonormalize(&pst_Hie->st_LocalMatrix);
		}
	}
}

//////////////// RETARD
//////////////// RETARD
//////////////// RETARD
//////////////// RETARD
//////////////// RETARD
//////////////// RETARD
#if 0
	GAO_tdst_ModifierSNAKE	*p_SNAKE;
	float					DT;
	MATH_tdst_Vector		tv_temp;
	MATH_tdst_Vector		tv_temp1;
	MATH_tdst_Vector		tv_temp2;
	MATH_tdst_Vector		tv_temp3;
	MATH_tdst_Vector		tv_temp4;
	float					f_norm;
	float					ff1, ff;
	int						i;
	extern void AI_EvalFunc_MATHVecBlendRotate_C(MATH_tdst_Vector *, MATH_tdst_Vector *, float, MATH_tdst_Vector *);

	DT = TIM_gf_dt;
	if(DT == 0.0f) DT = 1.0f / 50.0f;
	p_SNAKE = (GAO_tdst_ModifierSNAKE *) _pst_Mod->p_Data;
	if(!p_SNAKE->pst_Followed) return;
	OBJ_ComputeGlobalWithLocal(p_SNAKE->pst_Followed, p_SNAKE->pst_Followed->pst_GlobalMatrix, 1);

	ff1 = 0;
	for(i = 1; i < p_SNAKE->i_NumBones; i++)
	{
		if(!p_SNAKE->pst_GO[i - 1]) return;
		if(!p_SNAKE->pst_GO[i]) return;

		if(_pst_Mod->ul_Flags & SNAKE_TAKEY)
		{
			MATH_CopyVector(&tv_temp1, MATH_pst_GetZAxis(p_SNAKE->pst_GO[i - 1]->pst_GlobalMatrix));
			MATH_CopyVector(&tv_temp2, MATH_pst_GetZAxis(p_SNAKE->pst_GO[i]->pst_GlobalMatrix));
		}
		else
		{
			MATH_NegVector(&tv_temp1, MATH_pst_GetYAxis(p_SNAKE->pst_GO[i - 1]->pst_GlobalMatrix));
			MATH_NegVector(&tv_temp2, MATH_pst_GetYAxis(p_SNAKE->pst_GO[i]->pst_GlobalMatrix));
		}

		AI_EvalFunc_MATHVecBlendRotate_C(&tv_temp2, &tv_temp1, p_SNAKE->f_Inertie * DT, &tv_temp3);
		MATH_CopyVector(&tv_temp4, &tv_temp3);

		if(_pst_Mod->ul_Flags & SNAKE_TAKEY)
		{
			MATH_NegVector(&tv_temp1, MATH_pst_GetYAxis(p_SNAKE->pst_GO[i - 1]->pst_GlobalMatrix));
			MATH_NegVector(&tv_temp2, MATH_pst_GetYAxis(p_SNAKE->pst_GO[i]->pst_GlobalMatrix));
		}
		else
		{
			MATH_CopyVector(&tv_temp1, MATH_pst_GetZAxis(p_SNAKE->pst_GO[i - 1]->pst_GlobalMatrix));
			MATH_CopyVector(&tv_temp2, MATH_pst_GetZAxis(p_SNAKE->pst_GO[i]->pst_GlobalMatrix));
		}

		AI_EvalFunc_MATHVecBlendRotate_C(&tv_temp2, &tv_temp1, p_SNAKE->f_Inertie * DT, &tv_temp3);

		MATH_CopyVector(&tv_temp1, &tv_temp4);
		MATH_CopyVector(&tv_temp2, &tv_temp3);

		MATH_SubVector(&tv_temp, OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i]), OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i - 1]));
		f_norm = MATH_f_NormVector(&tv_temp);

		ff = MATH_f_FloatBlend(p_SNAKE->af_GODist[i], f_norm, p_SNAKE->f_BlendDist * DT);
		MATH_SetNormVector(&tv_temp, &tv_temp2, ff);

/*		if(i >= 2)
			MATH_SubVector(&tv_temp2, OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i - 1]), OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i - 2]));
		else if(_pst_Mod->ul_Flags & SNAKE_TAKEY)
			MATH_CopyVector(&tv_temp2, MATH_pst_GetYAxis(p_SNAKE->pst_GO[i - 1]->pst_GlobalMatrix));
		else
			MATH_CopyVector(&tv_temp2, MATH_pst_GetZAxis(p_SNAKE->pst_GO[i - 1]->pst_GlobalMatrix));

		MATH_SetNormVector(&tv_temp3, &tv_temp2, p_SNAKE->af_GODist[i]);
		MATH_CopyVector(&tv_temp2, &tv_temp3);

		AI_EvalFunc_MATHVecBlendRotate_C(&tv_temp, &tv_temp2, (p_SNAKE->f_Inertie + ff1) * DT, &tv_temp3);
		MATH_CopyVector(&tv_temp, &tv_temp3);
		ff1 += p_SNAKE->f_Attenuation;
		if(p_SNAKE->f_Inertie + ff1 < 0) ff1 = -p_SNAKE->f_Inertie;

		ff = MATH_f_FloatBlend(p_SNAKE->af_GODist[i], f_norm, p_SNAKE->f_BlendDist * DT);
		MATH_SetNormVector(&tv_temp3, &tv_temp, ff);
		MATH_CopyVector(&tv_temp, &tv_temp3);*/

		MATH_NormalizeVector(&tv_temp3, &tv_temp);
		MATH_NormalizeVector(&tv_temp4, &tv_temp1);

		if(_pst_Mod->ul_Flags & SNAKE_TAKEY)
		{
//			MATH_NegEqualVector(&tv_temp3);
//			MATH_NegEqualVector(&tv_temp4);
			OBJ_SightGeneralSet(p_SNAKE->pst_GO[i], &tv_temp3, &tv_temp4);
		}
		else
			OBJ_BankingGeneralSet(p_SNAKE->pst_GO[i], &tv_temp4, &tv_temp3);

		MATH_NegEqualVector(&tv_temp);
		MATH_AddEqualVector(&tv_temp, OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i - 1]));
		MATH_CopyVector(OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i]), &tv_temp);
		OBJMY_ComputeLocalWhenHie(p_SNAKE->pst_GO[i]);
	}
#endif
//////////////// RETARD
//////////////// RETARD
//////////////// RETARD
//////////////// RETARD
//////////////// RETARD
//////////////// RETARD
/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierSNAKE2_Apply(MDF_tdst_Modifier * _pst_Mod, GEO_tdst_Object * _pst_Obj)
{
	GAO_tdst_ModifierSNAKE	*p_SNAKE;
	MATH_tdst_Vector		st_Scale;
	float					DT;
	int						i;
	
	DT = TIM_gf_dt;
	if(DT == 0.0f) 
		DT = 1.0f / 50.0f;
	p_SNAKE = (GAO_tdst_ModifierSNAKE *) _pst_Mod->p_Data;
	if(!p_SNAKE->pst_Followed) return;
	OBJ_ComputeGlobalWithLocal(p_SNAKE->pst_Followed, p_SNAKE-> pst_Followed->pst_GlobalMatrix, 1);

	MATH_GetScale(&st_Scale, p_SNAKE->pst_Followed->pst_GlobalMatrix);
	
	p_SNAKE->f_TimeLeft += TIM_gf_dt;
	DT = 1.0f / 50.0f;

	
	while ( p_SNAKE->f_TimeLeft >= DT )
	{
		MATH_tdst_Vector NormalizedNM1,NormalizedNM0;
		p_SNAKE->f_TimeLeft -= DT;
		/* Here local is out */

		for(i = 1; i < p_SNAKE->i_NumBones; i++)
		{
			MATH_tdst_Quaternion Quat1,Quat2,QuatLocal;
			MATH_tdst_Matrix MDst;
			OBJ_tdst_GameObject	*pst_GOm1;

			pst_GOm1 = p_SNAKE->pst_GO[i-1];
			if(!pst_GOm1) continue;
			if(!p_SNAKE->pst_GO[i]) continue;


			// Compute Destination quaternion 

			MATH_TransformVector(&NormalizedNM0 , pst_GOm1->pst_GlobalMatrix , MATH_pst_GetZAxis(p_SNAKE->pst_GO[i]->pst_GlobalMatrix + 1) );
			NormalizedNM1 = *MATH_pst_GetZAxis(pst_GOm1->pst_GlobalMatrix) ;
			MATH_AddVector(&NormalizedNM1, &NormalizedNM1 , &NormalizedNM0);
			if (MATH_f_SqrNormVector(&NormalizedNM1) > 0.000001f) 
			{
				MATH_NormalizeVector(&NormalizedNM1,&NormalizedNM1);
				MATH_CrossProduct((MATH_tdst_Vector *)&Quat1,&NormalizedNM1,&NormalizedNM0);
			} else
				Quat1.x = Quat1.y = Quat1.z = 0.0f;

			/// Compute Actual quaternion 

			NormalizedNM0 = *MATH_pst_GetZAxis(p_SNAKE->pst_GO[i]->pst_GlobalMatrix) ;
			NormalizedNM1 = *MATH_pst_GetZAxis(pst_GOm1->pst_GlobalMatrix) ;
			MATH_AddVector(&NormalizedNM1, &NormalizedNM1 , &NormalizedNM0);
			if (MATH_f_SqrNormVector(&NormalizedNM1) > 0.000001f) 
			{
				MATH_NormalizeVector(&NormalizedNM1,&NormalizedNM1);
				MATH_CrossProduct((MATH_tdst_Vector *)&Quat2,&NormalizedNM1,&NormalizedNM0);
			} else
				Quat2 = Quat1;

			// Absorbtion Model 
//			MATH_BlendVector((MATH_tdst_Vector *)&Quat1,(MATH_tdst_Vector *)&Quat1,(MATH_tdst_Vector *)&Quat2 , p_SNAKE->f_Inertie);

			// Mechanical model 
			{
				MATH_SubVector((MATH_tdst_Vector *)&QuatLocal,(MATH_tdst_Vector *)&Quat1,(MATH_tdst_Vector *)&Quat2);
				MATH_AddScaleVector(&p_SNAKE->QuaternionSpeed[i] , &p_SNAKE->QuaternionSpeed[i] , (MATH_tdst_Vector *)&QuatLocal , DT * p_SNAKE->f_Attenuation);
				if (MATH_f_SqrNormVector(&p_SNAKE->QuaternionSpeed[i]) > 1.0f) 
					MATH_NormalizeVector(&p_SNAKE->QuaternionSpeed[i],&p_SNAKE->QuaternionSpeed[i]);
				MATH_AddVector((MATH_tdst_Vector *)&QuatLocal,(MATH_tdst_Vector *)&Quat2,&p_SNAKE->QuaternionSpeed[i]);
				MATH_ScaleEqualVector(&p_SNAKE->QuaternionSpeed[i],DT * p_SNAKE->f_Inertie);
			}//*/
			
			QuatLocal.w = fSqrt(1.0f - fSqr(QuatLocal.x) - fSqr(QuatLocal.y) - fSqr(QuatLocal.z));
			MATH_ConvertQuaternionToMatrix(&MDst, &QuatLocal);
			
			MATH_Mul33MatrixMatrix(p_SNAKE->pst_GO[i]->pst_GlobalMatrix, pst_GOm1->pst_GlobalMatrix , &MDst , 1);

			MATH_SetScale(p_SNAKE->pst_GO[i]->pst_GlobalMatrix, &st_Scale);

			/*
			f_Zoom = fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
			if ( f_Zoom && (f_Zoom != 1.0f))
				p_SNAKE->af_GODist[i] /= f_Zoom;
			*/


			// Set a good pos 
			MATH_TransformVertex(&(p_SNAKE->pst_GO[i]->pst_GlobalMatrix)->T, pst_GOm1->pst_GlobalMatrix , &(p_SNAKE->pst_GO[i]->pst_GlobalMatrix + 1)->T );
		}//*/



		for(i = 1; i < p_SNAKE->i_NumBones; i++)
		{
			if(!p_SNAKE->pst_GO[i]) continue;
			OBJ_ComputeLocalWhenHie(p_SNAKE->pst_GO[i]);
		}
	}
}


/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierSNAKE_Apply(MDF_tdst_Modifier * _pst_Mod, GEO_tdst_Object * _pst_Obj)
{
	GAO_tdst_ModifierSNAKE	*p_SNAKE;
	float					DT, dist;
	MATH_tdst_Vector		tv_temp;
	MATH_tdst_Vector		tv_temp1;
	MATH_tdst_Vector		tv_temp2;
	MATH_tdst_Vector		tv_temp3;
	MATH_tdst_Vector		tv_temp4;
	float					f_norm;
	float					ff1, ff, f_Zoom;
	int						i;
	extern void AI_EvalFunc_MATHVecBlendRotate_C(MATH_tdst_Vector *, MATH_tdst_Vector *, float, MATH_tdst_Vector *);



	p_SNAKE = (GAO_tdst_ModifierSNAKE *) _pst_Mod->p_Data;
	if (p_SNAKE ->ul_Flags )
	{
		GAO_ModifierSNAKE2_Apply(_pst_Mod, _pst_Obj);
		return;//*/
	}

	DT = TIM_gf_dt;
	if(DT == 0.0f) DT = 1.0f / 50.0f;

	if(!p_SNAKE->pst_Followed) return;
	OBJ_ComputeGlobalWithLocal(p_SNAKE->pst_Followed, p_SNAKE->pst_Followed->pst_GlobalMatrix, 1);
	
	p_SNAKE->f_TimeLeft += TIM_gf_dt;
	DT = 1.0f / 50.0f;
	
	while ( p_SNAKE->f_TimeLeft >= DT )
	{
		p_SNAKE->f_TimeLeft -= DT;
		ff1 = 0;
		for(i = 1; i < p_SNAKE->i_NumBones; i++)
		{
			if(!p_SNAKE->pst_GO[i - 1]) return;
			if(!p_SNAKE->pst_GO[i]) return;

			if(_pst_Mod->ul_Flags & SNAKE_TAKEY)
			{
				MATH_CopyVector(&tv_temp1, MATH_pst_GetZAxis(p_SNAKE->pst_GO[i - 1]->pst_GlobalMatrix));
				MATH_CopyVector(&tv_temp2, MATH_pst_GetZAxis(p_SNAKE->pst_GO[i]->pst_GlobalMatrix));
			}
			else
			{
				MATH_NegVector(&tv_temp1, MATH_pst_GetYAxis(p_SNAKE->pst_GO[i - 1]->pst_GlobalMatrix));
				MATH_NegVector(&tv_temp2, MATH_pst_GetYAxis(p_SNAKE->pst_GO[i]->pst_GlobalMatrix));
			}
			
			dist = p_SNAKE->af_GODist[i];
			MATH_GetScale(&tv_temp4, p_SNAKE->pst_GO[i]->pst_GlobalMatrix );
			f_Zoom = fMax3(tv_temp4.x, tv_temp4.y, tv_temp4.z);
			if ( f_Zoom )
				dist *= f_Zoom;
			{
				static float Grotesk = 1.f;
				AI_EvalFunc_MATHVecBlendRotate_C(&tv_temp2, &tv_temp1, p_SNAKE->f_Inertie * DT * Grotesk, &tv_temp3);
				MATH_CopyVector(&tv_temp1, &tv_temp3);
			}

			MATH_SubVector(&tv_temp, OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i]), OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i - 1]));
			f_norm = MATH_f_NormVector(&tv_temp);

			if(i >= 2)
				MATH_SubVector(&tv_temp2, OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i - 1]), OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i - 2]));
			else if(_pst_Mod->ul_Flags & SNAKE_TAKEY)
				MATH_CopyVector(&tv_temp2, MATH_pst_GetYAxis(p_SNAKE->pst_GO[i - 1]->pst_GlobalMatrix));
			else
				MATH_CopyVector(&tv_temp2, MATH_pst_GetZAxis(p_SNAKE->pst_GO[i - 1]->pst_GlobalMatrix));

			//MATH_SetNormVector(&tv_temp3, &tv_temp2, p_SNAKE->af_GODist[i]);
			MATH_SetNormVector(&tv_temp3, &tv_temp2, dist );
			MATH_CopyVector(&tv_temp2, &tv_temp3);

			{
				static float Grotesk = 1.f;
				AI_EvalFunc_MATHVecBlendRotate_C(&tv_temp, &tv_temp2, (p_SNAKE->f_Inertie + ff1) * DT * Grotesk, &tv_temp3);
			}
			MATH_CopyVector(&tv_temp, &tv_temp3);
			ff1 += p_SNAKE->f_Attenuation;
			if(p_SNAKE->f_Inertie + ff1 < 0) ff1 = -p_SNAKE->f_Inertie;

			//ff = MATH_f_FloatBlend(p_SNAKE->af_GODist[i], f_norm, p_SNAKE->f_BlendDist * DT);
			ff = MATH_f_FloatBlend(dist, f_norm, p_SNAKE->f_BlendDist * DT);
			MATH_SetNormVector(&tv_temp3, &tv_temp, ff);
			MATH_CopyVector(&tv_temp, &tv_temp3);

			MATH_NormalizeVector(&tv_temp3, &tv_temp);
			MATH_NormalizeVector(&tv_temp4, &tv_temp1);

			if(_pst_Mod->ul_Flags & SNAKE_TAKEY)
			{
				MATH_NegEqualVector(&tv_temp3);
				OBJ_SightGeneralSet(p_SNAKE->pst_GO[i], &tv_temp3, &tv_temp4);
			}
			else
				OBJ_BankingGeneralSet(p_SNAKE->pst_GO[i], &tv_temp4, &tv_temp3);

			MATH_AddEqualVector(&tv_temp, OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i - 1]));
			MATH_CopyVector(OBJ_pst_GetAbsolutePosition(p_SNAKE->pst_GO[i]), &tv_temp);

			{
				MATH_tdst_Vector	S;
				MATH_GetScale(&S, &p_SNAKE->pst_GO[i]->pst_Base->pst_Hierarchy->st_LocalMatrix);
				OBJMY_ComputeLocalWhenHie(p_SNAKE->pst_GO[i]);
				MATH_SetScale(&p_SNAKE->pst_GO[i]->pst_Base->pst_Hierarchy->st_LocalMatrix, &S);
			}
		}
	}
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierSNAKE_Unapply(MDF_tdst_Modifier * _pst_Mod, GEO_tdst_Object * _pst_Obj)
{
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
void GAO_ModifierSNAKE_Reinit(MDF_tdst_Modifier * _pst_Mod)
{
	GAO_ModifierSNAKE_UpdatePointers(_pst_Mod);
}
#if defined(XML_CONV_TOOL)
extern ULONG gGaoMdfSnakeVersion;
#endif
/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
ULONG GAO_ModifierSNAKE_Load (MDF_tdst_Modifier * _pst_Mod, char *_pc_Buffer)
{
	int ulVersion;
	GAO_tdst_ModifierSNAKE * p_SNAKE;
	char *_pc_BufferSave;

	_pc_BufferSave = _pc_Buffer;
	p_SNAKE = (GAO_tdst_ModifierSNAKE *) _pst_Mod->p_Data;

	/* size */
	ulVersion = LOA_ReadLong(&_pc_Buffer); // skip size
#if defined(XML_CONV_TOOL)
	gGaoMdfSnakeVersion = ulVersion;
#endif
	p_SNAKE->i_NumBones = LOA_ReadInt(&_pc_Buffer);
	p_SNAKE->f_Inertie = LOA_ReadFloat(&_pc_Buffer);
	p_SNAKE->f_BlendDist = LOA_ReadFloat(&_pc_Buffer);
	p_SNAKE->f_Attenuation = LOA_ReadFloat(&_pc_Buffer);
	{
		ULONG i;
		INT * piBones;
		for(i = 0, piBones = p_SNAKE->ai_Bones; i < SNAKE_MXBONES; ++i, ++piBones)
		{
			*piBones = LOA_ReadInt(&_pc_Buffer);
		}
	}//*/
	p_SNAKE->ul_Flags = 0;
	if (ulVersion > 8)
	{
		p_SNAKE->ul_Flags = LOA_ReadInt(&_pc_Buffer);
	}
	if (ulVersion > 9)
	{
	}


	return _pc_Buffer - _pc_BufferSave;
}

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#ifdef ACTIVE_EDITORS
void GAO_ModifierSNAKE_Save (MDF_tdst_Modifier * _pst_Mod)
{
	GAO_tdst_ModifierSNAKE *p_SNAKE;
	ULONG ulSize;

	p_SNAKE = (GAO_tdst_ModifierSNAKE *) _pst_Mod->p_Data;

	/* Save Size */
	/* Philippe == Version number pour snake 2 */
	ulSize = 9;
#if defined(XML_CONV_TOOL)
	ulSize = gGaoMdfSnakeVersion;
#endif
	SAV_Buffer(&ulSize, 4);

	SAV_Buffer(&p_SNAKE->i_NumBones, 4);
	SAV_Buffer(&p_SNAKE->f_Inertie, 4);
	SAV_Buffer(&p_SNAKE->f_BlendDist, 4);
	SAV_Buffer(&p_SNAKE->f_Attenuation, 4);
	SAV_Buffer(&p_SNAKE->ai_Bones, SNAKE_MXBONES * 4);
#if defined(XML_CONV_TOOL)
	if (gGaoMdfSnakeVersion >= 9)
	{
		/* Snake 2 */
		SAV_Buffer(&p_SNAKE->ul_Flags, 4);
	}
#else
	/* Snake 2 */
	SAV_Buffer(&p_SNAKE->ul_Flags, 4);
#endif
}
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
