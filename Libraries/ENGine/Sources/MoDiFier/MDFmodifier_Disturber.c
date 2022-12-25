/*$T MDFModifier_Disturber.c GC! 1.081 06/09/00 09:00:51 */


/*$6
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*/


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"

#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_Disturber.h"
#include "GEOmetric/GEOobject.h"
#include "PArticleGenerator/PAGstruct.h"

#include "BASe/BENch/BENch.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#include "GEOmetric/GEO_SKIN.h"

// candidates for disturbances
#include "MoDiFier/MDFmodifier_WATER3D.h"
#include "ANImation/ANIstruct.h"
#include "OBJects/OBJorient.h"
#include "OBJects/OBJaccess.h"
#include "DYNamics/DYNaccess.h"
//#include "PArticleGenerator/PAGDispersion.h"

#define MATH_Max fMax
#define MATH_Min fMin

/*
=======================================================================================================================
=======================================================================================================================
*/
void Disturber_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	Disturber_tdst_Modifier *pst_Data;
	
	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(Disturber_tdst_Modifier));
	pst_Data = (Disturber_tdst_Modifier *) _pst_Mod->p_Data;
	
	if(!p_Data)
	{
		L_memset(pst_Data , 0 , sizeof(Disturber_tdst_Modifier));

		//pst_Data->nbDisturber = 0;
		pst_Data->fDisturbBoost = 1.0f;
		//pst_Data->bStaticDisturber = FALSE;
		pst_Data->fStaticDisturberForce = 1.0f;
		//pst_Data->fStaticDisturberDelay = 0.0f;
        pst_Data->fZOffset = 0.0f;

		//pst_Data->bFloatOnWater = FALSE;
		//pst_Data->vFloatOnWaterInitialVelocity = 0,0,0;
		//pst_Data->vFloatOnWaterVelocity = 0,0,0;
		//pst_Data->fFloatOnWaterInitialZRotationSpeed = 0.0f;
		//pst_Data->vFloatOnWaterPositionOffset = 0.0f

		pst_Data->fFloatOnWaterBankingDamping = 0.005f;
		pst_Data->fFloatOnWaterZStability = 0.8f;
		pst_Data->fFloatOnWaterWaveStrength = 1.0f;
		pst_Data->fFloatOnWaterEvaluationPlaneDelta=0.25f;
		pst_Data->fFloatOnWaterVelocityBackToInitialStrength=0.1f;
		pst_Data->fFloatOnWaterWaveInfluence = 0.025f;
		pst_Data->vFloatOnWaterVelocityDamping = 0.98f;


		/*pst_Data->bGenParticles = FALSE;
		pst_Data->bPonderateParticulesWithForce1 = FALSE;
		pst_Data->pParticuleGenerator1 = NULL;
		pst_Data->nbParticules1 = 0;
		pst_Data->bPonderateParticulesWithForce2 = FALSE;
		pst_Data->pParticuleGenerator2 = NULL;
		pst_Data->nbParticules2 = 0;
		pst_Data->bPonderateParticulesWithForce3 = FALSE;
		pst_Data->pParticuleGenerator3 = NULL;
		pst_Data->nbParticules3 = 0;*/
	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(Disturber_tdst_Modifier));
	}	

	_pst_Mod-> ul_Flags = MDF_C_Modifier_ApplyGao;

	MATH_CopyMatrix(&pst_Data->stFloatOnWaterInitialPosition, _pst_GO->pst_GlobalMatrix);

	pst_Data->ar_vDisturberWorldPos = NULL;
	pst_Data->ar_fDisturbances = NULL;
	pst_Data->fCurDelay = 0.0f;
	pst_Data->bActive = FALSE;
	//pst_Data->fCurParticleDelay = 0.0f;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void Disturber_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	Disturber_tdst_Modifier *pst_Data;
	
	pst_Data = (Disturber_tdst_Modifier *) _pst_Mod->p_Data;	
	if(!pst_Data) 
		return;

	if(pst_Data->ar_vDisturberWorldPos)
		MEM_Free(pst_Data->ar_vDisturberWorldPos);

	if(pst_Data->ar_fDisturbances)
		MEM_Free(pst_Data->ar_fDisturbances);

	MEM_Free(pst_Data);
	_pst_Mod->p_Data = NULL;
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void Disturber_Modifier_Apply( MDF_tdst_Modifier *_pst_Mod,	GEO_tdst_Object	*_pst_Obj )
{
	Disturber_tdst_Modifier *pst_Data = (Disturber_tdst_Modifier *) _pst_Mod->p_Data;
	MATH_tdst_Vector vec;

	if(!pst_Data->bActive)
    {
        return;
    }
		
    //_GSP_BeginRaster(Raster_MdfDisturber);

	if(pst_Data->nbDisturber==3) // actors or floating objects
	{
		/*TAB_tdst_PFelem* pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(_pst_Mod->pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
		const TAB_tdst_PFelem* const pst_EndBone = TAB_pst_PFtable_GetLastElem(_pst_Mod->pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);

		for (UCHAR i=0; i<pst_Data->nbDisturber; i++, pst_CurrentBone++)
		{
			if (i>=static_cast<UCHAR>(_pst_Mod->pst_GO->pst_Base->pst_AddMatrix->l_Number))
				break;

			MATH_SubVector(&vec, &pst_Data->ar_vDisturberWorldPos[i], MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject *)pst_CurrentBone->p_Pointer)));
			pst_Data->ar_fDisturbances[i] = MATH_f_NormVector(&vec) * pst_Data->fDisturbBoost;
			if((vec.z > 2*vec.x) && (vec.z > 2*vec.y))
				pst_Data->ar_fDisturbances[i] += fAbs(vec.z)*pst_Data->fDisturbBoost*5.0f;

			// get position of possibly colliding gameobject
			MATH_CopyVector(&pst_Data->ar_vDisturberWorldPos[i], MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix((OBJ_tdst_GameObject *)pst_CurrentBone->p_Pointer)));
		}*/

		MATH_SubVector(&vec, &pst_Data->ar_vDisturberWorldPos[0], MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO)));
        pst_Data->ar_fDisturbances[0] = MATH_f_NormVector(&vec) * pst_Data->fDisturbBoost;

		// vertical movement perturbation boost (jump)
