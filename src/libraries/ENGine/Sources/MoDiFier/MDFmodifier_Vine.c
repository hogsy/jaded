
#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/MEMory/MEM.h"

#include "GDInterface/GDInterface.h"
#include "MoDiFier/MDFstruct.h"
#include "MoDiFier/MDFmodifier_VINE.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/COLlision/COLreport.h"
#include "ENGine/Sources/COLlision/COLmain.h"
#include "ENGine/Sources/COLlision/COLaccess.h"

#ifdef JADEFUSION
#include "ENGine/Sources/OBJects/OBJaccess.h"
#endif

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"

extern float TIM_gf_dt;
/*
 *	
 */
void VINE_Modifier_Create(OBJ_tdst_GameObject *_pst_GO, MDF_tdst_Modifier *_pst_Mod, void *p_Data)
{
    VINE_tdst_Modifier * p_VINE;

    _pst_Mod->p_Data = MEM_p_Alloc(sizeof(VINE_tdst_Modifier));    

    p_VINE = (VINE_tdst_Modifier *) _pst_Mod->p_Data;
	p_VINE->pst_Colliders = (OBJ_tdst_GameObject** )MEM_p_Alloc(sizeof(OBJ_tdst_GameObject*) * Vine_C_MaxColliders);


	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
		
	if(p_Data == NULL)
	{
        p_VINE->pst_GO = _pst_Mod->pst_GO;
        p_VINE->bFirstFrame = TRUE;        

#ifdef JADEFUSION
        // Typically based on test in SFX test. 
        p_VINE->fDampeningFactor = 20.0f;
        p_VINE->fForceApplied = 200.0f;
        p_VINE->fMaxForce = 10000.0f;
#else
        p_VINE->fDampeningFactor = 50.0f;
        p_VINE->fForceApplied = 10.0f;
#endif
        p_VINE->fGravity = -10.0f;
		p_VINE->ul_ChildIsAlsoHook = FALSE;
        p_VINE->ul_ChildHookBrokeOnContact = FALSE;
        p_VINE->ul_BrokenVines = FALSE;
	}
	else
	{
        L_memcpy(_pst_Mod->p_Data, p_Data, sizeof(VINE_tdst_Modifier));
#ifdef JADEFUSION
        p_VINE->pst_GO = _pst_Mod->pst_GO;

        p_VINE->bFirstFrame = TRUE;
        p_VINE->a_VineVetices = NULL;
        p_VINE->p_HookVertices = NULL;
        p_VINE->p_FirstVertices = NULL; 
#endif
	}
#ifdef JADEFUSION
    p_VINE->pst_Colliders = (OBJ_tdst_GameObject**)MEM_p_Alloc(sizeof(OBJ_tdst_GameObject*) * Vine_C_MaxColliders);
#endif
}

/*
 *	
 */
void VINE_Modifier_Destroy(MDF_tdst_Modifier *_pst_Mod)
{   
	if(_pst_Mod->p_Data)
    {
        VINE_tdst_Modifier * p_VINE;
        p_VINE = (VINE_tdst_Modifier *) _pst_Mod->p_Data;
        MEM_Free(p_VINE->pst_Colliders);
        MEM_Free(_pst_Mod->p_Data);
    }
		
}

/*
 *	
 */
