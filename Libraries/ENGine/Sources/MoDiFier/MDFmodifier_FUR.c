
#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"

#include "GDInterface/GDInterface.h"

#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_FUR.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

#ifdef _XENON_RENDER
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeMesh.h"
#include "XenonGraphics/XeUtils.h"
#include "XenonGraphics/XeVertexShaderManager.h"
#include "XenonGraphics/XeFurManager.h"

float                 FurLength = 10;
ULONG                 FurMaxLitLayers = FUR_MAX_LIT_LAYERS;

#ifdef VIDEOCONSOLE_ENABLE
extern TBool NoFur;
extern TBool NoFurPhysics;
#endif

#endif

int					NbrFurLayers = 10;
float				OffsetNormal = 0.035f;
float				OffsetU = 0.0f;
float				OffsetV = 0.0f;
int					FurInvertAlpha = 0;
int					renderState_OutLine = 0;
int					DontDrawFirstLayer= 0;
ULONG				exLineColor=0;
int					i_FurNearLod=0;

void FUR_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
	FUR_tdst_Modifier * p_FUR;

	_pst_Mod->p_Data = MEM_p_Alloc(sizeof(FUR_tdst_Modifier));
	p_FUR = (FUR_tdst_Modifier *) _pst_Mod->p_Data;
	
//	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao  ;

	if(p_Data == NULL)
	{
		p_FUR->ul_Flags = 0;
		p_FUR->f_NormalOffset = 3.5f;
		p_FUR->f_UOffset = 0.0f;
		p_FUR->f_VOffset = 0.0f;
		p_FUR->NBR_Layers = 10;
		p_FUR->uc_NearLod =0;
		p_FUR->f_Near  = p_FUR->f_Far = 0.0f;

	}
	else
	{
		L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(FUR_tdst_Modifier));
	}
}

void FUR_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
	if(_pst_Mod->p_Data)
		MEM_Free(_pst_Mod->p_Data);
}

void FUR_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj){

	FUR_tdst_Modifier * p_FUR;
	float f_LOD,f_Far;
	p_FUR = (FUR_tdst_Modifier *) _pst_Mod->p_Data;

	NbrFurLayers = 0;
	DontDrawFirstLayer = 0;
	if (p_FUR)
	{
		f_LOD = 1.0f;
		f_Far = p_FUR->f_Far;

		if ((p_FUR->f_Near != 0.0f) && (f_Far != p_FUR->f_Near))
		{
			f_LOD = MATH_f_Distance(&_pst_Mod->pst_GO->pst_GlobalMatrix->T,&GDI_gpst_CurDD->st_Camera.st_Matrix.T);
			f_LOD -= p_FUR->f_Near;
			f_LOD /= f_Far - p_FUR->f_Near;
			f_LOD = 1.0f - fMin(fMax(f_LOD,0.0f),1.0f);
			if (f_LOD == 0.0f) 
				return;
		}

#ifdef JADEFUSION
		FurLength = -1.0f;
#endif
		OffsetNormal = p_FUR->f_NormalOffset/100.0f;
		OffsetU = p_FUR->f_UOffset/10000.0f;
		OffsetV = p_FUR->f_VOffset/10000.0f;
		NbrFurLayers = (int)((float)p_FUR->NBR_Layers * f_LOD);
		if(NbrFurLayers==0)
			NbrFurLayers = 1;
		FurInvertAlpha = 0;
		exLineColor = p_FUR->LineColor;
		if (p_FUR->uc_NearLod<0) p_FUR->uc_NearLod=0;
		if (p_FUR->uc_NearLod>255) p_FUR->uc_NearLod=255;
		i_FurNearLod = p_FUR->uc_NearLod;

		if (p_FUR->ul_Flags & FUR_C_OUTLINE) 
			renderState_OutLine = 1;
		else  
			renderState_OutLine = 0;

		if (p_FUR->ul_Flags & FUR_C_CONST) 
		{ 
			if (!renderState_OutLine)
				p_FUR->ul_Flags |=1;
			renderState_OutLine = 2;
		}

		if (p_FUR->ul_Flags & FUR_C_DONTDRAWLAYER0) 
			DontDrawFirstLayer = 1;
	}

}

void FUR_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
		renderState_OutLine = 0;
		DontDrawFirstLayer = 0;
		i_FurNearLod =0;
		NbrFurLayers = 0;
}