#ifndef PSX2_TARGET 		
		if((fAbs(vec.z) > 2.0f*fAbs(vec.x)) && (fAbs(vec.z) > 2.0f*fAbs(vec.y)) && (fAbs(vec.z) > 0.1f))
			pst_Data->ar_fDisturbances[0] += fAbs(vec.z)*pst_Data->fDisturbBoost*2.0f;
#else		
		if((fAbs(vec.z) > 2.0f*fAbs(vec.x)) && (fAbs(vec.z) > 2.0f*fAbs(vec.y)))
			pst_Data->ar_fDisturbances[0] += fAbs(vec.z)*pst_Data->fDisturbBoost*2.0f;
#endif

        MATH_CopyVector(&pst_Data->ar_vDisturberWorldPos[0], MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO)));

        MATH_tdst_Vector b, c;

        // equilateral distribution of prturbation (else it is too low)
		if(pst_Data->bFloatOnWater)
		{
			b.x = pst_Data->fFloatOnWaterEvaluationPlaneDelta;
            b.y = -pst_Data->fFloatOnWaterEvaluationPlaneDelta;
            b.z = 0.0f;

			c.x = -pst_Data->fFloatOnWaterEvaluationPlaneDelta;
            c.y = -pst_Data->fFloatOnWaterEvaluationPlaneDelta;
            c.z = 0.0f;
		}
		else
		{
			b.x = 0.05f;
            b.y = -0.05f;
            b.z = 0.0f;

			c.x = -0.05f;
            c.y = -0.05f;
            c.z = 0.0f;
		}
		
        MATH_AddVector(&pst_Data->ar_vDisturberWorldPos[1], &pst_Data->ar_vDisturberWorldPos[0], &c);
        MATH_AddVector(&pst_Data->ar_vDisturberWorldPos[2], &pst_Data->ar_vDisturberWorldPos[0], &b);

        pst_Data->ar_fDisturbances[1] = pst_Data->ar_fDisturbances[0];
		pst_Data->ar_fDisturbances[2] = pst_Data->ar_fDisturbances[0];
	}
	else // disturb with bottom center of GAO
	{
		if(pst_Data->bStaticDisturber)
		{
			pst_Data->fCurDelay += TIM_gf_dt;
			if(pst_Data->fStaticDisturberDelay<pst_Data->fCurDelay)
			{
				pst_Data->fCurDelay -= pst_Data->fStaticDisturberDelay;
				pst_Data->ar_fDisturbances[0] = fRand(-pst_Data->fStaticDisturberForce, pst_Data->fStaticDisturberForce);
			}
			else
			{
				//_GSP_EndRaster(Raster_MdfDisturber);
				return;
			}
			

            MATH_CopyVector(&pst_Data->ar_vDisturberWorldPos[0], MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO)));
		}
		else
		{
/*
            DYN_tdst_Dyna   *pst_Dyna;
            DYN_tdst_Solid  *pst_Solid;
            
            pst_Dyna = OBJ_pst_GetDynaStruct(_pst_Mod->pst_GO);

            if (pst_Dyna && (pst_Solid = DYN_pst_SolidGet(pst_Dyna)) != NULL)
            {
                MATH_tdst_Matrix * pst_M = OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO);
                MATH_tdst_Vector   st_Pos;
                MATH_tdst_Vector   st_GPos;

                MATH_CopyVector(&st_Pos, &pst_Solid->st_CenterOfMass);

                if (fAbs(pst_M->Iz) > fAbs(pst_M->Jz))
                {
                    if (fAbs(pst_M->Iz) > fAbs(pst_M->Kz))
                    {
                        FLOAT f_dx =  0.5f*L_sqrt(1.5f*DYN_f_GetInvMass(pst_Dyna)*((pst_Solid->st_Ibody.Kz - pst_Solid->st_Ibody.Ix) + pst_Solid->st_Ibody.Jy));
                        if (pst_M->Iz >= 0.0f)
                        {
                            st_Pos.x -= f_dx;
                        }
                        else
                        {
                            st_Pos.x += f_dx;
                        }
                    }
                    else
                    {
                        FLOAT f_dz =  0.5f*L_sqrt(1.5f*DYN_f_GetInvMass(pst_Dyna)*((pst_Solid->st_Ibody.Ix - pst_Solid->st_Ibody.Kz) + pst_Solid->st_Ibody.Jy));
                        if (pst_M->Kz >= 0.0f)
                        {
                            st_Pos.z -= f_dz;
                        }
                        else
                        {
                            st_Pos.z += f_dz;
                        }
                    }
                }
                else if (fAbs(pst_M->Jz) > fAbs(pst_M->Kz))
                {
                    FLOAT f_dy =  0.5f*L_sqrt(1.5f*DYN_f_GetInvMass(pst_Dyna)*((pst_Solid->st_Ibody.Kz - pst_Solid->st_Ibody.Jy) + pst_Solid->st_Ibody.Ix));
                    if (pst_M->Jz >= 0.0f)
                    {
                        st_Pos.y -= f_dy;
                    }
                    else
                    {
                        st_Pos.y += f_dy;
                    }
                }
                else
                {
                    FLOAT f_dz =  0.5f*L_sqrt(1.5f*DYN_f_GetInvMass(pst_Dyna)*((pst_Solid->st_Ibody.Ix - pst_Solid->st_Ibody.Kz) + pst_Solid->st_Ibody.Jy));
                    if (pst_M->Kz >= 0.0f)
                    {
                        st_Pos.z -= f_dz;
                    }
                    else
                    {
                        st_Pos.z += f_dz;
                    }
                }
                
                MATH_TransformVertex(&st_GPos, pst_M, &st_Pos);
                MATH_SubVector(&vec, &pst_Data->ar_vDisturberWorldPos[0], &st_GPos);
                pst_Data->ar_fDisturbances[0] = MATH_f_NormVector(&vec) * pst_Data->fDisturbBoost;

				if((vec.z > 2*vec.x) && (vec.z > 2*vec.y))
					pst_Data->ar_fDisturbances[0] += fAbs(vec.z)*pst_Data->fDisturbBoost*5.0f;

                MATH_CopyVector(&pst_Data->ar_vDisturberWorldPos[0], &st_GPos);
            }
            else*/
            {
                MATH_SubVector(&vec, &pst_Data->ar_vDisturberWorldPos[0], MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO)));
                pst_Data->ar_fDisturbances[0] = MATH_f_NormVector(&vec) * pst_Data->fDisturbBoost;

				// vertical movement perturbation boost (jump)
				if((fAbs(vec.z) > 2.0f*fAbs(vec.x)) && (fAbs(vec.z) > 2.0f*fAbs(vec.y)))
					pst_Data->ar_fDisturbances[0] += fAbs(vec.z)*pst_Data->fDisturbBoost*5.0f;
					
                MATH_CopyVector(&pst_Data->ar_vDisturberWorldPos[0], MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO)));
            }
		}

		
	}

	// apply disturbances to candidates
	MATH_tdst_Vector vMin, vMax;
	WATER3D_Modifier_Disturb(_pst_Mod->pst_GO, pst_Data->ar_vDisturberWorldPos, pst_Data->ar_fDisturbances, pst_Data->nbDisturber, pst_Data->fZOffset, pst_Data->bFloatOnWater, &vMin, &vMax);

	// floating object dynamic
	if(pst_Data->bFloatOnWater)
	{
		// compute rotation / banking using water waves inclination
		MATH_tdst_Vector vPlaneA, vPlaneB;
		MATH_tdst_Vector vOldPos;
		MATH_SubVector(&vPlaneA, &pst_Data->ar_vDisturberWorldPos[1], &pst_Data->ar_vDisturberWorldPos[0]);
		MATH_SubVector(&vPlaneB, &pst_Data->ar_vDisturberWorldPos[2], &pst_Data->ar_vDisturberWorldPos[0]);
		MATH_tdst_Matrix stPlane;
		MATH_tdst_Vector  vSight = {0,1,0};
		MATH_tdst_Vector vBanking;
		MATH_CrossProduct( &vBanking, &vPlaneA, &vPlaneB );
		MATH_BlendVector(&pst_Data->vFloatOnWaterBanking, &pst_Data->vFloatOnWaterBanking, &vBanking, pst_Data->fFloatOnWaterBankingDamping);
		MATH_MakeRotationMatrix_UsingBanking(&stPlane, &vSight, &pst_Data->vFloatOnWaterBanking, 1);
		MATH_CopyVector(&vOldPos, MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO)));
		MATH_MulMatrixMatrix(_pst_Mod->pst_GO->pst_GlobalMatrix, &stPlane, &pst_Data->stFloatOnWaterInitialPosition);

		// a little z rotation to break the pattern
		if(pst_Data->fFloatOnWaterInitialZRotationSpeed != 0.0f)
		{
			MATH_tdst_Matrix stRotationY;
			MATH_tdst_Vector  vAxis = {0,0,1};
			MATH_MakeRotationMatrix_AxisAngle( &stRotationY, &vAxis, pst_Data->fFloatOnWaterYRotation, 0, 1 );
			pst_Data->fFloatOnWaterYRotation += pst_Data->fFloatOnWaterInitialZRotationSpeed*(60.0f*TIM_gf_dt);
			if(pst_Data->fFloatOnWaterYRotation>Cf_2Pi)
				pst_Data->fFloatOnWaterYRotation -= Cf_2Pi;
			MATH_tdst_Matrix stOldMatrix;
			MATH_CopyMatrix(&stOldMatrix, _pst_Mod->pst_GO->pst_GlobalMatrix);
			MATH_MulMatrixMatrix(_pst_Mod->pst_GO->pst_GlobalMatrix, &stRotationY, &stOldMatrix);
		}

		// move object when a wave push it
		vBanking.z = 0.0f;
		MATH_ScaleVector(&vBanking, &vBanking, pst_Data->fFloatOnWaterWaveStrength);
		MATH_BlendVector(&pst_Data->vFloatOnWaterVelocity, &pst_Data->vFloatOnWaterVelocity, &vBanking, pst_Data->fFloatOnWaterWaveInfluence);
		MATH_ScaleVector(&pst_Data->vFloatOnWaterVelocity, &pst_Data->vFloatOnWaterVelocity, pst_Data->vFloatOnWaterVelocityDamping);
		MATH_BlendVector(&pst_Data->vFloatOnWaterVelocity, &pst_Data->vFloatOnWaterVelocity, &pst_Data->vFloatOnWaterInitialVelocity, pst_Data->fFloatOnWaterVelocityBackToInitialStrength);

		// translation adjustments
		MATH_tdst_Vector vNewPos, vDeltaPos;
		FLOAT fPreviousZ = vOldPos.z;		
		MATH_ScaleVector(&vDeltaPos, &pst_Data->vFloatOnWaterVelocity, (60.0f*TIM_gf_dt));
		MATH_AddVector(&vNewPos, &vDeltaPos, &vOldPos);//&pst_Data->ar_vDisturberWorldPos[0]);
		vNewPos.z = pst_Data->ar_vDisturberWorldPos[0].z + pst_Data->fFloatOnWaterZOffset;
		vNewPos.z = (fPreviousZ*pst_Data->fFloatOnWaterZStability)+(vNewPos.z*(1.0f-pst_Data->fFloatOnWaterZStability));

		// invisible wall
		if(pst_Data->bUseWaterBoundingBoxForCollision)
		{
			vNewPos.x = MATH_Max(vMin.x, MATH_Min(vMax.x, vNewPos.x));
			vNewPos.y = MATH_Max(vMin.y, MATH_Min(vMax.y, vNewPos.y));
			//vNewPos.z = MATH_Max(pvMin->z, MATH_Min(pvMax->z, vNewPos.z));
		}

		MATH_SetTranslation(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO), &vNewPos);
		//MATH_SetTranslation(&pst_Data->stFloatOnWaterInitialPosition, &vNewPos);
	}