void VINE_Modifier_Apply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
    VINE_tdst_Modifier  *p_VINE;
    FLOAT				f_DT;
    MATH_tdst_Vector    Vine_Vector;
    ULONG               ul_Iter = 0;
    OBJ_tdst_GameObject *_pst_Father;
    OBJ_tdst_GameObject *_pst_FirstBone = NULL;
    GAO_tdst_VineVertex *pst_LastVertice = NULL;
    GAO_tdst_VineVertex *pst_CurrentVertice = NULL;
    int                 i = 0;
    float               fGlobalDistance = 0.0f;

    p_VINE = (VINE_tdst_Modifier *) _pst_Mod->p_Data;
    f_DT = TIM_gf_dt;

    // First initialisation
    if (p_VINE->bFirstFrame)
    {      
        p_VINE->ul_NbVertices = 2;        
        p_VINE->fCurrentTime = 0.0f;
        p_VINE->ul_BrokenVines = FALSE;
#ifdef JADEFUSION
        p_VINE->ul_NbCollided = 0;

        p_VINE->pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit = p_VINE->pst_GO->pst_Base->pst_Hierarchy->pst_Father;
#endif
        _pst_Father = p_VINE->pst_GO->pst_Base->pst_Hierarchy->pst_Father;
        if (_pst_Father->pst_Base->pst_Hierarchy != NULL)
        {        
            // Count the number of vertices in the Vine
            while (_pst_Father->pst_Base->pst_Hierarchy != NULL) 
            {
                p_VINE->ul_NbVertices++;
#ifdef JADEFUSION
				_pst_Father->pst_Base->pst_Hierarchy->pst_FatherInit = _pst_Father->pst_Base->pst_Hierarchy->pst_Father;
#endif
				_pst_Father = _pst_Father->pst_Base->pst_Hierarchy->pst_Father;  
            }
            // Allocate the array of vertices for the vine
            p_VINE->a_VineVetices = (GAO_tdst_VineVertex*)MEM_p_Alloc(sizeof(GAO_tdst_VineVertex) * p_VINE->ul_NbVertices);
        
            _pst_Father = p_VINE->pst_GO;
            p_VINE->p_FirstVertices = &p_VINE->a_VineVetices[0];
            // Fill Info for all vertices
            for (i = 0; i < (int)(p_VINE->ul_NbVertices);i++)
            {
                MATH_InitVector(&p_VINE->a_VineVetices[i].m_Accel, 0.0f, 0.0f, 0.0f);
                p_VINE->a_VineVetices[i].m_CurrPos = _pst_Father->pst_GlobalMatrix->T;
                p_VINE->a_VineVetices[i].m_PrevPos = _pst_Father->pst_GlobalMatrix->T;;
                p_VINE->a_VineVetices[i].m_pBone = _pst_Father;

                // Build linked list (Children + Father) (Head and tail are NULL)
                p_VINE->a_VineVetices[i].m_pChildren = pst_LastVertice;
                pst_LastVertice = &p_VINE->a_VineVetices[i];
                
                if (i != (int)(p_VINE->ul_NbVertices) - 1)
                {
                    p_VINE->a_VineVetices[i].m_pFather = &p_VINE->a_VineVetices[i+1];
                    _pst_Father = _pst_Father->pst_Base->pst_Hierarchy->pst_Father;
                }
                else
                {
                    p_VINE->a_VineVetices[i].m_pFather = NULL;
                    p_VINE->p_HookVertices = &p_VINE->a_VineVetices[i];
                }
            }
        }
    
        pst_CurrentVertice = p_VINE->p_FirstVertices;
        while (pst_CurrentVertice->m_pFather != NULL)
        {
            // Compute distance (Father - Current)
            MATH_SubVector( &Vine_Vector,
                &pst_CurrentVertice->m_pBone->pst_GlobalMatrix->T,
                &pst_CurrentVertice->m_pFather->m_pBone->pst_GlobalMatrix->T);

            pst_CurrentVertice->fDistanceWithFather = MATH_f_NormVector( &Vine_Vector); 

            //pst_CurrentVertice->m_pBone->pst_Base->pst_Hierarchy->pst_Father = NULL;            
            pst_CurrentVertice = pst_CurrentVertice->m_pFather;
        }

        p_VINE->fPrevTime = 0.0f;
        p_VINE->bFirstFrame = FALSE;
        p_VINE->ul_Paused = FALSE;
    }

    p_VINE->fCurrentTime += TIM_gf_dt;
    MDFModifier_Vine_ApplyForces(p_VINE);

    if (p_VINE->ul_Paused == FALSE)
    {   
        MDFModifier_Vine_Solver(p_VINE,f_DT);

        while(ul_Iter < Vine_C_MaxIteration)
        {
            VINE_Modifier_Normalize(p_VINE);
            ul_Iter++;
        }

        pst_CurrentVertice = p_VINE->p_HookVertices->m_pChildren;


        while (pst_CurrentVertice != NULL)
        {
            MATH_tdst_Vector    VPrevCurr;

            pst_CurrentVertice->m_PrevPos = pst_CurrentVertice->m_pBone->pst_GlobalMatrix->T;
            MATH_CopyVector(OBJ_pst_GetAbsolutePosition(pst_CurrentVertice->m_pBone), &pst_CurrentVertice->m_CurrPos);
            OBJ_ComputeLocalWhenHie(pst_CurrentVertice->m_pBone);

            MATH_SubVector(&VPrevCurr, &pst_CurrentVertice->m_PrevPos, &pst_CurrentVertice->m_CurrPos);
            fGlobalDistance += MATH_f_SqrNormVector(&VPrevCurr);        

            pst_CurrentVertice = pst_CurrentVertice->m_pChildren;
            if (p_VINE->ul_ChildIsAlsoHook && pst_CurrentVertice == p_VINE->p_FirstVertices
                && p_VINE->ul_BrokenVines == FALSE)
            {                
                OBJ_ComputeLocalWhenHie(pst_CurrentVertice->m_pBone);
                pst_CurrentVertice = NULL;
            }
        }

        if (p_VINE->fCurrentTime != p_VINE->fPrevTime)
        {
            fGlobalDistance /= (p_VINE->fCurrentTime - p_VINE->fPrevTime);
        }
        else
        {
            fGlobalDistance = Vine_C_PausedTreshold;
        }
        

        if (fGlobalDistance < Vine_C_PausedTreshold)
        {
            p_VINE->ul_Paused = TRUE;
#ifdef JADEFUSION
			p_VINE->p_FirstVertices->m_pBone->ul_StatusAndControlFlags = p_VINE->p_FirstVertices->m_pBone->ul_StatusAndControlFlags & ~OBJ_C_ControlFlag_AlwaysVisible;
#endif
		}
    }

    p_VINE->fPrevTime = p_VINE->fCurrentTime;
}

