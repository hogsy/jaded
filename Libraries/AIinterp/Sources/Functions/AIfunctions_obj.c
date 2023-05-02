/*$T AIfunctions_obj.c GC! 1.081 10/16/02 09:42:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/*$T AIfunctions_obj.c GC! 1.081 10/16/02 09:42:09 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJinit.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/COLlision/COLstruct.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLaccess.h"
#include "ENGine/Sources/COLlision/COLmain.h"
#include "ENGine/Sources/COLlision/COLreport.h"
#include "ENGine/Sources/COLlision/COLzdx.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/COLlision/COLray.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/ENGinit.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "ENGine/Sources/OBJects/OBJculling.h"

/* #include "SPriteList/SPLstruct.h" */
#include "PArticleGenerator/PAGstruct.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDload.h"
#include "GraphicDK/Sources/GFX/GFX.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GAO.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SNAKE.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SPG2.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_SDW.h"

#include "BASe/BENch/BENch.h"

#ifdef _FINAL_
#define M_StartRaster()
#define M_StopRaster()
#else
#define M_StartRaster() _GSP_BeginRaster(39)
#define M_StopRaster()	_GSP_EndRaster(39)
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#ifdef ACTIVE_EDITORS
extern ULONG	ENG_gul_Loop;
extern char		*BIG_FileName(BIG_INDEX);
void			AI_Debug_Move(OBJ_tdst_GameObject *, char *);
void			AI_Debug_ControlFlag(OBJ_tdst_GameObject *, char *, ULONG);
#endif
extern void		SND_RegisterInactiveGAO(OBJ_tdst_GameObject *);
extern void		SND_DestroyInstanceOfGao(OBJ_tdst_GameObject *);

extern BOOL						OBJ_gb_DuplicateAI;
extern GDI_tdst_DisplayData  	gpst_GSP_stDD;
extern float					TIM_gf_LockTime;
extern void COL_FlushAndFreeUnCollidableList(OBJ_tdst_GameObject *);
extern void OBJ_GameObject_RemoveButEnFaitNon(OBJ_tdst_GameObject *, char);
/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	MANAGEMENT OF GENERATED OBJECT
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
 
/*
 =======================================================================================================================
	Structurs , constantes, globals
 =======================================================================================================================
 */
typedef struct AI_tdst_GeneratedObject_
{
	OBJ_tdst_GameObject	*pst_Model;
	OBJ_tdst_GameObject	*pst_Generated;
	float				f_DestroyTime;
	ULONG				ul_Used;
} AI_tdst_GeneratedObject;

OBJ_tdst_GameObject **AI_gppst_GeneratedObject_Destroyed = NULL;
int AI_gi_GeneratedObject_Destroyed = 0;

#define AI_gi_GeneratedObject_Destroyed_Gran 256

#define C_AI_MaxGeneratedObject	512
//#define C_AI_GenObject_LOG	

AI_tdst_GeneratedObject	AI_gst_GeneratedObject[ C_AI_MaxGeneratedObject ];
int						AI_gi_GeneratedObject_Number = 0;



#ifdef ODE_INSIDE
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void ODE_UpdateMatrix(OBJ_tdst_GameObject *_pst_GO)
{
	DYN_tdst_ODE		*pst_ODE;
	MATH_tdst_Vector	st_Offset;
	float				rot[12];


	if(!_pst_GO || !(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) || !_pst_GO->pst_Base || !_pst_GO->pst_Base->pst_ODE)
		return;

	pst_ODE = _pst_GO->pst_Base->pst_ODE;

	MATH_TransformVector(&st_Offset, _pst_GO->pst_GlobalMatrix, &pst_ODE->st_Offset);

	ODE_ComputeODEMatrixFromJade(_pst_GO->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot); 

	if((pst_ODE->uc_Flags & ODE_FLAGS_MATHCOLMAP) && (pst_ODE->uc_Type != ODE_TYPE_PLAN))
	{
		dGeomSetPosition(pst_ODE->ode_id_geom, _pst_GO->pst_GlobalMatrix->T.x + st_Offset.x, _pst_GO->pst_GlobalMatrix->T.y + st_Offset.y, _pst_GO->pst_GlobalMatrix->T.z + st_Offset.z); 
		dGeomSetRotation(pst_ODE->ode_id_geom, &rot[0]);
	}

}
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_GeneratedObject_Reinit( void )
{
	AI_gi_GeneratedObject_Number = 0;

	/* Objects générés détruit par le TTL */
	if(AI_gppst_GeneratedObject_Destroyed)
	{
		int i;

		for(i = 0; i < AI_gi_GeneratedObject_Destroyed; i++)
		{
			MEM_Free(AI_gppst_GeneratedObject_Destroyed[i]);
		}
		AI_gi_GeneratedObject_Destroyed = 0;

		MEM_Free(AI_gppst_GeneratedObject_Destroyed);
		AI_gppst_GeneratedObject_Destroyed = NULL;
	}
}

