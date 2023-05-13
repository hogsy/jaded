/*$T OBJinit.c GC! 1.081 09/28/00 10:21:02 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Creation and Destruction of all the objects the world */
#include "Precomp.h"
#include "ENGine/Sources/OBJects/OBJ.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJsave.h"
#include "ENGine/Sources/OBJects/OBJgizmo.h"
#include "ENGine/Sources/OBJects/OBJslowaccess.h"
#include "OBJBoundingVolume.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/BAStypes.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "stdio.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "ENGine/Sources/EOT/EOTmain.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ACTions/ACTinit.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AImsg.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "AIinterp/Sources/Events/EVEsave.h"
#include "AIinterp/Sources/Events/EVEload.h"
#include "COLlision/COLinit.h"
#include "COLlision/COLload.h"
#include "INTersection/INTSnP.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/DYNamics/DYNinit.h"
#include "ENGine/Sources/DYNamics/DYNconst.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDbank.h"
#include "MATHs/MATH_MEM.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"

#ifdef JADEFUSION
#include "texture/texfile.h"
#include "texture/texstruct.h"
#include "LIGHT/LIGHTmap.h"
#include "ENGine/Sources/Wind/CurrentWind.h"

#if defined(_XENON_RENDER)
#include "XenonGraphics/XeRenderer.h"
#include "XenonGraphics/XeUtils.h"
#include "XenonGraphics/XeMesh.h"

#include "GraphicDK/Sources/GEOmetric/GEOXenonPack.h"
#include "GraphicDK/Sources/GEOmetric/GEOstaticLOD.h"

#include "Light\LIGHTrejection.h"
#endif
#endif

#ifdef Active_CloneListe
#include "ENGine/Sources/INTersection/INTmain.h"
#endif

#ifdef ACTIVE_EDITORS
#include "GEOmetric/GEODebugObject.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "EDIpaths.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

extern ULONG		LOA_ul_FileTypeSize[40];
extern void AI_EvalFunc_MSGClear_C(OBJ_tdst_GameObject *);

BOOL OBJ_gb_DuplicateAI = FALSE;	/* True when duplication occurs in AI */
extern	BOOL	EDI_gb_ComputeMap;
extern OBJ_tdst_GameObject *AI_gp_ResolveGO;
static int i_CloneIdenticNumber=0;

//#if !defined(XML_CONV_TOOL)//popoverif il y etait plus

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ResolveAIRefSecondPass(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Model	*pst_Model;
	ULONG			i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Univ
	if
	(
		(!_pst_GO)
	||	(
			(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_AI))
		&&	(_pst_GO->pst_Extended)
		&&	(_pst_GO->pst_Extended->pst_Ai)
		&&	(_pst_GO->pst_Extended->pst_Ai->pst_Model)
		)
	)
	{
		if(!_pst_GO) pst_Model = WOR_gst_Universe.pst_AI->pst_Model;
		else pst_Model = _pst_GO->pst_Extended->pst_Ai->pst_Model;

		/* Init AI 2 C */
		if(pst_Model->pfn_CFunc) 
		{
			pst_Model->pfn_CFunc();
		}

		for(i = 0; i < pst_Model->u32_NbProcList; i++)
		{
            if (!pst_Model->pp_ProcList[i])
            {
                ERR_X_Warning(0,"Jade internal error in OBJ_ResolveAIRefSecondPass",(_pst_GO ? _pst_GO->sz_Name : NULL));
                continue;
            }
			if(pst_Model->pp_ProcList[i]->pfn_CFunc)
			{
				pst_Model->pp_ProcList[i]->pfn_CFunc();
			}
		}
	}
}

/*
 =======================================================================================================================
    Aim:    After an object has been loaded, we must resolve all the references concerning AI, COL. This cannot be
            completed during the loading procedure because we are not sure that models and instances have been entirely
            loaded.
 =======================================================================================================================
 */
void OBJ_ResolveAIRef(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i, j;
	ACT_st_ActionKit	*pst_Kit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	AI_gp_ResolveGO = _pst_GO;
#endif

	/* AI */
	if
	(
		(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_AI))
	&&	(_pst_GO->pst_Extended)
	&&	(_pst_GO->pst_Extended->pst_Ai)
	)
	{
#ifdef ACTIVE_EDITORS
		((AI_tdst_Instance *) _pst_GO->pst_Extended->pst_Ai)->pst_GameObject = _pst_GO;
#endif
		AI_ResolveInstanceRef((AI_tdst_Instance *) _pst_GO->pst_Extended->pst_Ai);
		AI_ReinitInstance((AI_tdst_Instance *) _pst_GO->pst_Extended->pst_Ai);
	}

	/* Update events */
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Events))
	{
#ifdef ACTIVE_EDITORS
		AI_gp_ResolveGO = _pst_GO;
#endif
		EVE_ResolveRefs(_pst_GO, TRUE);
	}

	/* Anims */
	if
	(
		(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
	&&	(_pst_GO->pst_Base)
	&&	(_pst_GO->pst_Base->pst_GameObjectAnim)
	)
	{
		if(_pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit)
		{
			pst_Kit = _pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit;
			for(i = 0; i < pst_Kit->uw_NumberOfAction; i++)
			{
				if(pst_Kit->apst_Action[i])
				{
					for(j = 0; j < pst_Kit->apst_Action[i]->uc_NumberOfActionItem; j++)
					{
						if(pst_Kit->apst_Action[i]->ast_ActionItem[j].pst_TrackList)
							EVE_ResolveListTrackRef(pst_Kit->apst_Action[i]->ast_ActionItem[j].pst_TrackList);

					}
				}
			}

#ifdef ACTIVE_EDITORS
		if(EDI_gb_ComputeMap) ACT_DegradeActionKit(_pst_GO, pst_Kit);
#endif		
		}
		else if
			(
				(_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0])
			&&	((ULONG) (_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]) != (ULONG) - 1)
			&&	(_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data)
			)
		{
			EVE_ResolveListTrackRef(_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks);
		}
	}

}

/*
 =======================================================================================================================
    Aim:    Reinit object
 =======================================================================================================================
 */
void OBJ_Reinit(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct *pst_Gro;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Bone) 
		 && (_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		 && _pst_GO->pst_Base->pst_Hierarchy->pst_Father
	)
	{
		OBJ_Reinit(_pst_GO->pst_Base->pst_Hierarchy->pst_Father);
	}
	//else POPOWARNING en + sur xenon
	OBJ_RestoreInitialPos(_pst_GO);

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
	{
		DYN_tdst_ODE	*pst_ODE;
		dMass			Mass;
		WOR_tdst_World	*pst_World;

		pst_ODE = _pst_GO->pst_Base->pst_ODE;
		pst_World = WOR_World_GetWorldOfObject(_pst_GO);

		if((pst_ODE->ode_id_geom) && (pst_ODE->uc_Type != ODE_TYPE_PLAN))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			float				rot[12];
			MATH_tdst_Vector	st_Offset;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			MATH_TransformVector(&st_Offset, _pst_GO->pst_GlobalMatrix, &pst_ODE->st_Offset);

			dGeomSetPosition(pst_ODE->ode_id_geom, _pst_GO->pst_GlobalMatrix->T.x + st_Offset.x, _pst_GO->pst_GlobalMatrix->T.y + + st_Offset.y, _pst_GO->pst_GlobalMatrix->T.z + + st_Offset.z);

			ODE_ComputeODEMatrixFromJade(_pst_GO->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot);

			dGeomSetRotation(pst_ODE->ode_id_geom, &rot[0]);
		}

		if(pst_ODE->uc_Flags & ODE_FLAGS_FORCEIMMOVABLE)
		{
			pst_ODE->uc_Flags &=  ~ODE_FLAGS_FORCEIMMOVABLE;

			if(pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
			{
				pst_ODE->ode_id_body = dBodyCreate(pst_World->ode_id_world);
				dBodySetData(pst_ODE->ode_id_body, _pst_GO);

				pst_ODE->uc_Flags |=  ODE_FLAGS_ENABLE;
				if(pst_ODE->ode_id_geom)
					dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
			}
		}

		if(pst_ODE->ode_id_body)
		{
			switch(pst_ODE->uc_Type)
			{
			case ODE_TYPE_SPHERE:
				{					
					dMassSetSphereTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X);
					dBodySetMass(pst_ODE->ode_id_body, &Mass);
				}
				break;

			case ODE_TYPE_BOX:
				{
					dMassSetBoxTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
					dBodySetMass(pst_ODE->ode_id_body, &Mass);
				}
				break;

			case ODE_TYPE_CYLINDER:
				{
					dMassSetCylinderTotal(&Mass, pst_ODE->mass_init, 2, pst_ODE->f_X, pst_ODE->f_Y);
					dBodySetMass(pst_ODE->ode_id_body, &Mass);
				}
				break;
			}

		}

		pst_ODE->bounce = pst_ODE->bounce_init;
		pst_ODE->mu = pst_ODE->mu;


		if(pst_ODE->ode_id_body)
		{	
			pst_ODE->uc_Flags |=  ODE_FLAGS_ENABLE;

			if(pst_ODE->uc_Flags & ODE_FLAGS_PAUSEINIT)
			{
				dBodyDisable(pst_ODE->ode_id_body);

				if(pst_ODE->ode_id_geom && (pst_ODE->uc_Flags & ODE_FLAGS_MATHCOLMAP) && (pst_ODE->uc_Type != ODE_TYPE_PLAN))
				{
					float				rot[12];
					MATH_tdst_Vector	st_Offset;

					MATH_TransformVector(&st_Offset, _pst_GO->pst_GlobalMatrix, &pst_ODE->st_Offset);


					ODE_ComputeODEMatrixFromJade(_pst_GO->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot); 

					dGeomSetPositionNoDirty(pst_ODE->ode_id_geom, _pst_GO->pst_GlobalMatrix->T.x + st_Offset.x, _pst_GO->pst_GlobalMatrix->T.y + st_Offset.y, _pst_GO->pst_GlobalMatrix->T.z + st_Offset.z); 
					dGeomSetRotationNoDirty(pst_ODE->ode_id_geom, &rot[0]);
				}

			}
			else
				dBodyEnable(pst_ODE->ode_id_body);

			dBodySetLinearVel(pst_ODE->ode_id_body, 0.0f, 0.0f, 0.0f);
			dBodySetAngularVel(pst_ODE->ode_id_body, 0.0f, 0.0f, 0.0f);
			dBodySetForce(pst_ODE->ode_id_body, 0.0f, 0.0f, 0.0f);
			dBodySetTorque(pst_ODE->ode_id_body, 0.0f, 0.0f, 0.0f);
		}
	}
#endif
	if(_pst_GO->pst_Base)
	{
		_pst_GO->pst_Base->pst_AddMaterial = NULL;
		if (OBJ_b_TestIdentityFlag( _pst_GO, OBJ_C_IdentityFlag_Visu ) && _pst_GO->pst_Base->pst_Visu )
#ifdef JADEFUSION
			_pst_GO->pst_Base->pst_Visu->ul_DrawMask |= GDI_Cul_DM_DontForceSorted | GDI_Cul_DM_UseNormalMaterial | GDI_Cul_DM_DontForceSorted | GDI_Cul_DM_DontAttenuateLight;
#else
			_pst_GO->pst_Base->pst_Visu->ul_DrawMask |= GDI_Cul_DM_UseNormalMaterial | GDI_Cul_DM_DontAttenuateLight;
#endif
	}

	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)
	{
		pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
		if(pst_Gro && pst_Gro->i) pst_Gro->i->pfn_Reinit(pst_Gro);
		pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Material;
		if(pst_Gro && pst_Gro->i) pst_Gro->i->pfn_Reinit(pst_Gro);
	}

#ifdef ACTIVE_EDITORS
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_AdditionalMatrix))
	{
		if
		(
			(_pst_GO->pst_Base)
		&&	(_pst_GO->pst_Base->pst_AddMatrix)
		&&	(_pst_GO->pst_Base->pst_AddMatrix->dpst_EditionGO)
		)
		{
			MEM_Free(_pst_GO->pst_Base->pst_AddMatrix->dpst_EditionGO);
			_pst_GO->pst_Base->pst_AddMatrix->dpst_EditionGO = NULL;
		}
	}

#endif
/*#ifdef PSX2_TARGET
			if (!OBJ_b_TestIdentityFlag(_pst_GO, (OBJ_C_IdentityFlag_Dyna|OBJ_C_IdentityFlag_Anims|OBJ_C_IdentityFlag_Bone|OBJ_C_IdentityFlag_Generated|OBJ_C_IdentityFlag_AI)))
			{
					if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Visu)) 
					{
						GRO_tdst_Struct		*pst_Gro;
						pst_Gro = _pst_GO->pst_Base->pst_Visu->pst_Object;
						if (pst_Gro && (pst_Gro->i->ul_Type == GRO_Geometric))
						{
							GEO_tdst_Object *p_GEO;
							p_GEO = (GEO_tdst_Object *)pst_Gro;
							if (!p_GEO->p_SKN_Objectponderation)
							{
								_pst_GO->pst_Base->pst_Visu->ul_DrawMask &= ~GDI_Cul_DM_NoAutoClone;
							}
						}
					}
			}
#endif//	*/
	MDF_Reinit(_pst_GO);