#ifdef ACTIVE_EDITORS
BOOL FUR_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg){
	// todo test if the obj contain a material
	return TRUE;
}
void FUR_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	FUR_tdst_Modifier	*pst_Data;
	ULONG				ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (FUR_tdst_Modifier *) _pst_Mod->p_Data;

	ul_Size = 8*4;
	SAV_Buffer(&ul_Size, 4);

	SAV_Buffer(&pst_Data->f_NormalOffset, 4);
	SAV_Buffer(&pst_Data->f_UOffset, 4);
	SAV_Buffer(&pst_Data->f_VOffset, 4);
	SAV_Buffer(&pst_Data->NBR_Layers, 4);
	SAV_Buffer(&pst_Data->ul_Flags, 4);
	SAV_Buffer(&pst_Data->LineColor, 4);
	SAV_Buffer(&pst_Data->uc_NearLod, 4);
	SAV_Buffer(&pst_Data->f_Near, 4);
	SAV_Buffer(&pst_Data->f_Far, 4);
}
#endif

ULONG FUR_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char                        *pc_Cur;
	FUR_tdst_Modifier   *pst_Data;
	ULONG				ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Data = (FUR_tdst_Modifier *) _pst_Mod->p_Data;
	pc_Cur = _pc_Buffer;
//	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao  ;

	/* Size */
	ul_Size = LOA_ReadULong(&pc_Cur); // skip size

	pst_Data->f_NormalOffset = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_UOffset = LOA_ReadFloat(&pc_Cur);
	pst_Data->f_VOffset = LOA_ReadFloat(&pc_Cur);
	pst_Data->NBR_Layers = LOA_ReadULong(&pc_Cur);
	//Version +Outline
	if (ul_Size>3*4)
	{
		pst_Data->ul_Flags = LOA_ReadULong(&pc_Cur);
		pst_Data->LineColor = LOA_ReadULong(&pc_Cur);
	}
	//Version +LOD
	if (ul_Size>5*4) pst_Data->uc_NearLod = LOA_ReadUInt(&pc_Cur);

	if (ul_Size>6*4)
	{
		pst_Data->f_Near = LOA_ReadFloat(&pc_Cur);
		pst_Data->f_Far = LOA_ReadFloat(&pc_Cur);
	} else
		pst_Data->f_Near = pst_Data->f_Far = 0.0f;


	return (pc_Cur - _pc_Buffer);
}
#ifdef JADEFUSION
/////////////////////////// Dynamic FUR modifier (XENON only) ///////////////////////////////////


void DYNFUR_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
    DYNFUR_tdst_Modifier * p_FUR;

    _pst_Mod->p_Data = MEM_p_Alloc(sizeof(DYNFUR_tdst_Modifier));
    p_FUR = (DYNFUR_tdst_Modifier *) _pst_Mod->p_Data;

    //	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao  ;

    if(p_Data == NULL)
    {
        p_FUR->m_pavLastBonesRotation  = NULL;
        p_FUR->m_pavBonesVelocity      = NULL;
        p_FUR->m_pavBonesAccel         = NULL;
        p_FUR->f_FurLength             = 10.0f;
        p_FUR->f_UOffset               = 0.0f;
        p_FUR->f_VOffset               = 0.0f;
        p_FUR->NBR_Layers              = 10;

        p_FUR->f_Gravity               = 0.2f;
        p_FUR->f_GlobalLinVelBoost     = 1.0f;
        p_FUR->f_LocalAngVelBoost      = 1.0f;
        p_FUR->f_GlobalAngVelBoost     = 1.0f;
        p_FUR->f_Damping               = 1.0f;
		p_FUR->f_Near  = p_FUR->f_Far = 0.0f;

        DYNFUR_Modifier_Reinit(_pst_Mod);
    }
    else
    {
        L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(DYNFUR_tdst_Modifier));
    }
}

void DYNFUR_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{
    DYNFUR_tdst_Modifier * p_FUR = (DYNFUR_tdst_Modifier *) _pst_Mod->p_Data;

    if (p_FUR)
    {
        if (p_FUR->m_pavLastBonesRotation != NULL)
            MEM_Free(p_FUR->m_pavLastBonesRotation);
        if (p_FUR->m_pavBonesVelocity != NULL)
            MEM_Free(p_FUR->m_pavBonesVelocity);
        if (p_FUR->m_pavBonesAccel != NULL)
            MEM_Free(p_FUR->m_pavBonesAccel);

        MEM_Free(_pst_Mod->p_Data);
    }
}