#define AI_C_GeneratedObject_TimeToLive 20.0


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_GeneratedObject_FreeIndex( int i_Best )
{
	OBJ_tdst_GameObject		*pst_GO;
	WOR_tdst_World			*pst_World;
	
	if (i_Best == -1)
		return;

	if (i_Best >= AI_gi_GeneratedObject_Number)
		return;
	
	pst_GO = AI_gst_GeneratedObject[ i_Best ].pst_Generated;

	/* if object AI was in a special running context, delete it from the table */
	AI_DeleteFromRunContext( pst_GO );
		
	/* Delete object from current world */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(pst_World)
	{
		WOR_DelObj(pst_World, pst_GO);
		WOR_World_DetachObject(pst_World, pst_GO);
		INT_SnP_DetachObject(pst_GO, pst_World);
		SND_DetachObject(pst_GO);
		COL_FlushAndFreeUnCollidableList(pst_GO);
	}
		
	/* Remove engine object But en fait non */
	OBJ_GameObject_RemoveButEnFaitNon(pst_GO, 1);

	/* Je copie comme un pouerk le contenu du GO model */
	L_memcpy(pst_GO, AI_gst_GeneratedObject[ i_Best ].pst_Model, sizeof(struct	OBJ_tdst_GameObject_));
	
	pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_Generated;

	if(!AI_gppst_GeneratedObject_Destroyed)
	{
		AI_gppst_GeneratedObject_Destroyed = (OBJ_tdst_GameObject **) MEM_p_Alloc(AI_gi_GeneratedObject_Destroyed_Gran * sizeof(OBJ_tdst_GameObject *));
	}
	else
	{
		if(!(AI_gi_GeneratedObject_Destroyed % AI_gi_GeneratedObject_Destroyed_Gran))
		{
			AI_gppst_GeneratedObject_Destroyed = (OBJ_tdst_GameObject ** )MEM_p_Realloc(AI_gppst_GeneratedObject_Destroyed, (AI_gi_GeneratedObject_Destroyed + AI_gi_GeneratedObject_Destroyed_Gran) * sizeof(OBJ_tdst_GameObject *));
		}
	}

	AI_gppst_GeneratedObject_Destroyed[AI_gi_GeneratedObject_Destroyed++] = pst_GO;

	
	if (i_Best != AI_gi_GeneratedObject_Number - 1)
	{
		L_memcpy( AI_gst_GeneratedObject + i_Best, AI_gst_GeneratedObject + (AI_gi_GeneratedObject_Number - 1), sizeof( AI_tdst_GeneratedObject ) );
		AI_gi_GeneratedObject_Number--;
	}
	else
	{
		AI_gi_GeneratedObject_Number--;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_GeneratedObject_TimeToLive( void )
{
	int						i_Index;	

	for (i_Index = 0; i_Index < AI_gi_GeneratedObject_Number; i_Index++ )
	{
		if (AI_gst_GeneratedObject[ i_Index ].ul_Used )
			continue;

		if (((TIM_gf_MainClock - TIM_gf_LockTime) -  AI_gst_GeneratedObject[ i_Index ].f_DestroyTime) > AI_C_GeneratedObject_TimeToLive)
		{
			AI_GeneratedObject_FreeIndex(i_Index);
			i_Index --;
		}
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_GeneratedObject_FreeOne( void )
{
	int						i_Index, i_Best;
	float					f_Time;
	OBJ_tdst_GameObject		*pst_GO;
	WOR_tdst_World			*pst_World;
	
	i_Best = -1;
	f_Time = -1;
	for (i_Index = 0; i_Index < AI_gi_GeneratedObject_Number; i_Index++ )
	{
		if (AI_gst_GeneratedObject[ i_Index ].ul_Used )
			continue;

		if (( f_Time == -1 ) || (f_Time > AI_gst_GeneratedObject[ i_Index ].f_DestroyTime ) )
		{
			f_Time = AI_gst_GeneratedObject[ i_Index ].f_DestroyTime;
			i_Best = i_Index;
		}
	}
	
	if (i_Best == -1)
		return;
	
	pst_GO = AI_gst_GeneratedObject[ i_Best ].pst_Generated;
	/* if object AI was in a special running context, delete it from the table */
	AI_DeleteFromRunContext( pst_GO );
		
	/* Delete object from current world */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	if(pst_World)
	{
		WOR_DelObj(pst_World, pst_GO);
		WOR_World_DetachObject(pst_World, pst_GO);
		INT_SnP_DetachObject(pst_GO, pst_World);
		SND_DetachObject(pst_GO);
		COL_FlushAndFreeUnCollidableList(pst_GO);
	}
		
	/* Remove engine object */
	OBJ_GameObject_Remove(pst_GO, 1);
	
	if (i_Best != AI_gi_GeneratedObject_Number - 1)
	{
		L_memcpy( AI_gst_GeneratedObject + i_Best, AI_gst_GeneratedObject + (AI_gi_GeneratedObject_Number - 1), sizeof( AI_tdst_GeneratedObject ) );
		AI_gi_GeneratedObject_Number--;
	}
	else
	{
		AI_gi_GeneratedObject_Number--;
	}
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_GeneratedObject_Store( OBJ_tdst_GameObject *pst_Model, OBJ_tdst_GameObject *pst_Dup )
{
	AI_tdst_GeneratedObject *pst_Gen;
	
	if( AI_gi_GeneratedObject_Number >= C_AI_MaxGeneratedObject )//POPOWARNING
	{
		AI_GeneratedObject_FreeOne();
		if (AI_gi_GeneratedObject_Number >= C_AI_MaxGeneratedObject )//POPOWARNING
		{
			ERR_X_Warning(0, "too many generated object to store", NULL );
			return;
		}
	}
		
	pst_Gen = AI_gst_GeneratedObject + AI_gi_GeneratedObject_Number++;
	pst_Gen->pst_Model = pst_Model;
	pst_Gen->ul_Used = 1;
	pst_Gen->pst_Generated = pst_Dup;
	
#ifdef C_AI_GenObject_LOG
#ifdef _DEBUG
	{
	char sz_Msg[ 1024];
	sprintf ( sz_Msg, "[GEN] Store (%d) %s - %s", pst_Gen - AI_gst_GeneratedObject, pst_Model->sz_Name, pst_Dup->sz_Name );
	LINK_PrintStatusMsg( sz_Msg );
	}
#endif
#endif
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_GeneratedObject_Destroy( OBJ_tdst_GameObject *_pst_Dup )
{
	AI_tdst_GeneratedObject *pst_Gen, *pst_Last;
		
	if(AI_gi_GeneratedObject_Destroyed)
	{
		int i;
		
		for(i = 0; i < AI_gi_GeneratedObject_Destroyed; i++)
		{
			if(AI_gppst_GeneratedObject_Destroyed[i] == _pst_Dup)
			{
#ifndef _FINAL_
				printf("Generated Object detroyed requested to be destroyed one more time ...\n");
#endif				
				return 1;	
			}
			
		}
	}
	
	pst_Gen = AI_gst_GeneratedObject;
	pst_Last = pst_Gen + AI_gi_GeneratedObject_Number;
	
	while (pst_Gen < pst_Last )
	{
		if ( pst_Gen->pst_Generated == _pst_Dup )
		{
			pst_Gen->f_DestroyTime = TIM_gf_MainClock - TIM_gf_LockTime;
			pst_Gen->ul_Used = 0;
			#ifdef C_AI_GenObject_LOG
			#ifdef _DEBUG
			{
				char sz_Msg[ 1024];
				sprintf ( sz_Msg, "[GEN] Destroy (%d - %.3f) %s", pst_Gen - AI_gst_GeneratedObject, pst_Gen->f_DestroyTime, _pst_Dup->sz_Name );
				LINK_PrintStatusMsg( sz_Msg );
			}
			#endif
			#endif
			return 1;
		}
		pst_Gen++;
	}
	#ifdef C_AI_GenObject_LOG
	LINK_PrintStatusMsg( "[GEN] Nothing to destroy");
	#endif
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_GeneratedObject_Reset(OBJ_tdst_GameObject *_pst_Dup, OBJ_tdst_GameObject *_pst_Model )
{
	OBJ_SetAbsoluteMatrix( _pst_Dup, OBJ_pst_GetAbsoluteMatrix( _pst_Model ) );
	OBJ_SetInitialAbsoluteMatrix( _pst_Dup, OBJ_pst_GetInitialAbsoluteMatrix( _pst_Model ) );

	_pst_Dup->c_FixFlags = _pst_Model->c_FixFlags;
	_pst_Dup->c_FixFlags &= ~OBJ_C_HasBeenMerge;
	_pst_Dup->uc_DesignFlags = _pst_Model->uc_DesignFlags;
	_pst_Dup->uc_VisiCoeff = _pst_Model->uc_VisiCoeff;

	_pst_Dup->ul_StatusAndControlFlags &= ~OBJ_C_CustomBitAll;
	_pst_Dup->ul_StatusAndControlFlags |= (_pst_Model->ul_StatusAndControlFlags & OBJ_C_CustomBitAll);
	_pst_Dup->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Links;

	/* DRL: Reset always visible & always active flags.
	If these are changed on a generated object, and that object is destroyed and later reused,
	by default these flags won't disappear. Dev oversight - probably this applies to other flags too... */
	_pst_Dup->ul_StatusAndControlFlags &= ~( OBJ_C_ControlFlag_AlwaysActive | OBJ_C_ControlFlag_AlwaysVisible );
	
	if(OBJ_b_TestControlFlag(_pst_Model, OBJ_C_ControlFlag_RayInsensitive))
		_pst_Dup->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_RayInsensitive;
	if(OBJ_b_TestControlFlag(_pst_Model, OBJ_C_ControlFlag_EditableBV))
		_pst_Dup->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EditableBV;
	if(OBJ_b_TestControlFlag(_pst_Model, OBJ_C_ControlFlag_EnableSnP))
		_pst_Dup->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
	if(OBJ_b_TestControlFlag(_pst_Model, OBJ_C_ControlFlag_ForceDetectionList))
		_pst_Dup->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_ForceDetectionList;
	if(OBJ_b_TestControlFlag(_pst_Model, OBJ_C_ControlFlag_LookAt))
		_pst_Dup->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_LookAt;

	if (_pst_Dup->pst_Extended && _pst_Model->pst_Extended)
	{
		_pst_Dup->pst_Extended->uw_ExtraFlags		= _pst_Model->pst_Extended->uw_ExtraFlags;
		_pst_Dup->pst_Extended->uc_AiPrio			= _pst_Model->pst_Extended->uc_AiPrio;
		_pst_Dup->pst_Extended->uw_Capacities		= _pst_Model->pst_Extended->uw_Capacities;
		_pst_Dup->pst_Extended->uw_CapacitiesInit	= _pst_Model->pst_Extended->uw_CapacitiesInit;
		
		if (_pst_Dup->pst_Extended->pst_Design && _pst_Model->pst_Extended->pst_Design )
			L_memcpy( _pst_Dup->pst_Extended->pst_Design, _pst_Model->pst_Extended->pst_Design, sizeof( OBJ_tdst_DesignStruct ) );
	}

	if(_pst_Dup->pst_Extended)
	{
		_pst_Dup->pst_Extended->auc_Sectos[0] = 0;
		_pst_Dup->pst_Extended->auc_Sectos[1] = 0;
		_pst_Dup->pst_Extended->auc_Sectos[2] = 0;
		_pst_Dup->pst_Extended->auc_Sectos[3] = 0;
	}
}
/**/
OBJ_tdst_GameObject *AI_pst_GeneratedObject_Get( OBJ_tdst_GameObject *_pst_Model )
{
	AI_tdst_GeneratedObject *pst_Gen, *pst_Last;
	
	pst_Gen = AI_gst_GeneratedObject;
	pst_Last = pst_Gen + AI_gi_GeneratedObject_Number;
	
	while (pst_Gen < pst_Last )
	{
		if (( !pst_Gen->ul_Used ) && (pst_Gen->pst_Model == _pst_Model ))
		{
			pst_Gen->ul_Used = 1;
			#ifdef C_AI_GenObject_LOG
			#ifdef _DEBUG
			{
				char sz_Msg[ 1024];
				sprintf ( sz_Msg, "[GEN] Get (%d) %s %s", pst_Gen - AI_gst_GeneratedObject, _pst_Model->sz_Name, pst_Gen->pst_Generated->sz_Name );
				LINK_PrintStatusMsg( sz_Msg );
			}
			#endif
			#endif

			AI_GeneratedObject_Reset(pst_Gen->pst_Generated, _pst_Model );
			return pst_Gen->pst_Generated;
		}
		pst_Gen++;
	}
	#ifdef C_AI_GenObject_LOG
	#ifdef _DEBUG
	{
		char sz_Msg[ 1024];
		sprintf ( sz_Msg, "[GEN] Nothing to get (%s)", _pst_Model->sz_Name);
		LINK_PrintStatusMsg( sz_Msg );
	}
	#endif
	#endif
	return NULL;
}


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	AI FUNCTIONS
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */
 
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJDrawMaskSet_C(OBJ_tdst_GameObject *_pst_GO, int on, int off)
{
	if(!_pst_GO->pst_Base) return;
	if(!_pst_GO->pst_Base->pst_Visu) return;
	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return;
	_pst_GO->pst_Base->pst_Visu->ul_DrawMask |= on;
	_pst_GO->pst_Base->pst_Visu->ul_DrawMask &= ~off;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJDrawMaskSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					on, off;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	off = AI_PopInt();
	on = AI_PopInt();
	AI_EvalFunc_OBJDrawMaskSet_C(pst_GO, on, off);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJDrawMaskGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	if(!_pst_GO->pst_Base) return 0;
	if(!_pst_GO->pst_Base->pst_Visu) return 0;
	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return 0;
	return _pst_GO->pst_Base->pst_Visu->ul_DrawMask;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJDrawMaskGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJDrawMaskGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_OBJMe_C(void)
{
	return AI_gpst_CurrentGameObject;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJMe(AI_tdst_Node *_pst_Node)
{
	AI_PushGameObject(AI_gpst_CurrentGameObject);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_OBJDuplicate_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *v)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_New;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OBJ_gb_DuplicateAI = TRUE;
	
	/* try to see if one alerady generated object is disponible */
	pst_New = AI_pst_GeneratedObject_Get( _pst_GO );

	/* Duplicate the game object */
	if (!pst_New)
	{
		pst_New = OBJ_GameObject_Duplicate(WOR_gpst_CurrentWorld, _pst_GO, TRUE, FALSE, v, 0, NULL);
		/* store generated object */
		AI_GeneratedObject_Store( _pst_GO, pst_New );
	}
	else
	{		
		/* Force the activation to be redone next frame */
		if(WOR_gpst_CurrentWorld)
		{
			WOR_gpst_CurrentWorld->b_ForceActivationRefresh = TRUE;
			WOR_gpst_CurrentWorld->b_ForceVisibilityRefresh = TRUE;
			WOR_gpst_CurrentWorld->b_ForceBVRefresh = TRUE;
		}

		if((pst_New->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims) && pst_New->pst_Base && pst_New->pst_Base->pst_GameObjectAnim && pst_New->pst_Base->pst_GameObjectAnim->pst_Skeleton)
		{
			OBJ_tdst_Group			*pst_Skeleton;
			TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
			OBJ_tdst_GameObject		*pst_BoneGO;
			int						i;


			pst_Skeleton = pst_New->pst_Base->pst_GameObjectAnim->pst_Skeleton;

			pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
			pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
			for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
			{
				pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
				ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));

				if(pst_BoneGO->pst_Base && pst_BoneGO->pst_Base->pst_Hierarchy)
				{
					if(pst_BoneGO->pst_Base->pst_Hierarchy->pst_FatherInit)
						pst_BoneGO->pst_Base->pst_Hierarchy->pst_Father = pst_BoneGO->pst_Base->pst_Hierarchy->pst_FatherInit;

					if(OBJ_b_TestIdentityFlag(pst_BoneGO, OBJ_C_IdentityFlag_HasInitialPos))
					{
						MATH_CopyMatrix(&pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix, pst_BoneGO->pst_GlobalMatrix + 1);
					}

					MATH_MulMatrixMatrix
					(
						OBJ_pst_GetAbsoluteMatrix(pst_BoneGO),
						&pst_BoneGO->pst_Base->pst_Hierarchy->st_LocalMatrix,
						OBJ_pst_GetAbsoluteMatrix(pst_BoneGO->pst_Base->pst_Hierarchy->pst_FatherInit)
					);
				}
			}
		}
	}

	/* Set initial position */
	OBJ_SetInitialAbsolutePosition(pst_New, v);
	OBJ_RestoreInitialPos(pst_New);

	/* Reinit object */
	ENG_ReinitOneObject(pst_New, UNI_Cuc_ForDuplicate);

	/* Add object in SnP if needed */
	INT_SnP_AttachObject(pst_New, WOR_gpst_CurrentWorld);

#ifdef ODE_INSIDE
	if(pst_New->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(pst_New);
#endif


	OBJ_gb_DuplicateAI = FALSE;
	return pst_New;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJDuplicate(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushGameObject(AI_EvalFunc_OBJDuplicate_C(pst_GO, AI_PopVectorPtr()));
	return ++_pst_Node;
}

#define WOR_C_MaxKilledGO	200
OBJ_tdst_GameObject *WOR_AllKilledGO[WOR_C_MaxKilledGO];
int					WOR_gi_NumKilledGO = 0;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void WOR_ResetKillGO(void)
{
	WOR_gi_NumKilledGO = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_HasBeenKilled_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < WOR_gi_NumKilledGO; i++)
	{
		if(WOR_AllKilledGO[i] == pst_GO) return 1;
	}

	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_HasBeenKilled(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GO = AI_PopGameObject();
	AI_PushInt(AI_EvalFunc_OBJ_HasBeenKilled_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJDestroy_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	BOOL			b_RealDestroy;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	_GSP_EndRaster(14);
	AI_ExecCallback(pst_GO, AI_C_Callback_WhenDestroy);
	_GSP_BeginRaster(14);

	/* Register killed GO */
	if(WOR_gi_NumKilledGO < WOR_C_MaxKilledGO)
	{
		WOR_AllKilledGO[WOR_gi_NumKilledGO++] = pst_GO;
	}

#ifdef ACTIVE_EDITORS
	else
	{
		LINK_PrintStatusMsg("Max number of destroyed GO reached.");
	}
#endif

	if ( pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Generated ) 
		b_RealDestroy = !AI_GeneratedObject_Destroy( pst_GO );
	else
		b_RealDestroy = FALSE;

	/* inactive sound */
	M_StartRaster();
	SND_DestroyInstanceOfGao(pst_GO);
	M_StopRaster();

	/* Object must have been generated to be destroy */
	//if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Generated))
	if ( !b_RealDestroy )
	{
		/*~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
		ULONG	ul_OldFlags;
		/*~~~~~~~~~~~~~~~~*/

		ul_OldFlags = pst_GO->ul_StatusAndControlFlags;
#endif
		OBJ_FlagsControlSet(pst_GO, OBJ_ul_FlagsControlGet(pst_GO) | OBJ_C_ControlFlag_ForceInvisible);
		OBJ_FlagsControlSet(pst_GO, OBJ_ul_FlagsControlGet(pst_GO) | OBJ_C_ControlFlag_ForceInactive);


#ifdef ACTIVE_EDITORS
		AI_Debug_ControlFlag(pst_GO, "OBJ_destroy", ul_OldFlags);
#endif
		/* Delete object from current world */
		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		if(pst_World)
		{
			if ( pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Generated )
				WOR_DelObj(pst_World, pst_GO);
			pst_World->b_ForceActivationRefresh = TRUE;
			pst_World->b_ForceVisibilityRefresh = TRUE;
		}


		/* We remove the Dynamic hierarchy Link if there is one. */
		if
		(
			(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		&&	(pst_GO->pst_Base)
		&&	(pst_GO->pst_Base->pst_Hierarchy)
		&&	(!(pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit))
		)
		{
			/* The Child has no longer a Father. We update its global Matrix. */
			OBJ_ComputeGlobalWhenHie(pst_GO);

			/* Update all Flash Matrix of the Child (Blend matrix too) */
			if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix)) ANI_RemoveHierarchyOnFlash(pst_GO);

			pst_GO->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Hierarchy;

			MEM_Free(pst_GO->pst_Base->pst_Hierarchy);
			pst_GO->pst_Base->pst_Hierarchy = NULL;
		}
	}
	else
	{
		/* if object AI was in a special running context, delete it from the table */
		AI_DeleteFromRunContext( pst_GO );
			
		/* Delete object from current world */
		pst_World = WOR_World_GetWorldOfObject(pst_GO);
		if(pst_World)
		{
			WOR_DelObj(pst_World, pst_GO);
			WOR_World_DetachObject(pst_World, pst_GO);
			INT_SnP_DetachObject(pst_GO, pst_World);
			SND_DetachObject(pst_GO);
			COL_FlushAndFreeUnCollidableList(pst_GO);
		}
		
		/* Remove engine object */
		OBJ_GameObject_Remove(pst_GO, 1);
		
		if(pst_GO == AI_gpst_CurrentGameObject) return AI_CR_Destroy;
	}
	
	if(pst_GO == AI_gpst_CurrentGameObject) return AI_CR_StopInstance;
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJDestroy(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					ires;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	ires = AI_EvalFunc_OBJDestroy_C(pst_GO);
	if(ires == AI_CR_StopInstance) return (AI_tdst_Node *) AI_CR_StopInstance;
	if(ires == AI_CR_Destroy) return (AI_tdst_Node *) AI_CR_Destroy;
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJMove_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *v)
{
	/* Event part */
	if(ENG_gb_EVERunning)
	{
		MATH_ScaleVector(v, v, EVE_f_GetCurrentRatio());
	}

	MATH_AddEqualVector(OBJ_pst_GetAbsolutePosition(_pst_GO), v);

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		ANI_UpdateFlashAfterMagicBoxTranslation(_pst_GO, v, ANI_C_UpdateAllAnims);


#ifdef ACTIVE_EDITORS
	AI_Debug_Move(_pst_GO, "OBJ_Move");
#endif
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Dyna))
	{
		_pst_GO->pst_Base->pst_Dyna->ul_DynFlags |= DYN_C_OptimizeColDisable;
	}

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

}
/**/
AI_tdst_Node *AI_EvalFunc_OBJMove(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJMove_C(pst_GO, AI_PopVectorPtr());
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotate_FromToBlend_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Start, MATH_tdst_Vector *_pst_To, float _f_Blend )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy, b_Flash;
	MATH_tdst_Matrix	M, Temp;
	MATH_tdst_Vector	S, T, CP, V, VT;
	float				dot, norm, alpha;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	b_Flash = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix);

	if(b_Hierarchy)
	{
		OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
	}

	MATH_NormalizeVector( &S, _pst_Start );
	MATH_NormalizeVector( &T, _pst_To );
	
	dot = MATH_f_DotProduct( &S, &T );
	MATH_CrossProduct( &CP, &T, &S );
	norm = MATH_f_NormVector( &CP );
	if ( norm < Cf_Epsilon )
		return;
#ifdef JADEFUSION
	if( norm > 1.0f )
    {
        norm = 1.0f;
    }
#endif		
	if ( _f_Blend < 0.98f )
	{
		alpha = fAsin( norm );
		alpha *= _f_Blend;
		MATH_ScaleEqualVector( &CP, fInv( norm ) );
		norm = fSin( alpha );
		dot = fCos( alpha );
		MATH_ScaleEqualVector( &CP, norm );
	}
	
	norm = fInv( norm );
	MATH_ScaleVector( &V, &CP, norm );
	MATH_ScaleVector( &VT, &V, 1.0f - dot );
	
	MATH_SetIdentityMatrix( &M );

	M.Ix = VT.x * V.x + dot;
	M.Jy = VT.y * V.y + dot;
	M.Kz = VT.z * V.z + dot;

	VT.x *= V.y;
	VT.z *= V.x;
	VT.y *= V.z;

	M.Iy = VT.x - CP.z;
	M.Iz = VT.z + CP.y;
	M.Jx = VT.x + CP.z;
	M.Jz = VT.y - CP.x;
	M.Kx = VT.z - CP.y;
	M.Ky = VT.y + CP.x;
	
	/* Sets the Rotation Type. */
	MATH_SetRotationType(&M);

	MATH_CopyMatrix(&Temp, _pst_GO->pst_GlobalMatrix);
	MATH_MulMatrixMatrix(_pst_GO->pst_GlobalMatrix, &M, &Temp);
	
	if(b_Flash)
	{
		MATH_CopyMatrix(&Temp, OBJ_pst_GetFlashMatrix(_pst_GO));
		MATH_MulMatrixMatrix(OBJ_pst_GetFlashMatrix(_pst_GO), &M, &Temp);
	}

	if(b_Hierarchy)
	{
		OBJ_ComputeLocalWhenHie(_pst_GO);
	}

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotate_FromToBlend(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	from, to;
	float				f_Blend;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Blend = AI_PopFloat();
	AI_PopVector( &to );
	AI_PopVector( &from );

	AI_EvalFunc_OBJRotate_FromToBlend_C(pst_GO, &from, &to, f_Blend );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotate_FromTo_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Start, MATH_tdst_Vector *_pst_To )
{
	AI_EvalFunc_OBJRotate_FromToBlend_C( _pst_GO, _pst_Start, _pst_To, 1.0f );
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotate_FromTo(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	from, to;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector( &to );
	AI_PopVector( &from );

	AI_EvalFunc_OBJRotate_FromTo_C(pst_GO, &from, &to );

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotateAround_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_V, float _f_Ratio)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy, b_Flash;
	MATH_tdst_Matrix	M, Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	b_Flash = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix);

	if(b_Hierarchy)
	{
		OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
	}

	MATH_MakeRotationMatrix_AxisAngle(&M, _pst_V, _f_Ratio, NULL, 1);
	MATH_CopyMatrix(&Temp, _pst_GO->pst_GlobalMatrix);
	MATH_MulMatrixMatrix(_pst_GO->pst_GlobalMatrix, &M, &Temp);
	if(b_Flash)
	{
		MATH_CopyMatrix(&Temp, OBJ_pst_GetFlashMatrix(_pst_GO));
		MATH_MulMatrixMatrix(OBJ_pst_GetFlashMatrix(_pst_GO), &M, &Temp);
	}

	if(b_Hierarchy)
	{
		OBJ_ComputeLocalWhenHie(_pst_GO);
	}

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotateAround(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	OBJ_tdst_GameObject *pst_GO;
	float				f_Ratio;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Ratio = AI_PopFloat();
	AI_PopVector(&v);

	AI_EvalFunc_OBJRotateAround_C(pst_GO, &v, f_Ratio);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotateLocal_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_V)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Ratio;
	BOOL				b_Hierarchy, b_Flash;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Event part */
	if(ENG_gb_EVERunning)
	{
		f_Ratio = EVE_f_GetCurrentRatio();
		_pst_V->x *= f_Ratio;
		_pst_V->y *= f_Ratio;
		_pst_V->z *= f_Ratio;
	}

	b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	b_Flash = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix);

	if(b_Hierarchy)
	{
		MATH_CopyVector(&st_SaveT, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
		OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
	}

	if(_pst_V->x)
	{
		MATH_RotateMatrix_AroundLocalXAxis(_pst_GO->pst_GlobalMatrix, _pst_V->x);
		if(b_Flash) MATH_RotateMatrix_AroundLocalXAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _pst_V->x);
	}

	if(_pst_V->y)
	{
		MATH_RotateMatrix_AroundLocalYAxis(_pst_GO->pst_GlobalMatrix, _pst_V->y);
		if(b_Flash) MATH_RotateMatrix_AroundLocalYAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _pst_V->y);
	}

	if(_pst_V->z)
	{
		MATH_RotateMatrix_AroundLocalZAxis(_pst_GO->pst_GlobalMatrix, _pst_V->z);
		if(b_Flash) MATH_RotateMatrix_AroundLocalZAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _pst_V->z);
	}

	MATH_Orthonormalize(_pst_GO->pst_GlobalMatrix);
	if(b_Flash) MATH_Orthonormalize(OBJ_pst_GetFlashMatrix(_pst_GO));

	if(b_Hierarchy)
	{
		OBJ_ComputeLocalWhenHie(_pst_GO);
		MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
	}

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotateLocal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&v);

	AI_EvalFunc_OBJRotateLocal_C(pst_GO, &v);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotateLocalX_C(OBJ_tdst_GameObject *_pst_GO, float _f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_f)
	{
		if(ENG_gb_EVERunning)
		{
			_f *= EVE_f_GetCurrentRatio();
		}

		b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);

		if(b_Hierarchy)
		{
			MATH_CopyVector(&st_SaveT, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
			OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
		}

		if(_f)
		{
			MATH_RotateMatrix_AroundLocalXAxis(_pst_GO->pst_GlobalMatrix, _f);
			if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
				MATH_RotateMatrix_AroundLocalXAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _f);
			if(ANI_b_Blend(_pst_GO))
			{
				MATH_RotateMatrix_AroundLocalXAxis(&_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->st_Ref, _f);
				MATH_Orthonormalize(&_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->st_Ref);
			}

			MATH_Orthonormalize(_pst_GO->pst_GlobalMatrix);
		}

		if(b_Hierarchy)
		{
			OBJ_ComputeLocalWhenHie(_pst_GO);
			MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
		}

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotateLocalX(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f = AI_PopFloat();

	AI_EvalFunc_OBJRotateLocalX_C(pst_GO, f);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotateLocalY_C(OBJ_tdst_GameObject *_pst_GO, float _f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_f)
	{
		if(ENG_gb_EVERunning)
		{
			_f *= EVE_f_GetCurrentRatio();
		}

		b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);

		if(b_Hierarchy)
		{
			MATH_CopyVector(&st_SaveT, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
			OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
		}

		if(_f)
		{
			MATH_RotateMatrix_AroundLocalYAxis(_pst_GO->pst_GlobalMatrix, _f);
			if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
				MATH_RotateMatrix_AroundLocalYAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _f);
			if(ANI_b_Blend(_pst_GO))
			{
				MATH_RotateMatrix_AroundLocalYAxis(&_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->st_Ref, _f);
				MATH_Orthonormalize(&_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->st_Ref);
			}

			MATH_Orthonormalize(_pst_GO->pst_GlobalMatrix);
		}

		if(b_Hierarchy)
		{
			OBJ_ComputeLocalWhenHie(_pst_GO);
			MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
		}

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotateLocalY(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f = AI_PopFloat();

	AI_EvalFunc_OBJRotateLocalY_C(pst_GO, f);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotateLocalZ_C(OBJ_tdst_GameObject *_pst_GO, float _f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_f)
	{
		if(ENG_gb_EVERunning)
		{
			_f *= EVE_f_GetCurrentRatio();
		}

		b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);

		if(b_Hierarchy)
		{
			MATH_CopyVector(&st_SaveT, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
			OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
		}

		if(_f)
		{
			MATH_RotateMatrix_AroundLocalZAxis(_pst_GO->pst_GlobalMatrix, _f);
			if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
				MATH_RotateMatrix_AroundLocalZAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _f);
			if(ANI_b_Blend(_pst_GO))
			{
				MATH_RotateMatrix_AroundLocalZAxis(&_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->st_Ref, _f);
				MATH_Orthonormalize(&_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->st_Ref);
			}

			MATH_Orthonormalize(_pst_GO->pst_GlobalMatrix);
		}

		if(b_Hierarchy)
		{
			OBJ_ComputeLocalWhenHie(_pst_GO);
			MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
		}

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotateLocalZ(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f = AI_PopFloat();

	AI_EvalFunc_OBJRotateLocalZ_C(pst_GO, f);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotateGlobal_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_V)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	f_Ratio;
	BOOL	b_Hierarchy, b_Flash;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Event part */
	if(ENG_gb_EVERunning)
	{
		f_Ratio = EVE_f_GetCurrentRatio();
		_pst_V->x *= f_Ratio;
		_pst_V->y *= f_Ratio;
		_pst_V->z *= f_Ratio;
	}

	b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	b_Flash = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix);

	if(b_Hierarchy)
	{
		OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
	}

	if(_pst_V->x)
	{
		MATH_RotateMatrix_AroundGlobalXAxis(_pst_GO->pst_GlobalMatrix, _pst_V->x);
		if(b_Flash) MATH_RotateMatrix_AroundGlobalXAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _pst_V->x);
	}

	if(_pst_V->y)
	{
		MATH_RotateMatrix_AroundGlobalYAxis(_pst_GO->pst_GlobalMatrix, _pst_V->y);
		if(b_Flash) MATH_RotateMatrix_AroundGlobalYAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _pst_V->y);
	}

	if(_pst_V->z)
	{
		MATH_RotateMatrix_AroundGlobalZAxis(_pst_GO->pst_GlobalMatrix, _pst_V->z);
		if(b_Flash) MATH_RotateMatrix_AroundGlobalZAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _pst_V->z);
	}

	if(b_Hierarchy) OBJ_ComputeLocalWhenHie(_pst_GO);

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotateGlobal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&v);

	AI_EvalFunc_OBJRotateGlobal_C(pst_GO, &v);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotateGlobalX_C(OBJ_tdst_GameObject *_pst_GO, float _f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_f)
	{
		if(ENG_gb_EVERunning)
		{
			_f *= EVE_f_GetCurrentRatio();
		}

		b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);

		if(b_Hierarchy)
		{
			MATH_CopyVector(&st_SaveT, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
			OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
		}

		if(_f)
		{
			MATH_RotateMatrix_AroundGlobalXAxis(_pst_GO->pst_GlobalMatrix, _f);
			if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
				MATH_RotateMatrix_AroundGlobalXAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _f);
		}

		if(b_Hierarchy)
		{
			OBJ_ComputeLocalWhenHie(_pst_GO);
			MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
		}

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotateGlobalX(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f = AI_PopFloat();

	AI_EvalFunc_OBJRotateGlobalX_C(pst_GO, f);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotateGlobalY_C(OBJ_tdst_GameObject *_pst_GO, float _f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_f)
	{
		if(ENG_gb_EVERunning)
		{
			_f *= EVE_f_GetCurrentRatio();
		}

		b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);

		if(b_Hierarchy)
		{
			MATH_CopyVector(&st_SaveT, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
			OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
		}

		if(_f)
		{
			MATH_RotateMatrix_AroundGlobalYAxis(_pst_GO->pst_GlobalMatrix, _f);
			if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
				MATH_RotateMatrix_AroundGlobalYAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _f);
		}

		if(b_Hierarchy)
		{
			OBJ_ComputeLocalWhenHie(_pst_GO);
			MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
		}
#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotateGlobalY(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f = AI_PopFloat();

	AI_EvalFunc_OBJRotateGlobalY_C(pst_GO, f);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJRotateGlobalZ_C(OBJ_tdst_GameObject *_pst_GO, float _f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_f)
	{
		if(ENG_gb_EVERunning)
		{
			_f *= EVE_f_GetCurrentRatio();
		}

		b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);

		if(b_Hierarchy)
		{
			MATH_CopyVector(&st_SaveT, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
			OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
		}

		if(_f)
		{
			MATH_RotateMatrix_AroundGlobalZAxis(_pst_GO->pst_GlobalMatrix, _f);
			if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
				MATH_RotateMatrix_AroundGlobalZAxis(OBJ_pst_GetFlashMatrix(_pst_GO), _f);
		}

		if(b_Hierarchy)
		{
			OBJ_ComputeLocalWhenHie(_pst_GO);
			MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
		}
#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJRotateGlobalZ(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f = AI_PopFloat();

	AI_EvalFunc_OBJRotateGlobalZ_C(pst_GO, f);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    If the GameObject is a Bone, we have to compute the Absolute Matrix of this bone and, to do so, the
            Absolute Matrix all the Fathers recursively.
 =======================================================================================================================
 */
void AI_EvalFunc_OBJGetPos_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *v)
{
	if(!(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Bone)))
	{
		MATH_CopyVector(v, OBJ_pst_GetAbsolutePosition(_pst_GO));
		return;
	}
	else
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	st_Temp, st_Temp2;
		OBJ_tdst_GameObject *pst_Ref, *pst_Current;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Ref = ANI_pst_GetReference(_pst_GO);
		MATH_SetIdentityMatrix(&st_Temp);

		if(pst_Ref->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			OBJ_ComputeGlobalWhenHie(pst_Ref);

		pst_Current = _pst_GO;
		if(_pst_GO->pst_Base && _pst_GO->pst_Base->pst_Hierarchy && !_pst_GO->pst_Base->pst_Hierarchy->pst_Father)
		{
			MATH_CopyVector(v, OBJ_pst_GetAbsolutePosition(_pst_GO));
		}
		else
		{
			while(pst_Current && pst_Current != pst_Ref)
			{
				MATH_MulMatrixMatrix(&st_Temp2, &st_Temp, OBJ_pst_GetLocalMatrix(pst_Current));
				MATH_CopyMatrix(&st_Temp, &st_Temp2);
				pst_Current = OBJ_pst_GetFather(pst_Current);
			}

			MATH_MulMatrixMatrix(&st_Temp2, &st_Temp, pst_Ref->pst_GlobalMatrix);
			MATH_CopyVector(v, &st_Temp2.T);
		}
	}
}
/**/
void AI_EvalFunc_OBJGetPos_C_CURRENT(MATH_tdst_Vector *v)
{
	AI_EvalFunc_OBJGetPos_C(AI_gpst_CurrentGameObject, v);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJGetPos(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	if ( pst_GO != NULL )
	{
		AI_EvalFunc_OBJGetPos_C( pst_GO, &st_Pos );
		AI_PushVector( &st_Pos );
	}
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJRestoreInitMatrix(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	OBJ_RestoreInitialPos(pst_GO);

#ifdef ODE_INSIDE
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(pst_GO);
#endif

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJGetPosInit_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *v)
{
	MATH_CopyVector(v, OBJ_pst_GetInitialAbsolutePosition(_pst_GO));

}
/**/
AI_tdst_Node *AI_EvalFunc_OBJGetPosInit(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJGetPosInit_C(pst_GO, &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_Debug_Move(OBJ_tdst_GameObject *pst_GO, char *_pst_Context)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char	asz_Log[500];
	char	asz_Extra[200];
	BIG_KEY AI_File_Key;
	int		i;
	/*~~~~~~~~~~~~~~~~~~~*/

	if(TIM_gf_dt && (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_TraceMove) && (ENG_gb_AIRunning || ENG_gb_EVERunning))
	{
		if(ENG_gb_AIRunning)
		{
			for(i = 0; i < 10; i++)
			{
				AI_File_Key = *(LONG *) &(*((*AI_gpst_CurrentFunction).pst_OtherRootNode + i)).w_Param;
				if(AI_File_Key != BIG_C_InvalidKey) break;
			}
		}
		else
			AI_File_Key = BIG_C_InvalidKey;

		if(AI_File_Key != BIG_C_InvalidKey)
			sprintf(asz_Extra, "<%s>", BIG_FileName(BIG_ul_SearchKeyToFat(AI_File_Key)));
		else
			sprintf(asz_Extra, "<->");

		if(ENG_gb_AIRunning)
		{
			sprintf
			(
				asz_Log,
				"[Frame %u] IA of %s moves %s [%s]. %s",
				ENG_gul_Loop,
				AI_gpst_CurrentGameObject->sz_Name,
				pst_GO->sz_Name,
				_pst_Context,
				asz_Extra
			);
		}

		if(ENG_gb_EVERunning)
		{
			sprintf
			(
				asz_Log,
				"[Frame %u] %s moves %s [%s].",
				ENG_gul_Loop,
				EVE_gpst_OwnerGAO->sz_Name,
				pst_GO->sz_Name,
				_pst_Context
			);
		}

		LINK_PrintStatusMsg(asz_Log);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_Debug_ControlFlag(OBJ_tdst_GameObject *pst_GO, char *_pst_Context, ULONG _ul_OldFlags)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char	asz_Log[500];
	char	asz_Extra[200];
	BIG_KEY AI_File_Key;
	int		i;
	/*~~~~~~~~~~~~~~~~~~~*/

	if(TIM_gf_dt && (pst_GO->ul_EditorFlags & OBJ_C_EditFlags_TraceMove) && (ENG_gb_AIRunning || ENG_gb_EVERunning))
	{
		if(ENG_gb_AIRunning)
		{
			for(i = 0; i < 10; i++)
			{
				AI_File_Key = *(LONG *) &(*((*AI_gpst_CurrentFunction).pst_OtherRootNode + i)).w_Param;
				if(AI_File_Key != BIG_C_InvalidKey) break;
			}
		}
		else
			AI_File_Key = BIG_C_InvalidKey;

		if(AI_File_Key != BIG_C_InvalidKey)
			sprintf(asz_Extra, "<%s>", BIG_FileName(BIG_ul_SearchKeyToFat(AI_File_Key)));
		else
			sprintf(asz_Extra, "<->");

		if(ENG_gb_AIRunning)
		{
			if
			(
				(
					_ul_OldFlags &
						(
							OBJ_C_ControlFlag_ForceInvisible | OBJ_C_ControlFlag_ForceInactive |
								OBJ_C_ControlFlag_AlwaysActive | OBJ_C_ControlFlag_AlwaysVisible |
									OBJ_C_ControlFlag_RayInsensitive
						)
				) ==
					(
						pst_GO->ul_StatusAndControlFlags &
							(
								OBJ_C_ControlFlag_ForceInvisible | OBJ_C_ControlFlag_ForceInactive |
									OBJ_C_ControlFlag_AlwaysActive | OBJ_C_ControlFlag_AlwaysVisible |
										OBJ_C_ControlFlag_RayInsensitive
							)
					)
			) return;

			if
			(
				(_ul_OldFlags & OBJ_C_ControlFlag_RayInsensitive)
			&&	!(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_RayInsensitive)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] IA of %s resets the OBJ_C_ControlFlag_RayInsensitive Flag of %s (%s) %s",
					ENG_gul_Loop,
					AI_gpst_CurrentGameObject->sz_Name,
					pst_GO->sz_Name,
					_pst_Context,
					asz_Extra
				);
				LINK_PrintStatusMsg(asz_Log);
			}
			
			if
			(
				!(_ul_OldFlags & OBJ_C_ControlFlag_RayInsensitive)
			&&	(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_RayInsensitive)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] IA of %s sets the OBJ_C_ControlFlag_RayInsensitive Flag of %s (%s) %s",
					ENG_gul_Loop,
					AI_gpst_CurrentGameObject->sz_Name,
					pst_GO->sz_Name,
					_pst_Context,
					asz_Extra
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				(_ul_OldFlags & OBJ_C_ControlFlag_ForceInvisible)
			&&	!(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInvisible)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] IA of %s resets the OBJ_C_ControlFlag_ForceInvisible Flag of %s (%s) %s",
					ENG_gul_Loop,
					AI_gpst_CurrentGameObject->sz_Name,
					pst_GO->sz_Name,
					_pst_Context,
					asz_Extra
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				!(_ul_OldFlags & OBJ_C_ControlFlag_ForceInvisible)
			&&	(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInvisible)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] IA of %s sets the OBJ_C_ControlFlag_ForceInvisible Flag of %s (%s) %s",
					ENG_gul_Loop,
					AI_gpst_CurrentGameObject->sz_Name,
					pst_GO->sz_Name,
					_pst_Context,
					asz_Extra
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				(_ul_OldFlags & OBJ_C_ControlFlag_ForceInactive)
			&&	!(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInactive)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] IA of %s resets the OBJ_C_ControlFlag_ForceInactive Flag of %s (%s) %s",
					ENG_gul_Loop,
					AI_gpst_CurrentGameObject->sz_Name,
					pst_GO->sz_Name,
					_pst_Context,
					asz_Extra
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				!(_ul_OldFlags & OBJ_C_ControlFlag_ForceInactive)
			&&	(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInactive)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] IA of %s sets the OBJ_C_ControlFlag_ForceInactive Flag of %s (%s) %s",
					ENG_gul_Loop,
					AI_gpst_CurrentGameObject->sz_Name,
					pst_GO->sz_Name,
					_pst_Context,
					asz_Extra
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				(_ul_OldFlags & OBJ_C_ControlFlag_AlwaysActive)
			&&	!(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysActive)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] IA of %s resets the OBJ_C_ControlFlag_AlwaysActive Flag of %s (%s) %s",
					ENG_gul_Loop,
					AI_gpst_CurrentGameObject->sz_Name,
					pst_GO->sz_Name,
					_pst_Context,
					asz_Extra
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				!(_ul_OldFlags & OBJ_C_ControlFlag_AlwaysActive)
			&&	(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysActive)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] IA of %s sets the OBJ_C_ControlFlag_AlwaysActive Flag of %s (%s) %s",
					ENG_gul_Loop,
					AI_gpst_CurrentGameObject->sz_Name,
					pst_GO->sz_Name,
					_pst_Context,
					asz_Extra
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				(_ul_OldFlags & OBJ_C_ControlFlag_AlwaysVisible)
			&&	!(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysVisible)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] IA of %s resets the OBJ_C_ControlFlag_AlwaysVisible Flag of %s (%s) %s",
					ENG_gul_Loop,
					AI_gpst_CurrentGameObject->sz_Name,
					pst_GO->sz_Name,
					_pst_Context,
					asz_Extra
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				!(_ul_OldFlags & OBJ_C_ControlFlag_AlwaysVisible)
			&&	(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysVisible)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] IA of %s sets the OBJ_C_ControlFlag_AlwaysVisible Flag of %s (%s) %s",
					ENG_gul_Loop,
					AI_gpst_CurrentGameObject->sz_Name,
					pst_GO->sz_Name,
					_pst_Context,
					asz_Extra
				);
				LINK_PrintStatusMsg(asz_Log);
			}
		}

		if(ENG_gb_EVERunning)
		{
			if
			(
				(
					_ul_OldFlags &
						(
							OBJ_C_ControlFlag_ForceInvisible | OBJ_C_ControlFlag_ForceInactive |
								OBJ_C_ControlFlag_AlwaysActive | OBJ_C_ControlFlag_AlwaysVisible
						)
				) ==
					(
						pst_GO->ul_StatusAndControlFlags &
							(
								OBJ_C_ControlFlag_ForceInvisible | OBJ_C_ControlFlag_ForceInactive |
									OBJ_C_ControlFlag_AlwaysActive | OBJ_C_ControlFlag_AlwaysVisible
							)
					)
			) return;

			if
			(
				(_ul_OldFlags & OBJ_C_ControlFlag_ForceInvisible)
			&&	!(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInvisible)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] %s resets the OBJ_C_ControlFlag_ForceInvisible Flag of %s (%s)",
					ENG_gul_Loop,
					EVE_gpst_OwnerGAO->sz_Name,
					pst_GO->sz_Name,
					_pst_Context
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				!(_ul_OldFlags & OBJ_C_ControlFlag_ForceInvisible)
			&&	(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInvisible)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] %s sets the OBJ_C_ControlFlag_ForceInvisible Flag of %s (%s)",
					ENG_gul_Loop,
					EVE_gpst_OwnerGAO->sz_Name,
					pst_GO->sz_Name,
					_pst_Context
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				(_ul_OldFlags & OBJ_C_ControlFlag_ForceInactive)
			&&	!(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInactive)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] %s resets the OBJ_C_ControlFlag_ForceInactive Flag of %s (%s)",
					ENG_gul_Loop,
					EVE_gpst_OwnerGAO->sz_Name,
					pst_GO->sz_Name,
					_pst_Context
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				!(_ul_OldFlags & OBJ_C_ControlFlag_ForceInactive)
			&&	(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceInactive)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] %s sets the OBJ_C_ControlFlag_ForceInactive Flag of %s (%s)",
					ENG_gul_Loop,
					EVE_gpst_OwnerGAO->sz_Name,
					pst_GO->sz_Name,
					_pst_Context
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				(_ul_OldFlags & OBJ_C_ControlFlag_AlwaysActive)
			&&	!(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysActive)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] %s resets the OBJ_C_ControlFlag_AlwaysActive Flag of %s (%s)",
					ENG_gul_Loop,
					EVE_gpst_OwnerGAO->sz_Name,
					pst_GO->sz_Name,
					_pst_Context
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				!(_ul_OldFlags & OBJ_C_ControlFlag_AlwaysActive)
			&&	(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysActive)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] %s sets the OBJ_C_ControlFlag_AlwaysActive Flag of %s (%s)",
					ENG_gul_Loop,
					EVE_gpst_OwnerGAO->sz_Name,
					pst_GO->sz_Name,
					_pst_Context
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				(_ul_OldFlags & OBJ_C_ControlFlag_AlwaysVisible)
			&&	!(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysVisible)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] %s resets the OBJ_C_ControlFlag_AlwaysVisible Flag of %s (%s)",
					ENG_gul_Loop,
					EVE_gpst_OwnerGAO->sz_Name,
					pst_GO->sz_Name,
					_pst_Context
				);
				LINK_PrintStatusMsg(asz_Log);
			}

			if
			(
				!(_ul_OldFlags & OBJ_C_ControlFlag_AlwaysVisible)
			&&	(pst_GO->ul_StatusAndControlFlags & OBJ_C_ControlFlag_AlwaysVisible)
			)
			{
				sprintf
				(
					asz_Log,
					"[Frame %u] %s sets the OBJ_C_ControlFlag_AlwaysVisible Flag of %s (%s)",
					ENG_gul_Loop,
					EVE_gpst_OwnerGAO->sz_Name,
					pst_GO->sz_Name,
					_pst_Context
				);
				LINK_PrintStatusMsg(asz_Log);
			}
		}
	}
}