#ifdef JADEFUSION
    // Wind
    CCurrentWind * p_CurrWind = OBJ_pst_GetCurrentWind(_pst_GO);
    if (p_CurrWind)
    {
#ifdef _GAMECUBE    
        if(((u32)p_CurrWind & 0x80000000) != 0x80000000)
        {
            p_CurrWind = NULL;
            _pst_GO->pst_Extended->po_CurrentWind = NULL;
        }
        else
#endif        
        {
            p_CurrWind->Reinit();
        }
    }
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_SetInactive(OBJ_tdst_GameObject *_pst_GO)
{
	/* We remove the Dynamic hierarchy Link if there is one. */
	if
	(
		(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	&&	(_pst_GO->pst_Base)
	&&	(_pst_GO->pst_Base->pst_Hierarchy)
	&&	(!(_pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit))
	)
	{
		/* The Child has no longer a Father. We update its global Matrix. */
		OBJ_ComputeGlobalWhenHie(_pst_GO);

		/* Update all Flash Matrix of the Child (Blend matrix too) */
		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix)) ANI_RemoveHierarchyOnFlash(_pst_GO);

		_pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Hierarchy;

		MEM_Free(_pst_GO->pst_Base->pst_Hierarchy);
		_pst_GO->pst_Base->pst_Hierarchy = NULL;
	}

	/* set vol 0 if any sound instance */
	SND_RegisterInactiveGAO(_pst_GO);
}

//#endif // !defined(XML_CONV_TOOL)^//popoverif il n'y etait pas

/*
 =======================================================================================================================
    Aim:    Creates a new empty GameObject
 =======================================================================================================================
 */
#ifdef USE_DOUBLE_RENDERING
#define ENG_BDL_RDNG 2
extern void WOR_CreateObjectHook(OBJ_tdst_GameObject *pst_GO);
#define WOR_CREATE_OBJECT_SIGNAL(pst_GO) WOR_CreateObjectHook(pst_GO);
#else
#define ENG_BDL_RDNG 0
#define WOR_CREATE_OBJECT_SIGNAL(pst_GO)
#endif	 
 
OBJ_tdst_GameObject *OBJ_GameObject_Create(ULONG _ulIdentityFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Object;
	OBJ_tdst_Base		*pst_Base;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Allocate the main game object structure */
	pst_Object = (OBJ_tdst_GameObject *) MEM_p_Alloc(sizeof(struct OBJ_tdst_GameObject_));
	LOA_ul_FileTypeSize[37] += sizeof(struct OBJ_tdst_GameObject_);
#ifdef ACTIVE_EDITORS
	LOA_ul_FileTypeSize[37] -= (9 * 4);
#endif
	L_memset(pst_Object, 0, sizeof(struct OBJ_tdst_GameObject_));

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Depending on the identity flags we allocate what is needed
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* Set the identity flags */
	pst_Object->ul_IdentityFlags = _ulIdentityFlags;
	pst_Object->pst_BV = NULL;
	pst_Object->us_SnP_Ref = 0xFFFF;
	pst_Object->uc_VisiCoeff = 16;

#ifdef Active_CloneListe
	pst_Object->p_CloneNextGao = NULL;
	pst_Object->CloneLightList = NULL;
	i_CloneIdenticNumber=0;
#endif

	/* Set the default status and control flags */
	pst_Object->ul_StatusAndControlFlags = OBJ_C_DefaultStatusAndControlFlags;

	/* Set the default Design flags */
	pst_Object->uc_DesignFlags = 0;

	/* If it's an oriented object, we allocate its matrix */
	if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_HasInitialPos))
	{
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_FlashMatrix))
		{
            pst_Object->pst_GlobalMatrix = MATH_Matrix_AllocArray( 3+ ENG_BDL_RDNG );
			LOA_ul_FileTypeSize[29] += sizeof(MATH_tdst_Matrix) * 3;
			MATH_SetIdentityMatrix(pst_Object->pst_GlobalMatrix + 1);
			MATH_SetIdentityMatrix(pst_Object->pst_GlobalMatrix + 2);
		}
		else
		{
			pst_Object->pst_GlobalMatrix = MATH_Matrix_AllocArray( 2+ ENG_BDL_RDNG );
			LOA_ul_FileTypeSize[29] += sizeof(MATH_tdst_Matrix) * 2;
			MATH_SetIdentityMatrix(pst_Object->pst_GlobalMatrix + 1);
		}
	}
	else
	{
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_FlashMatrix))
		{
			pst_Object->pst_GlobalMatrix = MATH_Matrix_AllocArray( 2+ ENG_BDL_RDNG );
			LOA_ul_FileTypeSize[29] += sizeof(MATH_tdst_Matrix) * 2;
			MATH_SetIdentityMatrix(pst_Object->pst_GlobalMatrix + 1);
		}
		else
		{
			pst_Object->pst_GlobalMatrix = MATH_Matrix_AllocArray( 1+ ENG_BDL_RDNG );
			LOA_ul_FileTypeSize[29] += sizeof(MATH_tdst_Matrix);
		}
	}
	WOR_CREATE_OBJECT_SIGNAL(pst_Object);
	MATH_SetIdentityMatrix(pst_Object->pst_GlobalMatrix);

	/* It's an object with base data : alloc base data */
	if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_BaseObject))
	{
		pst_Object->pst_Base = (OBJ_tdst_Base *) MEM_p_Alloc(sizeof(OBJ_tdst_Base));
		LOA_ul_FileTypeSize[37] += sizeof(OBJ_tdst_Base);
		pst_Base = pst_Object->pst_Base;
		L_memset(pst_Base, 0, sizeof(OBJ_tdst_Base));

		/* Object with a visu */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Visu))
		{
			pst_Base->pst_Visu = (GRO_tdst_Visu *) MEM_p_Alloc(sizeof(GRO_tdst_Visu));
			LOA_ul_FileTypeSize[37] += sizeof(GRO_tdst_Visu);
			L_memset(pst_Base->pst_Visu, 0, sizeof(GRO_tdst_Visu));
			pst_Base->pst_Visu->ul_DrawMask = 0xFFFFFFFF;
#ifdef JADEFUSION
            pst_Base->pst_Visu->ul_DrawMask &= ~(GDI_Cul_DM_EmitShadowBuffer | GDI_Cul_DM_ReceiveShadowBuffer);

            // DJ_TEMP : remove once all the implementation is done
#ifdef ACTIVE_EDITORS
            pst_Base->pst_Visu->pst_LightmapSettings.bCastShadows = true;
            pst_Base->pst_Visu->pst_LightmapSettings.bCustomTexelRatio = false;
            pst_Base->pst_Visu->pst_LightmapSettings.bReceiveShadows = true;
            pst_Base->pst_Visu->pst_LightmapSettings.bTemporaryStopUsingLightmaps = false;
            pst_Base->pst_Visu->pst_LightmapSettings.bUseLightmaps = true;
            pst_Base->pst_Visu->pst_LightmapSettings.fTexelPerMeter = 2.0f;
#endif
#endif
		}

		/* Object with anims */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Anims))
		{
			pst_Base->pst_GameObjectAnim = ANI_pst_CreateGameObjectAnim();
		}

#ifdef ODE_INSIDE
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_ODE))
		{
			pst_Base->pst_ODE = (DYN_tdst_ODE *) MEM_p_Alloc(sizeof(DYN_tdst_ODE));
			L_memset(pst_Base->pst_ODE, 0, sizeof(DYN_tdst_ODE));

#ifdef ACTIVE_EDITORS
			pst_Base->pst_ODE->pst_GO = pst_Object;
#endif
			pst_Base->pst_ODE->ode_id_body = 0;
			pst_Base->pst_ODE->ode_id_geom = 0;

			/* Current version of the load/save ode process */
			pst_Base->pst_ODE->uc_Type = 0;

			/* Current version of the load/save ode process */
			pst_Base->pst_ODE->uc_Version = 6;

			/* Default Flags for ODE */
			pst_Base->pst_ODE->uc_Flags = 0;

			/* Sound */
			pst_Base->pst_ODE->uc_Sound = 0;

			/* Sound */
			pst_Base->pst_ODE->uc_Sound = 0;

			/* Surface */
			pst_Base->pst_ODE->SurfaceMode = dContactBounce;
			pst_Base->pst_ODE->mu = 5000.0f;
			pst_Base->pst_ODE->mu2 = 0;
			pst_Base->pst_ODE->bounce = 0.01f;
			pst_Base->pst_ODE->bounce_vel = 0;
			pst_Base->pst_ODE->soft_erp = 0;
			pst_Base->pst_ODE->soft_cfm = 0;
			pst_Base->pst_ODE->motion1 = 0;
			pst_Base->pst_ODE->motion2 = 0;
			pst_Base->pst_ODE->slip1 = 0;
			pst_Base->pst_ODE->slip2 = 0;
		}
		else
			pst_Base->pst_ODE = NULL;

#endif

		/* It's an object with Dynamics, we allocate the dynamics structure */

		/*$F --------------- USEFULL FOR DUPLICATION --------------- */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Dyna))
		{
			pst_Base->pst_Dyna = DYN_pst_Dyna_Create();

			/*
			 * Make the position pointer of the Dyna structure point to the position vector of
			 * the Game object
			 */
			DYN_Dyna_Init
			(
				pst_Base->pst_Dyna,
				DYN_C_DefaultDynaFlags,
				Cf_One,					/* Mass=1 */
				Cf_Infinit,				/* No speed Limit Horiz */
				Cf_Infinit,				/* No speed Limit Vert */
				&MATH_gst_NulVector,	/* Init with null speed */
				OBJ_pst_GetAbsolutePosition(pst_Object)
			);

			/* DYN_Dyna_RegisterWithName(pst_Base->pst_Dyna, NULL, TRUE); */
		}

		/* Object with hierarchy */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Hierarchy))
		{
#if defined(_XBOX) || defined(_XENON)
			pst_Object->pst_Base->pst_Hierarchy = (OBJ_tdst_Hierarchy *) MEM_p_AllocAlign(sizeof(OBJ_tdst_Hierarchy),16);
#else
			pst_Object->pst_Base->pst_Hierarchy = (OBJ_tdst_Hierarchy *) MEM_p_VMAlloc(sizeof(OBJ_tdst_Hierarchy));
#endif
			LOA_ul_FileTypeSize[37] += sizeof(OBJ_tdst_Hierarchy);
			pst_Object->pst_Base->pst_Hierarchy->pst_Father = NULL;
			MATH_SetIdentityMatrix(&pst_Object->pst_Base->pst_Hierarchy->st_LocalMatrix);
		}

		/* Object with additional matrix */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_AddMatArePointer))
			pst_Object->ul_IdentityFlags |= OBJ_C_IdentityFlag_AdditionalMatrix;

		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_AdditionalMatrix))
		{
			pst_Object->pst_Base->pst_AddMatrix = (OBJ_tdst_AdditionalMatrix *) MEM_p_Alloc(sizeof(OBJ_tdst_AdditionalMatrix));
			LOA_ul_FileTypeSize[37] += sizeof(OBJ_tdst_AdditionalMatrix);
			pst_Object->pst_Base->pst_AddMatrix->l_Number = 0;
			pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo = NULL;
#ifdef ACTIVE_EDITORS
			pst_Object->pst_Base->pst_AddMatrix->pst_GO = pst_Object;
			pst_Object->pst_Base->pst_AddMatrix->dpst_EditionGO = NULL;
#endif
		}
	}

	/* Bounding Volume: OBBox and ABBox. */
	if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_OBBox))
	{
		pst_Object->pst_BV = MEM_p_VMAlloc(sizeof(OBJ_tdst_ComplexBV));
		LOA_ul_FileTypeSize[37] += sizeof(OBJ_tdst_ComplexBV);
		L_memset(pst_Object->pst_BV, 0, sizeof(OBJ_tdst_ComplexBV));
	}
	else
	{
		pst_Object->pst_BV = MEM_p_Alloc(sizeof(OBJ_tdst_SingleBV));
		LOA_ul_FileTypeSize[37] += sizeof(OBJ_tdst_SingleBV);
		L_memset(pst_Object->pst_BV, 0, sizeof(OBJ_tdst_SingleBV));
	}

	/* It's an object with extended data : alloc extended data */
	if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_ExtendedObject))
	{
		OBJ_GameObject_CreateExtended(pst_Object);

		if
		(
			OBJ_b_TestIdentityFlag
			(
				pst_Object,
				OBJ_C_IdentityFlag_ColMap | OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE
			)
		)
		{
			/* Allocation of the COL Base structure. */
			pst_Object->pst_Extended->pst_Col = MEM_p_Alloc(sizeof(COL_tdst_Base));
			LOA_ul_FileTypeSize[37] += sizeof(COL_tdst_Base);
			L_memset(pst_Object->pst_Extended->pst_Col, 0, sizeof(COL_tdst_Base));
		}

		/* SnP detection list */
		if((pst_Object->ul_IdentityFlags & OBJ_C_IdentityFlag_AI) && !(pst_Object->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Detection))
		{
			OBJ_SetStatusFlag(pst_Object, OBJ_C_StatusFlag_Detection);
			COL_AllocDetectionList(pst_Object);
		}

		/* Object with an msg struct */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Msg))
		{
			pst_Object->pst_Extended->pst_Msg = MEM_p_Alloc(sizeof(AI_tdst_MessageLiFo));
			LOA_ul_FileTypeSize[37] += sizeof(AI_tdst_MessageLiFo);
			L_memset(pst_Object->pst_Extended->pst_Msg, 0, sizeof(AI_tdst_MessageLiFo));
		}

		/* Init Priority Level */
		pst_Object->pst_Extended->uc_AiPrio = 127;
	}

#ifdef ACTIVE_EDITORS
	((OBJ_tdst_SingleBV *) pst_Object->pst_BV)->pst_GO = pst_Object;
	pst_Object->ul_EditorFlags = 0;
	pst_Object->sz_Name = NULL;
	pst_Object->pst_Trace = NULL;
	pst_Object->ul_InvisibleObjectIndex = 0;

#ifdef JADEFUSION
	// -NOTE- Added a bitfield to know if this GAO's properties can be
	// merged when integrating from PS2 Data (by default everything is merged)
	pst_Object->ul_XeMergeFlags = NULL ;	
#endif

#endif
	return(pst_Object);
}

//#if !defined(XML_CONV_TOOL) //il etait plus
/*
 =======================================================================================================================
    Aim:    Creates a new copy of an existing GameObject

    In:     _b_CreateFile   TRUE if we must create new files (for editor duplication).
 =======================================================================================================================
 */