void DYNFUR_Modifier_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
    DYNFUR_tdst_Modifier * p_FUR = (DYNFUR_tdst_Modifier *) _pst_Mod->p_Data;

    if (p_FUR)
    {
        if (p_FUR->m_pavLastBonesRotation != NULL)
        {
            MEM_Free(p_FUR->m_pavLastBonesRotation);
            p_FUR->m_pavLastBonesRotation = NULL;
        }

        if (p_FUR->m_pavBonesVelocity != NULL)
        {
            MEM_Free(p_FUR->m_pavBonesVelocity);
            p_FUR->m_pavBonesVelocity = NULL;
        }

        if (p_FUR->m_pavBonesAccel != NULL)
        {
            MEM_Free(p_FUR->m_pavBonesAccel);
            p_FUR->m_pavBonesAccel = NULL;
        }

        p_FUR->m_vAccel         = D3DXVECTOR4(0,0,0,0);
        p_FUR->m_vPrevVel       = D3DXVECTOR4(0,0,0,0);
        p_FUR->m_vOmegaAccel    = D3DXVECTOR4(0,0,0,0);
        p_FUR->m_vPrevOmega     = D3DXVECTOR4(0,0,0,0);
        p_FUR->m_vTransDelta    = D3DXVECTOR4(0,0,0,0);
        D3DXQuaternionIdentity(&p_FUR->m_qLastRot);

        if (_pst_Mod->pst_GO && _pst_Mod->pst_GO->pst_GlobalMatrix)
        {
            p_FUR->m_vLastPosition.x    = _pst_Mod->pst_GO->pst_GlobalMatrix->T.x;
            p_FUR->m_vLastPosition.y    = _pst_Mod->pst_GO->pst_GlobalMatrix->T.y;
            p_FUR->m_vLastPosition.z    = _pst_Mod->pst_GO->pst_GlobalMatrix->T.z;
        }
        else
            p_FUR->m_vLastPosition  = D3DXVECTOR4(0,0,0,0);
    }
}

void DYNFUR_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
    DYNFUR_tdst_Modifier * p_FUR;
    p_FUR = (DYNFUR_tdst_Modifier *) _pst_Mod->p_Data;
    if (p_FUR)
    {
        FurLength = p_FUR->f_FurLength / 100.0f;
        OffsetU = p_FUR->f_UOffset/10000.0f;
        OffsetV = p_FUR->f_VOffset/10000.0f;
        NbrFurLayers = p_FUR->NBR_Layers;
        FurInvertAlpha = 0;

        // check if we have a last bone position array.
        if (p_FUR->m_pavLastBonesRotation == NULL)
        {
            if ((_pst_Obj->p_SKN_Objectponderation != NULL) &&
                (_pst_Obj->p_SKN_Objectponderation->NumberPdrtLists != 0) &&
                (_pst_Obj->p_SKN_Objectponderation->pp_PdrtLst != NULL))
            {
                // allocate one
                ULONG ulSize = _pst_Obj->p_SKN_Objectponderation->NumberPdrtLists*sizeof(D3DXVECTOR4);

                p_FUR->m_pavLastBonesRotation = (D3DXQUATERNION*) MEM_p_Alloc(_pst_Obj->p_SKN_Objectponderation->NumberPdrtLists*sizeof(D3DXQUATERNION));
                L_memset(p_FUR->m_pavLastBonesRotation, 0, _pst_Obj->p_SKN_Objectponderation->NumberPdrtLists*sizeof(D3DXQUATERNION));
                p_FUR->m_pavBonesVelocity     = (D3DXVECTOR4*) MEM_p_Alloc(ulSize);
                L_memset(p_FUR->m_pavBonesVelocity, 0, ulSize);
                p_FUR->m_pavBonesAccel        = (D3DXVECTOR4*) MEM_p_Alloc(ulSize);
                L_memset(p_FUR->m_pavBonesAccel, 0, ulSize);
            }
        }
    }
}