#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJSetPos_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Pos)
{
	/*~~~~~~~~~~~~~~~~*/
	BOOL	b_Hierarchy;
	/*~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	AI_Debug_Move(pst_GO, "OBJ_PosSet");
#endif
	b_Hierarchy = OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	if(b_Hierarchy) OBJ_ComputeGlobalWithLocal(pst_GO, pst_GO->pst_GlobalMatrix, TRUE);
	OBJ_SetAbsolutePosition(pst_GO, pst_Pos);
	if(b_Hierarchy) OBJ_ComputeLocalWhenHie(pst_GO);
	if( /* !ENG_gb_EVERunning */ 1 && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		ANI_UpdateFlash(pst_GO, ANI_C_UpdateAllAnims);

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Dyna))
	{
		pst_GO->pst_Base->pst_Dyna->ul_DynFlags |= DYN_C_OptimizeColDisable;
	}

#ifdef ODE_INSIDE
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(pst_GO);
#endif


#if defined(USE_DOUBLE_RENDERING)
#ifdef WIN32
	if (MAI_gst_MainHandles.pst_DisplayData->ul_DisplayInfo & (GDI_Cul_DI_DoubleRendering_I|GDI_Cul_DI_DoubleRendering_K)) 
#else
	if (gpst_GSP_stDD.ul_DisplayInfo & (GDI_Cul_DI_DoubleRendering_I|GDI_Cul_DI_DoubleRendering_K)) 
#endif
	{
		// This is called in case of display Hook
		extern void WOR_Compute_SetPos_In_Display_Hook(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Pos);
		WOR_Compute_SetPos_In_Display_Hook(pst_GO, pst_Pos);
	}
#endif	
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJSetPos_Teleport_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Pos)
{
	/*~~~~~~~~~~~~~~~~*/
	BOOL	b_Hierarchy;
	/*~~~~~~~~~~~~~~~~*/

#ifdef ACTIVE_EDITORS
	AI_Debug_Move(pst_GO, "OBJ_PosSet_Teleport");
#endif
	b_Hierarchy = OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	if(b_Hierarchy) OBJ_ComputeGlobalWithLocal(pst_GO, pst_GO->pst_GlobalMatrix, TRUE);
	OBJ_SetAbsolutePosition(pst_GO, pst_Pos);
	if(b_Hierarchy) OBJ_ComputeLocalWhenHie(pst_GO);
#if defined(USE_DOUBLE_RENDERING)
	{
		extern void WOR_Compute_SetPos_Teleport(OBJ_tdst_GameObject *pst_GO);
		WOR_Compute_SetPos_Teleport(pst_GO);
	}
#endif
	if( /* !ENG_gb_EVERunning */ 1 && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		ANI_UpdateFlash(pst_GO, ANI_C_UpdateAllAnims);

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Dyna))
	{
		pst_GO->pst_Base->pst_Dyna->ul_DynFlags |= DYN_C_OptimizeColDisable;
	}