OBJ_tdst_GameObject *OBJ_GameObject_Duplicate
(
	WOR_tdst_World		*pst_World,
	OBJ_tdst_GameObject *_pst_SrcGO,
	BOOL				_b_AddinTable,
	BOOL				_b_CreateFile,
	MATH_tdst_Vector	*_pst_SrcPos,
	ULONG				_ul_PrefabKey,
	char				*asz_NewName
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_NewGO;
	ULONG ul;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* The World info is not necessary if we are not in Creation File mode. */
	ERR_X_Assert(pst_World || !_b_CreateFile);

	ul = OBJ_ul_FlagsIdentityGet(_pst_SrcGO);

#ifdef ACTIVE_EDITORS
	if(_ul_PrefabKey) ul &= ~OBJ_C_IdentityFlag_Bone;
#endif

	pst_NewGO = OBJ_GameObject_Create(ul | OBJ_C_IdentityFlag_Generated);
	pst_NewGO->c_FixFlags = _pst_SrcGO->c_FixFlags;
	pst_NewGO->c_FixFlags &= ~OBJ_C_HasBeenMerge;
	pst_NewGO->uc_DesignFlags = _pst_SrcGO->uc_DesignFlags;
	pst_NewGO->uc_VisiCoeff = _pst_SrcGO->uc_VisiCoeff;

#ifdef ACTIVE_EDITORS
	pst_NewGO->ul_EditorFlags = _pst_SrcGO->ul_EditorFlags;
	pst_NewGO->ul_ObjectModel = _pst_SrcGO->ul_ObjectModel;
	pst_NewGO->ul_InvisibleObjectIndex = _pst_SrcGO->ul_InvisibleObjectIndex;
	pst_NewGO->ul_ForceLODIndex = _pst_SrcGO->ul_ForceLODIndex;
	pst_NewGO->ul_User3 = _pst_SrcGO->ul_User3;
#endif

	/* Custom bits */
	pst_NewGO->ul_StatusAndControlFlags &= ~OBJ_C_CustomBitAll;
	pst_NewGO->ul_StatusAndControlFlags |= (_pst_SrcGO->ul_StatusAndControlFlags & OBJ_C_CustomBitAll);

	/* Links (never duplicate links) */
	pst_NewGO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Links;

#ifdef ODE_INSIDE
	if(_pst_SrcGO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
	{
		dMass		Mass;

		pst_NewGO->pst_Base->pst_ODE->uc_Flags = _pst_SrcGO->pst_Base->pst_ODE->uc_Flags;
		pst_NewGO->pst_Base->pst_ODE->uc_Version = _pst_SrcGO->pst_Base->pst_ODE->uc_Version;

		if(pst_NewGO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
		{
			pst_NewGO->pst_Base->pst_ODE->ode_id_body = dBodyCreate(pst_World->ode_id_world);
			dBodySetData(pst_NewGO->pst_Base->pst_ODE->ode_id_body, pst_NewGO);

			if(pst_NewGO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_AUTODISABLE)
			{
				dBodySetAutoDisableFlag(pst_NewGO->pst_Base->pst_ODE->ode_id_body, 1);
				dBodySetAutoDisableLinearThreshold(pst_NewGO->pst_Base->pst_ODE->ode_id_body, pst_NewGO->pst_Base->pst_ODE->f_LinearThres);
				dBodySetAutoDisableAngularThreshold(pst_NewGO->pst_Base->pst_ODE->ode_id_body, pst_NewGO->pst_Base->pst_ODE->f_AngularThres);

				dBodySetAutoDisableSteps (pst_NewGO->pst_Base->pst_ODE->ode_id_body, 10);
			}
			else
			{
				dBodySetAutoDisableFlag(pst_NewGO->pst_Base->pst_ODE->ode_id_body, 0);
			}

		}
		pst_NewGO->pst_Base->pst_ODE->uc_Type = _pst_SrcGO->pst_Base->pst_ODE->uc_Type;

		pst_NewGO->pst_Base->pst_ODE->mass_init		= _pst_SrcGO->pst_Base->pst_ODE->mass_init;
		pst_NewGO->pst_Base->pst_ODE->bounce		= _pst_SrcGO->pst_Base->pst_ODE->bounce;
		pst_NewGO->pst_Base->pst_ODE->bounce_init	= _pst_SrcGO->pst_Base->pst_ODE->bounce_init;
		pst_NewGO->pst_Base->pst_ODE->bounce_vel	= _pst_SrcGO->pst_Base->pst_ODE->bounce_vel;
		pst_NewGO->pst_Base->pst_ODE->motion1		= _pst_SrcGO->pst_Base->pst_ODE->motion1;
		pst_NewGO->pst_Base->pst_ODE->motion2		= _pst_SrcGO->pst_Base->pst_ODE->motion2;
		pst_NewGO->pst_Base->pst_ODE->mu_init		= _pst_SrcGO->pst_Base->pst_ODE->mu_init;
		pst_NewGO->pst_Base->pst_ODE->mu			= pst_NewGO->pst_Base->pst_ODE->mu;
		pst_NewGO->pst_Base->pst_ODE->mu2			= _pst_SrcGO->pst_Base->pst_ODE->mu2;
		pst_NewGO->pst_Base->pst_ODE->slip1			= _pst_SrcGO->pst_Base->pst_ODE->slip1;
		pst_NewGO->pst_Base->pst_ODE->slip2			= _pst_SrcGO->pst_Base->pst_ODE->slip2;
		pst_NewGO->pst_Base->pst_ODE->soft_cfm		= _pst_SrcGO->pst_Base->pst_ODE->soft_cfm;
		pst_NewGO->pst_Base->pst_ODE->soft_erp		= _pst_SrcGO->pst_Base->pst_ODE->soft_erp;
		pst_NewGO->pst_Base->pst_ODE->f_LinearThres		= _pst_SrcGO->pst_Base->pst_ODE->f_LinearThres;
		pst_NewGO->pst_Base->pst_ODE->f_AngularThres	= _pst_SrcGO->pst_Base->pst_ODE->f_AngularThres;


		MATH_CopyVector(&pst_NewGO->pst_Base->pst_ODE->st_Offset, &_pst_SrcGO->pst_Base->pst_ODE->st_Offset);
		MATH_CopyMatrix(&pst_NewGO->pst_Base->pst_ODE->st_RotMatrix, &_pst_SrcGO->pst_Base->pst_ODE->st_RotMatrix);

		if(pst_NewGO->pst_Base->pst_ODE->uc_Type)
		{
			pst_NewGO->pst_Base->pst_ODE->f_X = _pst_SrcGO->pst_Base->pst_ODE->f_X;
			pst_NewGO->pst_Base->pst_ODE->f_Y = _pst_SrcGO->pst_Base->pst_ODE->f_Y;
			pst_NewGO->pst_Base->pst_ODE->f_Z = _pst_SrcGO->pst_Base->pst_ODE->f_Z;

			if(!pst_NewGO->pst_Base->pst_ODE->mass_init)
				pst_NewGO->pst_Base->pst_ODE->mass_init = 1.0f;

			switch(pst_NewGO->pst_Base->pst_ODE->uc_Type)
			{
				case ODE_TYPE_SPHERE:
					dMassSetSphereTotal(&Mass, pst_NewGO->pst_Base->pst_ODE->mass_init, pst_NewGO->pst_Base->pst_ODE->f_X);

					pst_NewGO->pst_Base->pst_ODE->ode_id_geom = dCreateSphere(pst_World->ode_id_space, pst_NewGO->pst_Base->pst_ODE->f_X);
					dGeomSetData(pst_NewGO->pst_Base->pst_ODE->ode_id_geom, (void *) pst_NewGO);
					dGeomSetBody(pst_NewGO->pst_Base->pst_ODE->ode_id_geom, pst_NewGO->pst_Base->pst_ODE->ode_id_body);
					if(pst_NewGO->pst_Base->pst_ODE->ode_id_body)
						dBodySetMass(pst_NewGO->pst_Base->pst_ODE->ode_id_body, &Mass);
					break;

				case ODE_TYPE_BOX:
					dMassSetBoxTotal(&Mass, pst_NewGO->pst_Base->pst_ODE->mass_init, pst_NewGO->pst_Base->pst_ODE->f_X, pst_NewGO->pst_Base->pst_ODE->f_Y, pst_NewGO->pst_Base->pst_ODE->f_Z);

					pst_NewGO->pst_Base->pst_ODE->ode_id_geom = dCreateBox(pst_World->ode_id_space, pst_NewGO->pst_Base->pst_ODE->f_X, pst_NewGO->pst_Base->pst_ODE->f_Y, pst_NewGO->pst_Base->pst_ODE->f_Z);
					dGeomSetData(pst_NewGO->pst_Base->pst_ODE->ode_id_geom, (void *) pst_NewGO);
					dGeomSetBody(pst_NewGO->pst_Base->pst_ODE->ode_id_geom, pst_NewGO->pst_Base->pst_ODE->ode_id_body);
					if(pst_NewGO->pst_Base->pst_ODE->ode_id_body)	
						dBodySetMass(pst_NewGO->pst_Base->pst_ODE->ode_id_body, &Mass);
					break;

				case ODE_TYPE_CYLINDER:
					dMassSetCylinderTotal(&Mass, pst_NewGO->pst_Base->pst_ODE->mass_init, 2, pst_NewGO->pst_Base->pst_ODE->f_X, pst_NewGO->pst_Base->pst_ODE->f_Y);

					pst_NewGO->pst_Base->pst_ODE->ode_id_geom = dCreateCylinder(pst_World->ode_id_space, pst_NewGO->pst_Base->pst_ODE->f_X, pst_NewGO->pst_Base->pst_ODE->f_Y);
					dGeomSetData(pst_NewGO->pst_Base->pst_ODE->ode_id_geom, (void *) pst_NewGO);
					dGeomSetBody(pst_NewGO->pst_Base->pst_ODE->ode_id_geom, pst_NewGO->pst_Base->pst_ODE->ode_id_body);
					if(pst_NewGO->pst_Base->pst_ODE->ode_id_body)
						dBodySetMass(pst_NewGO->pst_Base->pst_ODE->ode_id_body, &Mass);
					break;


				case ODE_TYPE_PLAN:
					{
						MATH_tdst_Vector	st_Norm;

						MATH_InitVector(&st_Norm, pst_NewGO->pst_GlobalMatrix->Kx, pst_NewGO->pst_GlobalMatrix->Ky, pst_NewGO->pst_GlobalMatrix->Kz);
						if(_pst_SrcPos)
							pst_NewGO->pst_Base->pst_ODE->f_X = MATH_f_DotProduct(_pst_SrcPos, &st_Norm);
						else
							pst_NewGO->pst_Base->pst_ODE->f_X = MATH_f_DotProduct(&_pst_SrcGO->pst_GlobalMatrix->T, &st_Norm);

						pst_NewGO->pst_Base->pst_ODE->ode_id_geom = dCreatePlane(pst_World->ode_id_space, _pst_SrcGO->pst_GlobalMatrix->Kx, _pst_SrcGO->pst_GlobalMatrix->Ky, _pst_SrcGO->pst_GlobalMatrix->Kz, pst_NewGO->pst_Base->pst_ODE->f_X);
						dGeomSetData(pst_NewGO->pst_Base->pst_ODE->ode_id_geom, (void *) pst_NewGO);
					}

					break;

			}
		}
	}
#endif

	if(_b_CreateFile)
	{
		if(_pst_SrcGO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInactiveInit)
			pst_NewGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceInactiveInit;
		if(_pst_SrcGO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInvisibleInit)
			pst_NewGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceInvisibleInit;
		if(pst_NewGO->pst_Extended && _pst_SrcGO->pst_Extended)
		{
			pst_NewGO->pst_Extended->auc_Sectos[0] = _pst_SrcGO->pst_Extended->auc_Sectos[0];
			pst_NewGO->pst_Extended->auc_Sectos[1] = _pst_SrcGO->pst_Extended->auc_Sectos[1];
			pst_NewGO->pst_Extended->auc_Sectos[2] = _pst_SrcGO->pst_Extended->auc_Sectos[2];
			pst_NewGO->pst_Extended->auc_Sectos[3] = _pst_SrcGO->pst_Extended->auc_Sectos[3];
		}
	}

#ifdef ACTIVE_EDITORS
		pst_NewGO->ul_PrefabKey = _pst_SrcGO->ul_PrefabKey;
		pst_NewGO->ul_PrefabObjKey = _pst_SrcGO->ul_PrefabObjKey;
#endif

	/* Sets Ray-Insensitive Flag, if the Source GameObject has it. */
	if(OBJ_b_TestControlFlag(_pst_SrcGO, OBJ_C_ControlFlag_RayInsensitive))
		pst_NewGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_RayInsensitive;

	/* Sets OBJ_C_ControlFlag_EditableBV Flag, if the Source GameObject has it. */
	if(OBJ_b_TestControlFlag(_pst_SrcGO, OBJ_C_ControlFlag_EditableBV))
		pst_NewGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EditableBV;


	if(OBJ_b_TestControlFlag(_pst_SrcGO, OBJ_C_ControlFlag_EnableSnP))
		pst_NewGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;

	/* Detection List */
	if(OBJ_b_TestControlFlag(_pst_SrcGO, OBJ_C_ControlFlag_ForceDetectionList))
		pst_NewGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceDetectionList;

	/* Look At */
	if(OBJ_b_TestControlFlag(_pst_SrcGO, OBJ_C_ControlFlag_LookAt))
		pst_NewGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_LookAt;

#if defined(_XENON_RENDER)
    if (OBJ_b_TestControlFlag(_pst_SrcGO, OBJ_C_ControlFlag_ForceRTL))
        pst_NewGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceRTL;

    if (OBJ_b_TestControlFlag(_pst_SrcGO, OBJ_C_ControlFlag_ForceNoRTL))
        pst_NewGO->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceNoRTL;
#endif


	if(_pst_SrcGO->pst_Extended)
	{
		pst_NewGO->pst_Extended->uw_ExtraFlags = _pst_SrcGO->pst_Extended->uw_ExtraFlags;
//		L_memcpy(pst_NewGO->pst_Extended->auc_Sectos, _pst_SrcGO->pst_Extended->auc_Sectos, sizeof(pst_NewGO->pst_Extended->auc_Sectos));
	}

	/* World of object and pointer registration */
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~*/
		LINK_tdst_Pointer	*p2;
		/*~~~~~~~~~~~~~~~~~~~~*/

		pst_NewGO->pst_World = pst_World;

		LINK_RegisterDuplicate(_pst_SrcGO, pst_NewGO, _ul_PrefabKey );
		if(asz_NewName)
		{
			pst_NewGO->sz_Name = (char* )MEM_p_Alloc(L_strlen(asz_NewName) + 1);
			L_strcpy(pst_NewGO->sz_Name, asz_NewName);
		}
		else
		{	
		p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer(pst_NewGO);
		if(p2)
		{
			pst_NewGO->sz_Name = (char*)MEM_p_Alloc(L_strlen(p2->asz_Name) + 1);
			L_strcpy(pst_NewGO->sz_Name, p2->asz_Name);
		}
	}
	}


#endif
	/* Position/Matrix of object, initial Position/Matrix and Flash Position/Matrix */
	MATH_CopyMatrix(pst_NewGO->pst_GlobalMatrix, _pst_SrcGO->pst_GlobalMatrix);
	if(_pst_SrcPos) MATH_CopyVector(&pst_NewGO->pst_GlobalMatrix->T, _pst_SrcPos);

	if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_HasInitialPos))
	{
		MATH_CopyMatrix(pst_NewGO->pst_GlobalMatrix + 1, _pst_SrcGO->pst_GlobalMatrix + 1);
		if(_pst_SrcPos) MATH_CopyVector(&(pst_NewGO->pst_GlobalMatrix + 1)->T, _pst_SrcPos);
		if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_FlashMatrix))
		{
			MATH_CopyMatrix(pst_NewGO->pst_GlobalMatrix + 2, _pst_SrcGO->pst_GlobalMatrix);
			if(_pst_SrcPos) MATH_CopyVector(&(pst_NewGO->pst_GlobalMatrix + 2)->T, _pst_SrcPos);
		}
	}
	else
	{
		if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_FlashMatrix))
		{
			MATH_CopyMatrix(pst_NewGO->pst_GlobalMatrix + 1, _pst_SrcGO->pst_GlobalMatrix);
			if(_pst_SrcPos) MATH_CopyVector(&(pst_NewGO->pst_GlobalMatrix + 1)->T, _pst_SrcPos);
		}
	}

	if(pst_NewGO->pst_Extended)
	{
		pst_NewGO->pst_Extended->uw_CapacitiesInit = _pst_SrcGO->pst_Extended->uw_CapacitiesInit;
		pst_NewGO->pst_Extended->uc_AiPrio = _pst_SrcGO->pst_Extended->uc_AiPrio;
		MDF_Modifier_DuplicateList(pst_NewGO, _pst_SrcGO->pst_Extended->pst_Modifiers);
	}

	/* Visu */
	if(OBJ_b_TestIdentityFlag(_pst_SrcGO, OBJ_C_IdentityFlag_Visu))
	{
		L_memcpy(pst_NewGO->pst_Base->pst_Visu, _pst_SrcGO->pst_Base->pst_Visu, sizeof(GRO_tdst_Visu));
        if (_pst_SrcGO->pst_Base->pst_Visu->dul_VertexColors)
        {
            LONG l_Size;

            l_Size = (*_pst_SrcGO->pst_Base->pst_Visu->dul_VertexColors + 1) * 4;
            pst_NewGO->pst_Base->pst_Visu->dul_VertexColors = (ULONG *) MEM_p_Alloc( l_Size );
            L_memcpy( pst_NewGO->pst_Base->pst_Visu->dul_VertexColors, _pst_SrcGO->pst_Base->pst_Visu->dul_VertexColors, l_Size );
        }
        else
		    pst_NewGO->pst_Base->pst_Visu->dul_VertexColors = NULL;
#if defined(_XENON_RENDER)
        OBJ_UpdateXenonVisu(pst_NewGO->pst_Base->pst_Visu, _pst_SrcGO->pst_Base->pst_Visu, FALSE);
#endif
	}

	/* Groups */
	if(OBJ_b_TestIdentityFlag(_pst_SrcGO, OBJ_C_IdentityFlag_Group))
		pst_NewGO->pst_Extended->pst_Group = _pst_SrcGO->pst_Extended->pst_Group;

	/* Hierarchy */
	if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_Hierarchy))
	{
		L_memcpy(pst_NewGO->pst_Base->pst_Hierarchy, _pst_SrcGO->pst_Base->pst_Hierarchy, sizeof(OBJ_tdst_Hierarchy));
		L_memcpy
		(
			&pst_NewGO->pst_Base->pst_Hierarchy->st_LocalMatrix,
			&_pst_SrcGO->pst_Base->pst_Hierarchy->st_LocalMatrix,
			sizeof(MATH_tdst_Matrix)
		);
	}

	/* Additional matrix */
	if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_AdditionalMatrix))
	{
		if(_pst_SrcGO->pst_Base->pst_AddMatrix->l_Number)
		{
			pst_NewGO->pst_Base->pst_AddMatrix->l_Number = _pst_SrcGO->pst_Base->pst_AddMatrix->l_Number;
			if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_AddMatArePointer))
			{
				pst_NewGO->pst_Base->pst_AddMatrix->dst_GizmoPtr = (OBJ_tdst_GizmoPtr *) MEM_p_Alloc(sizeof(OBJ_tdst_GizmoPtr) * pst_NewGO->pst_Base->pst_AddMatrix->l_Number);
				L_memcpy
				(
					pst_NewGO->pst_Base->pst_AddMatrix->dst_GizmoPtr,
					_pst_SrcGO->pst_Base->pst_AddMatrix->dst_GizmoPtr,
					_pst_SrcGO->pst_Base->pst_AddMatrix->l_Number * sizeof(OBJ_tdst_GizmoPtr)
				);
			}
			else
			{
#if defined(_XBOX) || defined(_XENON)
				pst_NewGO->pst_Base->pst_AddMatrix->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_AllocAlign(sizeof(OBJ_tdst_Gizmo) * pst_NewGO->pst_Base->pst_AddMatrix->l_Number,16);
#else
				pst_NewGO->pst_Base->pst_AddMatrix->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_Alloc(sizeof(OBJ_tdst_Gizmo) * pst_NewGO->pst_Base->pst_AddMatrix->l_Number);
#endif
				L_memcpy
				(
					pst_NewGO->pst_Base->pst_AddMatrix->dst_Gizmo,
					_pst_SrcGO->pst_Base->pst_AddMatrix->dst_Gizmo,
					_pst_SrcGO->pst_Base->pst_AddMatrix->l_Number * sizeof(OBJ_tdst_Gizmo)
				);
			}
		}
	}

	/* Anims */
	if(OBJ_b_TestIdentityFlag(_pst_SrcGO, OBJ_C_IdentityFlag_Anims))
		ANI_pst_DuplicateGameObjectAnim(pst_World, pst_NewGO, _pst_SrcGO);

	/* AI */
	if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_AI))
		AI_DuplicateInstance(_pst_SrcGO, pst_NewGO, _b_CreateFile);

	/* Events */
	if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_Events))
		EVE_Duplicate(_pst_SrcGO, pst_NewGO, _b_CreateFile);

	/* Sound */
	if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_Sound))
		SND_Duplicate(_pst_SrcGO, pst_NewGO, _b_CreateFile);

	/* Light */
	if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_Lights))
		pst_NewGO->pst_Extended->pst_Light = _pst_SrcGO->pst_Extended->pst_Light;

	/* ColSet */
	if(OBJ_b_TestIdentityFlag(pst_NewGO, (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE)))
		COL_DuplicateInstance(pst_NewGO, _pst_SrcGO, _b_CreateFile);

	/* ColMap */
	if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_ColMap))
		COL_DuplicateColMap(pst_NewGO, _pst_SrcGO, _b_CreateFile);

	/* Detection List */
	if((pst_NewGO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceDetectionList) && !(pst_NewGO->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Detection))
	{
		COL_AllocDetectionList(pst_NewGO);
		OBJ_SetStatusFlag(pst_NewGO, OBJ_C_StatusFlag_Detection);
	}

	/* Design struct */
	if(OBJ_b_TestIdentityFlag(pst_NewGO, OBJ_C_IdentityFlag_DesignStruct))
	{
		pst_NewGO->pst_Extended->pst_Design = (OBJ_tdst_DesignStruct *) MEM_p_Alloc(sizeof(OBJ_tdst_DesignStruct));
		L_memcpy
		(
			pst_NewGO->pst_Extended->pst_Design,
			_pst_SrcGO->pst_Extended->pst_Design,
			sizeof(OBJ_tdst_DesignStruct)
		);
	}

	/* Bounding Volume */
	OBJ_DuplicateBV(_pst_SrcGO, pst_NewGO);

	/* Add GameObject in the World's Tables if needed */
	if(_b_AddinTable) WOR_World_AttachGameObject(pst_World, pst_NewGO);

	/* Force the activation to be redone next frame */
	if(pst_World)
	{
		pst_World->b_ForceActivationRefresh = TRUE;
		pst_World->b_ForceVisibilityRefresh = TRUE;
		pst_World->b_ForceBVRefresh = TRUE;
	}