/*
    // ... may add other disturbed objects here (smoke, ...)
    if (pst_Data->bPAGDispersionActive)
    {
        TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
        OBJ_tdst_GameObject *pst_GO;
        PAG_tdst_Disturber   st_PAGDisturber;

        st_PAGDisturber.fRadius = pst_Data->fRadius;
        st_PAGDisturber.fFrontForceTrans = pst_Data->fFrontForceTrans;
        st_PAGDisturber.fFrontForceRot = pst_Data->fFrontForceRot;
        st_PAGDisturber.fRearForceTrans = pst_Data->fRearForceTrans;
        st_PAGDisturber.fRearForceRot = pst_Data->fRearForceRot;
        st_PAGDisturber.fDraftNear = pst_Data->fDraftNear;
        st_PAGDisturber.fDraftFar = pst_Data->fDraftFar;


        st_PAGDisturber.st_Velocity.x = vec.x;
        st_PAGDisturber.st_Velocity.y = vec.y;
        st_PAGDisturber.st_Velocity.z = pst_Data->iIntensity;

        // Slicing effect
        if (_pst_Mod->pst_GO->uc_DesignFlags & OBJ_C_DesignFlag_Projectile)
        {
            st_PAGDisturber.st_Velocity = *(MATH_pst_GetZAxis(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO)));
            st_PAGDisturber.st_Velocity.z = -1.0f;
        }

        // Reset explosion effect
        if (pst_Data->iIntensity != 0)
            pst_Data->iIntensity = 0;

        st_PAGDisturber.f_Speed = pst_Data->ar_fDisturbances[0];

        // All PAGs with dispersion are put in a seperate list at load time
        pst_Elem = TAB_pst_PFtable_GetFirstElem(&WOR_gpst_CurrentWorld->st_PAGs);
        pst_LastElem = TAB_pst_PFtable_GetLastElem(&WOR_gpst_CurrentWorld->st_PAGs);

        for(; pst_Elem <= pst_LastElem; pst_Elem++)
        {
            pst_GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;

            if(TAB_b_IsAHole(pst_GO)) 
                continue;

            if (OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
            {
                GRO_tdst_Visu *pst_Visu = pst_GO->pst_Base->pst_Visu;

                if (pst_Visu->pst_Object &&
                    pst_Visu->pst_Object->i->ul_Type == GRO_ParticleGenerator &&
                    pst_Visu->pParticlesBuffer &&
                    pst_Visu->pParticlesBuffer->ast_Gen)
                {
                    PAG_tdst_Struct* pst_P = (PAG_tdst_Struct *) pst_Visu->pst_Object;

                    for (int i = 0; i < pst_GO->pst_Base->pst_Visu->pParticlesBuffer->l_NbPools; i++)
                    {
                        if (pst_P->ul_MoreFlags & PAG_MFlags_Dispersion)
                        {
                            if (_pst_Mod->pst_GO)
                            {
                                OBJ_tdst_SingleBV* pst_A_BV = (OBJ_tdst_SingleBV*) pst_GO->pst_BV;
                                MATH_tdst_Vector st_GlobalMin, st_GlobalMax, st_Point;
                                MATH_CopyVector(&st_Point, OBJ_pst_GetAbsolutePosition(_pst_Mod->pst_GO));
                                MATH_AddVector(&st_GlobalMin, &pst_A_BV->st_GMin, OBJ_pst_GetAbsolutePosition(pst_GO));
                                MATH_AddVector(&st_GlobalMax, &pst_A_BV->st_GMax, OBJ_pst_GetAbsolutePosition(pst_GO));

                                if (fInf (st_GlobalMin.x, st_Point.x) && fSup (st_GlobalMax.x, st_Point.x) &&
                                    fInf (st_GlobalMin.y, st_Point.y) && fSup (st_GlobalMax.y, st_Point.y) &&
                                    fInf (st_GlobalMin.z, st_Point.z) && fSup (st_GlobalMax.z, st_Point.z))
                                {
                                    PAG_tdst_Generator* pst_Gen = &pst_Visu->pParticlesBuffer->ast_Gen[i];
                                    MATH_CopyVector(&st_PAGDisturber.st_Position, OBJ_pst_GetAbsolutePosition(_pst_Mod->pst_GO));

                                    // Add offset in axis direction
                                    if (_pst_Mod->pst_GO->uc_DesignFlags & OBJ_C_DesignFlag_Projectile)
                                    {
                                        st_PAGDisturber.st_Normal = *(MATH_pst_GetXAxis(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO)));
                                    }

                                    PAG_RegisterDisturber(pst_Gen, _pst_Mod->pst_GO, &st_PAGDisturber);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
*/
	// actual active perturbation detection
	/*if(pst_Data->bGenParticles)
	{
		float fPond;
		pst_Data->fCurParticleDelay += TIM_gf_dt;
		// tune particle generation for 1/60th second
		if(pst_Data->fCurParticleDelay>=0.016667f) 
		{
			fPond = pst_Data->fCurParticleDelay/0.016667f;
			pst_Data->fCurParticleDelay -= fPond*0.016667f;
			for(int i=0; i<pst_Data->nbDisturber; i++)
			{
				if(pst_Data->ar_fDisturbances[i] > 500.0f)
				{
					pst_Data->ar_fDisturbances[i] -= 1000.0f; // remove value added for detection purpose

					if(pst_Data->pParticuleGenerator1 && pst_Data->nbParticules1>0)
					{
						// generate particules
						int nbParticules = fPond * pst_Data->nbParticules1 * (pst_Data->bPonderateParticulesWithForce1 ? fAbs(pst_Data->ar_fDisturbances[0]) : 1);
						if(nbParticules)
						{
							MATH_CopyVector(MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(pst_Data->pParticuleGenerator1)), &pst_Data->ar_vDisturberWorldPos[i]);
							PAG_Generate(pst_Data->pParticuleGenerator1, 
										(PAG_tdst_Struct*)pst_Data->pParticuleGenerator1->pst_Base->pst_Visu->pst_Object, 
										pst_Data->pParticuleGenerator1->pst_Base->pst_Visu->pParticlesBuffer, 
										TIM_gf_dt, 
										nbParticules, 
										true); 
						}
					}

					if(pst_Data->pParticuleGenerator2 && pst_Data->nbParticules2>0)
					{
						// generate particules
						int nbParticules = fPond * pst_Data->nbParticules2 * (pst_Data->bPonderateParticulesWithForce2 ? fAbs(pst_Data->ar_fDisturbances[0]) : 1);
						if(nbParticules)
						{
							MATH_CopyVector(MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(pst_Data->pParticuleGenerator2)), &pst_Data->ar_vDisturberWorldPos[i]);
							PAG_Generate(pst_Data->pParticuleGenerator2, 
										(PAG_tdst_Struct*)pst_Data->pParticuleGenerator2->pst_Base->pst_Visu->pst_Object, 
										pst_Data->pParticuleGenerator2->pst_Base->pst_Visu->pParticlesBuffer, 
										TIM_gf_dt, 
										nbParticules, 
										true); 
						}
					}

					if(pst_Data->pParticuleGenerator3 && pst_Data->nbParticules3>0)
					{
						// generate particules
						int nbParticules = fPond * pst_Data->nbParticules3 * (pst_Data->bPonderateParticulesWithForce3 ? fAbs(pst_Data->ar_fDisturbances[0]) : 1);
						if(nbParticules)
						{
							MATH_CopyVector(MATH_pst_GetTranslation(OBJ_pst_GetAbsoluteMatrix(pst_Data->pParticuleGenerator3)), &pst_Data->ar_vDisturberWorldPos[i]);
							PAG_Generate(pst_Data->pParticuleGenerator3, 
										(PAG_tdst_Struct*)pst_Data->pParticuleGenerator3->pst_Base->pst_Visu->pst_Object, 
										pst_Data->pParticuleGenerator3->pst_Base->pst_Visu->pParticlesBuffer, 
										TIM_gf_dt, 
										nbParticules, 
										true); 
						}
					}
				}
			}
		}
	}*/
	
	//_GSP_EndRaster(Raster_MdfDisturber);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