void DYNFUR_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
	if((GDI_gpst_CurDD->ul_DisplayFlags & GDI_Cul_DF_DoNotRender))
        return;
    
    DYNFUR_tdst_Modifier * p_FUR;
    p_FUR = (DYNFUR_tdst_Modifier *) _pst_Mod->p_Data;
    if (p_FUR)
    {
        // update fur offset now
        OBJ_tdst_GameObject* pst_GO = GDI_gpst_CurDD->pst_CurrentGameObject;

        if (pst_GO && 
            pst_GO->pst_Base && 
            pst_GO->pst_Base->pst_Visu && 
            pst_GO->pst_Base->pst_Visu->p_VBFurOffsets && 
            pst_GO->pst_Base->pst_Visu->p_XeElements   &&
            pst_GO->pst_Base->pst_Visu->p_FurOffsetVertex)
        {
            BOOL        bSkinnedMesh = FALSE;
            FLOAT       dt;
            D3DXVECTOR4 vGravity;
            D3DXVECTOR4 vGlobalVelocity;
            D3DXVECTOR4 vOmega, vOmegaDT;

            GRO_tdst_Visu* pst_Visu = GDI_gpst_CurDD->pst_CurrentGameObject->pst_Base->pst_Visu;

            if (  (((*(int*)&p_FUR->f_GlobalLinVelBoost) == 0)
                && ((*(int*)&p_FUR->f_LocalAngVelBoost)  == 0)
                && ((*(int*)&p_FUR->f_GlobalAngVelBoost) == 0))
              #ifdef VIDEOCONSOLE_ENABLE
                || NoFur
                || NoFurPhysics
              #endif
                )
            {
                // don't apply any physique but still request a VB since there is one in the vertex declaration
                pst_Visu->p_VBFurOffsets->Lock(pst_Visu->l_VBVertexCount, 2*sizeof(FLOAT), FALSE, TRUE);
                pst_Visu->p_VBFurOffsets->Unlock();
                return;
            }

            // update vertex buffer now
            FLOAT*           pOffsetBuffer = (FLOAT*) pst_Visu->p_VBFurOffsets->Lock(pst_Visu->l_VBVertexCount, 2*sizeof(FLOAT), FALSE, TRUE);
            FurOffsetVertex* pFurData      = pst_Visu->p_FurOffsetVertex;

          #if defined(_XENON)
            // prefetch first 16 vertices (64 bytes each)
            for (INT iCurLine = 0; iCurLine < 1024; iCurLine += 128)
                __dcbt(iCurLine, pFurData);
          #endif

            // calculate dt
            dt = fMax( 0.01f, fMin( 0.1f, TIM_gf_dt ) );
            static FLOAT dt2 = 0.025f;
            dt2 = 0.9f * dt2 + 0.1f * dt;

            // calculate InvWorld
            D3DXMATRIX oInvWorld;
            FLOAT fDummy;
            if(MATH_b_TestScaleType(pst_GO->pst_GlobalMatrix))
            {
                MATH_tdst_Matrix stOGLMatrix;
                MATH_MakeOGLMatrix(&stOGLMatrix, pst_GO->pst_GlobalMatrix);
                D3DXMatrixInverse(&oInvWorld, &fDummy, (D3DXMATRIX*)&stOGLMatrix);
            }
            else
            {
                D3DXMatrixInverse(&oInvWorld, &fDummy, (D3DXMATRIX*)pst_GO->pst_GlobalMatrix);
            }

            // calculate gravity
            vGravity = D3DXVECTOR4( 0.0f, 0.0f, -p_FUR->f_Gravity, 0.0f ); 
            D3DXVec4Transform(&vGravity, &vGravity, &oInvWorld);
            vGravity.y = -vGravity.y;
            vGravity.z = -vGravity.z;

            // check for skinning
            if ((_pst_Obj->p_SKN_Objectponderation != NULL) && 
                (_pst_Obj->p_SKN_Objectponderation->NumberPdrtLists != 0) &&
                (_pst_Obj->p_SKN_Objectponderation->pp_PdrtLst != NULL) &&
                (GDI_gpst_CurDD->ul_CurrentDrawMask &  GDI_Cul_DM_ActiveSkin) &&
                (pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_AdditionalMatrix))
            {        
                bSkinnedMesh = TRUE;
            }

            FLOAT fDamping = fMin(1.0f, dt/0.125f);
            D3DXVECTOR4 vTranslation;

            // calculate translational velocity
            vTranslation.x = pst_GO->pst_GlobalMatrix->T.x - p_FUR->m_vLastPosition.x;
            vTranslation.y = -(pst_GO->pst_GlobalMatrix->T.y - p_FUR->m_vLastPosition.y);
            vTranslation.z = pst_GO->pst_GlobalMatrix->T.z - p_FUR->m_vLastPosition.z;
            vTranslation.w = 0.0f;
            p_FUR->m_vLastPosition.x = pst_GO->pst_GlobalMatrix->T.x;
            p_FUR->m_vLastPosition.y = pst_GO->pst_GlobalMatrix->T.y;
            p_FUR->m_vLastPosition.z = pst_GO->pst_GlobalMatrix->T.z;
            p_FUR->m_vTransDelta = (1-fDamping) * p_FUR->m_vTransDelta + fDamping * vTranslation;

            if (p_FUR->f_GlobalLinVelBoost > 0.0f)
            {
                vGlobalVelocity.x = +p_FUR->m_vTransDelta.x / dt2;
                vGlobalVelocity.y = -p_FUR->m_vTransDelta.y / dt2;
                vGlobalVelocity.z = +p_FUR->m_vTransDelta.z / dt2;
                vGlobalVelocity.w = 0.0f;
                D3DXVec4Transform(&vGlobalVelocity, &vGlobalVelocity, &oInvWorld);
                vGlobalVelocity.y = -vGlobalVelocity.y;
                vGlobalVelocity.z = -vGlobalVelocity.z;
                vGlobalVelocity *= p_FUR->f_GlobalLinVelBoost;
            }
            else
            {
                vGlobalVelocity = D3DXVECTOR4(0,0,0,0);
            }

            // Filter coefficient for calculating m_Acceleration
            FLOAT fFilter = fMin( 0.25f, dt2/0.1f );

            // Translational m_Acceleration
            p_FUR->m_vAccel = (1.0f - fFilter) * p_FUR->m_vAccel + fFilter * ((vGlobalVelocity - p_FUR->m_vPrevVel) / dt2);
            p_FUR->m_vPrevVel = vGlobalVelocity;

            // Translational force = gravity + air resistance + inertia
            // (coefficients were determined experimentally)
            D3DXVECTOR4 vTRANSLATIONFORCE = vGravity - 0.12f*vGlobalVelocity - 0.018f*p_FUR->m_vAccel;

            // calculate angular velocity
            D3DXQUATERNION qCurrentRot, qRotDelta;
            MATH_ConvertMatrixToQuaternion((MATH_tdst_Quaternion*)&qCurrentRot, pst_GO->pst_GlobalMatrix);
            qRotDelta = (qCurrentRot - p_FUR->m_qLastRot);
            p_FUR->m_qLastRot = qCurrentRot;

            if ((qRotDelta.x != 0.0f) || (qRotDelta.y != 0.0f) || (qRotDelta.z != 0.0f) || (qRotDelta.w != 0.0f))
            { 
                D3DXQuaternionNormalize(&qRotDelta, &qRotDelta);
                D3DXQuaternionLn((D3DXQUATERNION*)&vOmega, &qRotDelta);
                vOmega *= 0.01f;
            }
            else
            {
                vOmega = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
            }
            vOmegaDT = 2*vOmega/dt2;
            D3DXVec4Transform(&vOmega, &vOmegaDT, &oInvWorld );
            vOmega *= p_FUR->f_GlobalAngVelBoost;

            // Angular m_Acceleration
            p_FUR->m_vOmegaAccel = (1.0f - fFilter) * p_FUR->m_vOmegaAccel + fFilter * ((vOmega - p_FUR->m_vPrevOmega) / dt2);
            p_FUR->m_vPrevOmega  = vOmega;

            if (bSkinnedMesh)
            {
                D3DXVECTOR3 vLocalVelocity;

                // calculate velocity for all bones
                XeSkinningSet* poSkinningSet = g_pXeContextManagerEngine->GetLastSkinningSet();

                for (ULONG ulCurBone = 0; ulCurBone < _pst_Obj->p_SKN_Objectponderation->NumberPdrtLists; ulCurBone++)
                {
                    D3DXMATRIX oBoneMatrix;
                    BOOL bSkinMatrixValid  = g_pXeContextManagerEngine->GetSkinningMatrix( oBoneMatrix, poSkinningSet, ulCurBone);

                    if (bSkinMatrixValid)
                    {
                        // calculate angular velocity
                        D3DXQuaternionRotationMatrix(&qCurrentRot, &oBoneMatrix);
                        qRotDelta = (qCurrentRot - p_FUR->m_pavLastBonesRotation[ulCurBone]);
                        p_FUR->m_pavLastBonesRotation[ulCurBone] = qCurrentRot;

                        if ((qRotDelta.x != 0.0f) || (qRotDelta.y != 0.0f) || (qRotDelta.z != 0.0f) || (qRotDelta.w != 0.0f))
                        { 
                            D3DXQuaternionNormalize(&qRotDelta, &qRotDelta);
                            D3DXQuaternionLn((D3DXQUATERNION*)&vOmega, &qRotDelta);
                            vOmega *= 0.01f;
                        }
                        else
                        {
                            vOmega = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
                        }
                        vOmega = 2*vOmega/dt2;
                        //D3DXVec4Transform(&vOmega, &vOmegaDT, &oInvWorld );
                        vOmega *= -p_FUR->f_LocalAngVelBoost;

                        // Angular m_Acceleration
                        p_FUR->m_pavBonesAccel[ulCurBone] = (1.0f - fFilter) * p_FUR->m_pavBonesAccel[ulCurBone] + fFilter * ((vOmega - p_FUR->m_pavBonesVelocity[ulCurBone]) / dt2);
                        p_FUR->m_pavBonesVelocity[ulCurBone]  = vOmega;
                    }
                }
            }

            // Angular force = -CrossProduct(I_OMEGA, vertex_pos)
            D3DXVECTOR4 vOMEGA = 0.12f*vOmega + 0.018f*p_FUR->m_vOmegaAccel;

            // Damping factor
            FLOAT fDAMPING = fMax( 0.0625f, fMin( 0.5f, dt2/0.3f ) );            

            GEO_tdst_ElementIndexedTriangles* pst_Element = _pst_Obj->dst_Element;
            GEO_tdst_ElementIndexedTriangles* pst_LastElement = pst_Element + _pst_Obj->l_NbElements;

            // SC: TODO: Could probably optimize this by skipping vertices that don't need offset (flag could be
            //           set in the FurOffsetVertex while packing the mesh)

            for (ULONG ulCurVertex = 0; ulCurVertex < (ULONG)pst_Visu->l_VBVertexCount; ulCurVertex++, pFurData++)
            {
            
              #if defined(_XENON)
                // prefetch next 2 vertices (64 bytes each)
                if ((ulCurVertex % 2) == 1)
                    __dcbt(0, pFurData+16);
              #endif

				D3DXVECTOR3 Force, vTemp;
                D3DXVECTOR4 vBoneVelocity, vBoneAccel;

                D3DXVec3Cross(&vTemp, (D3DXVECTOR3*)&vOMEGA, (D3DXVECTOR3*)&pFurData->fX);

                if (bSkinnedMesh)
                {
                    D3DXVECTOR4 vTemp2;
                    vBoneVelocity = p_FUR->m_pavBonesVelocity[pFurData->abyIndex[0]] * pFurData->afWeight[0] +
                                    p_FUR->m_pavBonesVelocity[pFurData->abyIndex[1]] * pFurData->afWeight[1] +
                                    p_FUR->m_pavBonesVelocity[pFurData->abyIndex[2]] * pFurData->afWeight[2] +
                                    p_FUR->m_pavBonesVelocity[pFurData->abyIndex[3]] * pFurData->afWeight[3];

                    vBoneAccel    = p_FUR->m_pavBonesAccel[pFurData->abyIndex[0]] * pFurData->afWeight[0] +
                                    p_FUR->m_pavBonesAccel[pFurData->abyIndex[1]] * pFurData->afWeight[1] +
                                    p_FUR->m_pavBonesAccel[pFurData->abyIndex[2]] * pFurData->afWeight[2] +
                                    p_FUR->m_pavBonesAccel[pFurData->abyIndex[3]] * pFurData->afWeight[3];

                    vTemp2 = 0.12f * vBoneVelocity + 0.018f * vBoneAccel;
                    D3DXVec3Cross((D3DXVECTOR3*)&vTemp2, (D3DXVECTOR3*)&vTemp2, (D3DXVECTOR3*)&pFurData->fX);
                    vTemp += *((D3DXVECTOR3*)&vTemp2);
                }

                Force = *((D3DXVECTOR3*)&vTRANSLATIONFORCE) - vTemp;

                FLOAT FS = D3DXVec3Dot(&Force, (D3DXVECTOR3*)&pFurData->fTangentX);
                FLOAT FT = D3DXVec3Dot(&Force, (D3DXVECTOR3*)&pFurData->fBinormalX);
                FLOAT ST = D3DXVec3Dot((D3DXVECTOR3*)&pFurData->fBinormalX, (D3DXVECTOR3*)&pFurData->fTangentX);

                FLOAT Det = 1.0f / ( 1.0f - (ST * ST));

                Force.x = ( FS - FT * ST ) * Det;
                Force.y = ( FT - FS * ST ) * Det;

                pFurData->fOffsetX = *pOffsetBuffer     = fDAMPING * Force.x + (1-fDAMPING)*pFurData->fOffsetX;
                pFurData->fOffsetY = *(pOffsetBuffer+1) = fDAMPING * Force.y + (1-fDAMPING)*pFurData->fOffsetY;

                pOffsetBuffer += 2;
            }

            pst_Visu->p_VBFurOffsets->Unlock();
        }
    }
}