#ifdef ACTIVE_EDITORS
	if(_b_CreateFile)
	{
		/*~~~~~~~~~~~~~~~*/
		BIG_INDEX	ul_Fat;
		/*~~~~~~~~~~~~~~~*/

		/* Create new object file and register the link address/Fat */
		pst_NewGO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Generated;
		ul_Fat = OBJ_ul_GameObject_Save(pst_World, pst_NewGO, NULL);
		LOA_AddAddress(ul_Fat, pst_NewGO);
		pst_NewGO->ul_MyKey = BIG_FileKey(ul_Fat);
	}

#endif
#if defined(_XENON_RENDER)
    OBJ_UpdateStatusFlagRTL(pst_NewGO);
#endif

	return pst_NewGO;
}
#ifdef JADEFUSION
void OBJ_GameObject_FreeXenonVisu(GRO_tdst_Visu* _pst_Visu)
{
#if defined(_XENON_RENDER)
    // Free the Xenon meshes
    if (_pst_Visu->p_XeElements != NULL)
    {
        for (int iXeMesh = 0; iXeMesh < _pst_Visu->l_NbXeElements; ++iXeMesh)
        {
            if ( _pst_Visu->p_XeElements[iXeMesh].pst_Mesh &&
                !_pst_Visu->p_XeElements[iXeMesh].pst_Mesh->IsDynamic())
            {
                THREAD_SAFE_MESH_DELETE(_pst_Visu->p_XeElements[iXeMesh].pst_Mesh);
            }

            if (_pst_Visu->p_XeElements[iXeMesh].dst_TangentSpace)
            {
                MEM_Free(_pst_Visu->p_XeElements[iXeMesh].dst_TangentSpace);
                _pst_Visu->p_XeElements[iXeMesh].dst_TangentSpace = NULL;
            }

#if defined(ACTIVE_EDITORS)
            if (_pst_Visu->p_XeElements[iXeMesh].puw_Indices)
            {
                MEM_Free(_pst_Visu->p_XeElements[iXeMesh].puw_Indices);
                _pst_Visu->p_XeElements[iXeMesh].puw_Indices = NULL;
            }
#endif

            THREAD_SAFE_IB_RELEASE(_pst_Visu->p_XeElements[iXeMesh].pst_IndexBuffer);
        }
        MEM_Free(_pst_Visu->p_XeElements);
        _pst_Visu->p_XeElements = NULL;
    }

    // Free the fur offsets
    if (_pst_Visu->p_FurOffsetVertex)
    {
        MEM_Free(_pst_Visu->p_FurOffsetVertex);
        _pst_Visu->p_FurOffsetVertex = NULL;
    }

    THREAD_SAFE_VB_RELEASE(_pst_Visu->p_VBVertex);
    THREAD_SAFE_VB_RELEASE(_pst_Visu->p_VBFurOffsets);
    THREAD_SAFE_VB_RELEASE(_pst_Visu->pVB_MorphStatic);
    THREAD_SAFE_VB_RELEASE(_pst_Visu->pVB_MorphDynamic);
#endif
}
#endif
/*
 =======================================================================================================================
    Aim:    Removes a Game object but keep its highlest level structure ...
 =======================================================================================================================
 */