#ifdef ODE_INSIDE
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(pst_GO);
#endif

}
/**/
void AI_EvalFunc_OBJSetPos_C_CURRENT(MATH_tdst_Vector *pst_Pos)
{
	AI_EvalFunc_OBJSetPos_C(AI_gpst_CurrentGameObject, pst_Pos);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJSetPos(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJSetPos_C(pst_GO, AI_PopVectorPtr());
	return ++_pst_Node;
}

/**/
AI_tdst_Node *AI_EvalFunc_OBJSetPos_Teleport(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJSetPos_Teleport_C(pst_GO, AI_PopVectorPtr());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJSetLocalPos_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Pos)
{

	OBJ_SetLocalPosition(pst_GO, pst_Pos);
	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix)) ANI_UpdateFlash(pst_GO, ANI_C_UpdateAllAnims);
	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Dyna))
		pst_GO->pst_Base->pst_Dyna->ul_DynFlags |= DYN_C_OptimizeColDisable;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJSetLocalPos(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJSetLocalPos_C(pst_GO, AI_PopVectorPtr());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_Rejoin_C(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_Dest)
{
	/*~~~~~~~~~~~~~~~~*/
	BOOL	b_Hierarchy;
	/*~~~~~~~~~~~~~~~~*/


	if(!_pst_Dest) return;
	b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);

	if(b_Hierarchy) OBJ_ComputeGlobalWhenHie(_pst_Dest);
	OBJ_SetAbsolutePosition(_pst_GO, OBJ_pst_GetAbsolutePosition(_pst_Dest));
	if(b_Hierarchy) OBJ_ComputeLocalWhenHie(_pst_GO);
	if( /* !ENG_gb_EVERunning */ 1 && OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		ANI_UpdateFlash(_pst_GO, ANI_C_UpdateAllAnims);

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif


#ifdef ACTIVE_EDITORS
	AI_Debug_Move(_pst_GO, "OBJ_Rejoin");
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_Rejoin(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_Rejoin_C(pst_GO, AI_PopGameObject());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_RejoinAndOrient_C(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_Dest)
{
	/*~~~~~~~~~~~~~~~~*/
	BOOL	b_Hierarchy;
	/*~~~~~~~~~~~~~~~~*/


	if(!_pst_Dest) return;
	b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	if(b_Hierarchy) OBJ_ComputeGlobalWhenHie(_pst_Dest);
	OBJ_SetAbsoluteMatrix(_pst_GO, OBJ_pst_GetAbsoluteMatrix(_pst_Dest));
	if(b_Hierarchy) OBJ_ComputeLocalWhenHie(_pst_GO);
	if( /* !ENG_gb_EVERunning */ 1 && OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		ANI_UpdateFlash(_pst_GO, ANI_C_UpdateAllAnims);

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Dyna))
	{
		_pst_GO->pst_Base->pst_Dyna->ul_DynFlags |= DYN_C_OptimizeColDisable;
	}

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif


#ifdef ACTIVE_EDITORS
	AI_Debug_Move(_pst_GO, "OBJ_RejoinAndOrient");
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_RejoinAndOrient(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_RejoinAndOrient_C(pst_GO, AI_PopGameObject());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJReinit_C(OBJ_tdst_GameObject *_pst_GO, int _i_Type)
{
	int				secto;
	WOR_tdst_World	*pst_World;

	secto = _pst_GO->uc_Secto;
	ENG_ReinitOneObject(_pst_GO, _i_Type);

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

	if(_i_Type == 1000) _pst_GO->uc_Secto = secto;

	/* Add object in SnP if needed */
	INT_SnP_AttachObject(_pst_GO, WOR_gpst_CurrentWorld);

	/* Delete object from current world */
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	if(pst_World)
	{
		pst_World->b_ForceActivationRefresh = TRUE;
		pst_World->b_ForceVisibilityRefresh = TRUE;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJReinit(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJReinit_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_ExtraFlagsSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_On, ULONG _ul_Off)
{
	/*~~~~~~~~~~~~~~~~~~*/
	USHORT	uw_ExtraFlags;
	/*~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO->pst_Extended) return;

	uw_ExtraFlags = _pst_GO->pst_Extended->uw_ExtraFlags;
	uw_ExtraFlags = (uw_ExtraFlags & (~((USHORT) _ul_Off))) | ((USHORT) _ul_On);
	_pst_GO->pst_Extended->uw_ExtraFlags = uw_ExtraFlags;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_ExtraFlagsSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Off;
	ULONG				ul_On;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	ul_Off = AI_PopInt();
	ul_On = AI_PopInt();

	AI_EvalFunc_OBJ_ExtraFlagsSet_C(pst_GO, ul_On, ul_Off);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_OBJ_SqrDist_C(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_GO1)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Vec;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Vec, OBJ_pst_GetAbsolutePosition(_pst_GO1), OBJ_pst_GetAbsolutePosition(_pst_GO));
	return(MATH_f_SqrNormVector(&st_Vec));
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_SqrDist(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_GO1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_GO1 = AI_PopGameObject();
	AI_Check(pst_GO1, "Game object parameter is invalid");
	AI_PushFloat(AI_EvalFunc_OBJ_SqrDist_C(pst_GO, pst_GO1));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_OBJ_SqrDistHorz_C(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_GO1)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Vec;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Vec, OBJ_pst_GetAbsolutePosition(_pst_GO1), OBJ_pst_GetAbsolutePosition(_pst_GO));
	st_Vec.z = 0.0f;
	return(MATH_f_SqrNormVector(&st_Vec));
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_SqrDistHorz(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_GO1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_GO1 = AI_PopGameObject();
	AI_Check(pst_GO1, "Game object parameter is invalid");
	AI_PushFloat(AI_EvalFunc_OBJ_SqrDistHorz_C(pst_GO, pst_GO1));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Change the control flags of an object

    In:     bits to set as 1st parameter, ans the bits to clear as second parameter 

    Note:   Example of use: (this example clears the OBJ_C_ControlFlag_ForceInactive flag and sets the
            OBJ_C_ControlFlag_ForceInvisible flag) £
            OBJ_FlagsControlSet(OBJ_C_ControlFlag_ForceInvisible,OBJ_C_ControlFlag_ForceInactive) See the file
            objconst.h to know the control flags. £
 =======================================================================================================================
 */
ULONG AI_EvalFunc_OBJ_FlagsControlSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_On, ULONG _ul_Off)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	ULONG			ul_ControlFlags;
#ifdef ACTIVE_EDITORS
	ULONG			ul_OldFlags;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_ControlFlags = OBJ_ul_FlagsControlGet(_pst_GO);

#ifdef ACTIVE_EDITORS
	ul_OldFlags = ul_ControlFlags;
#endif
	ul_ControlFlags = (ul_ControlFlags & (~_ul_Off)) | _ul_On;
	OBJ_FlagsControlSet(_pst_GO, ul_ControlFlags);

	if(_ul_On & OBJ_C_ControlFlag_ForceInactive)
	{
		M_StartRaster();
		SND_RegisterInactiveGAO(_pst_GO);
		M_StopRaster();
	}

#ifdef ACTIVE_EDITORS
	AI_Debug_ControlFlag(_pst_GO, "OBJ_ControlFlagSet", ul_OldFlags);
#endif
	/* Force reconstruct EOT ? */
	if((_ul_On & OBJ_C_ControlFlag_ForceInactive) || (_ul_Off & OBJ_C_ControlFlag_ForceInactive))
	{
		pst_World = WOR_World_GetWorldOfObject(_pst_GO);
		pst_World->b_ForceActivationRefresh = TRUE;
		pst_World->b_ForceVisibilityRefresh = TRUE;
		pst_World->b_ForceBVRefresh = TRUE;
	}

	if((_ul_On & OBJ_C_ControlFlag_ForceInvisible) || (_ul_Off & OBJ_C_ControlFlag_ForceInvisible))
	{
		pst_World = WOR_World_GetWorldOfObject(_pst_GO);
		pst_World->b_ForceVisibilityRefresh = TRUE;
	}

	if(_ul_On & OBJ_C_ControlFlag_AlwaysVisible)
	{
		_pst_GO->ul_StatusAndControlFlags |= OBJ_C_StatusFlag_Visible;
		_pst_GO->ul_StatusAndControlFlags &= ~OBJ_C_StatusFlag_Culled;
	}


	/* We return the control flags */
	return(ul_ControlFlags & 0xffff0000);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsControlSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Off;
	ULONG				ul_On;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Off = AI_PopInt();
	ul_On = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_OBJ_FlagsControlSet_C(pst_GO, ul_On, ul_Off));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Read the control flags of an object

    Note:   See the file objconst.h to know the control flags
 =======================================================================================================================
 */
ULONG AI_EvalFunc_OBJ_FlagsControlGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_ControlFlags;
	/*~~~~~~~~~~~~~~~~~~~~*/

	ul_ControlFlags = 0;
	if(_pst_GO) ul_ControlFlags = OBJ_ul_FlagsControlGet(_pst_GO);
	return ul_ControlFlags;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsControlGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_FlagsControlGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Change force invisible control flag of an object

    In:     value of invisible flag to set (0 or not 0)
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_FlagInvisibleSet_C(OBJ_tdst_GameObject *_pst_GO, int _i_Invisible)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	ULONG			ul_ControlFlags;
#ifdef ACTIVE_EDITORS
	ULONG			ul_OldFlags;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_ControlFlags = OBJ_ul_FlagsControlGet(_pst_GO);
#ifdef ACTIVE_EDITORS
	ul_OldFlags = ul_ControlFlags;
#endif
	if(_i_Invisible)
	{
		if(ul_ControlFlags & OBJ_C_ControlFlag_ForceInvisible) return;
		ul_ControlFlags |= OBJ_C_ControlFlag_ForceInvisible;
	}
	else
	{
		if(!(ul_ControlFlags & OBJ_C_ControlFlag_ForceInvisible)) return;
		ul_ControlFlags &= ~OBJ_C_ControlFlag_ForceInvisible;
	}

	OBJ_FlagsControlSet(_pst_GO, ul_ControlFlags);

#ifdef ACTIVE_EDITORS
	AI_Debug_ControlFlag(_pst_GO, "OBJ_FlagInvisibleSet", ul_OldFlags);
#endif
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	pst_World->b_ForceVisibilityRefresh = TRUE;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagInvisibleSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_FlagInvisibleSet_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Change force invisible control flag of an object

    In:     value of invisible flag to set (0 or not 0)
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_FlagInactiveSet_C(OBJ_tdst_GameObject *_pst_GO, int _i_Inactive)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	ULONG			ul_ControlFlags;
#ifdef ACTIVE_EDITORS
	ULONG			ul_OldFlags;
#endif
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_ControlFlags = OBJ_ul_FlagsControlGet(_pst_GO);
#ifdef ACTIVE_EDITORS
	ul_OldFlags = ul_ControlFlags;
#endif
	if(_i_Inactive)
	{
		if(ul_ControlFlags & OBJ_C_ControlFlag_ForceInactive) return;
		ul_ControlFlags |= OBJ_C_ControlFlag_ForceInactive;
	}
	else
	{
		if(!(ul_ControlFlags & OBJ_C_ControlFlag_ForceInactive)) return;
		ul_ControlFlags &= ~OBJ_C_ControlFlag_ForceInactive;
	}

	OBJ_FlagsControlSet(_pst_GO, ul_ControlFlags);

#ifdef ACTIVE_EDITORS
	AI_Debug_ControlFlag(_pst_GO, "OBJ_FlagInactiveSet", ul_OldFlags);
#endif
	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	pst_World->b_ForceActivationRefresh = TRUE;
	pst_World->b_ForceVisibilityRefresh = TRUE;
	pst_World->b_ForceBVRefresh = TRUE;

	if(_i_Inactive) OBJ_SetInactive(_pst_GO);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagInactiveSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_FlagInactiveSet_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Read the status flags of an object

    Note:   See the file objconst.h to know the status flags
 =======================================================================================================================
 */
ULONG AI_EvalFunc_OBJ_FlagsStatusGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	ULONG	ul_StatusFlags;
	/*~~~~~~~~~~~~~~~~~~~*/

	ul_StatusFlags = 0;
	if(_pst_GO) ul_StatusFlags = OBJ_ul_FlagsStatusGet(_pst_GO);
	return(ul_StatusFlags);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsStatusGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_FlagsStatusGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_CustomBitsSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Add, ULONG _ul_Remove)
{
	_ul_Add &= 0x0000FF00;
	_ul_Remove &= 0x0000FF00;
	_pst_GO->ul_StatusAndControlFlags &= ~_ul_Remove;
	_pst_GO->ul_StatusAndControlFlags |= _ul_Add;
}
/**/
void AI_EvalFunc_OBJ_CustomBitsSet_C_CURRENT(ULONG _ul_Add, ULONG _ul_Remove)
{
	_ul_Add &= 0x0000FF00;
	_ul_Remove &= 0x0000FF00;
	AI_gpst_CurrentGameObject->ul_StatusAndControlFlags &= ~_ul_Remove;
	AI_gpst_CurrentGameObject->ul_StatusAndControlFlags |= _ul_Add;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_CustomBitsSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Add, ul_Remove;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	ul_Remove = AI_PopInt();
	ul_Add = AI_PopInt();
	AI_EvalFunc_OBJ_CustomBitsSet_C(pst_GO, ul_Add, ul_Remove);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_CustomBitsGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	return(_pst_GO->ul_StatusAndControlFlags & 0x0000FF00);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_CustomBitsGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_CustomBitsGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_CustomBitsTest_C(OBJ_tdst_GameObject *_pst_GO, int _i_Test)
{
	return(((_pst_GO->ul_StatusAndControlFlags & 0x0000FF00) & (_i_Test & 0x0000FF00)) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_CustomBitsTest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	if(AI_EvalFunc_OBJ_CustomBitsTest_C(pst_GO, AI_PopInt()))
		AI_PushInt(1);
	else
		AI_PushInt(0);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_CustomBitsNot_C(OBJ_tdst_GameObject *_pst_GO, int _i_Test)
{
	_i_Test &= 0x0000FF00;
	_pst_GO->ul_StatusAndControlFlags ^= _i_Test;
	return _pst_GO->ul_StatusAndControlFlags;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_CustomBitsNot(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_CustomBitsNot_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_CapaSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG _ul_Add, ULONG _ul_Remove)
{
	AI_Check(_pst_GO->pst_Extended, "Object has no capacities (extended not allocated)");
	_pst_GO->pst_Extended->uw_Capacities &= ~_ul_Remove;
	_pst_GO->pst_Extended->uw_Capacities |= _ul_Add;
}
/**/
void AI_EvalFunc_OBJ_CapaSet_C_CURRENT(ULONG _ul_Add, ULONG _ul_Remove)
{
	AI_gpst_CurrentGameObject->pst_Extended->uw_Capacities &= ~_ul_Remove;
	AI_gpst_CurrentGameObject->pst_Extended->uw_Capacities |= _ul_Add;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_CapaSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Add, ul_Remove;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	ul_Remove = AI_PopInt();
	ul_Add = AI_PopInt();
	AI_EvalFunc_OBJ_CapaSet_C(pst_GO, ul_Add, ul_Remove);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_OBJ_CapaGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	AI_Check(_pst_GO->pst_Extended, "Object has no capacities (extended not allocated)");
	return _pst_GO->pst_Extended->uw_Capacities;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_CapaGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_CapaGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_OBJ_CapaTest_C(OBJ_tdst_GameObject *_pst_GO, ULONG _i_Test)
{
	if(!_pst_GO->pst_Extended) return 0;
	return((_pst_GO->pst_Extended->uw_Capacities & _i_Test) ? 1 : 0);
}
/**/
#ifdef JADEFUSION
int AI_EvalFunc_OBJ_CapaTest_C_CURRENT(ULONG _i_Test)
#else
ULONG AI_EvalFunc_OBJ_CapaTest_C_CURRENT(ULONG _i_Test)
#endif
{
	if(!AI_gpst_CurrentGameObject->pst_Extended) return 0;
	return((AI_gpst_CurrentGameObject->pst_Extended->uw_Capacities & _i_Test) ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_CapaTest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	if(!pst_GO->pst_Extended)
	{
		AI_PushInt(0);
		return ++_pst_Node;
	}

	if(AI_EvalFunc_OBJ_CapaTest_C(pst_GO, AI_PopInt()))
		AI_PushInt(1);
	else
		AI_PushInt(0);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_OBJ_CapaNot_C(OBJ_tdst_GameObject *_pst_GO, ULONG _i_Test)
{
	//_i_Test &= 0x0000FFFF;
	AI_Check(_pst_GO->pst_Extended, "Object has no capacities (extended not allocated)");
	_pst_GO->pst_Extended->uw_Capacities ^= _i_Test;
	return _pst_GO->pst_Extended->uw_Capacities;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_CapaNot(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO->pst_Extended, "Object has no capacities (extended not allocated)");
	AI_PushInt(AI_EvalFunc_OBJ_CapaNot_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Changes the sighting of a object, using a given wanted vector for the banking

    Note:   The sighting is normalized and imposed. The current Z axis of the object is used to keep the object as near
            as possible from its current orientation
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_SightGeneralSet_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*st_Sighting,
	MATH_tdst_Vector	*st_Banking
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Hierarchy = OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	if(b_Hierarchy)
	{
		MATH_CopyVector(&st_SaveT, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
		OBJ_ComputeGlobalWithLocal(pst_GO, pst_GO->pst_GlobalMatrix, 1);
	}

	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Sighting, Cf_Epsilon), "Sight is null");
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Banking, Cf_Epsilon), "Banking is null");
	OBJ_SightGeneralSet(pst_GO, st_Sighting, st_Banking);
	if(b_Hierarchy)
	{
		OBJ_ComputeLocalWhenHie(pst_GO);
		MATH_CopyVector(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
	}

	if( /* !ENG_gb_EVERunning */ 1 && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		ANI_UpdateFlash(pst_GO, ANI_C_UpdateAllAnims);

#ifdef ODE_INSIDE
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(pst_GO);
#endif

}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_SightGeneralSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Sighting;
	MATH_tdst_Vector	st_Banking;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Banking);
	AI_PopVector(&st_Sighting);
	AI_EvalFunc_OBJ_SightGeneralSet_C(pst_GO, &st_Sighting, &st_Banking);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Changes the sight of a object

    Note:   The sight is normalized and imposed. The current Z axis of the object is used to keep the object as
            vertical as possible
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_SightSet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Sight)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Hierarchy = OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	if(b_Hierarchy)
	{
		MATH_CopyVector(&st_SaveT, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
		OBJ_ComputeGlobalWithLocal(pst_GO, pst_GO->pst_GlobalMatrix, 1);
	}

	OBJ_SightSet(pst_GO, pst_Sight);
	if(b_Hierarchy)
	{
		OBJ_ComputeLocalWhenHie(pst_GO);
		MATH_CopyVector(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
	}

	if( /* !ENG_gb_EVERunning */ 1 && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		ANI_UpdateFlash(pst_GO, ANI_C_UpdateAllAnims);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_SightSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Sight;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Sight);
	AI_EvalFunc_OBJ_SightSet_C(pst_GO, &st_Sight);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_BoneDisplayOrder_C(OBJ_tdst_GameObject *pst_GO, int bone, int val)
{
	if(bone != -1) pst_GO = ANI_pst_GetObjectByAICanal(pst_GO, (UCHAR) bone);
	if(pst_GO && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu) && pst_GO->pst_Base && pst_GO->pst_Base->pst_Visu)
		pst_GO->pst_Base->pst_Visu->c_DisplayOrder = val;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_BoneDisplayOrder(AI_tdst_Node *_pst_Node)
{
	OBJ_tdst_GameObject *pst_GO;
	int					bone, val;

	AI_M_GetCurrentObject(pst_GO);
	val = AI_PopInt();
	bone = AI_PopInt();
	AI_EvalFunc_OBJ_BoneDisplayOrder_C(pst_GO, bone, val);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
    Aim:    Changes the sighting of a object, using a given wanted vector for the banking

    Note:   The sighting is normalized and imposed. The current Z axis of the object is used to keep the object as near
            as possible from its current orientation
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_LocalSightSet_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*st_Sighting,
	MATH_tdst_Vector	*st_Banking
)
{
	if(!OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy)) return;

	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Sighting, Cf_Epsilon), "Sight is null");
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Banking, Cf_Epsilon), "Banking is null");

	OBJ_SightGeneralSet(pst_GO, st_Sighting, st_Banking);
	MATH_Copy33Matrix(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, pst_GO->pst_GlobalMatrix);
	OBJ_ComputeGlobalWithLocal(pst_GO, pst_GO->pst_GlobalMatrix, 1);

	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix)) ANI_UpdateFlash(pst_GO, ANI_C_UpdateAllAnims);

#ifdef ODE_INSIDE
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(pst_GO);
#endif

}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_LocalSightSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Sighting;
	MATH_tdst_Vector	st_Banking;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Banking);
	AI_PopVector(&st_Sighting);
	AI_EvalFunc_OBJ_LocalSightSet_C(pst_GO, &st_Sighting, &st_Banking);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_OrientTo_C(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_GO1)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Pos;
	MATH_tdst_Vector	st_Banking;
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_pst_GO1)
	{
		b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);
		if(b_Hierarchy)
		{
			MATH_CopyVector(&st_SaveT, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
			OBJ_ComputeGlobalWhenHie(_pst_GO);
		}

		if(OBJ_b_TestIdentityFlag(_pst_GO1, OBJ_C_IdentityFlag_Hierarchy))
		{
			OBJ_ComputeGlobalWhenHie(_pst_GO1);
		}

		MATH_CopyVector(&st_Pos, OBJ_pst_GetAbsolutePosition(_pst_GO1));
		MATH_SubVector(&st_Pos, &st_Pos, OBJ_pst_GetAbsolutePosition(_pst_GO));

		st_Banking.x = 0.0f;
		st_Banking.y = 0.0f;
		st_Banking.z = 1.0f;
		OBJ_SightGeneralSet(_pst_GO, &st_Pos, &st_Banking);
		if(b_Hierarchy)
		{
			OBJ_ComputeLocalWhenHie(_pst_GO);
			MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
		}

		if( /* !ENG_gb_EVERunning */ 1 && OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
			ANI_UpdateFlash(_pst_GO, ANI_C_UpdateAllAnims);

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_OrientTo(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_GO1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_GO1 = AI_PopGameObject();
	AI_EvalFunc_OBJ_OrientTo_C(pst_GO, pst_GO1);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the sight of an object
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_SightGet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Sight)
{
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	st_Temp;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		OBJ_ComputeGlobalWithLocal(pst_GO, &st_Temp, 1);
		MATH_NegVector(pst_Sight, MATH_pst_GetYAxis(&st_Temp));
	}
	else
	{
		MATH_NegVector(pst_Sight, MATH_pst_GetYAxis(pst_GO->pst_GlobalMatrix));
	}
}
/**/
void AI_EvalFunc_OBJ_SightGet_C_CURRENT(MATH_tdst_Vector *v)
{
	AI_EvalFunc_OBJ_SightGet_C(AI_gpst_CurrentGameObject, v);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_SightGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Sight;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_SightGet_C(pst_GO, &st_Sight);
	AI_PushVector(&st_Sight);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the sight of an object
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_SightInitGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Sight)
{
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy) OBJ_ComputeGlobalWhenHie(_pst_GO);
	OBJ_SightInitGet(_pst_GO, _pst_Sight);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_SightInitGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Sight;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_SightInitGet_C(pst_GO, &st_Sight);
	AI_PushVector(&st_Sight);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Changes the banking of a object, using a given wanted vector for the sighting

    Note:   No need of normalizing any vector before calling the function
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_BankingGeneralSet_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*st_Sighting,
	MATH_tdst_Vector	*st_Banking
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Hierarchy = OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	if(b_Hierarchy)
	{
		MATH_CopyVector(&st_SaveT, &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
		OBJ_ComputeGlobalWithLocal(pst_GO, pst_GO->pst_GlobalMatrix, 1);
	}

	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Sighting, Cf_Epsilon), "Sight is null");
	AI_Check(!MATH_b_NulVectorWithEpsilon(st_Banking, Cf_Epsilon), "Banking is null");
	OBJ_BankingGeneralSet(pst_GO, st_Sighting, st_Banking);
	if(b_Hierarchy)
	{
		OBJ_ComputeLocalWhenHie(pst_GO);
		MATH_CopyVector(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
	}

	if( /* !ENG_gb_EVERunning */ 1 && OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		ANI_UpdateFlash(pst_GO, ANI_C_UpdateAllAnims);

#ifdef ODE_INSIDE
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(pst_GO);
#endif

}
/**/
void AI_EvalFunc_OBJ_BankingGeneralSet_C_CURRENT(MATH_tdst_Vector *st_Sighting, MATH_tdst_Vector *st_Banking)
{
	AI_EvalFunc_OBJ_BankingGeneralSet_C(AI_gpst_CurrentGameObject, st_Sighting, st_Banking);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_BankingGeneralSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Sighting;
	MATH_tdst_Vector	st_Banking;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Banking);
	AI_PopVector(&st_Sighting);
	AI_EvalFunc_OBJ_BankingGeneralSet_C(pst_GO, &st_Sighting, &st_Banking);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Changes the banking of a object using the current sight

    Note:   No need to normalize the banking vector
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_BankingSet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Banking)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Hierarchy;
	MATH_tdst_Vector	st_SaveT;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	if(b_Hierarchy)
	{
		MATH_CopyVector(&st_SaveT, &_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
		OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
	}

	OBJ_BankingSet(_pst_GO, _pst_Banking);
	if(b_Hierarchy)
	{
		OBJ_ComputeLocalWhenHie(_pst_GO);
		MATH_CopyVector(&_pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T, &st_SaveT);
	}

	if( /* !ENG_gb_EVERunning */ 1 && OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_FlashMatrix))
		ANI_UpdateFlash(_pst_GO, ANI_C_UpdateAllAnims);

#ifdef ODE_INSIDE
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)
		ODE_UpdateMatrix(_pst_GO);
#endif

}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_BankingSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Banking;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	if ( pst_GO != NULL)
	{
		AI_PopVector( &st_Banking );
		AI_EvalFunc_OBJ_BankingSet_C( pst_GO, &st_Banking );
	}
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the banking of an object
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_BankingGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Banking)
{
	if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	st_Temp;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		OBJ_ComputeGlobalWithLocal(_pst_GO, &st_Temp, 1);
		MATH_CopyVector(_pst_Banking, MATH_pst_GetZAxis(&st_Temp));
	}
	else
	{
		MATH_CopyVector(_pst_Banking, MATH_pst_GetZAxis(_pst_GO->pst_GlobalMatrix));
	}
}
/**/
void AI_EvalFunc_OBJ_BankingGet_C_CURRENT(MATH_tdst_Vector *v)
{
	AI_EvalFunc_OBJ_BankingGet_C(AI_gpst_CurrentGameObject, v);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_BankingGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Banking;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	if (pst_GO != NULL )
	{
		AI_EvalFunc_OBJ_BankingGet_C( pst_GO, &st_Banking );
		AI_PushVector( &st_Banking );
	}
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the horizon of an object
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_HorizonGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Horizon)
{
	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
		OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);
	OBJ_HorizonGet(_pst_GO, _pst_Horizon);
}
/**/
void AI_EvalFunc_OBJ_HorizonGet_C_CURRENT(MATH_tdst_Vector *_pst_Horizon)
{
	if(OBJ_b_TestIdentityFlag(AI_gpst_CurrentGameObject, OBJ_C_IdentityFlag_Hierarchy))
		OBJ_ComputeGlobalWithLocal(AI_gpst_CurrentGameObject, AI_gpst_CurrentGameObject->pst_GlobalMatrix, 1);
	OBJ_HorizonGet(AI_gpst_CurrentGameObject, _pst_Horizon);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_HorizonGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Horizon;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	if ( pst_GO != NULL )
	{
		AI_EvalFunc_OBJ_HorizonGet_C( pst_GO, &st_Horizon );
		AI_PushVector( &st_Horizon );
	}
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_ScaleSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Scale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector(&st_Scale);
	AI_M_GetCurrentObject(pst_GO);
	OBJ_ScaleSet(pst_GO, &st_Scale);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_ScaleGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Scale)
{
	MATH_GetScale(_pst_Scale, _pst_GO->pst_GlobalMatrix);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_ScaleGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Scale;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	if ( pst_GO != NULL )
	{
		AI_EvalFunc_OBJ_ScaleGet_C( pst_GO, &st_Scale );
		AI_PushVector( &st_Scale );
	}

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_ZoomSet_C(OBJ_tdst_GameObject *_pst_GO, float _f_Zoom)
{
	/*~~~~~~~~~~~~~~~~*/
	BOOL	b_Hierarchy;
	/*~~~~~~~~~~~~~~~~*/

	b_Hierarchy = OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy);
	if(b_Hierarchy) OBJ_ComputeGlobalWithLocal(_pst_GO, _pst_GO->pst_GlobalMatrix, 1);

	OBJ_ZoomSet(_pst_GO, _f_Zoom);

	if(b_Hierarchy) OBJ_ComputeLocalWhenHie(_pst_GO);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_ZoomSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Zoom;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Zoom = AI_PopFloat();
	AI_EvalFunc_OBJ_ZoomSet_C(pst_GO, f_Zoom);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_OBJ_ZoomGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Scale;
	float				f_Zoom;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_GetScale(&st_Scale, _pst_GO->pst_GlobalMatrix);
	f_Zoom = fMax3(st_Scale.x, st_Scale.y, st_Scale.z);
	return(f_Zoom);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_ZoomGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_OBJ_ZoomGet_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_FlagsIdentityGet_C(OBJ_tdst_GameObject *pst_GO)
{
	return OBJ_ul_FlagsIdentityGet(pst_GO);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsIdentityGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_FlagsIdentityGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_FlagsIdentitySet_C(OBJ_tdst_GameObject *pst_GO, ULONG ul_Add, ULONG ul_Rem)
{
	/*~~~~~~~~~~~~~*/
	ULONG	ul_Flags;
	/*~~~~~~~~~~~~~*/

	ul_Flags = OBJ_ul_FlagsIdentityGet(pst_GO);
	ul_Flags &= ~ul_Rem;
	ul_Flags |= ul_Add;
	OBJ_ChangeIdentityFlags(pst_GO, ul_Flags, OBJ_ul_FlagsIdentityGet(pst_GO));
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsIdentitySet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Rem, ul_Add;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	ul_Rem = AI_PopInt();
	ul_Add = AI_PopInt();
	AI_EvalFunc_OBJ_FlagsIdentitySet_C(pst_GO, ul_Add, ul_Rem);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_FlagsIdentityTest_C(OBJ_tdst_GameObject *pst_GO, int test)
{
	return(OBJ_ul_FlagsIdentityGet(pst_GO) & test ? 1 : 0);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsIdentityTest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_FlagsIdentityTest_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_FlagsDesignGet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	UCHAR	uc_DesignFlags;
	/*~~~~~~~~~~~~~~~~~~~*/

	uc_DesignFlags = 0;
	if(pst_GO) uc_DesignFlags = OBJ_uc_FlagsDesignGet(pst_GO);
	return uc_DesignFlags;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsDesignGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_FlagsDesignGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_FlagsDesignSet_C(OBJ_tdst_GameObject *pst_GO, UCHAR uc_DesignFlagsOn, UCHAR uc_DesignFlagsOff)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	UCHAR	uc_DesignFlags;
	/*~~~~~~~~~~~~~~~~~~~*/

	if(pst_GO)
	{
		uc_DesignFlags = OBJ_uc_FlagsDesignGet(pst_GO);
		uc_DesignFlags = (uc_DesignFlags & (~uc_DesignFlagsOff)) | uc_DesignFlagsOn;
		OBJ_FlagsDesignSet(pst_GO, uc_DesignFlags);
		return uc_DesignFlags;
	}

	return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsDesignSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	UCHAR				uc_DesignFlagsOff;
	UCHAR				uc_DesignFlagsOn;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	uc_DesignFlagsOff = AI_PopInt();
	uc_DesignFlagsOn = AI_PopInt();
	AI_PushInt(AI_EvalFunc_OBJ_FlagsDesignSet_C(pst_GO, uc_DesignFlagsOn, uc_DesignFlagsOff));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_FlagsFixSet_C(OBJ_tdst_GameObject *pst_GO, int uc_FlagsOn, int uc_FlagsOff)
{
	pst_GO->c_FixFlags &= ~uc_FlagsOff;
	pst_GO->c_FixFlags |= uc_FlagsOn;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsFixSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					uc_FlagsOn, uc_FlagsOff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	uc_FlagsOff = AI_PopInt();
	uc_FlagsOn = AI_PopInt();
	AI_EvalFunc_OBJ_FlagsFixSet_C(pst_GO, uc_FlagsOn, uc_FlagsOff);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_FlagsFixGet_C(OBJ_tdst_GameObject *pst_GO)
{
	return pst_GO->c_FixFlags;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsFixGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(pst_GO->c_FixFlags);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_FlagsFixSetInit(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	UCHAR				uc_Flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	uc_Flags = AI_PopInt();
	pst_GO->c_FixFlags |= (uc_Flags & 0xc);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    morphing
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_MorphCopy_C(OBJ_tdst_GameObject *_pst_ObjDst, OBJ_tdst_GameObject *_pst_ObjSrc)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing			*pst_Dst, *pst_Src;
	GEO_tdst_ModifierMorphing_Channel	*pst_CDst, *pst_CSrc;
	int									channel, nbdata, data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dst = GEO_pst_ModifierMorphing_Get(_pst_ObjDst);
	if(!pst_Dst) return;
	pst_Src = GEO_pst_ModifierMorphing_Get(_pst_ObjSrc);
	if(!pst_Src) return;

	channel = (pst_Dst->l_NbChannel > pst_Src->l_NbChannel) ? pst_Src->l_NbChannel : pst_Dst->l_NbChannel;
	pst_CDst = pst_Dst->dst_MorphChannel;
	pst_CSrc = pst_Src->dst_MorphChannel;

	while(channel--)
	{
		pst_CDst->f_Blend = pst_CSrc->f_Blend;
		pst_CDst->f_ChannelBlend = pst_CSrc->f_ChannelBlend;
		nbdata = (pst_CDst->l_NbData < pst_CSrc->l_NbData) ? pst_CDst->l_NbData : pst_CSrc->l_NbData;
		for(data = 0; data < nbdata; data++)
		{
			pst_CDst->dl_DataIndex[data] = pst_CSrc->dl_DataIndex[data];
			if(pst_CDst->dl_DataIndex[data] >= pst_Dst->l_NbMorphData) pst_CDst->dl_DataIndex[data] = -1;
		}

		pst_CDst++;
		pst_CSrc++;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_MorphCopy(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_MorphCopy_C(pst_GO, AI_PopGameObject());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_NumberOfData_C(OBJ_tdst_GameObject *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing	*pst_Morph;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Morph = GEO_pst_ModifierMorphing_Get(_pst_Obj);
	if(!pst_Morph) return 0;
	return pst_Morph->l_NbMorphData;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_NumberOfData(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_NumberOfData_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_NumberOfChannel_C(OBJ_tdst_GameObject *_pst_Obj)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing	*pst_Morph;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Morph = GEO_pst_ModifierMorphing_Get(_pst_Obj);
	if(!pst_Morph) return 0;
	return pst_Morph->l_NbChannel;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_NumberOfChannel(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_OBJ_NumberOfChannel_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_OBJ_GetChannelFactor_C(OBJ_tdst_GameObject *_pst_Obj, int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing	*pst_Morph;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Morph = GEO_pst_ModifierMorphing_Get(_pst_Obj);
	if(!pst_Morph) return 0;
	if((i < 0) || (i >= pst_Morph->l_NbChannel)) return 0;
	return pst_Morph->dst_MorphChannel[i].f_Blend;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_GetChannelFactor(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_OBJ_GetChannelFactor_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_SetChannelFactor_C(OBJ_tdst_GameObject *_pst_Obj, int i, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing	*pst_Morph;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Morph = GEO_pst_ModifierMorphing_Get(_pst_Obj);
	if(!pst_Morph) return;
	if((i < 0) || (i >= pst_Morph->l_NbChannel)) return;
	pst_Morph->dst_MorphChannel[i].f_Blend = f;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_SetChannelFactor(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f = AI_PopFloat();
	AI_EvalFunc_OBJ_SetChannelFactor_C(pst_GO, AI_PopInt(), f);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_OBJ_GetChannelProg_C(OBJ_tdst_GameObject *_pst_Obj, int i)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing	*pst_Morph;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Morph = GEO_pst_ModifierMorphing_Get(_pst_Obj);
	if(!pst_Morph) return 0;
	if((i < 0) || (i >= pst_Morph->l_NbChannel)) return 0;
	return pst_Morph->dst_MorphChannel[i].f_ChannelBlend;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_GetChannelProg(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_OBJ_GetChannelProg_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_SetChannelProg_C(OBJ_tdst_GameObject *_pst_Obj, int i, float f)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing	*pst_Morph;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Morph = GEO_pst_ModifierMorphing_Get(_pst_Obj);
	if(!pst_Morph) return;
	if((i < 0) || (i >= pst_Morph->l_NbChannel)) return;
	pst_Morph->dst_MorphChannel[i].f_ChannelBlend = f;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_SetChannelProg(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f = AI_PopFloat();
	AI_EvalFunc_OBJ_SetChannelProg_C(pst_GO, AI_PopInt(), f);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_GetChannelData_C(OBJ_tdst_GameObject *_pst_Obj, int i, int j)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing			*pst_Morph;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Morph = GEO_pst_ModifierMorphing_Get(_pst_Obj);
	if(!pst_Morph) return 0;
	if((i < 0) || (i >= pst_Morph->l_NbChannel)) return 0;
	pst_Channel = pst_Morph->dst_MorphChannel + i;
	if((j < 0) || (j >= pst_Channel->l_NbData))
		return pst_Channel->l_NbData;
	else
		return (int) pst_Channel->dl_DataIndex[j];
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_GetChannelData(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i = AI_PopInt();
	AI_PushInt(AI_EvalFunc_OBJ_GetChannelData_C(pst_GO, AI_PopInt(), i));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_SetChannelData_C(OBJ_tdst_GameObject *_pst_Obj, int i, int k, int j)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing			*pst_Morph;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Morph = GEO_pst_ModifierMorphing_Get(_pst_Obj);
	if(!pst_Morph) return;
	if((i < 0) || (i >= pst_Morph->l_NbChannel)) return;
	pst_Channel = pst_Morph->dst_MorphChannel + i;
	if((j < 0) || (j >= pst_Channel->l_NbData)) return;
	if(k >= pst_Morph->l_NbMorphData) return;
	pst_Channel->dl_DataIndex[j] = k;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_SetChannelData(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i = AI_PopInt();
	j = AI_PopInt();
	AI_EvalFunc_OBJ_SetChannelData_C(pst_GO, AI_PopInt(), j, i);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_SetModuleChannelData_C
(
	OBJ_tdst_GameObject *_pst_Obj,
	int					_i_AnimChannel,
	int					_i_MorphChannel,
	int					_i_DataIndex,
	int					_i_DataValue
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing			*pst_Morph;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_AnimChannel != -1)
	{
		_pst_Obj = ANI_pst_GetObjectByAICanal(_pst_Obj, (UCHAR) _i_AnimChannel);
		if(!_pst_Obj) return;
	}

	pst_Morph = GEO_pst_ModifierMorphing_Get(_pst_Obj);
	if(!pst_Morph) return;

	if((_i_MorphChannel < 0) || (_i_MorphChannel >= pst_Morph->l_NbChannel)) return;
	pst_Channel = pst_Morph->dst_MorphChannel + _i_MorphChannel;

	if((_i_DataIndex < 0) || (_i_DataIndex >= pst_Channel->l_NbData)) return;
	if(_i_DataValue >= pst_Morph->l_NbMorphData) return;

	pst_Channel->dl_DataIndex[_i_DataIndex] = _i_DataValue;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_SetModuleChannelData(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i, j, i_MorphChannel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i = AI_PopInt();
	j = AI_PopInt();
	i_MorphChannel = AI_PopInt();
	AI_EvalFunc_OBJ_SetModuleChannelData_C(pst_GO, AI_PopInt(), i_MorphChannel, j, i);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_ResetModuleChannelData_C(OBJ_tdst_GameObject *_pst_Obj, int _i_AnimChannel, int _i_MorphChannel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GEO_tdst_ModifierMorphing			*pst_Morph;
	GEO_tdst_ModifierMorphing_Channel	*pst_Channel;
	int									i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_AnimChannel != -1)
	{
		_pst_Obj = ANI_pst_GetObjectByAICanal(_pst_Obj, (UCHAR) _i_AnimChannel);
		if(!_pst_Obj) return;
	}

	pst_Morph = GEO_pst_ModifierMorphing_Get(_pst_Obj);
	if(!pst_Morph) return;

	if((_i_MorphChannel < 0) || (_i_MorphChannel >= pst_Morph->l_NbChannel)) return;
	pst_Channel = pst_Morph->dst_MorphChannel + _i_MorphChannel;

	for(i = 0; i < pst_Channel->l_NbData; i++) pst_Channel->dl_DataIndex[i] = -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_ResetModuleChannelData(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_MorphChannel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_MorphChannel = AI_PopInt();
	AI_EvalFunc_OBJ_ResetModuleChannelData_C(pst_GO, AI_PopInt(), i_MorphChannel);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_AnimMorphFactor_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject *_pst_Ani,
	int					_i_CanalAnim,
	int					_i_CanalMorph,
	int					_i_Src,
	int					_i_Tgt
)
{
	if(_i_CanalAnim != -1)
	{
		_pst_Ani = ANI_pst_GetObjectByAICanal(_pst_Ani, (UCHAR) _i_CanalAnim);
		if(!_pst_Ani) return;
	}

	AI_EvalFunc_OBJ_SetChannelData_C(_pst_Ani, _i_CanalMorph, _i_Src, 0);
	AI_EvalFunc_OBJ_SetChannelData_C(_pst_Ani, _i_CanalMorph, _i_Tgt, 1);

	AI_EvalFunc_OBJ_SetChannelFactor_C(_pst_Ani, _i_CanalMorph, OBJ_pst_GetAbsolutePosition(_pst_GO)->x);
	AI_EvalFunc_OBJ_SetChannelProg_C(_pst_Ani, _i_CanalMorph, OBJ_pst_GetAbsolutePosition(_pst_GO)->y);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_AnimMorphFactor(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_Ani;
	int					canalanim, canalmorph;
	int					source, dest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	dest = AI_PopInt();
	source = AI_PopInt();
	canalmorph = AI_PopInt();
	canalanim = AI_PopInt();
	pst_Ani = AI_PopGameObject();

	AI_EvalFunc_OBJ_AnimMorphFactor_C(pst_GO, pst_Ani, canalanim, canalmorph, source, dest);

	/*$F
    if(canalanim != -1)
	{
		pst_Ani = ANI_pst_GetObjectByAICanal(pst_Ani, (UCHAR) canalanim);
		if(!pst_Ani) return ++_pst_Node;
	}

	AI_EvalFunc_OBJ_SetChannelData_C(pst_Ani, canalmorph, source, 0);
	AI_EvalFunc_OBJ_SetChannelData_C(pst_Ani, canalmorph, dest, 1);

	AI_EvalFunc_OBJ_SetChannelFactor_C(pst_Ani, canalmorph, OBJ_pst_GetAbsolutePosition(pst_GO)->x);
	AI_EvalFunc_OBJ_SetChannelProg_C(pst_Ani, canalmorph, OBJ_pst_GetAbsolutePosition(pst_GO)->y);
    */
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_AnimMorphFactorNoKey_C
(
	OBJ_tdst_GameObject *_pst_GO,
	OBJ_tdst_GameObject *_pst_Ani,
	int					_i_CanalAnim,
	int					_i_CanalMorph
)
{
	if(_i_CanalAnim != -1)
	{
		_pst_Ani = ANI_pst_GetObjectByAICanal(_pst_Ani, (UCHAR) _i_CanalAnim);
		if(!_pst_Ani) return;
	}

	AI_EvalFunc_OBJ_SetChannelFactor_C(_pst_Ani, _i_CanalMorph, OBJ_pst_GetAbsolutePosition(_pst_GO)->x);
	AI_EvalFunc_OBJ_SetChannelProg_C(_pst_Ani, _i_CanalMorph, OBJ_pst_GetAbsolutePosition(_pst_GO)->y + 1.0f);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_AnimMorphFactorNoKey(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_Ani;
	int					canalanim, canalmorph;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	canalmorph = AI_PopInt();
	canalanim = AI_PopInt();
	pst_Ani = AI_PopGameObject();
	if(!pst_Ani) return ++_pst_Node;

	AI_EvalFunc_OBJ_AnimMorphFactorNoKey_C(pst_GO, pst_Ani, canalanim, canalmorph);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_MorphCheckDummies_C(OBJ_tdst_GameObject *_pst_GO, int _i_MorphBone)
{
#ifdef ACTIVE_EDITORS

	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_MorphGao;
	GEO_tdst_ModifierMorphing	*pst_MorphModif;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Find morph modifier
	pst_MorphModif = GEO_pst_ModifierMorphing_Get(_pst_GO);
	if(!pst_MorphModif)
	{
		pst_MorphGao = ANI_pst_GetObjectByAICanal(_pst_GO, (UCHAR) _i_MorphBone);
		if(!pst_MorphGao) return;
		pst_MorphModif = GEO_pst_ModifierMorphing_Get(pst_MorphGao);
		if(!pst_MorphModif) return;
	}

	// Check dummy = Create dummy
	GEO_ModifierMorphing_CreateDummyChannels( pst_MorphModif );

#endif
}
/**/
AI_tdst_Node * AI_EvalFunc_OBJ_MorphCheckDummies(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_MorphCheckDummies_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Modifier RLICarte
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_RLICarteSet_C(OBJ_tdst_GameObject *_pst_GO, int Param, int Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier			*pst_Mod;
	GEO_tdst_ModifierRLICarte	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = MDF_pst_GetByType(_pst_GO, MDF_C_Modifier_RLICarte);
	if((!pst_Mod) || (!pst_Mod->p_Data)) return;
	pst_Data = (GEO_tdst_ModifierRLICarte *) pst_Mod->p_Data;

	if (Param == -1)
	{
		pst_Data->uc_Flags = (UCHAR) Value;
		return;
	}
	if (Param == -2)
	{
		pst_Data->uc_Op = (UCHAR) Value;
		return;
	}

	if ( (Param >= 0) && (Param < 64 ) )
		pst_Data->aul_Color[ Param ] = Value;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_RLICarteSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				Value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Value = AI_PopInt();
	AI_EvalFunc_OBJ_RLICarteSet_C(pst_GO, AI_PopInt(), Value);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Sprites
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpriteAdd(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpriteDel(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopInt();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpritePosGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopInt();
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpritePosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	v = AI_PopVectorPtr();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpriteSizeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopInt();
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpriteSizeSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	*v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	v = AI_PopVectorPtr();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpriteFlagGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	LONG				l_Value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	l_Value = AI_PopInt();
	AI_PopInt();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpriteFlagSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_On;
	LONG				l_Flag;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_On = AI_PopInt();
	l_Flag = AI_PopInt();
	AI_PopInt();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpriteColorGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopInt();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpriteColorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	LONG				l_Color;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	l_Color = AI_PopInt();
	AI_PopInt();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpriteMatIdGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJ_SpriteMatIdSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	LONG				l_MatId;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	l_MatId = AI_PopInt();
	AI_PopInt();
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Sprite mapper 1
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SPG_SetNoise_C(OBJ_tdst_GameObject *pst_GO, float inertia)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*p_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO->pst_Extended) return;
	p_Mod = pst_GO->pst_Extended->pst_Modifiers;
	while(p_Mod)
	{
		if(p_Mod->i->ul_Type == MDF_C_Modifier_SPG)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
			SPG_tdst_Modifier	*pst_MDF;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_MDF = (SPG_tdst_Modifier *) p_Mod->p_Data;
#ifdef USE_DOUBLE_RENDERING	
#ifdef WIN32
			if (MAI_gst_MainHandles.pst_DisplayData->ul_DisplayInfo & (GDI_Cul_DI_DoubleRendering_I|GDI_Cul_DI_DoubleRendering_K)) 
#else
			if (gpst_GSP_stDD.ul_DisplayInfo & (GDI_Cul_DI_DoubleRendering_I|GDI_Cul_DI_DoubleRendering_K)) 
#endif
			{
				pst_MDF->f_Noise_Out = inertia;
			} else
#endif			
			  pst_MDF->f_Noise = inertia;

		}

		p_Mod = p_Mod->pst_Next;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_SPG_SetNoise(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Inertia;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Inertia = AI_PopFloat();
	AI_EvalFunc_SPG_SetNoise_C(pst_GO, f_Inertia);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Sprite mapper 2
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
	SET FLOAT
 =======================================================================================================================
 */
void AI_EvalFunc_SPG2_SetFloat_C(OBJ_tdst_GameObject *pst_GO, int _i_Mod, int _i_Float, float _f_Val )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*p_Mod;
	SPG2_tdst_Modifier	*pst_MDF;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

#if (defined _GAMECUBE) || (defined _XBOX) || (defined PSX2_TARGET)
	return; // Peut faire planter les crabes -> je le vire
#endif // _GAMECUBE

	/* Global params for wind */
	if (_i_Float >= 1000)
	{
		_i_Float -= 1000;
		j = _i_Float % 5;
		i = (_i_Float - j) / 5;
		SPG2_GlobalParams[j][i] = _f_Val;
		return;
	}	
	
	if(!pst_GO->pst_Extended) return;
	p_Mod = pst_GO->pst_Extended->pst_Modifiers;
	while( p_Mod )
	{
		if(p_Mod->i->ul_Type == MDF_C_Modifier_SpriteMapper2)
			if ( _i_Mod-- == 0 ) break;
		p_Mod = p_Mod->pst_Next;
	}
	if (!p_Mod) return;
	
	pst_MDF = (SPG2_tdst_Modifier *) p_Mod->p_Data;

	if (_i_Float >= 200)
	{
		j = _i_Float % 10;
		i = ((_i_Float - 200) - j) / 10;
		if ((i < SPG2_WindNumbers) && (j < 3))
			((float *)(pst_MDF->CurrentPos + i))[ j ] = _f_Val;
		return;
	}
	
	if (_i_Float >= 100)
	{
		j = _i_Float % 10;
		i = ((_i_Float - 100) - j) / 10;
		if ((i < SPG2_WindNumbers) && (j < 3))
			((float *)(pst_MDF->CurrentSpeed + i))[ j ] = _f_Val;
		return;
	}
	
	switch( _i_Float )
	{
	case 0:	pst_MDF->SringStrenght = _f_Val;break;
	case 1: pst_MDF->SpeedAbsorbtion = _f_Val;break;
	case 2: pst_MDF->Freedom = _f_Val;break;
	case 4: pst_MDF->WindSensibility = _f_Val;break;
	case 5: pst_MDF->f_GlobalSize = _f_Val;break;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_SPG2_SetFloat(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Value;
	int					i_Modifier, i_Float;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	
	f_Value = AI_PopFloat();
	i_Float = AI_PopInt();
	i_Modifier = AI_PopInt();
	AI_EvalFunc_SPG2_SetFloat_C(pst_GO, i_Modifier, i_Float, f_Value );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
	GET FLOAT
 =======================================================================================================================
 */
float AI_EvalFunc_SPG2_GetFloat_C(OBJ_tdst_GameObject *pst_GO, int _i_Mod, int _i_Float )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*p_Mod;
	SPG2_tdst_Modifier	*pst_MDF;
	int					i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO->pst_Extended) return 0;
	p_Mod = pst_GO->pst_Extended->pst_Modifiers;
	while( p_Mod )
	{
		if(p_Mod->i->ul_Type == MDF_C_Modifier_SpriteMapper2)
			if ( _i_Mod-- == 0 ) break;
		p_Mod = p_Mod->pst_Next;
	}
	if (!p_Mod) return 0;
	
	pst_MDF = (SPG2_tdst_Modifier *) p_Mod->p_Data;
	if (_i_Float > 200)
	{
		j = _i_Float % 10;
		i = ((_i_Float - 200) - j) / 10;
		if ((i < SPG2_WindNumbers) && (j < 3))
			return ((float *)(pst_MDF->CurrentPos + i))[ j ];
		return 0;
	}
	
	if (_i_Float > 100)
	{
		j = _i_Float % 10;
		i = ((_i_Float - 100) - j) / 10;
		if ((i < SPG2_WindNumbers) && (j < 3))
			return ((float *)(pst_MDF->CurrentSpeed + i))[ j ];
		return 0;
	}
	
	switch( _i_Float )
	{
	case 0:	return pst_MDF->SringStrenght; break;
	case 1: return pst_MDF->SpeedAbsorbtion; break;
	case 2: return pst_MDF->Freedom; break;
	case 4: return pst_MDF->WindSensibility; break;
	case 5: return pst_MDF->f_GlobalSize; break;	
	}
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_SPG2_GetFloat(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Modifier, i_Float;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Float = AI_PopInt();
	i_Modifier = AI_PopInt();
	AI_PushFloat( AI_EvalFunc_SPG2_GetFloat_C(pst_GO, i_Modifier, i_Float ) );
	return ++_pst_Node;
}

void AI_EvalFunc_SPG2_GlobalWind_C(float value, float min, float max, MATH_tdst_Vector *pst_Dir)
{
	extern float SPG2_GlobalWind;
	extern float SPG2_GlobalWindFactorMin;
	extern float SPG2_GlobalWindFactorMax;
	extern MATH_tdst_Vector SPG2_GlobalWindVec;
	SPG2_GlobalWind = value;
	SPG2_GlobalWindFactorMin = min;
	SPG2_GlobalWindFactorMax = max;
	MATH_CopyVector(&SPG2_GlobalWindVec, pst_Dir);
}
/**/
AI_tdst_Node *AI_EvalFunc_SPG2_GlobalWind(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				f_Min, f_Max, f_Value;
	MATH_tdst_Vector	*pt_Dir;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pt_Dir = AI_PopVectorPtr();
	f_Max = AI_PopFloat();
	f_Min = AI_PopFloat();
	f_Value = AI_PopFloat();
	AI_EvalFunc_SPG2_GlobalWind_C(f_Value, f_Min, f_Max, pt_Dir);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
	FIRE ADD
 =======================================================================================================================
 */
int AI_EvalFunc_SPG2_FireAdd_C( MATH_tdst_Vector *_pst_Pos, int _i_Flags )
{
	return SPG2_AI_AddOne( _pst_Pos, _i_Flags );
}
/**/
AI_tdst_Node *AI_EvalFunc_SPG2_FireAdd(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Flags = AI_PopInt();
	AI_PushInt( AI_EvalFunc_SPG2_FireAdd_C( AI_PopVectorPtr(), i_Flags) );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
	SPG2 SetValue
 =======================================================================================================================
 */
extern void SPG2_SetValue_C( OBJ_tdst_GameObject *_pst_GO,ULONG _ul_UnicId, ULONG _ul_NumToSet, float _f_Value );/**/
AI_tdst_Node *AI_EvalFunc_SPG2_SetValue(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *_pst_GO;
	ULONG _ul_UnicId;
	ULONG _ul_NumToSet;
	float _f_Value ;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(_pst_GO);
	_f_Value = AI_PopFloat();
	_ul_NumToSet = AI_PopInt();
	_ul_UnicId = AI_PopInt();
	SPG2_SetValue_C( _pst_GO , _ul_UnicId , _ul_NumToSet , _f_Value) ;
	return ++_pst_Node;
}

/*
 =======================================================================================================================
	FIRE SET STATE
 =======================================================================================================================
 */
void AI_EvalFunc_SPG2_FireStateSet_C( int _i_ID, int _i_State )
{
	SPG2_AI_SetState( _i_ID, _i_State );
}
/**/
AI_tdst_Node *AI_EvalFunc_SPG2_FireStateSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Flags;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Flags = AI_PopInt();
	AI_EvalFunc_SPG2_FireStateSet_C( AI_PopInt(), i_Flags);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
	FIRE SET POS
 =======================================================================================================================
 */
void AI_EvalFunc_SPG2_FirePosSet_C( int _i_ID, MATH_tdst_Vector *_pst_Pos )
{
	SPG2_AI_SetPos( _i_ID, _pst_Pos );
}
/**/
AI_tdst_Node *AI_EvalFunc_SPG2_FirePosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector *pst_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Pos = AI_PopVectorPtr();
	AI_EvalFunc_SPG2_FirePosSet_C( AI_PopInt(), pst_Pos);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
	FIRE SET Float
 =======================================================================================================================
 */
void AI_EvalFunc_SPG2_FireFloatSet_C( int _i_ID, int _i_Float, float _f_Val )
{
	SPG2_AI_SetFloat( _i_ID, _i_Float, _f_Val );
}
/**/
AI_tdst_Node *AI_EvalFunc_SPG2_FireFloatSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float	f_Val;
	int		i_Float;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	f_Val = AI_PopFloat();
	i_Float = AI_PopInt();
	AI_EvalFunc_SPG2_FireFloatSet_C( AI_PopInt(), i_Float, f_Val);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Shadow
 ***********************************************************************************************************************
 */
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SDW_Activate_C(OBJ_tdst_GameObject *pst_GO, int activate)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*p_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO->pst_Extended) return;
	p_Mod = pst_GO->pst_Extended->pst_Modifiers;
	while(p_Mod)
	{
		if(p_Mod->i->ul_Type == MDF_C_Modifier_Shadow)
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			GAO_tdst_ModifierSDW	*pst_MDF;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_MDF = (GAO_tdst_ModifierSDW *) p_Mod->p_Data;
			if(activate)
				pst_MDF->ulFlags |= MDF_SDW_IsActivate;
			else
				pst_MDF->ulFlags &= ~MDF_SDW_IsActivate;
		}

		p_Mod = p_Mod->pst_Next;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_SDW_Activate(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					activate;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	activate = AI_PopInt();
	AI_EvalFunc_SDW_Activate_C(pst_GO, activate);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNAKE_SetInertia_C(OBJ_tdst_GameObject *pst_GO, float inertia)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_GameObject *pst_BoneGO;
	OBJ_tdst_Group		*pst_Skeleton;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Skeleton = NULL;
	pst_EndBone = pst_CurrentBone = NULL;

	if(!pst_GO) return;
	if(!pst_GO->pst_Base) goto GO_SET;
	if(!pst_GO->pst_Base->pst_GameObjectAnim) goto GO_SET;
	if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) goto GO_SET;
	if(!((ANI_st_GameObjectAnim *) (pst_GO->pst_Base->pst_GameObjectAnim))->pst_Skeleton) goto GO_SET;

	pst_Skeleton = pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;

	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);

GO_SET:
	for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	{
		if(pst_CurrentBone != NULL)
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		else
		{
			pst_BoneGO = pst_GO;
			if(pst_EndBone)
			{
				pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
				pst_CurrentBone--;
			}
		}

		if(TAB_b_IsAHole(pst_BoneGO)) continue;
		if(!pst_BoneGO->pst_Extended) continue;
		if(!pst_BoneGO->pst_Extended->pst_Modifiers) continue;
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~*/
			MDF_tdst_Modifier	*p_Mod;
			/*~~~~~~~~~~~~~~~~~~~~~~~*/

			p_Mod = pst_BoneGO->pst_Extended->pst_Modifiers;
			while(p_Mod)
			{
				if(p_Mod->i->ul_Type == MDF_C_Modifier_SNAKE)
				{
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
					GAO_tdst_ModifierSNAKE	*pst_SNAKE;
					/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

					pst_SNAKE = (GAO_tdst_ModifierSNAKE *) p_Mod->p_Data;
					pst_SNAKE->f_Inertie = inertia;
				}

				p_Mod = p_Mod->pst_Next;
			}
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_SNAKE_SetInertia(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Inertia;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Inertia = AI_PopFloat();
	AI_EvalFunc_SNAKE_SetInertia_C(pst_GO, f_Inertia);
	return ++_pst_Node;
}

/*$4 
 =======================================================================================================================
	PARTICLES GENERATOR 
 =======================================================================================================================
*/

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_PAG_AddGenerator_C(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_From, float _f_Nb)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(pst_PAG) return PAG_i_AddGenerator(pst_PAG, _pst_From, _f_Nb);
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_AddGenerator(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Nb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Nb = AI_PopFloat();
	AI_PushInt(AI_EvalFunc_PAG_AddGenerator_C(pst_GO, AI_PopGameObject(), f_Nb));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_PAG_DelGenerator_C(OBJ_tdst_GameObject *_pst_GO, int _i_Id)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(pst_PAG) PAG_DelGenerator(pst_PAG, _i_Id);
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_DelGenerator(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_PAG_DelGenerator_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_PAG_SetGeneratorFreq_C(OBJ_tdst_GameObject *_pst_GO, int _i_Id, float _f_Freq)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(pst_PAG) PAG_SetGeneratorFrequency(pst_PAG, _i_Id, _f_Freq);
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_SetGeneratorFreq(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Freq;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Freq = AI_PopFloat();
	AI_EvalFunc_PAG_SetGeneratorFreq_C(pst_GO, AI_PopInt(), f_Freq);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_PAG_GetGeneratorFreq_C(OBJ_tdst_GameObject *_pst_GO, int _i_Id)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(pst_PAG) return PAG_GetGeneratorFrequency(pst_PAG, _i_Id);
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_GetGeneratorFreq(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat( AI_EvalFunc_PAG_GetGeneratorFreq_C(pst_GO, AI_PopInt()) );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_PAG_SetFlag_C(OBJ_tdst_GameObject *_pst_GO, int _i_One, int _i_Zero)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(pst_PAG)
	{
		pst_PAG->ul_Flags |= _i_One;
		pst_PAG->ul_Flags &= ~_i_Zero;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_SetFlag(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Zero;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Zero = AI_PopInt();
	AI_EvalFunc_PAG_SetFlag_C(pst_GO, AI_PopInt(), i_Zero);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
LONG AI_EvalFunc_PAG_GetFlag_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(pst_PAG) return pst_PAG->ul_Flags;
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_GetFlag(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_PAG_GetFlag_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_PAG_Setf_C(OBJ_tdst_GameObject *_pst_GO, int _i_Param, float _f_Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if((_i_Param < 0) || (_i_Param >= PAG_Paramf_Number)) return;
	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(!pst_PAG) return;

	*((&pst_PAG->f_GenOffset) + _i_Param) = _f_Value;
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_Setf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Value;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Value = AI_PopFloat();
	AI_EvalFunc_PAG_Setf_C(pst_GO, AI_PopInt(), f_Value);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_PAG_Getf_C(OBJ_tdst_GameObject *_pst_GO, int _i_Param)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if((_i_Param < 0) || (_i_Param >= PAG_Paramf_Number)) return 0;
	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(!pst_PAG) return 0;

	return *((&pst_PAG->f_GenOffset) + _i_Param);
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_Getf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_PAG_Getf_C(pst_GO, AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_PAG_SetColor_C(OBJ_tdst_GameObject *_pst_GO, int _i_Param)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(!pst_PAG) return;
	pst_PAG->ul_Color = _i_Param;
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_SetColor(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_PAG_SetColor_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_PAG_SetObject_C(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_Src )
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(!pst_PAG) return;
	pst_PAG->pst_GO = _pst_Src;
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_SetObject(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_PAG_SetObject_C(pst_GO, AI_PopGameObject());
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_PAG_SetMatrixRef_C(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_Src )
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(!pst_PAG) return;
	pst_PAG->pst_GOMatrixRef = _pst_Src;
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_SetMatrixRef(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_PAG_SetMatrixRef_C(pst_GO, AI_PopGameObject());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_PAG_ShiftParticles_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Shift )
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	PAG_tdst_Struct *pst_PAG;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_PAG = PAG_pst_GetGameObjectParticleGenerator(_pst_GO);
	if(!pst_PAG) return;
	for (i = 0; i < pst_PAG->l_NbP; i++)
		MATH_AddEqualVector( &pst_PAG->dst_P[i].st_Pos, _pst_Shift );
}
/**/
AI_tdst_Node *AI_EvalFunc_PAG_ShiftParticles(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_PAG_ShiftParticles_C(pst_GO, AI_PopVectorPtr());
	return ++_pst_Node;
}


/*
 =======================================================================================================================
    Aim:    Returns the Visual LOD of an object

    Note:   Returns a float between 0 and 1, 0 if object is very far (1 pixel on the screen) and 1 if very near (size
            of the screen) £
            It is the view 0 that is taken to calculate the size of the object on the screen.
 =======================================================================================================================
 */
float AI_EvalFunc_OBJ_LodVisGet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~*/
	float	f_LodVis;
	LONG	l;
	/*~~~~~~~~~~~~~*/

	l = (LONG) pst_GO->uc_LOD_Vis;
	f_LodVis = fLongToFloat(l) * 0.00392156862745f; /* We divide by 255 */

	return f_LodVis;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_LodVisGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_OBJ_LodVisGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the distance of the object from the activator number 0 (main activator)
 =======================================================================================================================
 */
float AI_EvalFunc_OBJ_LodDistGet_C(OBJ_tdst_GameObject *pst_GO)
{
	return pst_GO->uc_LOD_Dist;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_LodDistGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_OBJ_LodDistGet_C(pst_GO));
	return ++_pst_Node;
}

void AI_EvalFunc_OBJ_LodDistSet_C(OBJ_tdst_GameObject *pst_GO, int val)
{
	pst_GO->uc_LOD_Dist = val;
#ifdef JADEFUSION
	return;
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_LodDistSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_LodDistSet_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_OBJ_HierarchyGet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Father = NULL;
	if(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
		pst_Father = pst_GO->pst_Base->pst_Hierarchy->pst_Father;
	return pst_Father;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_HierarchyGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushGameObject(AI_EvalFunc_OBJ_HierarchyGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_HierarchyGetChilds_C(OBJ_tdst_GameObject *pst_GO, OBJ_tdst_GameObject **ppst_Array)
{
	return WOR_i_GetAllChildsOf(WOR_World_GetWorldOfObject(pst_GO), pst_GO, ppst_Array, TRUE);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_HierarchyGetChilds(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVar(&Val, &st_Var);
	AI_PushInt(AI_EvalFunc_OBJ_HierarchyGetChilds_C(pst_GO, (OBJ_tdst_GameObject **) st_Var.pv_Addr));
	return ++_pst_Node;
}

extern void DYN_OneCall(OBJ_tdst_GameObject *);
extern void COL_OneCall(OBJ_tdst_GameObject *, ULONG);
extern void REC_MainCall(WOR_tdst_World *, ULONG);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJEngineCall_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	ULONG			ul_FirstReport;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_gpst_CurrentWorld;
	if(!pst_World) return;

	ul_FirstReport = WOR_gpst_CurrentWorld->ul_NbReports;

//	DYN_OneCall(pst_GO);
	COL_OneCall(pst_GO, ul_FirstReport);
	REC_MainCall(pst_World, ul_FirstReport);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJEngineCall(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJEngineCall_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_HierarchyReset_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/*
	 * AI_Check(pst_GO->pst_Base && pst_GO->pst_Base->pst_Hierarchy, "Object does not
	 * have hierarchy");
	 */
	if(!pst_GO->pst_Base || !pst_GO->pst_Base->pst_Hierarchy) return;

	/* Update all Flash Matrix of the Child (Blend matrix too) */
	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix)) ANI_RemoveHierarchyOnFlash(pst_GO);

	/* Need to recompute the OldGlobalMatrix in Global Coordinate System */
	if(pst_GO->ul_IdentityFlags & (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	st_Temp;
		/*~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_MulMatrixMatrix
		(
			&st_Temp,
			((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
			pst_GO->pst_Base->pst_Hierarchy->pst_Father->pst_GlobalMatrix
		);
		MATH_CopyMatrix(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix, &st_Temp);
	}

	OBJ_ChangeIdentityFlags
	(
		pst_GO,
		OBJ_ul_FlagsIdentityGet(pst_GO) &~OBJ_C_IdentityFlag_Hierarchy,
		OBJ_ul_FlagsIdentityGet(pst_GO)
	);

	/* Remove Object from EOT Hierarchy Table */
	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	TAB_PFtable_RemoveElemWithPointer(&pst_World->st_EOT.st_Hierarchy, pst_GO);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_HierarchyReset(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_HierarchyReset_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_HierarchyResetCurrent_C(OBJ_tdst_GameObject *pst_GO)
{
	if(!pst_GO->pst_Base || !pst_GO->pst_Base->pst_Hierarchy) return;
	pst_GO->pst_Base->pst_Hierarchy->pst_Father = NULL;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_HierarchyResetCurrent(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_HierarchyResetCurrent_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_OBJ_HierarchySet_C(OBJ_tdst_GameObject *pst_GO, OBJ_tdst_GameObject *pst_Father)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Last;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* anti-bug*/
	if(!pst_Father || (pst_Father == pst_GO))
		return NULL;
	
	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	AI_EvalFunc_OBJ_HierarchyReset_C(pst_GO);
	OBJ_ChangeIdentityFlags
	(
		pst_GO,
		OBJ_ul_FlagsIdentityGet(pst_GO) | OBJ_C_IdentityFlag_Hierarchy,
		OBJ_ul_FlagsIdentityGet(pst_GO)
	);
	pst_Last = pst_GO->pst_Base->pst_Hierarchy->pst_Father;
	pst_GO->pst_Base->pst_Hierarchy->pst_Father = pst_Father;
	OBJ_ComputeLocalWhenHie(pst_GO);

	/* Need to recompute the OldGlobalMatrix in Local Coordinate System of the Father */
	if(pst_GO->ul_IdentityFlags & (OBJ_C_IdentityFlag_ZDM | OBJ_C_IdentityFlag_ZDE))
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		MATH_tdst_Matrix	st_Inverse, st_Temp;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		MATH_InvertMatrix(&st_Inverse, pst_Father->pst_GlobalMatrix);
		MATH_MulMatrixMatrix
		(
			&st_Temp,
			((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix,
			&st_Inverse
		);
		MATH_CopyMatrix(((COL_tdst_Base *) pst_GO->pst_Extended->pst_Col)->pst_Instance->pst_OldGlobalMatrix, &st_Temp);
	}

	/* Update all Flash Matrix of the Child (Blend matrix too) */
	if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_FlashMatrix)) ANI_ApplyHierarchyOnFlash(pst_GO);

	/* Force EOT ReBuild */
	if(pst_World) pst_World->b_ForceActivationRefresh = TRUE;

	return pst_Last;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_HierarchySet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_Father = AI_PopGameObject();
	AI_EvalFunc_OBJ_HierarchyReset_C(pst_GO);
	AI_PushGameObject(AI_EvalFunc_OBJ_HierarchySet_C(pst_GO, pst_Father));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_OBJ_HierarchySetFather_C(OBJ_tdst_GameObject *pst_GO, OBJ_tdst_GameObject *pst_Father)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Last;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* anti-bug*/
	if(pst_Father == pst_GO) return NULL;
	
	if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)) return NULL;
	pst_Last = pst_GO->pst_Base->pst_Hierarchy->pst_Father;
	pst_GO->pst_Base->pst_Hierarchy->pst_Father = pst_Father;
	OBJ_ComputeLocalWhenHie(pst_GO);
	return pst_Last;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_HierarchySetFather(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	pst_Father = AI_PopGameObject();
	AI_PushGameObject(AI_EvalFunc_OBJ_HierarchySetFather_C(pst_GO, pst_Father));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_HierarchyMoveFather_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Pos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_Father, *GO;
	MATH_tdst_Vector	V;
	TAB_tdst_PFelem		*pst_Elem;
	TAB_tdst_PFelem		*pst_LastElem;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)) return;
	pst_Father = pst_GO->pst_Base->pst_Hierarchy->pst_Father;
	if(!pst_Father) return;

	/* il faut que MatrixFather * LocalMatrixGO = pst_Pos */
	MATH_TransformVector(&V, OBJ_pst_GetAbsoluteMatrix(pst_Father), &pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix.T);
	MATH_SubVector(&V, pst_Pos, &V);
	OBJ_SetAbsolutePosition(pst_Father, &V);

	pst_World = WOR_gpst_CurrentWorld;
	pst_Elem = pst_World->st_AllWorldObjects.p_Table;
	pst_LastElem = pst_World->st_AllWorldObjects.p_NextElem;
	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		GO = (OBJ_tdst_GameObject *) pst_Elem->p_Pointer;
		if(TAB_b_IsAHole(GO)) continue;
		if(!(GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)) continue;
		if((!GO->pst_Base) || (!GO->pst_Base->pst_Hierarchy)) continue;
		if(GO->pst_Base->pst_Hierarchy->pst_Father != pst_Father) continue;
		if(GO == pst_GO) V.x = 0;
		OBJ_ComputeGlobalWhenHie(GO);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_HierarchyMoveFather(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_HierarchyMoveFather_C(pst_GO, AI_PopVectorPtr());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_IsValidGAO_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_Elem;
	TAB_tdst_PFelem		*pst_LastElem;
	WOR_tdst_World		*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_gpst_CurrentWorld;
	pst_Elem = pst_World->st_AllWorldObjects.p_Table;
	pst_LastElem = pst_World->st_AllWorldObjects.p_NextElem;
	for(; pst_Elem < pst_LastElem; pst_Elem++)
	{
		if(pst_Elem->p_Pointer == pst_GO)
		{
			return 1;			
		}
	}

	return (0);	
}

AI_tdst_Node *AI_EvalFunc_OBJ_IsValidGAO(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
    AI_PushInt(AI_EvalFunc_OBJ_IsValidGAO_C(pst_GO));
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GRPFillArray_C(OBJ_tdst_GameObject *pst_GO, OBJ_tdst_GameObject **pp)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group		*pst_Group;
	TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
	int					num;
	OBJ_tdst_GameObject *pst_GrpGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Group)) return 0;
	if(!pst_GO->pst_Extended || !pst_GO->pst_Extended->pst_Group) return 0;

	pst_Group = pst_GO->pst_Extended->pst_Group;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
	num = 0;
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GrpGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GrpGO)) continue;
		pp[num++] = pst_GrpGO;
	}

	return num;
}
/**/
AI_tdst_Node *AI_EvalFunc_GRPFillArray(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO, **ppst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVar(&Val, &st_Var);
	ppst_GO = (OBJ_tdst_GameObject**)st_Var.pv_Addr;
	AI_PushInt(AI_EvalFunc_GRPFillArray_C(pst_GO, ppst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRPInvisibleSet_C(OBJ_tdst_GameObject *pst_GO, int i_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group		*pst_Group;
	TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
	OBJ_tdst_GameObject *pst_GrpGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Group)) return;
	if(!pst_GO->pst_Extended || !pst_GO->pst_Extended->pst_Group) return;

	pst_Group = pst_GO->pst_Extended->pst_Group;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GrpGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GrpGO)) continue;
		AI_EvalFunc_OBJ_FlagInvisibleSet_C(pst_GrpGO, i_Val);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_GRPInvisibleSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Val = AI_PopInt();
	AI_EvalFunc_GRPInvisibleSet_C(pst_GO, i_Val);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_GRPInactiveSet_C(OBJ_tdst_GameObject *pst_GO, int i_Val)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_Group		*pst_Group;
	TAB_tdst_PFelem		*pst_CurrentElem, *pst_EndElem;
	OBJ_tdst_GameObject *pst_GrpGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Group)) return;
	if(!pst_GO->pst_Extended || !pst_GO->pst_Extended->pst_Group) return;

	pst_Group = pst_GO->pst_Extended->pst_Group;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_Group->pst_AllObjects);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GrpGO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GrpGO)) continue;
		AI_EvalFunc_OBJ_FlagInactiveSet_C(pst_GrpGO, i_Val);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_GRPInactiveSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Val;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Val = AI_PopInt();
	AI_EvalFunc_GRPInactiveSet_C(pst_GO, i_Val);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_AddMaterial_C(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_From, int matid)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_Multi	*pst_Mat;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	if(!(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_BaseObject)) return;
	if(!(_pst_From->ul_IdentityFlags & OBJ_C_IdentityFlag_Visu)) return;

	if(matid == -1)
	{
		_pst_GO->pst_Base->pst_AddMaterial = NULL;
		return;
	}

	pst_Mat = (MAT_tdst_Multi *) _pst_From->pst_Base->pst_Visu->pst_Material;
	if((pst_Mat) && (pst_Mat->st_Id.i->ul_Type == GRO_MaterialMulti))
	{
		matid %= pst_Mat->l_NumberOfSubMaterials;
		pst_Mat = (MAT_tdst_Multi *) pst_Mat->dpst_SubMaterial[matid];
	}

	_pst_GO->pst_Base->pst_AddMaterial = &pst_Mat->st_Id;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_AddMaterial(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	LONG				l_MatId;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	l_MatId = AI_PopInt();
	AI_EvalFunc_OBJ_AddMaterial_C(pst_GO, AI_PopGameObject(), l_MatId);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_RegisterKill_C
(
	OBJ_tdst_GameObject *pst_GO1,
	OBJ_tdst_GameObject *pst_GO,
	OBJ_tdst_GameObject **ppst_GO
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	WOR_AddDelObj(pst_World, pst_GO, (void *) ppst_GO);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_RegisterKill(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO1;
	OBJ_tdst_GameObject *pst_GO, **ppst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	ppst_GO = (OBJ_tdst_GameObject**)st_Var.pv_Addr;
	pst_GO = AI_PopGameObject();
	AI_M_GetCurrentObject(pst_GO1);
	AI_EvalFunc_OBJ_RegisterKill_C(pst_GO1, pst_GO, ppst_GO);
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_UnRegisterKill_C
(
	OBJ_tdst_GameObject *pst_GO1,
	OBJ_tdst_GameObject *pst_GO,
	OBJ_tdst_GameObject **ppst_GO
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(pst_GO);
	WOR_DelObjData(pst_World, pst_GO, (void *) ppst_GO);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_UnRegisterKill(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO1;
	OBJ_tdst_GameObject *pst_GO, **ppst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&Val, &st_Var);
	ppst_GO = (OBJ_tdst_GameObject**)st_Var.pv_Addr;
	pst_GO = AI_PopGameObject();
	AI_M_GetCurrentObject(pst_GO1);
	AI_EvalFunc_OBJ_UnRegisterKill_C(pst_GO1, pst_GO, ppst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_OBJIsInCone_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pos, float fAngle, float fDist, int iZ)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				fDot, fNorm;
	MATH_tdst_Vector	st_Sight;
	MATH_tdst_Vector	st_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	MATH_SubVector(&st_Temp, pos, OBJ_pst_GetAbsolutePosition(pst_GO));
	if(iZ) st_Temp.z = 0;
	fNorm = MATH_f_NormVector(&st_Temp);
	if(fNorm > fDist)
	{
		return 0;
	}

	MATH_NormalizeVector(&st_Temp, &st_Temp);
	OBJ_SightGet(pst_GO, &st_Sight);
	if(iZ)
	{
		st_Sight.z = 0;
		MATH_NormalizeVector(&st_Sight, &st_Sight);
	}

	fDot = MATH_f_DotProduct(&st_Temp, &st_Sight);
	if(fDot > fAngle) return fNorm;
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJIsInCone(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					iZ;
	float				fDist, fAngle;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	iZ = AI_PopInt();
	fDist = AI_PopFloat();
	fAngle = AI_PopFloat();
	AI_PopVector(&st_Pos);

	AI_PushFloat(AI_EvalFunc_OBJIsInCone_C(pst_GO, &st_Pos, fAngle, fDist, iZ));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_OBJIKFatherPosSet
(
	OBJ_tdst_GameObject *pst_GO,
	OBJ_tdst_GameObject *pst_GOA,
	OBJ_tdst_GameObject *pst_GOB,
	float				AB,
	float				BC,
	MATH_tdst_Vector	*BAxis
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix I	ONLY_PSX2_ALIGNED(16);
	MATH_tdst_Matrix	M, MA, MB, MC;
	OBJ_tdst_GameObject *pst_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	OBJ_ComputeGlobalWithLocal(pst_GOA, &MA, TRUE);
	OBJ_ComputeGlobalWithLocal(pst_GOB, &MB, TRUE);
	OBJ_ComputeGlobalWithLocal(pst_GO, &MC, TRUE);
	if(!AB) AB = MATH_f_NormVector(&OBJ_pst_GetInitialAbsoluteMatrix(pst_GOB)->T);
	if(!BC) BC = MATH_f_NormVector(&OBJ_pst_GetInitialAbsoluteMatrix(pst_GO)->T);
	
	GAO_ModifierLegLink_Compute(&MA, &MB, &MC, AB, BC, BAxis);

	if((pst_Father = OBJ_pst_GetFather(pst_GOA)))
	{
		OBJ_ComputeGlobalWithLocal(pst_Father, &M, TRUE);
		MATH_InvertMatrix(&I, &M);
		MATH_MulMatrixMatrix(&pst_GOA->pst_Base->pst_Hierarchy->st_LocalMatrix, &MA, &I);
		MATH_Orthonormalize(&pst_GOA->pst_Base->pst_Hierarchy->st_LocalMatrix);
	}
	else
	{
		MATH_CopyMatrix(pst_GOA->pst_GlobalMatrix, &MA);
	}

	if((pst_Father = OBJ_pst_GetFather(pst_GOB)))
	{
		MATH_InvertMatrix(&I, &MA);
		MATH_MulMatrixMatrix(&pst_GOB->pst_Base->pst_Hierarchy->st_LocalMatrix, &MB, &I);
		MATH_Orthonormalize(&pst_GOB->pst_Base->pst_Hierarchy->st_LocalMatrix);
	}
	else
	{
		MATH_CopyMatrix(pst_GOB->pst_GlobalMatrix, &MB);
	}

	if((pst_Father = OBJ_pst_GetFather(pst_GO)))
	{
		OBJ_ComputeGlobalWithLocal(pst_Father, &M, TRUE);
		MATH_InvertMatrix(&I, &M);
		MATH_MulMatrixMatrix(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix, &MC, &I);
		MATH_Orthonormalize(&pst_GO->pst_Base->pst_Hierarchy->st_LocalMatrix);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJIKFatherPosSetExt_C
(
	OBJ_tdst_GameObject *pst_GO,
	OBJ_tdst_GameObject *pst_GOA,
	OBJ_tdst_GameObject *pst_GOB,
	float				AB,
	float				BC
)
{
	AI_OBJIKFatherPosSet(pst_GO, pst_GOA, pst_GOB, AB, BC, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJIKFatherPosSetExt(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_GOA, *pst_GOB;
	float				AB, BC;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	BC = AI_PopFloat();
	AB = AI_PopFloat();
	pst_GOB = AI_PopGameObject();
	pst_GOA = AI_PopGameObject();
	AI_EvalFunc_OBJIKFatherPosSetExt_C(pst_GO, pst_GOA, pst_GOB, AB, BC);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJIKFatherPosSet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GOA, *pst_GOB;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_GOB = OBJ_pst_GetFather(pst_GO);
	if(pst_GOB)
	{
		pst_GOA = OBJ_pst_GetFather(pst_GOB);
		if(pst_GOA)
		{
			AI_OBJIKFatherPosSet(pst_GO, pst_GOA, pst_GOB, 0, 0, NULL);
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJIKFatherPosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJIKFatherPosSet_C(pst_GO);

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJIKCompute_C(OBJ_tdst_GameObject *B, OBJ_tdst_GameObject *A, OBJ_tdst_GameObject *C, float AB, float BC, MATH_tdst_Vector *BConstraint )
{
	AI_OBJIKFatherPosSet( C, A, B, AB, BC, BConstraint );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_OBJIKCompute(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *Hand, *Shoulder, *Elbow;
	float				ShoulderElbow, ElbowHand;
	MATH_tdst_Vector	*ElbowConstraint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject( Elbow );
	ElbowConstraint = AI_PopVectorPtr();
	ElbowHand		= AI_PopFloat();
	ShoulderElbow	= AI_PopFloat();
	Shoulder		= AI_PopGameObject();
	Hand			= AI_PopGameObject();
	
	AI_EvalFunc_OBJIKCompute_C( Elbow, Shoulder, Hand, ShoulderElbow, ElbowHand, ElbowConstraint);

	return ++_pst_Node;
}

float	OBJ_gf_EYEFOLLOW_HeadZMax;
float	OBJ_gf_EYEFOLLOW_HeadZMin;
float	OBJ_gf_EYEFOLLOW_HeadXMax;
float	OBJ_gf_EYEFOLLOW_HeadXMin;
float	OBJ_gf_EYEFOLLOW_TorsoZMax;
float	OBJ_gf_EYEFOLLOW_TorsoZMin;
float	OBJ_gf_EYEFOLLOW_HeadXOffset;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_EyeFollowParamSet_C
(
	float	_f_Head_Z_Max,
	float	_f_Head_Z_Min,
	float	_f_Head_X_Max,
	float	_f_Head_X_Min,
	float	_f_Torso_Z_Max,
	float	_f_Torso_Z_Min,
	float	_f_Head_X_Offset
)
{
	OBJ_gf_EYEFOLLOW_HeadZMax = _f_Head_Z_Max;
	OBJ_gf_EYEFOLLOW_HeadZMin = _f_Head_Z_Min;
	OBJ_gf_EYEFOLLOW_HeadXMax = _f_Head_X_Max;
	OBJ_gf_EYEFOLLOW_HeadXMin = _f_Head_X_Min;
	OBJ_gf_EYEFOLLOW_TorsoZMax = _f_Torso_Z_Max;
	OBJ_gf_EYEFOLLOW_TorsoZMin = _f_Torso_Z_Min;
	OBJ_gf_EYEFOLLOW_HeadXOffset = _f_Head_X_Offset;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_EyeFollowParamSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	float	_f_Head_Z_Max;
	float	_f_Head_Z_Min;
	float	_f_Head_X_Max;
	float	_f_Head_X_Min;
	float	_f_Torso_Z_Max;
	float	_f_Torso_Z_Min;
	float	_f_Head_X_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	_f_Head_X_Offset = AI_PopFloat();
	_f_Torso_Z_Min = AI_PopFloat();
	_f_Torso_Z_Max = AI_PopFloat();
	_f_Head_X_Min = AI_PopFloat();
	_f_Head_X_Max = AI_PopFloat();
	_f_Head_Z_Min = AI_PopFloat();
	_f_Head_Z_Max = AI_PopFloat();

	AI_EvalFunc_OBJ_EyeFollowParamSet_C
	(
		_f_Head_Z_Max,
		_f_Head_Z_Min,
		_f_Head_X_Max,
		_f_Head_X_Min,
		_f_Torso_Z_Max,
		_f_Torso_Z_Min,
		_f_Head_X_Offset
	);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJEyeFollow_C
(
	OBJ_tdst_GameObject *pst_GO,
	MATH_tdst_Vector	*pst_Target,
	float				f_Blend,
	ULONG				ul_ActorID
)
{
	/*~~~~~~~~~~~~~~*/
	BOOL	b_FlagX;
	BOOL	b_FlagXX;
	BOOL	b_FlagXXX;
	BOOL	b_FlagX4;
	BOOL	b_FlagX5;
	BOOL	b_FlagX6;
	/*~~~~~~~~~~~~~~*/

	b_FlagX = ul_ActorID & 0x80000000;
	ul_ActorID &= ~0x80000000;

	b_FlagXX = ul_ActorID & 0x40000000;
	ul_ActorID &= ~0x40000000;

	b_FlagXXX = ul_ActorID & 0x20000000;
	ul_ActorID &= ~0x20000000;

	b_FlagX4 = ul_ActorID & 0x10000000;
	ul_ActorID &= ~0x10000000;

	b_FlagX5 = ul_ActorID & 0x08000000;
	ul_ActorID &= ~0x08000000;

	b_FlagX6 = ul_ActorID & 0x04000000;
	ul_ActorID &= ~0x04000000;


	OBJ_EyeFollow(pst_GO, pst_Target, f_Blend, ul_ActorID, b_FlagX, b_FlagXX, b_FlagXXX, b_FlagX4, b_FlagX5, b_FlagX6);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJEyeFollow(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Blend;
	MATH_tdst_Vector	st_Target;
	ULONG				ul_ActorID;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_ActorID = AI_PopInt();
	f_Blend = AI_PopFloat();
	AI_PopVector(&st_Target);
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJEyeFollow_C(pst_GO, &st_Target, f_Blend, ul_ActorID);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_FreezeModifier_C(OBJ_tdst_GameObject *_pt_GO, int type, int on)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*po_Mdf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Mdf = MDF_pst_GetByType(_pt_GO, type);
	if(!po_Mdf) return;
	if(on)
		po_Mdf->ul_Flags |= MDF_C_Modifier_Inactive;
	else
		po_Mdf->ul_Flags &= ~MDF_C_Modifier_Inactive;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJFreezeModifier(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					type, on;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	on = AI_PopInt();
	type = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO->pst_Extended, "No extended struct allocated");
	AI_Check(pst_GO->pst_Extended->pst_Modifiers, "No modifiers");
	OBJ_FreezeModifier_C(pst_GO, type, on);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int OBJ_ModifierType_C(OBJ_tdst_GameObject *_pt_GO, int type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*po_Mdf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	po_Mdf = MDF_pst_GetByType(_pt_GO, type);
	if(po_Mdf)
		return 1;
	else
		return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJModifierType(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	type = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	if(!pst_GO->pst_Extended || !pst_GO->pst_Extended->pst_Modifiers) AI_PushInt(0);

	AI_PushInt(OBJ_ModifierType_C(pst_GO, type));

	return ++_pst_Node;
}
#ifdef JADEFUSION
extern ULONG	IMG_ObjectAnalyser
				(
	OBJ_tdst_GameObject		*_pst_GO,
	WOR_tdst_World			*_pst_World,
	GDI_tdst_DisplayData	*_pst_DD,
	ULONG					_ul_MissionMask,
	BOOL					_b_RayCast,
	BOOL					_b_NoDecal,
	BOOL					_b_SkipMissionTest,
	ULONG					*_pul_Return
				);
#else
extern ULONG	IMG_ObjectAnalyser
				(
					OBJ_tdst_GameObject *,
					WOR_tdst_World *,
					GDI_tdst_DisplayData *,
					ULONG,
					BOOL,
					BOOL,
					BOOL,
					CHAR *
				);
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL AI_EvalFunc_OBJHasPhotoValue_C(OBJ_tdst_GameObject *_pst_GO, int _ul_Mission, int *_pc_Result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	GDI_tdst_DisplayData	*pst_DD;
	ULONG					ul_MissionMask;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	if(!pst_World) return 0;

	pst_DD = (GDI_tdst_DisplayData*)pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas;

	if(_ul_Mission == -1)
		ul_MissionMask = -1;
	else
		ul_MissionMask = _ul_Mission << 24;

	if(IMG_ObjectAnalyser(_pst_GO, pst_World, pst_DD, ul_MissionMask, TRUE, FALSE, FALSE, (ULONG *) _pc_Result))
		return 1;
	else
		return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJHasPhotoValue(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Arr;
	AI_tdst_UnionVar	s_Val;
	OBJ_tdst_GameObject *pst_GO;
	ULONG				ul_Mission;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&s_Val, &st_Arr);
	ul_Mission = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	if(AI_EvalFunc_OBJHasPhotoValue_C(pst_GO, (int) ul_Mission, (int *) st_Arr.pv_Addr))
		AI_PushInt(1);
	else
		AI_PushInt(0);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL AI_EvalFunc_OBJCentred_C(OBJ_tdst_GameObject *_pst_GO, int *_pc_Result)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World			*pst_World;
	GDI_tdst_DisplayData	*pst_DD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_World = WOR_World_GetWorldOfObject(_pst_GO);
	if(!pst_World) return FALSE;

	pst_DD = (GDI_tdst_DisplayData*)pst_World->pst_View[0].st_DisplayInfo.pst_DisplayDatas;

#ifdef JADEFUSION
	if(IMG_ObjectAnalyser(_pst_GO, pst_World, pst_DD, -1, TRUE, FALSE, TRUE, (ULONG *) _pc_Result))
#else
		if(IMG_ObjectAnalyser(_pst_GO, pst_World, pst_DD, -1, TRUE, FALSE, TRUE, (char *) _pc_Result))
#endif
		return 1;
	else
		return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJCentred(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_PushVar		st_Arr;
	AI_tdst_UnionVar	s_Val;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVar(&s_Val, &st_Arr);
	AI_M_GetCurrentObject(pst_GO);

	if(AI_EvalFunc_OBJCentred_C(pst_GO, (int *) st_Arr.pv_Addr))
		AI_PushInt(1);
	else
		AI_PushInt(0);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG AI_EvalFunc_OBJ_ColMapOptimize_C(OBJ_tdst_GameObject *_pst_GO)
{
	if
	(
		!(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_ColMap))
	||	!_pst_GO->pst_Extended
	||	!(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap)
	||	!(((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob)
	)
	{
		return 0;
	}

	return((((COL_tdst_Base *) _pst_GO->pst_Extended->pst_Col)->pst_ColMap->dpst_Cob[0])->uc_Flag & COL_C_Cob_ReadyForCamera);
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_ColMapOptimize(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_OBJ_ColMapOptimize_C(pst_GO));

	return ++_pst_Node;
}

extern MDF_tdst_Modifier	*GAO_ModifierPhoto_Get(OBJ_tdst_GameObject *, BOOL);
extern OBJ_tdst_GameObject	*GAO_ModifierPhoto_SnapGOGet(OBJ_tdst_GameObject *, GAO_tdst_ModifierPhoto *, int);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_InfoPhotoPivotGet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Pivot)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	OBJ_tdst_GameObject *pst_SnapGO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = GAO_ModifierPhoto_Get(_pst_GO, FALSE);
	if(pst_Mod)
	{
		pst_SnapGO = GAO_ModifierPhoto_SnapGOGet(_pst_GO, (GAO_tdst_ModifierPhoto*)pst_Mod->p_Data, 1);
		MATH_TransformVertex(_pst_Pivot, (MATH_tdst_Matrix*)((GAO_tdst_ModifierPhoto *)pst_SnapGO->pst_GlobalMatrix), &((GAO_tdst_ModifierPhoto *)pst_Mod->p_Data)->st_InfoOffset);
	}
	else
	{
		/* No Info-Photo Modifier, return center of BV */
		if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			OBJ_tdst_GameObject *pst_HeadGO;
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_HeadGO = ANI_pst_GetObjectByAICanal(_pst_GO, 0);
			if(pst_HeadGO)
			{
				OBJ_BV_ComputeCenter(pst_HeadGO, _pst_Pivot);
				return;
			}
		}

		OBJ_BV_ComputeCenter(_pst_GO, _pst_Pivot);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_InfoPhotoPivotGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Pivot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_OBJ_InfoPhotoPivotGet_C(pst_GO, &st_Pivot);

	AI_PushVector(&st_Pivot);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_InfoPhotoMissionGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = GAO_ModifierPhoto_Get(_pst_GO, FALSE);
	if(pst_Mod)
		return(((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->i_Mission);
	else
		return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_InfoPhotoMissionGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_OBJ_InfoPhotoMissionGet_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_OBJ_InfoPhotoInfoGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = GAO_ModifierPhoto_Get(_pst_GO, FALSE);
	if(pst_Mod)
		return(((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->i_Info);
	else
		return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_InfoPhotoInfoGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_OBJ_InfoPhotoInfoGet_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_OBJ_InfoPhotoLODGet_C(OBJ_tdst_GameObject *_pst_GO, int mode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = GAO_ModifierPhoto_Get(_pst_GO, FALSE);
	if(!pst_Mod) return -1;

	switch(mode)
	{
	case 0:
		return(((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_CurrentLOD);
		break;

	case 1:
		return(((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_LODMin);
		break;


	case 2:
		return(((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_LODMax);
		break;

	}

	return -1.0f;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_InfoPhotoLODGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					mode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	mode = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushFloat(AI_EvalFunc_OBJ_InfoPhotoLODGet_C(pst_GO, mode));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_OBJ_InfoPhotoFrameGet_C(OBJ_tdst_GameObject *_pst_GO, int mode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = GAO_ModifierPhoto_Get(_pst_GO, FALSE);
	if(!pst_Mod) return -1.0f;

	switch(mode)
	{
	case 0:
		return(((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_CurrentFrame);
		break;

	case 1:
		return(((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_FrameMin);
		break;


	case 2:
		return(((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_FrameMax);
		break;

	}

	return -1.0f;

}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_InfoPhotoFrameGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					mode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	mode = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushFloat(AI_EvalFunc_OBJ_InfoPhotoFrameGet_C(pst_GO, mode));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_InfoPhotoParamSet_C
(
	OBJ_tdst_GameObject *_pst_GO,
	int _i_Mission,
	int _i_Info,
	int _i_BoneForSpherePivot,
	int _i_BoneForInfoPivot,
	float _f_LODMin,
	float _f_LODMax,
	float _f_FrameMin,
	float _f_Radius
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	MDF_tdst_Modifier	**ppst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = GAO_ModifierPhoto_Get(_pst_GO, FALSE);
	if(!pst_Mod)
	{
		OBJ_GameObject_CreateExtendedIfNot(_pst_GO);
		ppst_Mod = &_pst_GO->pst_Extended->pst_Modifiers;

		while(*ppst_Mod)
		{
			ppst_Mod = &((*ppst_Mod)->pst_Next);
		}

		*ppst_Mod = MDF_pst_Modifier_Create(_pst_GO, MDF_C_Modifier_InfoPhoto, NULL);
		pst_Mod = *ppst_Mod;

		pst_Mod->ul_Flags |= MDF_C_Modifier_ApplyGao;
		pst_Mod->pst_GO = _pst_GO;

	}

	((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_Radius = _f_Radius;
	((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_FrameMin = _f_FrameMin;
	((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_FrameMax = 1.001f;
	((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_LODMin = _f_LODMin;
	((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->f_LODMax = _f_LODMax;
	((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->i_BoneForSpherePivot = _i_BoneForSpherePivot;
	((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->i_BoneForInfoPivot = _i_BoneForInfoPivot;
	((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->i_Info = _i_Info;
	((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->i_Mission = _i_Mission;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_InfoPhotoParamSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Mission;
	int					i_Info;
	int					i_BoneForSpherePivot;
	int					i_BoneForInfoPivot;
	float				f_LODMin, f_LODMax;
	float				f_FrameMin;
	float				f_Radius;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	f_Radius = AI_PopFloat();
	f_FrameMin = AI_PopFloat();
	f_LODMax = AI_PopFloat();
	f_LODMin = AI_PopFloat();
	i_BoneForInfoPivot = AI_PopInt();
	i_BoneForSpherePivot = AI_PopInt();
	i_Info = AI_PopInt();
	i_Mission = AI_PopInt();

	AI_EvalFunc_OBJ_InfoPhotoParamSet_C
	(
		pst_GO,
		i_Mission,
		i_Info,
		i_BoneForSpherePivot,
		i_BoneForInfoPivot,
		f_LODMin,
		f_LODMax,
		f_FrameMin,
		f_Radius
	);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_InfoPhotoSphereOffsetSet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Offset)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = GAO_ModifierPhoto_Get(_pst_GO, FALSE);
	if(pst_Mod)
	{
		MATH_CopyVector( &(((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->st_SphereOffset), _pst_Offset);
		return;
	}
#ifdef ACTIVE_EDITORS
	LINK_PrintStatusMsg("Create/UnFreeze the Info-Photo Modifier before initializing the Info-Photo Sphere Offset");
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_InfoPhotoSphereOffsetSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v;	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&v);
	AI_EvalFunc_OBJ_InfoPhotoSphereOffsetSet_C(pst_GO, &v);
	return ++_pst_Node;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_InfoPhotoInfoOffsetSet_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Offset)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = GAO_ModifierPhoto_Get(_pst_GO, FALSE);
	if(pst_Mod)
	{
		MATH_CopyVector( &(((GAO_tdst_ModifierPhoto *) pst_Mod->p_Data)->st_InfoOffset), _pst_Offset);
		return;
	}
#ifdef ACTIVE_EDITORS
	LINK_PrintStatusMsg("Create/UnFreeze the Info-Photo Modifier before initializing the Info-Photo Info Offset");
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_InfoPhotoInfoOffsetSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	v;	
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&v);
	AI_EvalFunc_OBJ_InfoPhotoInfoOffsetSet_C(pst_GO, &v);
	return ++_pst_Node;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_InfoPhotoFlagsSet_C(OBJ_tdst_GameObject *_pst_GO, ULONG On, ULONG Off)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = GAO_ModifierPhoto_Get(_pst_GO, FALSE);
	if(pst_Mod)
	{
		((GAO_tdst_ModifierPhoto *)pst_Mod->p_Data)->ul_Flags |= On;
		((GAO_tdst_ModifierPhoto *)pst_Mod->p_Data)->ul_Flags &= ~Off;
		return;
	}
#ifdef ACTIVE_EDITORS
	LINK_PrintStatusMsg("Create/UnFreeze the Info-Photo Modifier before setting Flags");
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_InfoPhotoFlagsSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	ULONG				On, Off;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Off = AI_PopInt();
	On = AI_PopInt();
	AI_EvalFunc_OBJ_InfoPhotoFlagsSet_C(pst_GO, On, Off);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_OBJ_InfoPhotoRadiusGet_C(OBJ_tdst_GameObject *_pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Mod;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Mod = GAO_ModifierPhoto_Get(_pst_GO, FALSE);
	if(pst_Mod) return ((GAO_tdst_ModifierPhoto *)pst_Mod->p_Data)->f_Radius;
	return -1.0f;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_InfoPhotoRadiusGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_OBJ_InfoPhotoRadiusGet_C(pst_GO));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_ShadowColorSet_C(OBJ_tdst_GameObject *_pst_GO, int Color)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier	*pst_Modifier;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO || !_pst_GO->pst_Extended || !_pst_GO->pst_Extended->pst_Modifiers)
		return;

	pst_Modifier = _pst_GO->pst_Extended->pst_Modifiers;
	while(pst_Modifier)
	{
			if
			(
				(
					pst_Modifier->ul_Flags &
						(
							MDF_C_Modifier_ApplyGao | MDF_C_Modifier_Inactive | MDF_C_Modifier_ApplyGen |
								MDF_C_Modifier_NoApply
						)
				) == 0
				&& 
				(pst_Modifier->i->ul_Type == MDF_C_Modifier_Shadow)
			)
			{

				((GAO_tdst_ModifierSDW *)pst_Modifier->p_Data)->ulShadowColor = Color ^ 0xFFFFFFFF;
				return;
			}

			pst_Modifier = pst_Modifier->pst_Next;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_ShadowColorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_ShadowColorSet_C(pst_GO, AI_PopInt());
	return ++_pst_Node;
}




/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_OBJ_DrawAtEnd_C(OBJ_tdst_GameObject *_pst_GO)
{
	_pst_GO->ul_IdentityFlags |= OBJ_C_IdentityFlag_DrawAtEnd;
}
/**/
AI_tdst_Node *AI_EvalFunc_OBJ_DrawAtEnd(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_OBJ_DrawAtEnd_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */

int AI_EvalFunc_OBJ_PointInGameObject_C(OBJ_tdst_GameObject *_pst_GO, MATH_tdst_Vector *_pst_Point)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix		st_33_InvertMatrix, st_33_GlobalMatrix;
	MATH_tdst_Vector		st_Max, st_Min;
	MATH_tdst_Vector		st_OCS_Point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO || !_pst_Point) return FALSE;

	if
	(
		(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims) 
	&& _pst_GO->pst_Base 
	&& _pst_GO->pst_Base->pst_GameObjectAnim 
	&& _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
	)
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		OBJ_tdst_Group			*pst_Skeleton;
		TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
		OBJ_tdst_GameObject		*pst_BoneGO;
		int						i;
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		pst_Skeleton = _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;

		pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
		pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
		for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
		{
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
			ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));

			/* Skip Bones without OBBox */
			if(!(pst_BoneGO->ul_IdentityFlags & OBJ_C_IdentityFlag_OBBox))
				continue;

			/* Creates a 3x3 matrix containing the possible scale of the object */
			MATH_MakeOGLMatrix(&st_33_GlobalMatrix, pst_BoneGO->pst_GlobalMatrix);
			MATH_ClearScale(&st_33_GlobalMatrix, 1);

			/* Inverts the 3x3 matrix */
			MATH_Invert33Matrix(&st_33_InvertMatrix, &st_33_GlobalMatrix);

			MATH_ClearScale(&st_33_InvertMatrix, 1);
			MATH_NegVector(MATH_pst_GetTranslation(&st_33_InvertMatrix), MATH_pst_GetTranslation(pst_BoneGO->pst_GlobalMatrix));
			MATH_TransformVector(&st_33_InvertMatrix.T, &st_33_InvertMatrix, &st_33_InvertMatrix.T);
			
			MATH_TransformVertex(&st_OCS_Point, &st_33_InvertMatrix, _pst_Point);

			MATH_CopyVector(&st_Max, OBJ_pst_BV_GetLMax(pst_BoneGO->pst_BV));
			MATH_CopyVector(&st_Min, OBJ_pst_BV_GetLMin(pst_BoneGO->pst_BV));

			if
			(
				((st_OCS_Point.x >= st_Min.x) && (st_OCS_Point.x <= st_Max.x))
			&&	((st_OCS_Point.y >= st_Min.y) && (st_OCS_Point.y <= st_Max.y))
			&&	((st_OCS_Point.z >= st_Min.z) && (st_OCS_Point.z <= st_Max.z))
			)
			{
				return TRUE;
			}
		}
	}
	else 
	{
		if(_pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_OBBox)
		{
			/* Creates a 3x3 matrix containing the possible scale of the object */
			MATH_MakeOGLMatrix(&st_33_GlobalMatrix, _pst_GO->pst_GlobalMatrix);
			MATH_ClearScale(&st_33_GlobalMatrix, 1);

			/* Inverts the 3x3 matrix */
			MATH_Invert33Matrix(&st_33_InvertMatrix, &st_33_GlobalMatrix);

			MATH_ClearScale(&st_33_InvertMatrix, 1);
			MATH_NegVector(MATH_pst_GetTranslation(&st_33_InvertMatrix), MATH_pst_GetTranslation(_pst_GO->pst_GlobalMatrix));
			MATH_TransformVector(&st_33_InvertMatrix.T, &st_33_InvertMatrix, &st_33_InvertMatrix.T);
			
			MATH_TransformVertex(&st_OCS_Point, &st_33_InvertMatrix, _pst_Point);

			MATH_CopyVector(&st_Max, OBJ_pst_BV_GetLMax(_pst_GO->pst_BV));
			MATH_CopyVector(&st_Min, OBJ_pst_BV_GetLMin(_pst_GO->pst_BV));

			if
			(
				((st_OCS_Point.x >= st_Min.x) && (st_OCS_Point.x <= st_Max.x))
			&&	((st_OCS_Point.y >= st_Min.y) && (st_OCS_Point.y <= st_Max.y))
			&&	((st_OCS_Point.z >= st_Min.z) && (st_OCS_Point.z <= st_Max.z))
			)
			{
				return TRUE;
			}
		}
	}
	
	return FALSE;

}

/**/
AI_tdst_Node *AI_EvalFunc_OBJ_PointInGameObject(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	point;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_PopVector( &point );
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_OBJ_PointInGameObject_C(pst_GO, &point));
	return ++_pst_Node;
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