#ifdef ACTIVE_EDITORS
BOOL DYNFUR_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg){
    // todo test if the obj contain a material
    return TRUE;
}
void DYNFUR_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    DYNFUR_tdst_Modifier	*pst_Data;
    ULONG				ul_Size;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (DYNFUR_tdst_Modifier *) _pst_Mod->p_Data;

    ul_Size = 10*4;
    SAV_Buffer(&ul_Size, 4);

    ULONG ul_Version = 3;
    SAV_Buffer(&ul_Version, 4);
    SAV_Buffer(&pst_Data->f_FurLength, 4);
    SAV_Buffer(&pst_Data->f_UOffset, 4);
    SAV_Buffer(&pst_Data->f_VOffset, 4);
    SAV_Buffer(&pst_Data->NBR_Layers, 4);
    SAV_Buffer(&pst_Data->f_Gravity, 4);
    SAV_Buffer(&pst_Data->f_GlobalLinVelBoost, 4);
    SAV_Buffer(&pst_Data->f_GlobalAngVelBoost, 4);
    SAV_Buffer(&pst_Data->f_Damping, 4);
    SAV_Buffer(&pst_Data->f_LocalAngVelBoost, 4);
    SAV_Buffer(&pst_Data->f_Near, 4);
    SAV_Buffer(&pst_Data->f_Far, 4);
}
#endif