void OBJ_GameObject_RemoveButEnFaitNon(OBJ_tdst_GameObject *pst_Object, char _c_DecGroRef )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct			*pst_Gro;
	MDF_tdst_Modifier		*pst_NextMdf, *pst_Modifier;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	{
		WOR_tdst_World	*pst_World;
		pst_World = WOR_World_GetWorldOfObject(pst_Object);
		if(pst_World)
		{
			if(pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas)
				if(pst_Object == ((GDI_tdst_DisplayData *) pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject) 
					((GDI_tdst_DisplayData *) pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject = NULL;
			if(pst_World->pst_View[1].st_DisplayInfo.pst_DisplayDatas)
				if(pst_Object == ((GDI_tdst_DisplayData *) pst_World->pst_View[1].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject) 
					((GDI_tdst_DisplayData *) pst_World->pst_View[1].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject = NULL;
			if(pst_World->pst_View[2].st_DisplayInfo.pst_DisplayDatas)
				if(pst_Object == ((GDI_tdst_DisplayData *) pst_World->pst_View[2].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject) 
					((GDI_tdst_DisplayData *) pst_World->pst_View[2].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject = NULL;
			if(pst_World->pst_View[3].st_DisplayInfo.pst_DisplayDatas)
				if(pst_Object == ((GDI_tdst_DisplayData *) pst_World->pst_View[3].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject) 
					((GDI_tdst_DisplayData *) pst_World->pst_View[3].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject = NULL;
		}
	}
#endif

	if(!OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_SharedMatrix))
	{
        MATH_Matrix_Free( pst_Object->pst_GlobalMatrix );
	}

#ifdef ACTIVE_EDITORS
	// Skeleton Display Case.
	if(pst_Object->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
		pst_Object->pst_Base->pst_Visu->pst_Object = NULL;
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    It's an object with base data: free base data
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_BaseObject))
	{
		/* Object with visu */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Visu))
		{
			pst_Gro = pst_Object->pst_Base->pst_Visu->pst_Object;
			if (pst_Gro)
            {
                if (_c_DecGroRef) pst_Gro->i->pfn_AddRef(pst_Gro, -1);
                pst_Gro->i->pfn_Destroy( pst_Gro );
            }
			
            pst_Gro = pst_Object->pst_Base->pst_Visu->pst_Material;
			if(pst_Gro)
            {
                if(_c_DecGroRef) pst_Gro->i->pfn_AddRef(pst_Gro, -1);
                pst_Gro->i->pfn_Destroy( pst_Gro );
            }
#ifdef JADEFUSION			
            OBJ_GameObject_FreeXenonVisu(pst_Object->pst_Base->pst_Visu);
#endif
            OBJ_VertexColor_Free( pst_Object );
            /*
            if(pst_Object->pst_Base->pst_Visu->dul_VertexColors)
            {
				MEM_Free(pst_Object->pst_Base->pst_Visu->dul_VertexColors);
				LOA_DeleteAddress( pst_Object->pst_Base->pst_Visu->dul_VertexColors );
			}
			pst_Object->pst_Base->pst_Visu->dul_VertexColors = NULL;
			*/
			MEM_Free(pst_Object->pst_Base->pst_Visu);
		}

		/* Object with anims */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Anims))
		{
			ANI_Free(pst_Object->pst_Base->pst_GameObjectAnim);
		}

		/* It's an object with Dynamics, free the dynamics structure */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Dyna))
			DYN_Dyna_Close(pst_Object->pst_Base->pst_Dyna);


#ifdef ODE_INSIDE
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_ODE))
		{
			if((pst_Object->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_MATHCOLMAP) && pst_Object->pst_Base->pst_ODE->ode_id_geom)
				dGeomDestroy(pst_Object->pst_Base->pst_ODE->ode_id_geom);
			if((pst_Object->pst_Base->pst_ODE->ode_id_body) && pst_Object->pst_Base->pst_ODE->ode_id_body)
				dBodyDestroy (pst_Object->pst_Base->pst_ODE->ode_id_body);

			MEM_Free(pst_Object->pst_Base->pst_ODE);
		}
#endif


		/* Object with hierarchy */
		if(!OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_SharedMatrix))
		{
			if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Hierarchy))
#if defined(_XBOX) || defined(_XENON)
				MEM_FreeAlign(pst_Object->pst_Base->pst_Hierarchy);
#else
				MEM_Free(pst_Object->pst_Base->pst_Hierarchy);
#endif
		}

		/* Object with additional matrix */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_AdditionalMatrix))
		{
			if(pst_Object->pst_Base->pst_AddMatrix->l_Number)
			{
#if defined(_XBOX) || defined(_XENON)
				MEM_FreeAlign(pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo);
#else
				MEM_Free(pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo);
#endif
#ifdef ACTIVE_EDITORS
				if(pst_Object->pst_Base->pst_AddMatrix->dpst_EditionGO)
					MEM_Free(pst_Object->pst_Base->pst_AddMatrix->dpst_EditionGO);
#endif
			}

			MEM_Free(pst_Object->pst_Base->pst_AddMatrix);
		}

		/* Free the base structure */
		MEM_Free(pst_Object->pst_Base);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    It's an object with extended data: free extended data
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_ExtendedObject))
	{
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Group)) GRP_ObjDetachGroup(pst_Object);

		/* Object with AI */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_AI))
			AI_FreeInstance((AI_tdst_Instance *) pst_Object->pst_Extended->pst_Ai);

		/* Events */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Events)) EVE_FreeMainStruct(pst_Object);

		/* Object with links */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Links))
		{
#ifdef ACTIVE_EDITORS
			WAY_UnRegisterAllLinks(pst_Object);
#endif
			WAY_FreeStruct((WAY_tdst_Struct *) pst_Object->pst_Extended->pst_Links);
		}

		/* Object with light */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Lights))
		{
			pst_Gro = pst_Object->pst_Extended->pst_Light;
			if(pst_Gro) 
            {
                pst_Gro->i->pfn_AddRef(pst_Gro, -1);
                pst_Gro->i->pfn_Destroy( pst_Gro );
            }
		}

		/* Design struct */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_DesignStruct))
		{
			MEM_Free(pst_Object->pst_Extended->pst_Design);
			pst_Object->pst_Extended->pst_Design = NULL;
		}

		/* Object with an msg struct */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Msg))
		{
			AI_EvalFunc_MSGClear_C(pst_Object);
			MEM_Free(pst_Object->pst_Extended->pst_Msg);
			pst_Object->pst_Extended->pst_Msg = NULL;
		}

		/* Free the Col structure */
		COL_Free(pst_Object);

		/* Modifiers */
		pst_Modifier = pst_Object->pst_Extended->pst_Modifiers;
		while(pst_Modifier)
		{
			pst_NextMdf = pst_Modifier->pst_Next;
			MDF_Modifier_Destroy(pst_Modifier);
			pst_Modifier = pst_NextMdf;
		}

		/* Sound */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Sound)) SND_FreeMainStruct(pst_Object);


        MEM_Free(pst_Object->pst_Extended);
	}

	/* Free : Bounding Volume. No Test: All the objects have a bounding volume. */
	MEM_Free(pst_Object->pst_BV);

#ifdef _DEBUG
	if(pst_Object->sz_Name) MEM_Free(pst_Object->sz_Name);
#endif
#ifdef ACTIVE_EDITORS
//	LINK_DelRegisterPointer(pst_Object);
	if(pst_Object->pst_Trace)
	{
		MEM_Free(pst_Object->pst_Trace->dst_Pos);
		MEM_Free(pst_Object->pst_Trace);
	}

#endif
	/* Delete pointer from loading tables */
//	LOA_DeleteAddress(pst_Object);

	/* Finally, we free the object */
//	MEM_Free(pst_Object);
	
	#ifdef _GAMECUBE
//	MEM_Defrag(0);	
	#endif
}


/*
 =======================================================================================================================
    Aim:    Removes a Game object
 =======================================================================================================================
 */
void OBJ_GameObject_Remove(OBJ_tdst_GameObject *pst_Object, char _c_DecGroRef )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct			*pst_Gro;
	MDF_tdst_Modifier		*pst_NextMdf, *pst_Modifier;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	{
		WOR_tdst_World	*pst_World;
		pst_World = WOR_World_GetWorldOfObject(pst_Object);
		if(pst_World)
		{
			if(pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas)
				if(pst_Object == ((GDI_tdst_DisplayData *) pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject) 
					((GDI_tdst_DisplayData *) pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject = NULL;
			if(pst_World->pst_View[1].st_DisplayInfo.pst_DisplayDatas)
				if(pst_Object == ((GDI_tdst_DisplayData *) pst_World->pst_View[1].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject) 
					((GDI_tdst_DisplayData *) pst_World->pst_View[1].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject = NULL;
			if(pst_World->pst_View[2].st_DisplayInfo.pst_DisplayDatas)
				if(pst_Object == ((GDI_tdst_DisplayData *) pst_World->pst_View[2].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject) 
					((GDI_tdst_DisplayData *) pst_World->pst_View[2].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject = NULL;
			if(pst_World->pst_View[3].st_DisplayInfo.pst_DisplayDatas)
				if(pst_Object == ((GDI_tdst_DisplayData *) pst_World->pst_View[3].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject) 
					((GDI_tdst_DisplayData *) pst_World->pst_View[3].st_DisplayInfo.pst_DisplayDatas)->pst_EditorCamObject = NULL;
		}
	}
#endif

	if(!OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_SharedMatrix))
	{
        MATH_Matrix_Free( pst_Object->pst_GlobalMatrix );
		pst_Object->pst_GlobalMatrix = NULL;
	}

#ifdef ACTIVE_EDITORS
	// Skeleton Display Case.
	if(pst_Object->ul_EditorFlags & OBJ_C_EditFlags_GizmoObject)
		pst_Object->pst_Base->pst_Visu->pst_Object = NULL;
#endif

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    It's an object with base data: free base data
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_BaseObject))
	{
		/* Object with visu */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Visu))
		{
			pst_Gro = pst_Object->pst_Base->pst_Visu->pst_Object;
			if (pst_Gro)
            {
                if (_c_DecGroRef) pst_Gro->i->pfn_AddRef(pst_Gro, -1);
                pst_Gro->i->pfn_Destroy( pst_Gro );
            }
			
            pst_Gro = pst_Object->pst_Base->pst_Visu->pst_Material;
			if(pst_Gro)
            {
                if(_c_DecGroRef) pst_Gro->i->pfn_AddRef(pst_Gro, -1);
                pst_Gro->i->pfn_Destroy( pst_Gro );
            }
			
            OBJ_VertexColor_Free( pst_Object );

#ifdef JADEFUSION
            MEM_SafeFree(pst_Object->pst_Base->pst_Visu->p_us_NbTrianglesInElement);
            if (pst_Object->pst_Base->pst_Visu->pp_st_LightmapCoords)
            {
                for (USHORT j = 0; j < pst_Object->pst_Base->pst_Visu->usNbElements; j++)
                {
                    MEM_SafeFree(pst_Object->pst_Base->pst_Visu->pp_st_LightmapCoords[j]);
                }


                MEM_SafeFree(pst_Object->pst_Base->pst_Visu->pp_st_LightmapCoords);

                LIGHT_Lightmaps_RemoveRefLightmapPage(pst_Object->pst_Base->pst_Visu->pLMPage);
                pst_Object->pst_Base->pst_Visu->pLMPage = NULL;				
            }

            OBJ_GameObject_FreeXenonVisu(pst_Object->pst_Base->pst_Visu);
#endif
            /*
            if(pst_Object->pst_Base->pst_Visu->dul_VertexColors)
            {
				MEM_Free(pst_Object->pst_Base->pst_Visu->dul_VertexColors);
				LOA_DeleteAddress( pst_Object->pst_Base->pst_Visu->dul_VertexColors );
			}
			pst_Object->pst_Base->pst_Visu->dul_VertexColors = NULL;
			*/
			MEM_Free(pst_Object->pst_Base->pst_Visu);
		}

		/* Object with anims */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Anims)
			&& pst_Object->pst_Base
			&& pst_Object->pst_Base->pst_GameObjectAnim)
		{
			ANI_Free(pst_Object->pst_Base->pst_GameObjectAnim);
		}

		/* It's an object with Dynamics, free the dynamics structure */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Dyna))
			DYN_Dyna_Close(pst_Object->pst_Base->pst_Dyna);


#ifdef ODE_INSIDE
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_ODE))
		{
			if((pst_Object->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_MATHCOLMAP) && pst_Object->pst_Base->pst_ODE->ode_id_geom)
				dGeomDestroy(pst_Object->pst_Base->pst_ODE->ode_id_geom);
			if((pst_Object->pst_Base->pst_ODE->ode_id_body) && pst_Object->pst_Base->pst_ODE->ode_id_body)
				dBodyDestroy (pst_Object->pst_Base->pst_ODE->ode_id_body);

			MEM_Free(pst_Object->pst_Base->pst_ODE);
		}
#endif


		/* Object with hierarchy */
		if(!OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_SharedMatrix))
		{
			if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Hierarchy))
#if defined(_XBOX) || defined(_XENON)
				MEM_FreeAlign(pst_Object->pst_Base->pst_Hierarchy);
#else
				MEM_Free(pst_Object->pst_Base->pst_Hierarchy);
				
#endif
		}

		/* Object with additional matrix */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_AdditionalMatrix))
		{
			if(pst_Object->pst_Base->pst_AddMatrix->l_Number)
			{
#if defined(_XBOX) || defined(_XENON)
				MEM_FreeAlign(pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo);
#else
				MEM_Free(pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo);
#endif
#ifdef ACTIVE_EDITORS
				if(pst_Object->pst_Base->pst_AddMatrix->dpst_EditionGO)
					MEM_Free(pst_Object->pst_Base->pst_AddMatrix->dpst_EditionGO);
#endif
			}

			MEM_Free(pst_Object->pst_Base->pst_AddMatrix);
		}

		/* Free the base structure */
		MEM_Free(pst_Object->pst_Base);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    It's an object with extended data: free extended data
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_ExtendedObject))
	{
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Group)) GRP_ObjDetachGroup(pst_Object);

		/* Object with AI */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_AI))
			AI_FreeInstance((AI_tdst_Instance *) pst_Object->pst_Extended->pst_Ai);

		/* Events */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Events)) EVE_FreeMainStruct(pst_Object);

		/* Object with links */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Links))
		{
#ifdef ACTIVE_EDITORS
			WAY_UnRegisterAllLinks(pst_Object);
#endif
			WAY_FreeStruct((WAY_tdst_Struct *) pst_Object->pst_Extended->pst_Links);
		}

		/* Object with light */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Lights))
		{
			pst_Gro = pst_Object->pst_Extended->pst_Light;
			if(pst_Gro) 
            {
                pst_Gro->i->pfn_AddRef(pst_Gro, -1);
                pst_Gro->i->pfn_Destroy( pst_Gro );
            }
		}

		/* Design struct */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_DesignStruct))
		{
			MEM_Free(pst_Object->pst_Extended->pst_Design);
			pst_Object->pst_Extended->pst_Design = NULL;
		}

		/* Object with an msg struct */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Msg))
		{
			AI_EvalFunc_MSGClear_C(pst_Object);
			MEM_Free(pst_Object->pst_Extended->pst_Msg);
			pst_Object->pst_Extended->pst_Msg = NULL;
		}

		/* Free the Col structure */
		COL_Free(pst_Object);

		/* Modifiers */
		pst_Modifier = pst_Object->pst_Extended->pst_Modifiers;
		while(pst_Modifier)
		{
			pst_NextMdf = pst_Modifier->pst_Next;
			MDF_Modifier_Destroy(pst_Modifier);
			pst_Modifier = pst_NextMdf;
		}

		/* Sound */
		if(OBJ_b_TestIdentityFlag(pst_Object, OBJ_C_IdentityFlag_Sound)) SND_FreeMainStruct(pst_Object);