/*
 *	
 */
void VINE_Modifier_Unapply(MDF_tdst_Modifier *_pst_Mod, GEO_tdst_Object *_pst_Obj)
{
}

#ifdef ACTIVE_EDITORS
/*
 *	
 */
BOOL VINE_Modifier_CanBeApply(OBJ_tdst_GameObject *_pst_GO, char *_psz_Msg)
{
	return TRUE;
}

/*
 *	
 */
void VINE_Modifier_Save(MDF_tdst_Modifier *_pst_Mod)
{    
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    VINE_tdst_Modifier	*pst_Data;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (VINE_tdst_Modifier *) _pst_Mod->p_Data;

    SAV_Buffer(&pst_Data->fDampeningFactor, sizeof(FLOAT));
    SAV_Buffer(&pst_Data->fGravity, sizeof(FLOAT));
#ifdef JADEFUSION
	SAV_Buffer(&pst_Data->fMaxForce, sizeof(FLOAT));
#endif
	SAV_Buffer(&pst_Data->fForceApplied, sizeof(FLOAT));
    SAV_Buffer(&pst_Data->ul_ChildIsAlsoHook, sizeof(ULONG));
    SAV_Buffer(&pst_Data->ul_ChildHookBrokeOnContact, sizeof(ULONG));
}
#endif

/*
 *	
 */
ULONG VINE_Modifier_Load(MDF_tdst_Modifier *_pst_Mod, char *_pc_Buffer)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char                *pc_Cur;
    VINE_tdst_Modifier  *pst_Data;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    pst_Data = (VINE_tdst_Modifier *) _pst_Mod->p_Data;
    pc_Cur = _pc_Buffer;    

    pst_Data->fDampeningFactor = LOA_ReadFloat(&_pc_Buffer);
    pst_Data->fGravity = LOA_ReadFloat(&_pc_Buffer);
#ifdef JADEFUSION
	pst_Data->fMaxForce = LOA_ReadFloat(&_pc_Buffer);
#endif
	pst_Data->fForceApplied = LOA_ReadFloat(&_pc_Buffer);
    pst_Data->ul_ChildIsAlsoHook = LOA_ReadULong(&_pc_Buffer);
    pst_Data->ul_ChildHookBrokeOnContact = LOA_ReadULong(&_pc_Buffer);

	_pst_Mod->ul_Flags = MDF_C_Modifier_ApplyGao;
    return (_pc_Buffer - pc_Cur);
}

/*
 *	
 */