void Disturber_Modifier_Unapply( MDF_tdst_Modifier *_pst_Mod,	GEO_tdst_Object	*_pst_Obj )
{ 
	Disturber_tdst_Modifier *pst_Data = (Disturber_tdst_Modifier *) _pst_Mod->p_Data;

	if(!pst_Data->bActive)
	{
		// animation object with bones (actors)
		if(((OBJ_ul_FlagsIdentityGet(_pst_Mod->pst_GO) & (OBJ_C_IdentityFlag_Dyna|OBJ_C_IdentityFlag_Anims)) == (OBJ_C_IdentityFlag_Dyna|OBJ_C_IdentityFlag_Anims)) 
			&& _pst_Mod->pst_GO->pst_Base->pst_GameObjectAnim && _pst_Mod->pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton && _pst_Mod->pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects)
		{
			//TAB_tdst_PFelem* pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(_pst_Mod->pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);
			//const TAB_tdst_PFelem* const pst_EndBone = TAB_pst_PFtable_GetLastElem(_pst_Mod->pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton->pst_AllObjects);

			pst_Data->nbDisturber = 3;//((pst_EndBone-pst_CurrentBone)+1);

			//if(pst_Data->nbDisturber<=0)
				//pst_Data->nbDisturber = 1;
		}
		// other... take center of object
		else
		{
            if(pst_Data->bFloatOnWater || (!OBJ_pst_GetDynaStruct(_pst_Mod->pst_GO) && !(pst_Data->bStaticDisturber)))
				pst_Data->nbDisturber = 3;
			else
				pst_Data->nbDisturber = 1;
		}

		if(pst_Data->ar_vDisturberWorldPos)
			MEM_Free(pst_Data->ar_vDisturberWorldPos);

		if(pst_Data->ar_fDisturbances)
			MEM_Free(pst_Data->ar_fDisturbances);

		pst_Data->ar_vDisturberWorldPos = (MATH_tdst_Vector*)MEM_p_Alloc(sizeof(MATH_tdst_Vector)*pst_Data->nbDisturber);
		L_memset(pst_Data->ar_vDisturberWorldPos , 0 , sizeof(MATH_tdst_Vector)*pst_Data->nbDisturber);

		pst_Data->ar_fDisturbances = (FLOAT*)MEM_p_Alloc(sizeof(FLOAT)*pst_Data->nbDisturber);
		L_memset(pst_Data->ar_fDisturbances , 0 , sizeof(FLOAT)*pst_Data->nbDisturber);

		pst_Data->bActive = TRUE;
	}
}