#ifdef JADEFUSION
        // delete the wind (either static or dynamic) allocated at load time
        if(pst_Object->pst_Extended->po_CurrentWind)
        {
            delete pst_Object->pst_Extended->po_CurrentWind;
        }
#endif
        MEM_Free(pst_Object->pst_Extended);
	}

	/* Free : Bounding Volume. No Test: All the objects have a bounding volume. */
	MEM_Free(pst_Object->pst_BV);

#ifdef Active_CloneListe
	//-- Clone --
	if ( pst_Object->CloneLightList )
	OBJ_GameObject_RemoveAllLightClone( pst_Object );
	pst_Object->p_CloneNextGao = NULL;
	pst_Object->CloneLightList = NULL;
	i_CloneIdenticNumber=0;
	//-----------
#endif

#ifdef _DEBUG
	if(pst_Object->sz_Name) MEM_Free(pst_Object->sz_Name);
#endif
#ifdef ACTIVE_EDITORS
	LINK_DelRegisterPointer(pst_Object);
	if(pst_Object->pst_Trace)
	{
		MEM_Free(pst_Object->pst_Trace->dst_Pos);
		MEM_Free(pst_Object->pst_Trace);
	}

#endif
	/* Delete pointer from loading tables */
	LOA_DeleteAddress(pst_Object);

	/* Finally, we free the object */
	MEM_Free(pst_Object);
	
	#ifdef _GAMECUBE
//	MEM_Defrag(0);	
	#endif
}



#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_GameObject_BuildName(char *_sz_Name, char *_sz_Out)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	LONG	l_Length, l_Ext;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(_sz_Name == NULL) return;

	l_Length = L_strlen(_sz_Name);
	l_Ext = (*(_sz_Name + l_Length - 4) != '.') ? 4 : 0;

	L_strcpy(_sz_Out, _sz_Name);

	l_Length = L_strlen(_sz_Name);
	if((*(_sz_Name + l_Length - 4) != '.')) L_strcat(_sz_Out, EDI_Csz_ExtGameObject);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_GameObject_SetName(OBJ_tdst_GameObject *_pst_Object, char *_sz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	LONG	l_Length, l_Ext;
	/*~~~~~~~~~~~~~~~~~~~~*/

	if(_sz_Name == NULL)
		_pst_Object->sz_Name = NULL;
	else
	{
		l_Length = L_strlen(_sz_Name);
		l_Ext = (*(_sz_Name + l_Length - 4) != '.') ? 4 : 0;

		_pst_Object->sz_Name = L_strcpy((char*)MEM_p_Alloc(L_strlen(_sz_Name) + l_Ext + 1), _sz_Name);
		if(l_Ext) L_strcat(_pst_Object->sz_Name, EDI_Csz_ExtGameObject);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_GameObject_RegisterWithName(OBJ_tdst_GameObject *_pst_GO, BIG_INDEX _ul_File, char *_psz_Name, LONG _l_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		asz_Tmp[255];
	char		asz_Path[BIG_C_MaxLenPath];
	static int	i_NameNumber = 0;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Name of file */
	if(_psz_Name == NULL)
	{
		sprintf(asz_Tmp, "Unnamed%i", i_NameNumber);
		L_strcat(asz_Tmp, EDI_Csz_ExtGameObject);
		i_NameNumber++;
	}
	else
		strcpy(asz_Tmp, _psz_Name);

	/* Path if not yet file. Register in the default path */
	if(_ul_File == BIG_C_InvalidIndex) WOR_GetGaoPath(WOR_gpst_WorldToLoadIn, asz_Path);

	/* File already exists. Get the path of it */
	else
	{
		BIG_ComputeFullName(BIG_ParentFile(_ul_File), asz_Path);
	}

	LINK_RegisterPointer(_pst_GO, LINK_C_ENG_GameObjectOriented, asz_Tmp, asz_Path);

	if(_l_Flags & OBJ_GameObject_RegSetName) OBJ_GameObject_SetName(_pst_GO, asz_Tmp);
}

#endif

//#endif // !defined XML_CONV_TOOL
/*
 =======================================================================================================================
    Aim:    Create an extended structure, without changing the identity flags
 =======================================================================================================================
 */
void OBJ_GameObject_CreateExtended(OBJ_tdst_GameObject *_pst_GO)
{
	_pst_GO->pst_Extended = (OBJ_tdst_Extended *) MEM_p_Alloc(sizeof(OBJ_tdst_Extended));
	L_memset(_pst_GO->pst_Extended, 0, sizeof(OBJ_tdst_Extended));
	_pst_GO->pst_Extended->uc_AiPrio = 127;
	_pst_GO->pst_Extended->auc_Sectos[0] = 0;
	_pst_GO->pst_Extended->auc_Sectos[1] = 0;
	_pst_GO->pst_Extended->auc_Sectos[2] = 0;
	_pst_GO->pst_Extended->auc_Sectos[3] = 0;
}

//#if !defined(XML_CONV_TOOL) est plus
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_GameObject_CreateExtendedIfNot(OBJ_tdst_GameObject *_pst_GO)
{
	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ExtendedObject))
	{
		OBJ_GameObject_CreateExtended(_pst_GO);
		OBJ_SetIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ExtendedObject);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_GameObject_AllocateGizmo(OBJ_tdst_GameObject *_pst_GO, ULONG ul_NbOfGizmos, BOOL b_PointerToMatrix)
{
	/*~~~~~~~~~~~~~*/
	LONG	l_Size;
	ULONG	ul_Flags;
	/*~~~~~~~~~~~~~*/

	ul_Flags = b_PointerToMatrix ?
		(OBJ_ul_FlagsIdentityGet(_pst_GO) | OBJ_C_IdentityFlag_AdditionalMatrix | OBJ_C_IdentityFlag_AddMatArePointer) :
			(OBJ_ul_FlagsIdentityGet(_pst_GO) | OBJ_C_IdentityFlag_AdditionalMatrix);

	/* If the GameObject has not the Flag, sets it. */
	if
	(
		(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_AdditionalMatrix))
	&&	(_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo)
	)
	{
#if defined(_XBOX) || defined(_XENON)
		MEM_FreeAlign(_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo);
#else
		MEM_Free(_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo);
#endif
		MEM_Free(_pst_GO->pst_Base->pst_AddMatrix);
		_pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_AdditionalMatrix;
		_pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_AddMatArePointer;
	}

	OBJ_ChangeIdentityFlags(_pst_GO, ul_Flags, OBJ_C_UnvalidFlags);

	_pst_GO->pst_Base->pst_AddMatrix->l_Number = ul_NbOfGizmos;
	l_Size = ul_NbOfGizmos * ((b_PointerToMatrix) ? sizeof(OBJ_tdst_GizmoPtr) : sizeof(OBJ_tdst_Gizmo));
#if defined(_XBOX) || defined(_XENON)
	_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_AllocAlign(l_Size,16);
#else
	_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_VMAlloc(l_Size);
#endif
	L_memset(_pst_GO->pst_Base->pst_AddMatrix->dst_Gizmo, 0, l_Size);
}

//#endif // !defined(XML_CONV_TOOL)
#ifdef ACTIVE_EDITORS

void OBJ_GameObject_Rename(OBJ_tdst_GameObject *pst_GO, char *asz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX			ul_Index;
    ULONG               ul_Color, i;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Name1[BIG_C_MaxLenPath];
    char				asz_OldName[BIG_C_MaxLenPath];
	LINK_tdst_Pointer	*p2;
	bool				isNewFileName = true;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    ERR_X_Error(strlen(asz_Name) < BIG_C_MaxLenName, "File name is too long", NULL);
	ul_Index = LOA_ul_SearchKeyWithAddress((ULONG) pst_GO);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	ul_Index = BIG_ul_SearchKeyToFat(ul_Index);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

    // Backup color
    ul_Color = LINK_gul_ColorTxt;
    LINK_gul_ColorTxt = 0;

	/* Rename file */
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
#ifdef JADEFUSION
	char sz_Msg[512];
	sprintf( sz_Msg, "[%08X] Rename: %s -> %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_Name),
    LINK_PrintStatusMsg( sz_Msg );
#else
	if (BIG_ul_SearchFileExt(asz_Path, asz_Name) != BIG_C_InvalidIndex)
		isNewFileName = FALSE;

    LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Rename: %s -> %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_Name));
#endif
_Try_
	BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(ul_Index));
_Catch_
_End_

	/* Name of GO */
    L_strcpy(asz_OldName, pst_GO->sz_Name);
    *L_strrchr(asz_OldName, '.') = 0;

	MEM_Free(pst_GO->sz_Name);
	pst_GO->sz_Name = (char *) MEM_p_Alloc/*Tag*/(L_strlen(asz_Name) + 1/*, "ENG::OBJ::GAO Name2"*/);
	L_strcpy(pst_GO->sz_Name, asz_Name);
    
	/* Update GO */
    OBJ_GameObject_SetName(pst_GO, asz_Name);
    OBJ_ul_GameObject_Save(NULL, pst_GO, asz_Path);
    LINK_UpdatePointerAndName(pst_GO, asz_Name);
     

	// Rename ColMap
	if(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Col)
	{
		COL_tdst_ColMap *pst_ColMap = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap;

		if ( pst_ColMap )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap);
			ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
			L_strcpy(asz_Name1, BIG_NameFile(ul_Index));

			*L_strrchr(asz_Name, '.') = 0;
			L_strcat(asz_Name, EDI_Csz_ExtCOLMap);
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			
			
			// If GAO's name is new but corresponds to an already existing colmap (see brickmapper)
			if (isNewFileName == FALSE || BIG_ul_SearchFileExt(asz_Path, asz_Name) == BIG_C_InvalidIndex)
			{
				LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Rename: %s -> %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_Name));
			
_Try_
				BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(ul_Index));
_Catch_
_End_
			}
            
            for(i=0; i<pst_ColMap->uc_NbOfCob; i++)
            {
                ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap->dpst_Cob[i]);
			    ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
			    L_strcpy(asz_Name1, BIG_NameFile(ul_Index));
                *L_strrchr(asz_Name1, '.') = 0;

                if (!(isNewFileName == TRUE && BIG_ul_SearchFileExt(asz_Path, asz_Name) != BIG_C_InvalidIndex) || !strcmp(asz_Name1, asz_OldName))
                {
                    *L_strrchr(asz_Name, '.') = 0;
                    L_strcat(asz_Name, EDI_Csz_ExtCOLObject);
                    BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

                    LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Rename: %s -> %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_Name));
					
_Try_
						BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(ul_Index));
_Catch_
_End_  
                }
                else
                {
					LINK_PrintStatusMsg( ERR_szFormatMessage("[%08X] This file was not renamed because it does not have the same name as its associated gao.  You should rename this file manually if appropriate", BIG_FileKey(ul_Index)) );
                }
            }
		}
	}

	// Rename ColInstance
	if(pst_GO->ul_IdentityFlags & (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		COL_tdst_Instance *pst_ColInstance = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance;

		if ( pst_ColInstance )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColInstance);
			ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
			L_strcpy(asz_Name1, BIG_NameFile(ul_Index));

			*L_strrchr(asz_Name, '.') = 0;
			L_strcat(asz_Name, EDI_Csz_ExtCOLInstance);
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			
            LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Rename: %s -> %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_Name));
_Try_
			BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(ul_Index));
_Catch_
_End_
		}
	}

	/* Rename AI */
	if(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Ai)
	{
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO->pst_Extended->pst_Ai);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
		L_strcpy(asz_Name1, BIG_NameFile(ul_Index));

		/* AI instance */
		*L_strrchr(asz_Name, '.') = 0;
		L_strcat(asz_Name, EDI_Csz_ExtAIEngineInstance);
		BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

        LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Rename: %s -> %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_Name));
_Try_
		BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(ul_Index));
_Catch_
_End_
        
		p2 = LINK_p_SearchPointer(pst_GO->pst_Extended->pst_Ai);
		ERR_X_Assert(p2);
		L_strcpy(p2->asz_Name, asz_Name);
		LINK_UpdatePointer(pst_GO->pst_Extended->pst_Ai);

		/* AI vars */
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO->pst_Extended->pst_Ai);
		*L_strrchr(asz_Name1, '.') = 0;
		L_strcat(asz_Name1, EDI_Csz_ExtAIEngineVars);
		ul_Index = BIG_ul_SearchFileExt(asz_Path, asz_Name1);
		if(ul_Index != BIG_C_InvalidIndex)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
_Try_
			*L_strrchr(asz_Name, '.') = 0;
			L_strcat(asz_Name, EDI_Csz_ExtAIEngineVars);
			
            LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Rename: %s -> %s", BIG_FileKey(ul_Index), asz_Name1, asz_Name));
			BIG_RenFile(asz_Name, asz_Path, asz_Name1);