void VINE_Modifier_Reinit(MDF_tdst_Modifier *_pst_Mod)
{
    VINE_tdst_Modifier	*pst_Data;
    GAO_tdst_VineVertex *pst_CurrentVertice = NULL;

    pst_Data = (VINE_tdst_Modifier *) _pst_Mod->p_Data;
    
    pst_CurrentVertice = pst_Data->p_FirstVertices;
    if (pst_CurrentVertice)
    {
        while (pst_CurrentVertice->m_pFather != NULL)
        {
            pst_CurrentVertice->m_pBone->pst_Base->pst_Hierarchy->pst_Father = pst_CurrentVertice->m_pBone->pst_Base->pst_Hierarchy->pst_FatherInit;
            pst_CurrentVertice = pst_CurrentVertice->m_pFather;
        }

        MEM_Free(pst_Data->a_VineVetices);
    }   

    pst_Data->bFirstFrame = TRUE;
}

//------------------------------------------------------------------------
//
// Author			Jean-Sylvain Sormany
// Date				19 april 2005
// 
// Prototype		MDFModifier_Vine_ApplyForce
// Parameters		_p_Vine : 
// Return Type		void
// 
// Description		Apply force on vine
// 
//------------------------------------------------------------------------
#ifdef JADEFUSION
void MDFModifier_Vine_ApplyForces(VINE_tdst_Modifier * _p_Vine)
{
    GAO_tdst_VineVertex     *pst_CurrentVertice = NULL;    
    COL_tdst_Base           *pst_Collision;
    OBJ_tdst_GameObject     *_pst_Collider, *_pst_VineBone;
    COL_tdst_Instance	    *pst_Collider_Instance;
    COL_tdst_ColSet		    *pst_Collider_ColSet;
    COL_tdst_ZDx		    *pst_ZDx;
    MATH_tdst_Vector	    st_Center, st_BonePos, BoneColliderVector;
    UCHAR				    uc_ENG_Index;
    ULONG                   i;
    float				    f_Radius;
    float                   f_Distance;

    // Temporary apply force "function"
    pst_CurrentVertice = _p_Vine->p_FirstVertices;
    while (pst_CurrentVertice != NULL)
    {
        MATH_InitVector(&pst_CurrentVertice->m_Accel, 0.0f, 0.0f, _p_Vine->fGravity );
        
        _pst_VineBone = pst_CurrentVertice->m_pBone;


        if (!pst_CurrentVertice->m_pBone->pst_Extended)
        {
            pst_CurrentVertice = pst_CurrentVertice->m_pFather;
            continue;
        }
        
        pst_Collision = (COL_tdst_Base*)pst_CurrentVertice->m_pBone->pst_Extended->pst_Col;
    
        if (!pst_Collision)
        {
            pst_CurrentVertice = pst_CurrentVertice->m_pFather;
            continue;
        }

        for (i = 0; i < pst_Collision->pst_List->ul_NbCollidedObjects; i++)
        {
            _pst_Collider = pst_Collision->pst_List->dpst_CollidedObject[i];
            if(!OBJ_b_TestFlag(_pst_Collider, OBJ_C_IdentityFlag_ZDE, 0, OBJ_C_OR_OR_IdentityFlags)) 
                continue;

            /* If one of the 2 objects is inactive, return FALSE; */
            if(!OBJ_b_TestStatusFlag(_pst_VineBone, OBJ_C_StatusFlag_Active)
                ||	!OBJ_b_TestStatusFlag(_pst_Collider, OBJ_C_StatusFlag_Active)) 
                continue;

            /* If A has no instance, we return FALSE; */
            if((!OBJ_b_TestIdentityFlag(_pst_Collider, OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
                ||	!_pst_Collider->pst_Extended
                ||	!_pst_Collider->pst_Extended->pst_Col
                ||	!((COL_tdst_Base *) _pst_Collider->pst_Extended->pst_Col)->pst_Instance) 
                continue;

            pst_Collider_Instance = ((COL_tdst_Base *) _pst_Collider->pst_Extended->pst_Col)->pst_Instance;
            pst_Collider_ColSet = pst_Collider_Instance->pst_ColSet;

            if(!pst_Collider_ColSet)
                continue;

            uc_ENG_Index = pst_Collider_ColSet->pauc_AI_Indexes[0];

            if(uc_ENG_Index >= pst_Collider_Instance->uc_NbOfZDx)
                continue;
        
            // Hack because Kong ZDE is not active when is on ledge
            //if(!COL_b_Instance_IsActive(pst_Collider_Instance, uc_ENG_Index)) 
            //    continue;

            pst_ZDx = *(((COL_tdst_Base *) _pst_Collider->pst_Extended->pst_Col)->pst_Instance->dpst_ZDx + uc_ENG_Index);

            /* If the ZDx in not a ZDE, we return FALSE; */
            if(!COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_ZDM))
                continue;

            if (pst_ZDx->uc_Type == COL_C_Zone_Sphere)
            {      
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
                BOOL				b_Scale, b_Oriented;
                MATH_tdst_Matrix	*pst_Matrix;
                MATH_tdst_Vector	*pst_Position;
                /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

                pst_Matrix = NULL;
                pst_Position = NULL;

                pst_Matrix = OBJ_pst_GetAbsoluteMatrix(_pst_Collider);
                b_Scale = MATH_b_TestScaleType(pst_Matrix);
                b_Oriented = TRUE;

                if(b_Oriented)
        {
                    if((b_Scale) && !(COL_b_Zone_TestFlag(pst_ZDx, COL_C_Zone_NoScale)))

            {
                        /*~~~~~~~~~~~~~~~~~~~~~~~~~*/
                        MATH_tdst_Vector	st_Scale;
                        /*~~~~~~~~~~~~~~~~~~~~~~~~~*/


                        MATH_GetScale(&st_Scale, pst_Matrix);
                        MATH_TransformVertex(&st_Center,pst_Matrix,COL_pst_Shape_GetCenter(pst_ZDx->p_Shape));
                        f_Radius = COL_f_Shape_GetRadius(pst_ZDx->p_Shape) * fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
                    }
                    else
                    {
                        MATH_TransformVertexNoScale(&st_Center,pst_Matrix,COL_pst_Shape_GetCenter(pst_ZDx->p_Shape));
                        f_Radius = COL_f_Shape_GetRadius(pst_ZDx->p_Shape);
                    }
                }
                else
                {
                    MATH_AddVector(&st_Center, COL_pst_Shape_GetCenter(pst_ZDx->p_Shape), pst_Position);
                    f_Radius = COL_f_Shape_GetRadius(pst_ZDx->p_Shape);                   
                }

                // Compute distance between Bone and ZDM
                MATH_CopyVector(&st_BonePos, &_pst_VineBone->pst_GlobalMatrix->T);
                MATH_SubVector(&BoneColliderVector,&st_BonePos,&st_Center);
                f_Distance = MATH_f_NormVector(&BoneColliderVector);
                f_Distance = f_Radius - f_Distance;                

                if (f_Distance > 0)
                {
                    float fForce;

                    // Apply a force on bone proportional of distance from sphere radius
                    MATH_ScaleEqualVector(&BoneColliderVector,_p_Vine->fForceApplied * f_Distance);
                MATH_AddEqualVector(&pst_CurrentVertice->m_Accel, &BoneColliderVector);
                    fForce = MATH_f_SqrNormVector (&pst_CurrentVertice->m_Accel);
                    if (fForce > _p_Vine->fMaxForce)
                    {
                        float fScaleFactor;
                        fScaleFactor = _p_Vine->fMaxForce / fForce;
                        MATH_ScaleEqualVector(&pst_CurrentVertice->m_Accel, fScaleFactor);
                    }

                _p_Vine->ul_Paused = FALSE;
                _p_Vine->p_FirstVertices->m_pBone->ul_StatusAndControlFlags = _p_Vine->p_FirstVertices->m_pBone->ul_StatusAndControlFlags | OBJ_C_ControlFlag_AlwaysVisible;

                    // Broke vine on contact
                    if (_p_Vine->ul_ChildHookBrokeOnContact)
                    {
                        _p_Vine->ul_BrokenVines = TRUE;
                    }
                }                
            }            
        }
        // Dampening
        if (_p_Vine->ul_Paused == FALSE)
        {
            MATH_tdst_Vector DampeningForce;
            
            MATH_SubVector(&DampeningForce,&pst_CurrentVertice->m_PrevPos,&pst_CurrentVertice->m_CurrPos);
            MATH_ScaleEqualVector(&DampeningForce,_p_Vine->fDampeningFactor);
            DampeningForce.z = 0.0f;
            MATH_AddEqualVector(&pst_CurrentVertice->m_Accel, &DampeningForce);
        }
        pst_CurrentVertice = pst_CurrentVertice->m_pFather;
    }
}
#else //JADEFUSION
void MDFModifier_Vine_ApplyForces(VINE_tdst_Modifier * _p_Vine)
{
    GAO_tdst_VineVertex     *pst_CurrentVertice = NULL;    
    ULONG                   ul_Dampening = TRUE;
    WOR_tdst_World	        *pst_World;
    ULONG                   ul_NumCollider, ul_Counter;


    // Temporary apply force "function"
    pst_CurrentVertice = _p_Vine->p_FirstVertices;
    while (pst_CurrentVertice != NULL)
    {
        MATH_InitVector(&pst_CurrentVertice->m_Accel, 0.0f, 0.0f, _p_Vine->fGravity );
        
        /* Get world of the object */
        pst_World = WOR_World_GetWorldOfObject(pst_CurrentVertice->m_pBone);

        ul_NumCollider = COL_ListCreate(pst_CurrentVertice->m_pBone,OBJ_C_IdentityFlag_ZDE,0,OBJ_C_OR_OR_IdentityFlags,
            (UCHAR)1,pst_World,(ULONG *) _p_Vine->pst_Colliders,(UCHAR) 0xFE);
        
        if (ul_NumCollider)
        {
            // For every collision, apply a force Object -- Vine
            for (ul_Counter = 0; ul_Counter < ul_NumCollider; ul_Counter++)
            {
                // Collision with this object
                MATH_tdst_Vector BonePosition;
                MATH_tdst_Vector ColliderPosition;
                MATH_tdst_Vector BoneColliderVector;

                if (_p_Vine->ul_ChildHookBrokeOnContact)
                {
                    _p_Vine->ul_BrokenVines = TRUE;
                }

                // Collision with first object in collision list
                MATH_CopyVector(&BonePosition,&pst_CurrentVertice->m_pBone->pst_GlobalMatrix->T);
                MATH_CopyVector(&ColliderPosition,&_p_Vine->pst_Colliders[ul_Counter]->pst_GlobalMatrix->T);
                MATH_SubVector(&BoneColliderVector,&BonePosition,&ColliderPosition);
                MATH_NormalizeEqualVector(&BoneColliderVector);
                MATH_ScaleEqualVector(&BoneColliderVector,_p_Vine->fForceApplied);

                MATH_AddEqualVector(&pst_CurrentVertice->m_Accel, &BoneColliderVector);

                _p_Vine->ul_Paused = FALSE;
            }            
        }

        // Dampening
		if ( ( ul_Dampening == ( ULONG ) TRUE ) && ( _p_Vine->ul_Paused == ( ULONG ) FALSE ) )
	   {
            MATH_tdst_Vector DampeningForce;
            
            MATH_SubVector(&DampeningForce,&pst_CurrentVertice->m_PrevPos,&pst_CurrentVertice->m_CurrPos);
            MATH_ScaleEqualVector(&DampeningForce,_p_Vine->fDampeningFactor);
			MATH_AddEqualVector(&pst_CurrentVertice->m_Accel, &DampeningForce);
        }
        pst_CurrentVertice = pst_CurrentVertice->m_pFather;
    }
}
#endif //JADEFUSION

//------------------------------------------------------------------------
//
// Author			Jean-Sylvain Sormany
// Date				19 april 2005
// 
// Prototype		MDFModifier_Vine_Solver
// Parameters		_p_Vine : 
//					_f_DT : 
// Return Type		void
// 
// Description		Verlet method : x2 = 2*x1 - x0 + a * dt^2
// 
//------------------------------------------------------------------------
void MDFModifier_Vine_Solver(VINE_tdst_Modifier * _p_Vine, FLOAT _f_DT)
{
    MATH_tdst_Vector    st_Tmp1;
    MATH_tdst_Vector    st_Tmp2;
    GAO_tdst_VineVertex* p_Vertex;    

    if(_f_DT > 1.0f/30.0f)
    {
        _f_DT = 1.0f/30.0f;
    }

    // Normal moves
    p_Vertex = _p_Vine->p_FirstVertices;
#ifdef JADEFUSION
    if (_p_Vine->ul_ChildIsAlsoHook && _p_Vine->ul_BrokenVines == FALSE)
    {
        p_Vertex = _p_Vine->p_FirstVertices->m_pFather;
    }
#endif
	while(p_Vertex != NULL)
    {
        // Make sure the hook does not move (Apply Verlet method to every other node)
        if (p_Vertex != _p_Vine->p_HookVertices)
        {
            MATH_ScaleVector(&st_Tmp1, &p_Vertex->m_CurrPos, 2.0f);
            MATH_SubEqualVector(&st_Tmp1, &p_Vertex->m_PrevPos);
            MATH_ScaleVector(&st_Tmp2, &p_Vertex->m_Accel, _f_DT*_f_DT);
            MATH_CopyVector(&p_Vertex->m_PrevPos, &p_Vertex->m_CurrPos);
            MATH_AddVector(&p_Vertex->m_CurrPos, &st_Tmp1, &st_Tmp2);
            MATH_InitVectorToZero(&p_Vertex->m_Accel);     
        }
        p_Vertex = p_Vertex->m_pFather;
    }
}

//------------------------------------------------------------------------
//
// Author			Jean-Sylvain Sormany
// Date				19 april 2005
// 
// Prototype		VINE_Modifier_Normalize
// Parameters		_p_Rope : 
// Return Type		void
// 
// Description		
// 
//------------------------------------------------------------------------
void VINE_Modifier_Normalize(VINE_tdst_Modifier * _p_Vine)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    MATH_tdst_Vector	*pst_PreviousPos;
    MATH_tdst_Vector    *pst_CurrentPos;
    MATH_tdst_Vector    stDep;
    MATH_tdst_Vector	st_Dist;
    FLOAT				fBF;
    GAO_tdst_VineVertex *p_CurrVertex;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    p_CurrVertex = _p_Vine->p_HookVertices->m_pChildren;    
    pst_PreviousPos = &(_p_Vine->p_HookVertices->m_pBone->pst_GlobalMatrix->T);

    while (p_CurrVertex != NULL)
    {
        pst_CurrentPos = &p_CurrVertex->m_CurrPos;

        MATH_SubVector(&st_Dist, pst_PreviousPos, pst_CurrentPos);
        fBF = MATH_f_NormVector(&st_Dist);
        fBF -= p_CurrVertex->fDistanceWithFather;

        MATH_NormalizeAnyVector(&st_Dist, &st_Dist);

        if (p_CurrVertex->m_pFather == _p_Vine->p_HookVertices)
        {
            MATH_ScaleVector(&stDep, &st_Dist, fBF);
            MATH_AddEqualVector(pst_CurrentPos,  &stDep);
        }
        else
        {
            MATH_ScaleVector(&stDep, &st_Dist, fBF * 0.5f);
            MATH_SubEqualVector(pst_PreviousPos, &stDep);
            MATH_AddEqualVector(pst_CurrentPos,  &stDep);
        }

        pst_PreviousPos = &p_CurrVertex->m_CurrPos;   
        p_CurrVertex = p_CurrVertex->m_pChildren;
    }

    if (_p_Vine->ul_ChildIsAlsoHook && _p_Vine->ul_BrokenVines == FALSE)
    {
        p_CurrVertex = _p_Vine->p_FirstVertices->m_pFather;    
        pst_PreviousPos = &(_p_Vine->p_FirstVertices->m_pBone->pst_GlobalMatrix->T);

        while (p_CurrVertex != NULL)
        {
            pst_CurrentPos = &p_CurrVertex->m_CurrPos;

            MATH_SubVector(&st_Dist, pst_PreviousPos, pst_CurrentPos);
            fBF = MATH_f_NormVector(&st_Dist);
            fBF -= p_CurrVertex->m_pChildren->fDistanceWithFather;

            MATH_NormalizeAnyVector(&st_Dist, &st_Dist);

            if (p_CurrVertex->m_pChildren == _p_Vine->p_FirstVertices)
            {
                MATH_ScaleVector(&stDep, &st_Dist, fBF);
                MATH_AddEqualVector(pst_CurrentPos,  &stDep);
            }
            else
            {
                MATH_ScaleVector(&stDep, &st_Dist, fBF * 0.5f);
                MATH_SubEqualVector(pst_PreviousPos, &stDep);
                MATH_AddEqualVector(pst_CurrentPos,  &stDep);
            }

            pst_PreviousPos = &p_CurrVertex->m_CurrPos;   
            p_CurrVertex = p_CurrVertex->m_pFather;
        }
    }

}