void Disturber_Modifier_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
	Disturber_tdst_Modifier *pst_Data = (Disturber_tdst_Modifier *) _pst_Mod->p_Data;

	if(!pst_Data) return;

    if (pst_Data->bFloatOnWater)
        MATH_CopyMatrix(OBJ_pst_GetAbsoluteMatrix(_pst_Mod->pst_GO), &pst_Data->stFloatOnWaterInitialPosition);
}

/*
=======================================================================================================================
=======================================================================================================================
*/
#if defined(XML_CONV_TOOL)
ULONG gGaoDisturberVersion;
#endif

ULONG Disturber_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	Disturber_tdst_Modifier			*pst_Data;
	ULONG							ul_Version;
	unsigned char					*pc_Cur;
	ULONG							ulKey;

	pc_Cur = (UCHAR*)_pc_Buffer;
	pst_Data = (Disturber_tdst_Modifier *) _pst_Mod->p_Data;
	
	ul_Version = LOA_ReadLong((CHAR**)&pc_Cur);
#if defined(XML_CONV_TOOL)
	gGaoDisturberVersion = ul_Version;
#endif
	pst_Data->fDisturbBoost = LOA_ReadFloat((CHAR**)&pc_Cur);

	// overide because we no longer use bones to disturb
/*#if defined(_GAMECUBE)
	pst_Data->fDisturbBoost = 2.0f;
#elif defined(_XBOX) || defined (PC_TARGET)
	pst_Data->fDisturbBoost = 1.8f;
#else
	pst_Data->fDisturbBoost = 4.0f;
#endif*/
		
	if(ul_Version>1)
	{
		pst_Data->bStaticDisturber = LOA_ReadULong((CHAR**)&pc_Cur);
		pst_Data->fStaticDisturberForce = LOA_ReadFloat((CHAR**)&pc_Cur);
		pst_Data->fStaticDisturberDelay = LOA_ReadFloat((CHAR**)&pc_Cur);

		if(ul_Version<3)
		{
			ULONG ulBidon;

			//pst_Data->bGenParticles = 
			ulBidon = LOA_ReadULong((CHAR**)&pc_Cur);

			//pst_Data->bPonderateParticulesWithForce1 = 
			ulBidon = LOA_ReadULong((CHAR**)&pc_Cur);

			ulKey = LOA_ReadULong((CHAR**)&pc_Cur);
			/*if (ulKey && ulKey != BIG_C_InvalidKey)
			{
				LOA_MakeFileRef(ulKey, (ULONG *) &pst_Data->pParticuleGenerator1, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
			}
			else
			{
				pst_Data->pParticuleGenerator1 = NULL;
			}*/
			
			//pst_Data->nbParticules1 = 
			ulBidon = LOA_ReadULong((CHAR**)&pc_Cur);

			//pst_Data->bPonderateParticulesWithForce2 = 
			ulBidon = LOA_ReadULong((CHAR**)&pc_Cur);
			
			ulKey = LOA_ReadULong((CHAR**)&pc_Cur);
			/*if (ulKey && ulKey != BIG_C_InvalidKey)
			{
				LOA_MakeFileRef(ulKey, (ULONG *) &pst_Data->pParticuleGenerator2, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
			}
			else
			{
				pst_Data->pParticuleGenerator2 = NULL;
			}*/

			//pst_Data->nbParticules2 = 
			ulBidon = LOA_ReadULong((CHAR**)&pc_Cur);
			
			//pst_Data->bPonderateParticulesWithForce3 = 
			ulBidon = LOA_ReadULong((CHAR**)&pc_Cur);

			ulKey = LOA_ReadULong((CHAR**)&pc_Cur);
			/*if (ulKey && ulKey != BIG_C_InvalidKey)
			{
				LOA_MakeFileRef(ulKey, (ULONG *) &pst_Data->pParticuleGenerator3, OBJ_ul_GameObjectCallback, LOA_C_MustExists);
			}
			else
			{
				pst_Data->pParticuleGenerator3 = NULL;
			}*/

			//pst_Data->nbParticules3 = 
			ulBidon = LOA_ReadULong((CHAR**)&pc_Cur);
		}

        if (ul_Version >= 10)
        {
            pst_Data->bReflectOnWater = LOA_ReadULong((CHAR**)&pc_Cur);
        }

		if(ul_Version>=3)
		{
			pst_Data->bFloatOnWater = LOA_ReadULong((CHAR**)&pc_Cur);
			LOA_ReadVector((CHAR**)&pc_Cur, &pst_Data->vFloatOnWaterInitialVelocity);
			pst_Data->fFloatOnWaterInitialZRotationSpeed = LOA_ReadFloat((CHAR**)&pc_Cur);
			pst_Data->fFloatOnWaterZOffset = LOA_ReadFloat((CHAR**)&pc_Cur);
			pst_Data->fFloatOnWaterBankingDamping = LOA_ReadFloat((CHAR**)&pc_Cur);
			pst_Data->fFloatOnWaterZStability = LOA_ReadFloat((CHAR**)&pc_Cur);
			pst_Data->fFloatOnWaterWaveStrength = LOA_ReadFloat((CHAR**)&pc_Cur);
			pst_Data->fFloatOnWaterEvaluationPlaneDelta = LOA_ReadFloat((CHAR**)&pc_Cur);
			pst_Data->fFloatOnWaterVelocityBackToInitialStrength = LOA_ReadFloat((CHAR**)&pc_Cur);
			pst_Data->fFloatOnWaterWaveInfluence = LOA_ReadFloat((CHAR**)&pc_Cur);
			pst_Data->vFloatOnWaterVelocityDamping = LOA_ReadFloat((CHAR**)&pc_Cur);
		}

        if (ul_Version >= 8)
        {
            pst_Data->bPAGDispersionActive = LOA_ReadULong((CHAR**)&pc_Cur);
            if (pst_Data->bPAGDispersionActive)
            {
                pst_Data->fRadius = LOA_ReadFloat((CHAR**)&pc_Cur);
                pst_Data->fFrontForceTrans = LOA_ReadFloat((CHAR**)&pc_Cur);
                pst_Data->fFrontForceRot = LOA_ReadFloat((CHAR**)&pc_Cur);
                pst_Data->fRearForceTrans = LOA_ReadFloat((CHAR**)&pc_Cur);
                pst_Data->fRearForceRot = LOA_ReadFloat((CHAR**)&pc_Cur);
                pst_Data->fDraftNear = LOA_ReadFloat((CHAR**)&pc_Cur);
                pst_Data->fDraftFar = LOA_ReadFloat((CHAR**)&pc_Cur);
            }
        }

		if (ul_Version >= 9)
        {
			pst_Data->bUseWaterBoundingBoxForCollision = (LOA_ReadULong((CHAR**)&pc_Cur)>0);
		}
		else
		{
			pst_Data->bUseWaterBoundingBoxForCollision = FALSE;
		}

        if (ul_Version >= 11)
        {
            pst_Data->fZOffset = LOA_ReadFloat((CHAR**)&pc_Cur);
        }
	}

	return pc_Cur - (UCHAR*)_pc_Buffer;
}