_Catch_
_End_
		}
	}

    // Rename Visu
    if (OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
    {
        CHAR * psz_Ext;
        GRO_tdst_Struct * pst_Struct = pst_GO->pst_Base->pst_Visu->pst_Object;

        ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Struct);
        ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
        L_strcpy(asz_Name1, GRO_sz_Struct_GetName(pst_Struct));
        psz_Ext = L_strrchr(asz_Name1, '.');
        if (psz_Ext)
            *psz_Ext = 0;

        if (!(isNewFileName == TRUE && BIG_ul_SearchFileExt(asz_Path, asz_Name) != BIG_C_InvalidIndex) || !strcmp(asz_Name1, asz_OldName))
        {
            *L_strrchr(asz_Name, '.') = 0;
            L_strcat(asz_Name, pst_Struct->i->pfnsz_FileExtension());
            BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

            LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Rename: %s -> %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_Name));
_Try_
            BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(ul_Index));
_Catch_
_End_
            GRO_Struct_SetName(pst_Struct, asz_Name);
        }
        else
        {
			LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] This file was not renamed because it does not have the same name as its associated gao.  You should rename this file manually if appropriate", BIG_FileKey(ul_Index)));
        }
    }

	// Rename Rli

	/* Plante en dupliquant

	if (pst_GO && pst_GO->pst_Base && pst_GO->pst_Base->pst_Visu && pst_GO->pst_Base->pst_Visu->dul_VertexColors)
	{
		CHAR * psz_Ext;
	
		BIG_INDEX ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO->pst_Base->pst_Visu->dul_VertexColors );
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
		
		*L_strrchr(asz_Name, '.') = 0;
		L_strcat(asz_Name, EDI_Csz_ExtGameObjectRLI);
		BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

		LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Rename: %s -> %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_Name));
		_Try_
			BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(ul_Index));
		_Catch_
			_End_
	}
	*/

    // Rename Ligths
    if (OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Lights))
    {
        CHAR * psz_Ext;
        GRO_tdst_Struct * pst_Struct = pst_GO->pst_Extended->pst_Light;

        ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Struct);
        ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
        L_strcpy(asz_Name1, GRO_sz_Struct_GetName(pst_Struct));
        psz_Ext = L_strrchr(asz_Name1, '.');
        if (psz_Ext)
            *psz_Ext = 0;

        if (!strcmp(asz_Name1, asz_OldName))
        {
            *L_strrchr(asz_Name, '.') = 0;
            L_strcat(asz_Name, pst_Struct->i->pfnsz_FileExtension());
            BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
            LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Rename: %s -> %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_Name));
_Try_
            BIG_RenFile(asz_Name, asz_Path, BIG_NameFile(ul_Index));
_Catch_
_End_
            GRO_Struct_SetName(pst_Struct, asz_Name);
        }
        else
        {
			LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] This file was not renamed because it does not have the same name as its associated gao.  You should rename this file manually if appropriate", BIG_FileKey(ul_Index)));
        }   
    }

    LINK_gul_ColorTxt = ul_Color;
}

void OBJ_GameObject_MoveToWorld(OBJ_tdst_GameObject *pst_GO, BIG_KEY _ul_WorldKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX			ul_Index;
	ULONG               ul_Color;
	ULONG				i;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_NewPath[BIG_C_MaxLenPath];
	char				asz_Name1[BIG_C_MaxLenName];
	char				asz_Name2[BIG_C_MaxLenName];
	char				asz_Msg[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

	// Backup color
	ul_Color = LINK_gul_ColorTxt;
	LINK_gul_ColorTxt = 0;

	// Move file
	WOR_GetGaoPathWithKey(_ul_WorldKey, asz_NewPath);
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	BIG_MoveFile(asz_NewPath, asz_Path, BIG_NameFile(ul_Index));

	LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Move: %s to %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_NewPath));

	L_strcpy(asz_Name1, pst_GO->sz_Name);
	*L_strrchr(asz_Name1, '.') = 0;

	// Move ColMap
	if(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Col)
	{
		COL_tdst_ColMap *pst_ColMap = ((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_ColMap;

		if ( pst_ColMap )
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap);
			ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			WOR_GetSubPathWithKey(_ul_WorldKey, EDI_Csz_Path_COLInstances, asz_NewPath);
			BIG_MoveFile(asz_NewPath, asz_Path, BIG_NameFile(ul_Index));
			sprintf(asz_Msg, "[%08X] Move: %s to %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_NewPath);
			LINK_PrintStatusMsg(asz_Msg);

			for(i=0; i<pst_ColMap->uc_NbOfCob; i++)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_ColMap->dpst_Cob[i]);
				ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
				L_strcpy(asz_Name2, BIG_NameFile(ul_Index));
				*L_strrchr(asz_Name2, '.') = 0;

				if (!strcmp(asz_Name1, asz_Name2))
				{
					BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
					WOR_GetSubPathWithKey(_ul_WorldKey, EDI_Csz_Path_COLObjects, asz_NewPath);
					BIG_MoveFile(asz_NewPath, asz_Path, BIG_NameFile(ul_Index));

					LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Move: %s to %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_NewPath));
				}
			}
		}
	}

	// Move AI
	if(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Ai)
	{
		// Move AI instance
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO->pst_Extended->pst_Ai);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

		BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
		WOR_GetSubPathWithKey(_ul_WorldKey, EDI_Csz_Path_AIInstances, asz_NewPath);
		BIG_MoveFile(asz_NewPath, asz_Path, BIG_NameFile(ul_Index));
		LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Move: %s to %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_NewPath));

		// Move AI vars
		L_strcpy(asz_Name2, BIG_NameFile(ul_Index));
		*L_strrchr(asz_Name2, '.') = 0;
		L_strcat(asz_Name2, EDI_Csz_ExtAIEngineVars);
		ul_Index = BIG_ul_SearchFileExt(asz_Path, asz_Name2);

		if(ul_Index != BIG_C_InvalidIndex)
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			WOR_GetSubPathWithKey(_ul_WorldKey, EDI_Csz_Path_AIInstances, asz_NewPath);
			BIG_MoveFile(asz_NewPath, asz_Path, BIG_NameFile(ul_Index));
			LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Move: %s to %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_NewPath));
		}
	}

	// Move Visu
	if (OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu) && pst_GO->pst_Base->pst_Visu->pst_Object)
	{
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO->pst_Base->pst_Visu->pst_Object);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

		L_strcpy(asz_Name2, BIG_NameFile(ul_Index));
		*L_strrchr(asz_Name2, '.') = 0;

		if (!strcmp(asz_Name1, asz_Name2))
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			WOR_GetSubPathWithKey(_ul_WorldKey, EDI_Csz_Path_GraphicObject, asz_NewPath);
			BIG_MoveFile(asz_NewPath, asz_Path, BIG_NameFile(ul_Index));
			LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Move: %s to %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_NewPath));
		}
	}

	// Rename Ligths
	if (OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Lights) && pst_GO->pst_Extended->pst_Light)
	{
		ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO->pst_Extended->pst_Light);
		ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);

		L_strcpy(asz_Name2, BIG_NameFile(ul_Index));
		*L_strrchr(asz_Name2, '.') = 0;

		if (!strcmp(asz_Name1, asz_Name2))
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			WOR_GetSubPathWithKey(_ul_WorldKey, EDI_Csz_Path_GraphicObject, asz_NewPath);
			BIG_MoveFile(asz_NewPath, asz_Path, BIG_NameFile(ul_Index));
			LINK_PrintStatusMsg(ERR_szFormatMessage("[%08X] Move: %s to %s", BIG_FileKey(ul_Index), BIG_NameFile(ul_Index), asz_NewPath));
		}
	}

	LINK_gul_ColorTxt = ul_Color;
}

#endif // #ifdef ACTIVE_EDITORS


#ifdef Active_CloneListe

void OBJ_GameObject_AddLightClone(LightCloneListeGao **List,OBJ_tdst_GameObject *obj)
{
	LightCloneListeGao *ListNew;
	LightCloneListeGao *ListMove;

	ListNew = (LightCloneListeGao*)MEM_p_Alloc(sizeof(LightCloneListeGao));
	ListNew->p_Gao= obj;
	ListNew->p_Next= NULL;

	if (*List==NULL) *List = ListNew;
		//return (ListNew);
	else
	{
		for (ListMove=*List;ListMove->p_Next!=NULL;ListMove=ListMove->p_Next);
		ListMove->p_Next=ListNew;
		//return (*List);
	}
	
}
// Remove par adresse du Gao
/*void OBJ_GameObject_RemoveCloneObject(CloneListeGao **List,OBJ_tdst_GameObject *obj)
{
	CloneListeGao *ListMove;
	CloneListeGao *ListMove_prev;
	CloneListeGao *ListTemp;

	ListMove_prev = NULL;
	for(ListMove=*List;(ListMove!=NULL) && (ListMove->p_Gao!=obj);
		ListMove_prev=ListMove,ListMove=ListMove->p_Next);

	if( ListMove == NULL ) return;
	else
	{
		if (ListMove_prev == NULL)
		{
			ListTemp = ListMove->p_Next;
			MEM_Free(ListMove);
			return;
		}
		else
		{
			ListMove_prev->p_Next = ListMove->p_Next;
			MEM_Free(ListMove);
			return;
		}
	}
}
*/
void OBJ_GameObject_RemoveAllLightClone(OBJ_tdst_GameObject *obj)
{
	LightCloneListeGao *CloneLightList;
	LightCloneListeGao *CloneLightList2;

	CloneLightList = obj->CloneLightList;
	while (CloneLightList!=NULL)
	{
		CloneLightList2=CloneLightList->p_Next;
		MEM_Free(CloneLightList);
		CloneLightList = CloneLightList2;
	}
	obj->CloneLightList = NULL;
}
void StockCloneLight(OBJ_tdst_GameObject *Obj)
{
		LIGHT_tdst_List *_pst_LightList;
		register GDI_tdst_DisplayData 		*pst_DD;

		if ( Obj->CloneLightList )
			OBJ_GameObject_RemoveAllLightClone( Obj );

		pst_DD = &GDI_gpst_CurDD_SPR;
		_pst_LightList = &GDI_gpst_CurDD_SPR.st_LightList;//&pst_DD->st_LightList;


		if ( _pst_LightList->ul_Current )
		{
			OBJ_tdst_GameObject			**ppst_LightNode, **ppst_Last;
#ifdef ACTIVE_EDITORS
            int iLightNb = 0;
            int iDirectLightNb = 0;
#endif //ACTIVE_EDITORS

			
			ppst_LightNode = _pst_LightList->dpst_Light;
			ppst_Last = ppst_LightNode + _pst_LightList->ul_Current;
			while(ppst_LightNode < ppst_Last)
			{
                ULONG ulLightFlag;
				LIGHT_gpst_Cur = (LIGHT_tdst_Light *) (*ppst_LightNode)->pst_Extended->pst_Light;
                ulLightFlag = LIGHT_gpst_Cur->ul_Flags;

                // ok si lumière dyna sur objet dyna ou lumière non dyna sur objet non dyna.
                if (Obj->ul_StatusAndControlFlags & OBJ_C_StatusFlag_RTL ? 
                    ulLightFlag & LIGHT_Cul_LF_RealTimeOnDynam : 
                    ulLightFlag & LIGHT_Cul_LF_RealTimeOnNonDynam)
                {
                    BOOL bAddLight = FALSE;
                    //if ( LIGHT_gpst_Cur->ul_Flags & LIGHT_Cul_LF_CloneLight )
                    if((ulLightFlag & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Omni)
                    {
                        MATH_tdst_Vector    v2;//, *pv;
                        MATH_SubVector( &v2, &(*ppst_LightNode)->pst_GlobalMatrix->T, &Obj->pst_GlobalMatrix->T );

                        bAddLight = (INT_SphereAABBox(&v2,LIGHT_gpst_Cur->st_Omni.f_Far, OBJ_pst_BV_GetGMin( &Obj->pst_BV ), OBJ_pst_BV_GetGMax( &Obj->pst_BV ) ));
                    }			
                    else 
                        bAddLight = ((ulLightFlag & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct);


#ifdef ACTIVE_EDITORS
                    if (bAddLight)
                    {
                        // Constraints : no rli, 2 lights max, and 1 directional light max.
                        BOOL bNoMoreLight = (iLightNb >= 2) || 
                            ((iDirectLightNb >= 1) && ((ulLightFlag & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct));

                        if (bNoMoreLight)
                        {
                            char sMsg[256];
                            if (iLightNb >= 2)
                            {
                                sprintf(sMsg,"Clone %s(%08x) has more than 2 lights : %s, %s and %s",
                                    Obj->sz_Name,Obj->ul_MyKey,
                                    Obj->CloneLightList->p_Gao->sz_Name, 
                                    Obj->CloneLightList->p_Next->p_Gao->sz_Name,
                                    (*ppst_LightNode)->sz_Name);
                            }
                            else
                            {
                                sprintf(sMsg,"Clone %s(%08x) has more than 1 directionnal light : %s and %s",
                                    Obj->sz_Name,Obj->ul_MyKey,
                                    Obj->CloneLightList->p_Gao->sz_Name, 
                                    (*ppst_LightNode)->sz_Name);
                            }

                            ERR_X_Warning(0,sMsg,NULL);
                            bAddLight = FALSE;
                        }
                        else
                        {
                            iLightNb++;
                            if ((ulLightFlag & LIGHT_Cul_LF_Type) == LIGHT_Cul_LF_Direct)
                                iDirectLightNb++;
                        }
                    }
#endif //ACTIVE_EDITORS
                    if (bAddLight)
                        OBJ_GameObject_AddLightClone( &Obj->CloneLightList,*ppst_LightNode);
                }
				ppst_LightNode++;
			}
		}
}

extern ULONG MAT_GetFirstTransparentMaterialIndex(MAT_tdst_Material *pst_GRO, GEO_tdst_Object *_pst_Obj );

BOOL OBJ_b_CanBeCloned(OBJ_tdst_GameObject *_pst_GO)
{
    GRO_tdst_Visu *pst_Visu = _pst_GO->pst_Base->pst_Visu;
    ULONG ulMatIndex;  

    // No fur on clones.
    if (_pst_GO->pst_Extended)
    {
        MDF_tdst_Modifier *pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
        while (pst_Modifier)
        {
            if (pst_Modifier->i->ul_Type == MDF_C_Modifier_FUR)
            {
                ERR_X_Warning(0,"Cannot clone this GAO, it has fur",_pst_GO->sz_Name);
                return FALSE;
            }
            pst_Modifier = pst_Modifier->pst_Next;
        }
    }

    // First pass cannot be alpha on clones (else they would go to zlist)
    ulMatIndex = MAT_GetFirstTransparentMaterialIndex((MAT_tdst_Material *) pst_Visu->pst_Material,(GEO_tdst_Object *) pst_Visu->pst_Object);
    if (ulMatIndex != (ULONG)-1)
    {
        char sMsg[256];
        sprintf(sMsg,"Cannot clone this GAO, it uses material %d whose first layer is alpha",ulMatIndex);
        ERR_X_Warning(0,sMsg,_pst_GO->sz_Name);
        return FALSE;
    }
    return TRUE;
}

// Parse
void OBJ_GameObject_UpdateCloneListe()
{
	WOR_tdst_World *_pst_World;
	TAB_tdst_PFelem		*pst_Elem, *pst_LastElem;
	TAB_tdst_PFelem		*pst_ElemScan;
	OBJ_tdst_GameObject *pst_TempGO,*pst_TempGOScan,*pst_TempCurGO;
	//BOOL b_Find;
	//return;

	_pst_World = GDI_gpst_CurDD->pst_World;
	if (!_pst_World) return;

	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
	pst_LastElem = TAB_pst_PFtable_GetLastElem(&_pst_World->st_VisibleObjects);
	if ((pst_LastElem - pst_Elem)== i_CloneIdenticNumber) return;
		
	i_CloneIdenticNumber=pst_LastElem - pst_Elem;

	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
		pst_TempGO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_TempGO)) continue;
		pst_TempGO->p_CloneNextGao=NULL;
	}


	pst_Elem = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);
	for(; pst_Elem <= pst_LastElem; pst_Elem++)
	{
		pst_TempGO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(pst_TempGO)) continue;
	
		//1) recherche un premier objet flagé pour creer la chaine
		if ( pst_TempGO->p_CloneNextGao )  continue;
		
		// No flag -> continue
		if(!OBJ_b_TestIdentityFlag(pst_TempGO, OBJ_C_IdentityFlag_Visu)) continue;
		if ( (pst_TempGO->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_NoAutoClone) ) continue;
		if (!pst_TempGO->pst_Base->pst_Visu->pst_Object) continue;
		if ((pst_TempGO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_Geometric) ||
			(pst_TempGO->pst_Base->pst_Visu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)) 
		{
			
            // On applique les contraintes sur les clones : pas de fur ni de première couche en alpha.
            if (!OBJ_b_CanBeCloned(pst_TempGO))
                continue;

			//recherche les copy de geo ---->
			pst_ElemScan = TAB_pst_PFtable_GetFirstElem(&_pst_World->st_VisibleObjects);

			//b_Find = FALSE;
			pst_TempCurGO = pst_TempGO;

			for(pst_ElemScan = pst_Elem+1; pst_ElemScan <= pst_LastElem; pst_ElemScan++)
			{
				pst_TempGOScan = (OBJ_tdst_GameObject *) pst_ElemScan->p_Pointer;

				// pas flag => continue ...
				if(!OBJ_b_TestIdentityFlag(pst_TempGOScan, OBJ_C_IdentityFlag_Visu)) continue;
				if ( (pst_TempGOScan->pst_Base->pst_Visu->ul_DrawMask & GDI_Cul_DM_NoAutoClone) ) continue;
				
				// pas même geométrie => continue ...
				if (pst_TempGO->pst_Base->pst_Visu->pst_Object != pst_TempGOScan->pst_Base->pst_Visu->pst_Object) continue;

				// pas même matériau => continue ...
                if (pst_TempGO->pst_Base->pst_Visu->pst_Material != pst_TempGOScan->pst_Base->pst_Visu->pst_Material) continue;

				pst_TempCurGO->p_CloneNextGao = pst_TempGOScan;
				
				StockCloneLight(pst_TempGOScan);

				pst_TempCurGO = pst_TempCurGO-> p_CloneNextGao;
				pst_TempCurGO-> p_CloneNextGao = pst_TempGO;
			}
		}
		if (pst_TempGO->p_CloneNextGao) StockCloneLight(pst_TempGO);
	}
}