ULONG DYNFUR_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                        *pc_Cur;
    DYNFUR_tdst_Modifier        *pst_Data;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (DYNFUR_tdst_Modifier *) _pst_Mod->p_Data;
    pc_Cur = _pc_Buffer;
    //	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao  ;

    /* Size */
    LOA_ReadLong_Ed(&pc_Cur, NULL); // skip size
    ULONG ul_Version = LOA_ReadULong(&pc_Cur);

    pst_Data->f_FurLength = LOA_ReadFloat(&pc_Cur);
    pst_Data->f_UOffset   = LOA_ReadFloat(&pc_Cur);
    pst_Data->f_VOffset   = LOA_ReadFloat(&pc_Cur);
    pst_Data->NBR_Layers  = LOA_ReadULong(&pc_Cur);
    pst_Data->f_Gravity   = LOA_ReadFloat(&pc_Cur);
    pst_Data->f_GlobalLinVelBoost = LOA_ReadFloat(&pc_Cur);
    pst_Data->f_GlobalAngVelBoost= LOA_ReadFloat(&pc_Cur);
    pst_Data->f_Damping   = LOA_ReadFloat(&pc_Cur);

    if (ul_Version > 1)
    {
        pst_Data->f_LocalAngVelBoost = LOA_ReadFloat(&pc_Cur);
		if (ul_Version > 2)
		{
			pst_Data->f_Near = LOA_ReadFloat(&pc_Cur);
			pst_Data->f_Far = LOA_ReadFloat(&pc_Cur);
		} else
			pst_Data->f_Near = pst_Data->f_Far = 0.0f;
    }

    return (pc_Cur - _pc_Buffer);
}
#endif