#ifdef ACTIVE_EDITORS

/*
=======================================================================================================================
=======================================================================================================================
*/
void Disturber_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	Disturber_tdst_Modifier			*pst_Data;
	ULONG							ul_Version;

	pst_Data = (Disturber_tdst_Modifier *) _pst_Mod->p_Data;

	// Save version
	ul_Version = 11;
	SAV_Buffer(&ul_Version, 4);
	SAV_Buffer(&pst_Data->fDisturbBoost, 4);

	SAV_Buffer(&pst_Data->bStaticDisturber, 4);
	SAV_Buffer(&pst_Data->fStaticDisturberForce, 4);
	SAV_Buffer(&pst_Data->fStaticDisturberDelay, 4);

    SAV_Buffer(&pst_Data->bReflectOnWater, 4);
    SAV_Buffer(&pst_Data->bFloatOnWater, 4);
	SAV_Buffer(&pst_Data->vFloatOnWaterInitialVelocity, 12);
	SAV_Buffer(&pst_Data->fFloatOnWaterInitialZRotationSpeed, 4);
	SAV_Buffer(&pst_Data->fFloatOnWaterZOffset, 4);
	SAV_Buffer(&pst_Data->fFloatOnWaterBankingDamping, 4);
	SAV_Buffer(&pst_Data->fFloatOnWaterZStability, 4);

	SAV_Buffer(&pst_Data->fFloatOnWaterWaveStrength, 4);
	SAV_Buffer(&pst_Data->fFloatOnWaterEvaluationPlaneDelta, 4);
	SAV_Buffer(&pst_Data->fFloatOnWaterVelocityBackToInitialStrength, 4);
	SAV_Buffer(&pst_Data->fFloatOnWaterWaveInfluence, 4);

	SAV_Buffer(&pst_Data->vFloatOnWaterVelocityDamping, 4);

    SAV_Buffer(&pst_Data->bPAGDispersionActive, 4);

    // Added for version 8
    if (pst_Data->bPAGDispersionActive)
    {
        SAV_Buffer(&pst_Data->fRadius, 4);
        SAV_Buffer(&pst_Data->fFrontForceTrans, 4);
        SAV_Buffer(&pst_Data->fFrontForceRot, 4);
        SAV_Buffer(&pst_Data->fRearForceTrans, 4);
        SAV_Buffer(&pst_Data->fRearForceRot, 4);
        SAV_Buffer(&pst_Data->fDraftNear, 4);
        SAV_Buffer(&pst_Data->fDraftFar, 4);
    }

	SAV_Buffer(&pst_Data->bUseWaterBoundingBoxForCollision, 4);

    // Z offset
    SAV_Buffer(&pst_Data->fZOffset, 4);

    /*SAV_Buffer(&pst_Data->bGenParticles, 4);
	SAV_Buffer(&pst_Data->bPonderateParticulesWithForce1, 4);

	if (pst_Data->pParticuleGenerator1)
		ulData = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->pParticuleGenerator1);
	else
		ulData = 0;
	SAV_Buffer(&ulData, 4);

	SAV_Buffer(&pst_Data->nbParticules1, 4);

	SAV_Buffer(&pst_Data->bPonderateParticulesWithForce2, 4);

	if (pst_Data->pParticuleGenerator2)
		ulData = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->pParticuleGenerator2);
	else
		ulData = 0;
	SAV_Buffer(&ulData, 4);

	SAV_Buffer(&pst_Data->nbParticules2, 4);

	SAV_Buffer(&pst_Data->bPonderateParticulesWithForce3, 4);

	if (pst_Data->pParticuleGenerator3)
		ulData = LOA_ul_SearchKeyWithAddress((ULONG) pst_Data->pParticuleGenerator3);
	else
		ulData = 0;
	SAV_Buffer(&ulData, 4);

	SAV_Buffer(&pst_Data->nbParticules3, 4);*/
}

#endif