//

#endif

#if defined(_XENON_RENDER)
void OBJ_UpdateXenonVisu(GRO_tdst_Visu* _pst_DestVisu, GRO_tdst_Visu* _pst_SrcVisu, BOOL _b_Copy)
{
    // Release the old data and copy the new information
    if (_b_Copy)
    {
        // Clear
        THREAD_SAFE_VB_RELEASE(_pst_DestVisu->p_VBVertex);
        THREAD_SAFE_VB_RELEASE(_pst_DestVisu->p_VBFurOffsets);

        MEM_Free(_pst_DestVisu->p_FurOffsetVertex);



        for (LONG l_XeElem = 0; l_XeElem < _pst_DestVisu->l_NbXeElements; ++l_XeElem)
        {
            THREAD_SAFE_IB_RELEASE(_pst_DestVisu->p_XeElements[l_XeElem].pst_IndexBuffer);
            THREAD_SAFE_MESH_DELETE(_pst_DestVisu->p_XeElements[l_XeElem].pst_Mesh);

            if (_pst_DestVisu->p_XeElements[l_XeElem].dst_TangentSpace)
            {
                MEM_Free(_pst_DestVisu->p_XeElements[l_XeElem].dst_TangentSpace);
            }

#if defined(ACTIVE_EDITORS)
            if (_pst_DestVisu->p_XeElements[l_XeElem].puw_Indices)
            {
                MEM_Free(_pst_DestVisu->p_XeElements[l_XeElem].puw_Indices);
            }
#endif
        }

        if (_pst_DestVisu->p_XeElements)
        {
            MEM_Free(_pst_DestVisu->p_XeElements);
            _pst_DestVisu->p_XeElements = NULL;
        }

        // Copy
        _pst_DestVisu->p_VBVertex        = _pst_SrcVisu->p_VBVertex;
        _pst_DestVisu->p_VBFurOffsets    = _pst_SrcVisu->p_VBFurOffsets;
        _pst_DestVisu->p_FurOffsetVertex = _pst_SrcVisu->p_FurOffsetVertex;
        _pst_DestVisu->l_NbXeElements    = _pst_SrcVisu->l_NbXeElements;
        _pst_DestVisu->l_VBVertexCount   = _pst_SrcVisu->l_VBVertexCount;
        _pst_DestVisu->p_XeElements      = _pst_SrcVisu->p_XeElements;
        _pst_DestVisu->pVB_MorphStatic   = _pst_SrcVisu->pVB_MorphStatic;
        _pst_DestVisu->pVB_MorphDynamic  = NULL;
    }

    // Get the associated geometric object
    GEO_tdst_Object* pst_Geo = NULL;
    if (_pst_DestVisu->pst_Object)
    {
        if (_pst_DestVisu->pst_Object->i->ul_Type == GRO_Geometric)
        {
            pst_Geo = (GEO_tdst_Object*)_pst_DestVisu->pst_Object;
        }
        else if (_pst_DestVisu->pst_Object->i->ul_Type == GRO_GeoStaticLOD)
        {
            GEO_tdst_StaticLOD* pst_LOD = (GEO_tdst_StaticLOD*)_pst_DestVisu->pst_Object;

            if ((pst_LOD->uc_NbLOD > 0) && (pst_LOD->dpst_Id[0] != NULL) &&
                (pst_LOD->dpst_Id[0]->i->ul_Type == GRO_Geometric))
            {
                pst_Geo = (GEO_tdst_Object*)pst_LOD->dpst_Id[0];
            }
        }
    }

    // Share the main vertex buffer
    if (_pst_DestVisu->p_VBVertex)
    {
        _pst_DestVisu->p_VBVertex->AddRef();
    }

    // Create a new fur dynamic offset vertex buffer
    if (_pst_DestVisu->p_VBFurOffsets)
    {
        _pst_DestVisu->p_VBFurOffsets = (XeBuffer*)g_XeBufferMgr.CreateDynVertexBuffer();
    }

    // Duplicate the fur offset vertices
    if (_pst_DestVisu->p_FurOffsetVertex)
    {
        _pst_DestVisu->p_FurOffsetVertex = (FurOffsetVertex*)MEM_p_Alloc(_pst_DestVisu->l_VBVertexCount * sizeof(FurOffsetVertex));
        L_memcpy(_pst_DestVisu->p_FurOffsetVertex, _pst_SrcVisu->p_FurOffsetVertex, _pst_DestVisu->l_VBVertexCount * sizeof(FurOffsetVertex));
    }

    // Share the static morphing vertex buffer and create the dynamic one
    if (_pst_DestVisu->pVB_MorphStatic)
    {
        _pst_DestVisu->pVB_MorphStatic->AddRef();

        _pst_DestVisu->pVB_MorphDynamic = (XeBuffer*)g_XeBufferMgr.CreateDynVertexBuffer();
    }

    // Duplicate the Xenon elements
    if (_pst_DestVisu->l_NbXeElements > 0)
    {
        _pst_DestVisu->p_XeElements = (GRO_tdst_XeElement*)MEM_p_Alloc(_pst_DestVisu->l_NbXeElements * sizeof(GRO_tdst_XeElement));
        L_memset(_pst_DestVisu->p_XeElements, 0, _pst_DestVisu->l_NbXeElements * sizeof(GRO_tdst_XeElement));

        for (LONG l_XeElem = 0; l_XeElem < _pst_DestVisu->l_NbXeElements; ++l_XeElem)
        {
            GRO_tdst_XeElement* pst_NewElem = &_pst_DestVisu->p_XeElements[l_XeElem];
            GRO_tdst_XeElement* pst_SrcElem = &_pst_SrcVisu->p_XeElements[l_XeElem];

            MATH_CopyVector(&pst_NewElem->st_AABBMin, &pst_SrcElem->st_AABBMin);
            MATH_CopyVector(&pst_NewElem->st_AABBMax, &pst_SrcElem->st_AABBMax);

            // Index buffer
            if (pst_SrcElem->pst_IndexBuffer)
            {
                pst_NewElem->pst_IndexBuffer = pst_SrcElem->pst_IndexBuffer;
                pst_NewElem->pst_IndexBuffer->AddRef();
            }

            // Set the mesh
            if (pst_SrcElem->pst_Mesh)
            {
                pst_NewElem->pst_Mesh = new XeMesh();

                if (_pst_DestVisu->p_VBVertex)
                {
                    pst_NewElem->pst_Mesh->AddStream(pst_SrcElem->pst_Mesh->GetStream(0)->ulComponents, 
                                                     _pst_DestVisu->p_VBVertex);
                }

                // Set the maximum number of bones and weights
                pst_NewElem->pst_Mesh->SetMaxWeights(pst_SrcElem->pst_Mesh->GetMaxWeights());
                pst_NewElem->pst_Mesh->SetMaxBones(pst_SrcElem->pst_Mesh->GetMaxBones());

                // Set the bounding box if any
                if (pst_SrcElem->pst_Mesh->HasBoundingVolume())
                {
                    pst_NewElem->pst_Mesh->SetAxisAlignedBoundingVolume(pst_NewElem->st_AABBMin,
                        pst_NewElem->st_AABBMax);
                }

                // Add the index buffer
                if (pst_NewElem->pst_IndexBuffer)
                    pst_NewElem->pst_Mesh->SetIndices(pst_NewElem->pst_IndexBuffer);

                // Add the fur vertex buffer
                if (_pst_DestVisu->p_VBFurOffsets)
                    pst_NewElem->pst_Mesh->AddStream(pst_SrcElem->pst_Mesh->GetStream(1)->ulComponents, 
                    _pst_DestVisu->p_VBFurOffsets);

                // Add the morphing vertex buffers
                if (_pst_DestVisu->pVB_MorphStatic)
                {
                    pst_NewElem->pst_Mesh->AddStream(pst_SrcElem->pst_Mesh->GetStream(0)->ulComponents,
                                                     _pst_DestVisu->pVB_MorphStatic);

                    pst_NewElem->pst_Mesh->AddStream(XEVC_POSITION | XEVC_TANGENT | XEVC_NORMAL, 
                                                     _pst_DestVisu->pVB_MorphDynamic);
                }
            }

            // Copy the tangent space
            if (pst_SrcElem->dst_TangentSpace)
            {
                if ((pst_Geo != NULL) && (pst_Geo->l_NbPoints > 0))
                {
                    ULONG ulTangentSpaceSize = pst_Geo->l_NbPoints * sizeof(GEO_tdst_TextureSpaceBasis);
                    pst_NewElem->dst_TangentSpace = (GEO_tdst_TextureSpaceBasis*)MEM_p_Alloc(ulTangentSpaceSize);
                    L_memcpy(pst_NewElem->dst_TangentSpace, pst_SrcElem->dst_TangentSpace, ulTangentSpaceSize);
                }
            }

#if defined(ACTIVE_EDITORS)
            if (pst_NewElem->pst_IndexBuffer && pst_SrcElem->puw_Indices)
            {
                ULONG ulNbIndices = pst_NewElem->pst_IndexBuffer->GetIndexCount();




                pst_NewElem->puw_Indices = (USHORT*)MEM_p_Alloc(ulNbIndices * sizeof(USHORT));

                L_memcpy(pst_NewElem->puw_Indices, pst_SrcElem->puw_Indices, ulNbIndices * sizeof(USHORT));
            }
#endif
        }
    }

    // Update the visu-geo link
    if (pst_Geo != NULL)
        _pst_DestVisu->ul_VBObjectValidate = (ULONG)pst_Geo;
    else
        _pst_DestVisu->ul_VBObjectValidate = XENON_VB_OBJECT_VALIDATE_MAGIC;

    // Discard the list
    //_pst_DestVisu->pRejectedLights = NULL;
}
#endif


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
