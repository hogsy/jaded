/*$T OBJmain.c GC! 1.100 02/01/01 12:29:33 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "OBJ.h"
#include "OBJBoundingVolume.h"
#include "OBJmain.h"
#include "OBJgizmo.h"
#include "OBJslowaccess.h"
#include "BASe/MEMory/MEM.h"
#include "ENGine/Sources/WORld/WOR.h"
#include "ENGine/Sources/WORld/WORsave.h"
#include "ENGine/Sources/WORld/WORmain.h"
#include "ENGine/Sources/EOT/EOT.h"
#include "ENGine/Sources/WAYs/WAYinit.h"
#include "ENGine/Sources/COLlision/COLconst.h"
#include "ENGine/Sources/COLlision/COLinit.h"
#include "ENGine/Sources/COLlision/COLedit.h"
#include "ENGine/Sources/COLlision/COLcob.h"
#include "ENGine/Sources/INTersection/INTSnP.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/GRP/GRPmain.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AImsg.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "LINks/LINKtoed.h"
#include "ENGine/Sources/DYNamics/DYNinit.h"
#include "ENGine/Sources/DYNamics/DYNconst.h"
#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDbank.h"
#include "Edipaths.h"
#ifdef JADEFUSION
#include "Engine/Sources/Wind/CurrentWind.h"
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

extern void AI_EvalFunc_MSGClear_C(OBJ_tdst_GameObject *);

/*
 =======================================================================================================================
    Aim:    Update the status flag OBJ_C_StatusFlag_RTL that indicates if an object shoud be real-time lightned or not,
            according to the current identity flags and the control flags
 =======================================================================================================================
 */
void OBJ_UpdateStatusFlagRTL(OBJ_tdst_GameObject *_pst_Object)
{
	/*~~~~~~~~~~~~~~~*/
	ULONG	ul_IdFlags;
	/*~~~~~~~~~~~~~~~*/

	ul_IdFlags = OBJ_ul_FlagsIdentityGet(_pst_Object);

	if((ul_IdFlags & OBJ_C_IdentityFlag_Visu) || (ul_IdFlags & OBJ_C_IdentityFlag_Anims))
	{
		/*
		 * Object that have an animation, a dynamic or an AI are real-time lighted by
		 * default
		 */
		if
		(
			(ul_IdFlags & OBJ_C_IdentityFlag_Dyna)
		||	(ul_IdFlags & OBJ_C_IdentityFlag_AI)
		||	(ul_IdFlags & OBJ_C_IdentityFlag_Anims)
		||	(OBJ_b_TestControlFlag(_pst_Object, OBJ_C_ControlFlag_ForceRTL))
		)
		{
			OBJ_SetStatusFlag(_pst_Object, OBJ_C_StatusFlag_RTL);
		}
		else
			OBJ_ClearStatusFlag(_pst_Object, OBJ_C_StatusFlag_RTL);

		if(OBJ_b_TestControlFlag(_pst_Object, OBJ_C_ControlFlag_ForceNoRTL))
		{
			OBJ_ClearStatusFlag(_pst_Object, OBJ_C_StatusFlag_RTL);
		}
	}
	else
	{
		/* No visual, no real-time lightning */
		OBJ_ClearStatusFlag(_pst_Object, OBJ_C_StatusFlag_RTL);
	}
}

/*
 =======================================================================================================================
    Aim:    Change the identity flags of an object. It allocates/desallocates acording to the new identity flags

    Note:   WARNING. this function takes some time. Try not to change the identity of an object too often in real time £
            1. When changing from oriented to non oriented (and reciprocal) the global position of the object is kept £
 =======================================================================================================================
 */
void OBJ_ChangeIdentityFlags(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flag, ULONG _ul_OldFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna		*pst_NewDyna;
	WOR_tdst_World		*pst_CurrentWorld;
	BOOL				b_BV_HasChanged;
	GRO_tdst_Struct		*pst_Gro;
	OBJ_tdst_GameObject *pst_Cur;
	MATH_tdst_Matrix	M, M1;
	int					i;
#ifdef ACTIVE_EDITORS
	char				sz_Path[BIG_C_MaxLenPath];
#endif
	TAB_tdst_Ptable		*pst_PTable;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_BV_HasChanged = FALSE;

	/* Avoid errors */
	if(!_pst_Object) return;
	pst_CurrentWorld = WOR_World_GetWorldOfObject(_pst_Object);

	/* If No OldFlags specified, than the Old flags are the current flags */
	if(_ul_OldFlags == OBJ_C_UnvalidFlags) _ul_OldFlags = _pst_Object->ul_IdentityFlags;

	/* force additionnal matrix */
	if(_ul_Flag & OBJ_C_IdentityFlag_AddMatArePointer)
		_ul_Flag |= OBJ_C_IdentityFlag_AdditionalMatrix;

	/* Force base allocation ? */
	if
	(
		(_ul_Flag & OBJ_C_IdentityFlag_Visu)
	||	(_ul_Flag & OBJ_C_IdentityFlag_Anims)
	||	(_ul_Flag & OBJ_C_IdentityFlag_Dyna)
	||	(_ul_Flag & OBJ_C_IdentityFlag_AdditionalMatrix)
	||	(_ul_Flag & OBJ_C_IdentityFlag_AddMatArePointer)
	||	(_ul_Flag & OBJ_C_IdentityFlag_Hierarchy)
	)
	{
		_ul_Flag |= OBJ_C_IdentityFlag_BaseObject;
	}

	if(_ul_Flag & OBJ_C_IdentityFlag_BaseObject)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_BaseObject))
		{
			_pst_Object->pst_Base = (OBJ_tdst_Base *) MEM_p_Alloc(sizeof(OBJ_tdst_Base));
			L_memset(_pst_Object->pst_Base, 0, sizeof(OBJ_tdst_Base));
		}
	}


#ifdef ODE_INSIDE
	if((_ul_Flag & OBJ_C_IdentityFlag_ODE) && !(_ul_OldFlags & OBJ_C_IdentityFlag_ODE))
	{
		if(!(_ul_Flag & OBJ_C_IdentityFlag_BaseObject))
		{
			_ul_Flag |= OBJ_C_IdentityFlag_BaseObject;

			_pst_Object->pst_Base = (OBJ_tdst_Base *) MEM_p_Alloc(sizeof(OBJ_tdst_Base));
			L_memset(_pst_Object->pst_Base, 0, sizeof(OBJ_tdst_Base));
		}

		_pst_Object->pst_Base->pst_ODE = (DYN_tdst_ODE *) MEM_p_Alloc(sizeof(DYN_tdst_ODE));
		L_memset(_pst_Object->pst_Base->pst_ODE, 0, sizeof(DYN_tdst_ODE));

#ifdef ACTIVE_EDITORS
		_pst_Object->pst_Base->pst_ODE->pst_GO = _pst_Object;
#endif


		_pst_Object->pst_Base->pst_ODE->uc_Type = 0;

		/* Default Flags for ODE */
		_pst_Object->pst_Base->pst_ODE->uc_Flags |= ODE_FLAGS_ENABLE + ODE_FLAGS_AUTODISABLE;

		/* Sound */
		_pst_Object->pst_Base->pst_ODE->uc_Sound = 0;

		_pst_Object->pst_Base->pst_ODE->ode_id_geom = dCreateSphere(pst_CurrentWorld->ode_id_space, 1.0f);
		_pst_Object->pst_Base->pst_ODE->ode_id_body = 0;

		dGeomSetData(_pst_Object->pst_Base->pst_ODE->ode_id_geom, (void *) _pst_Object);
		dGeomSetBody(_pst_Object->pst_Base->pst_ODE->ode_id_geom, 0);

		_pst_Object->pst_Base->pst_ODE->SurfaceMode = dContactBounce;
		_pst_Object->pst_Base->pst_ODE->mu = 5000.0f;
		_pst_Object->pst_Base->pst_ODE->mu2 = 0.0f;
		_pst_Object->pst_Base->pst_ODE->bounce = 0.01f;
		_pst_Object->pst_Base->pst_ODE->bounce_vel = 0.0f;
		_pst_Object->pst_Base->pst_ODE->soft_erp = 0.0f;
		_pst_Object->pst_Base->pst_ODE->soft_cfm = 0.0f;
		_pst_Object->pst_Base->pst_ODE->motion1 = 0.0f;
		_pst_Object->pst_Base->pst_ODE->motion2 = 0.0f;
		_pst_Object->pst_Base->pst_ODE->slip1 = 0.0f;
		_pst_Object->pst_Base->pst_ODE->slip2 = 0.0f;

		//set RotMatrix to Identity
		MATH_SetIdentityMatrix(&_pst_Object->pst_Base->pst_ODE->st_RotMatrix);

	}

	if(!(_ul_Flag & OBJ_C_IdentityFlag_ODE) && (_ul_OldFlags & OBJ_C_IdentityFlag_ODE))
	{
		if(_pst_Object->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_MATHCOLMAP)
			dGeomDestroy(_pst_Object->pst_Base->pst_ODE->ode_id_geom);
		if(_pst_Object->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY)
			dBodyDestroy (_pst_Object->pst_Base->pst_ODE->ode_id_body);

		MEM_Free(_pst_Object->pst_Base->pst_ODE);
		_pst_Object->pst_Base->pst_ODE = NULL;
	}

#endif

	/* Force extended allocation ? */
	if
	(
		(_ul_Flag & OBJ_C_IdentityFlag_AI)
	||	(_ul_Flag & OBJ_C_IdentityFlag_DesignStruct)
	||	(_ul_Flag & OBJ_C_IdentityFlag_Msg)
	||	(_ul_Flag & OBJ_C_IdentityFlag_Lights)
	||	(_ul_Flag & OBJ_C_IdentityFlag_ColMap)
	||	(_ul_Flag & OBJ_C_IdentityFlag_ZDM)
	||	(_ul_Flag & OBJ_C_IdentityFlag_ZDE)
	||	(_ul_Flag & OBJ_C_IdentityFlag_Links)
	||	(_ul_Flag & OBJ_C_IdentityFlag_Events)
	||	(_ul_Flag & OBJ_C_IdentityFlag_Sound)
	)
	{
		_ul_Flag |= OBJ_C_IdentityFlag_ExtendedObject;
	}

	if(_ul_Flag & OBJ_C_IdentityFlag_ExtendedObject)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_ExtendedObject))
		{
			OBJ_GameObject_CreateExtended(_pst_Object);
		}
	}
	

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_HasInitialPos flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */
#ifdef USE_DOUBLE_RENDERING
#define ENG_BDL_RDNG 2
#else
#define ENG_BDL_RDNG 0
#endif	 

	if(_ul_Flag & OBJ_C_IdentityFlag_HasInitialPos)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_HasInitialPos))
		{
			_pst_Object->ul_IdentityFlags |= OBJ_C_IdentityFlag_HasInitialPos;
			
			if(_ul_Flag & OBJ_C_IdentityFlag_FlashMatrix)
			{
#if defined(_XBOX) || defined(_XENON)
				_pst_Object->pst_GlobalMatrix = (MATH_tdst_Matrix *) MEM_p_ReallocAlign
					(
						_pst_Object->pst_GlobalMatrix,
						(3 + ENG_BDL_RDNG) * sizeof(MATH_tdst_Matrix),
						16
					);
#else
				_pst_Object->pst_GlobalMatrix = (MATH_tdst_Matrix *) MEM_p_Realloc
					(
						_pst_Object->pst_GlobalMatrix,
						(3 + ENG_BDL_RDNG) * sizeof(MATH_tdst_Matrix)
					);
#endif
				/*
				 * The Flash Matrix is now at the Third Place. Copy it from the second place to
				 * the third.
				 */
				MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix + 2, _pst_Object->pst_GlobalMatrix + 1);

				OBJ_SetInitialAbsoluteMatrix(_pst_Object, _pst_Object->pst_GlobalMatrix);
			}
			else
			{
#if defined(_XBOX) || defined(_XENON)
				_pst_Object->pst_GlobalMatrix = (MATH_tdst_Matrix *) MEM_p_ReallocAlign
					(
						_pst_Object->pst_GlobalMatrix,
						(2 + ENG_BDL_RDNG) * sizeof(MATH_tdst_Matrix),
						16
					);
#else
				_pst_Object->pst_GlobalMatrix = (MATH_tdst_Matrix *) MEM_p_Realloc
					(
						_pst_Object->pst_GlobalMatrix,
						(2 + ENG_BDL_RDNG) * sizeof(MATH_tdst_Matrix)
					);
#endif
				OBJ_SetInitialAbsoluteMatrix(_pst_Object, _pst_Object->pst_GlobalMatrix);
			}
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_HasInitialPos)
	{
		if(_ul_Flag & OBJ_C_IdentityFlag_FlashMatrix)
		{
			/*
			 * The Flash Matrix is now at the Second Place. Copy it from the third place to
			 * the second.
			 */
			MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix + 1, _pst_Object->pst_GlobalMatrix + 2);
#if defined(_XBOX) || defined(_XENON)
			_pst_Object->pst_GlobalMatrix = (MATH_tdst_Matrix *) MEM_p_ReallocAlign
				(
					_pst_Object->pst_GlobalMatrix,
					(2 + ENG_BDL_RDNG) * sizeof(MATH_tdst_Matrix),
					16
				);
#else
			_pst_Object->pst_GlobalMatrix = (MATH_tdst_Matrix *) MEM_p_Realloc
				(
					_pst_Object->pst_GlobalMatrix,
					(2 + ENG_BDL_RDNG) * sizeof(MATH_tdst_Matrix)
				);
#endif
		}
		else
		{
#if defined(_XBOX) || defined(_XENON)
			_pst_Object->pst_GlobalMatrix = (MATH_tdst_Matrix *) MEM_p_ReallocAlign
				(
					_pst_Object->pst_GlobalMatrix,
					(1 + ENG_BDL_RDNG) * sizeof(MATH_tdst_Matrix),
					16
				);
#else
			_pst_Object->pst_GlobalMatrix = (MATH_tdst_Matrix *) MEM_p_Realloc
				(
					_pst_Object->pst_GlobalMatrix,
					(1 + ENG_BDL_RDNG) * sizeof(MATH_tdst_Matrix)
				);
#endif
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Object with a visu
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_Visu)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_Visu))
		{
			_pst_Object->pst_Base->pst_Visu = (GRO_tdst_Visu *) MEM_p_Alloc(sizeof(GRO_tdst_Visu));
			L_memset(_pst_Object->pst_Base->pst_Visu, 0, sizeof(GRO_tdst_Visu));
			_pst_Object->pst_Base->pst_Visu->ul_DrawMask = 0xFFFFFFFF;
#ifdef JADEFUSION
			_pst_Object->pst_Base->pst_Visu->ul_DrawMask &= ~(GDI_Cul_DM_EmitShadowBuffer | GDI_Cul_DM_ReceiveShadowBuffer);
#endif
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_Visu)
	{
		if((pst_Gro = _pst_Object->pst_Base->pst_Visu->pst_Object) != NULL)
		{
			pst_Gro->i->pfn_AddRef(pst_Gro, -1);
			pst_Gro->i->pfn_Destroy(pst_Gro);
		}

		if((pst_Gro = _pst_Object->pst_Base->pst_Visu->pst_Material) != NULL)
		{
			pst_Gro->i->pfn_AddRef(pst_Gro, -1);
			pst_Gro->i->pfn_Destroy(pst_Gro);
		}

		OBJ_VertexColor_Free( _pst_Object );
		//if(_pst_Object->pst_Base->pst_Visu->dul_VertexColors)
		//		MEM_Free(_pst_Object->pst_Base->pst_Visu->dul_VertexColors);
		
		MEM_Free(_pst_Object->pst_Base->pst_Visu);
		_pst_Object->pst_Base->pst_Visu = NULL;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Object with anims
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_Anims)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_Anims))
		{
			_pst_Object->pst_Base->pst_GameObjectAnim = ANI_pst_CreateGameObjectAnim();
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_Anims)
	{
		ANI_Free(_pst_Object->pst_Base->pst_GameObjectAnim);
		_pst_Object->pst_Base->pst_GameObjectAnim = NULL;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_Dyna flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_Dyna)
	{
		/* Is the object not already with a dynamic ? */
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_Dyna))
		{
			/*
			 * We create a new dynamic structure, initilise it and store the pointer in the
			 * game object
			 */
			pst_NewDyna = DYN_pst_Dyna_Create();
			DYN_Dyna_Init
			(
				pst_NewDyna,
				DYN_C_DefaultDynaFlags,
				Cf_One,
				Cf_Infinit,
				Cf_Infinit,
				&MATH_gst_NulVector,
				OBJ_pst_GetAbsolutePosition(_pst_Object)
			);
			_pst_Object->pst_Base->pst_Dyna = pst_NewDyna;
			OBJ_ResetProcess
			(
				_pst_Object,
				WOR_World_GetWorldOfObject(_pst_Object)->ul_ProcessCounterDyn,
				OBJ_C_ProcessedDyn
			);
		}
	}
	else
	{
		/* Did the object have Dynamic before ? */
		if(_ul_OldFlags & OBJ_C_IdentityFlag_Dyna)
		{
#ifdef ACTIVE_EDITORS
			/* We unregister the dynamic pointer in editor mode */
			LINK_DelRegisterPointer(_pst_Object->pst_Base->pst_Dyna);
#endif
			/* We free the dynamic structure */
			DYN_Dyna_Close(_pst_Object->pst_Base->pst_Dyna);
			_pst_Object->pst_Base->pst_Dyna = 0;

			/* And we update the Dyna EOT and the activ objects table flags... */
			EOT_UpdateIdentityFlagsOfTable(&(pst_CurrentWorld->st_ActivObjects));
			EOT_RemoveObjectFromTableIfBadFlag(&(pst_CurrentWorld->st_EOT.st_Dyna), OBJ_C_IdentityFlag_Dyna);
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_Hierarchy flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_Hierarchy)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_Hierarchy))
		{
#if defined(_XBOX) || defined(_XENON)
			_pst_Object->pst_Base->pst_Hierarchy = (OBJ_tdst_Hierarchy *) MEM_p_AllocAlign(sizeof(OBJ_tdst_Hierarchy),16);
#else
			_pst_Object->pst_Base->pst_Hierarchy = (OBJ_tdst_Hierarchy *) MEM_p_Alloc(sizeof(OBJ_tdst_Hierarchy));
#endif
			_pst_Object->pst_Base->pst_Hierarchy->pst_FatherInit = NULL;
			_pst_Object->pst_Base->pst_Hierarchy->pst_Father = NULL;
			MATH_SetIdentityMatrix(&_pst_Object->pst_Base->pst_Hierarchy->st_LocalMatrix);
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_Hierarchy)
	{
		if
		(
			(_ul_Flag & OBJ_C_IdentityFlag_HasInitialPos) 
		&&	(_pst_Object->pst_Base->pst_Hierarchy->pst_FatherInit)
		&&	(_pst_Object->pst_Base->pst_Hierarchy->pst_FatherInit->ul_IdentityFlags & OBJ_C_IdentityFlag_HasInitialPos)
		)
		{
			MATH_MulMatrixMatrix
			(
				&M,
				_pst_Object->pst_GlobalMatrix + 1,
				OBJ_pst_GetAbsoluteMatrix(_pst_Object->pst_Base->pst_Hierarchy->pst_FatherInit) + 1
			);
			pst_Cur = _pst_Object->pst_Base->pst_Hierarchy->pst_FatherInit;
			while
			(
				(pst_Cur->ul_IdentityFlags & OBJ_C_IdentityFlag_HasInitialPos)
			&&	(pst_Cur->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			&&	(pst_Cur->pst_Base->pst_Hierarchy->pst_FatherInit)
			&&	(pst_Cur->pst_Base->pst_Hierarchy->pst_FatherInit->ul_IdentityFlags & OBJ_C_IdentityFlag_HasInitialPos)
			)
			{
				MATH_MulMatrixMatrix
				(
					&M1,
					&M,
					OBJ_pst_GetAbsoluteMatrix(pst_Cur->pst_Base->pst_Hierarchy->pst_FatherInit) + 1
				);
				MATH_CopyMatrix(&M, &M1);
				pst_Cur = pst_Cur->pst_Base->pst_Hierarchy->pst_FatherInit;
			}

			MATH_CopyMatrix(_pst_Object->pst_GlobalMatrix + 1, &M);
		}
#if defined(_XBOX) || defined(_XENON)
		MEM_FreeAlign(_pst_Object->pst_Base->pst_Hierarchy);
#else
		MEM_SafeFree(_pst_Object->pst_Base->pst_Hierarchy);
#endif
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_AdditionalMatrix flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	/* Force additionalmatrix if ptr is requested */
	if((_ul_Flag & OBJ_C_IdentityFlag_AddMatArePointer) && !(_ul_OldFlags & OBJ_C_IdentityFlag_AdditionalMatrix))
		_ul_Flag |= OBJ_C_IdentityFlag_AdditionalMatrix;

	if(_ul_Flag & OBJ_C_IdentityFlag_AdditionalMatrix)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_AdditionalMatrix))
		{
			_pst_Object->pst_Base->pst_AddMatrix = (OBJ_tdst_AdditionalMatrix *) MEM_p_Alloc(sizeof(OBJ_tdst_AdditionalMatrix));
			_pst_Object->pst_Base->pst_AddMatrix->l_Number = 0;
			_pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo = NULL;
#ifdef ACTIVE_EDITORS
			_pst_Object->pst_Base->pst_AddMatrix->pst_GO = _pst_Object;
			_pst_Object->pst_Base->pst_AddMatrix->dpst_EditionGO = NULL;
            _pst_Object->pst_Base->pst_AddMatrix->ul_GrpIndex = 0;
#endif
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_AdditionalMatrix)
	{
		_pst_Object->ul_IdentityFlags |= OBJ_C_IdentityFlag_AdditionalMatrix;
		OBJ_Gizmo_DelEditObject(_pst_Object);
		if(_pst_Object->pst_Base->pst_AddMatrix->l_Number) MEM_Free(_pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo);
		MEM_SafeFree(_pst_Object->pst_Base->pst_AddMatrix);
	}

	/* Convert matrix to pointers */
	if(_ul_Flag & OBJ_C_IdentityFlag_AddMatArePointer)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_AddMatArePointer))
		{
			if(_pst_Object->pst_Base->pst_AddMatrix->l_Number)
			{
				MEM_SafeFree(_pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo);
				_pst_Object->pst_Base->pst_AddMatrix->dst_GizmoPtr = (OBJ_tdst_GizmoPtr *) MEM_p_Alloc(_pst_Object->pst_Base->pst_AddMatrix->l_Number * sizeof(OBJ_tdst_GizmoPtr));
				for(i = 0; i < _pst_Object->pst_Base->pst_AddMatrix->l_Number; i++)
				{
#ifdef ACTIVE_EDITORS
					_pst_Object->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_GO = NULL;
					_pst_Object->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].l_MatrixId = -1;
#endif
					_pst_Object->pst_Base->pst_AddMatrix->dst_GizmoPtr[i].pst_Matrix = NULL;
				}
			}
		}
	}

	/* Convert pointers to matrix */
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_AddMatArePointer)
	{
		if(_pst_Object->pst_Base->pst_AddMatrix->l_Number)
		{
			MEM_SafeFree(_pst_Object->pst_Base->pst_AddMatrix->dst_GizmoPtr);
#if defined(_XBOX) || defined(_XENON)
			_pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_AllocAlign(_pst_Object->pst_Base->pst_AddMatrix->l_Number * sizeof(OBJ_tdst_Gizmo),16);
#else
			_pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo = (OBJ_tdst_Gizmo *) MEM_p_Alloc(_pst_Object->pst_Base->pst_AddMatrix->l_Number * sizeof(OBJ_tdst_Gizmo));
#endif
			for(i = 0; i < _pst_Object->pst_Base->pst_AddMatrix->l_Number; i++)
			{
				MATH_SetIdentityMatrix(&_pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo[i].st_Matrix);
#ifdef ACTIVE_EDITORS
				sprintf(_pst_Object->pst_Base->pst_AddMatrix->dst_Gizmo[i].sz_Name, "Gizmo %d", i);
#endif
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_DesignStruct flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_DesignStruct)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_DesignStruct))
		{
			_pst_Object->pst_Extended->pst_Design = (OBJ_tdst_DesignStruct *) MEM_p_Alloc(sizeof(OBJ_tdst_DesignStruct));
			L_memset(_pst_Object->pst_Extended->pst_Design, 0, sizeof(OBJ_tdst_DesignStruct));
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_DesignStruct)
	{
		MEM_SafeFree( _pst_Object->pst_Extended->pst_Design );
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_Msg flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_Msg)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_Msg))
		{
			_pst_Object->pst_Extended->pst_Msg = MEM_p_Alloc(sizeof(AI_tdst_MessageLiFo));
			L_memset(_pst_Object->pst_Extended->pst_Msg, 0, sizeof(AI_tdst_MessageLiFo));
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_Msg)
	{
		AI_EvalFunc_MSGClear_C(_pst_Object);
		MEM_SafeFree( _pst_Object->pst_Extended->pst_Msg );
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the flags related to collision ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if((_ul_Flag & OBJ_C_IdentityFlag_ColMap) && (!(_ul_OldFlags & OBJ_C_IdentityFlag_ColMap)))
	{
		if(_ul_Flag & OBJ_C_IdentityFlag_Visu)
		{
			COL_AllocColMap(_pst_Object);
#ifdef ACTIVE_EDITORS
			COL_ColMap_AddCob(_pst_Object, COL_C_Zone_Triangles, NULL, NULL);
#endif
			_pst_Object->ul_StatusAndControlFlags |= OBJ_C_ControlFlag_EnableSnP;
			INT_SnP_AddObject(_pst_Object, pst_CurrentWorld->pst_SnP);
			INT_SnP_InsertionSort(pst_CurrentWorld->pst_SnP, 0);

		}
	}

	if((!(_ul_Flag & OBJ_C_IdentityFlag_ColMap)) && (_ul_OldFlags & OBJ_C_IdentityFlag_ColMap))
		COL_FreeColMap((struct COL_tdst_Base_ *) _pst_Object->pst_Extended->pst_Col);

	if((_ul_Flag & OBJ_C_IdentityFlag_ZDM) && (!(_ul_OldFlags & OBJ_C_IdentityFlag_ZDM)))
	{
		_ul_Flag &= ~OBJ_C_IdentityFlag_ZDM;
	}

	if((_ul_Flag & OBJ_C_IdentityFlag_ZDE) && (!(_ul_OldFlags & OBJ_C_IdentityFlag_ZDE)))
	{
		_ul_Flag &= ~OBJ_C_IdentityFlag_ZDE;
	}

	if((!(_ul_Flag & OBJ_C_IdentityFlag_ZDM)) && (_ul_OldFlags & OBJ_C_IdentityFlag_ZDM))
	{
		_ul_Flag &= ~OBJ_C_IdentityFlag_ZDE;
		COL_FreeInstance((struct COL_tdst_Base_ *) _pst_Object->pst_Extended->pst_Col);
	}

	if((!(_ul_Flag & OBJ_C_IdentityFlag_ZDE)) && (_ul_OldFlags & OBJ_C_IdentityFlag_ZDE))
	{
		_ul_Flag &= ~OBJ_C_IdentityFlag_ZDM;
		COL_FreeInstance((struct COL_tdst_Base_ *) _pst_Object->pst_Extended->pst_Col);
	}

	if((_ul_Flag & OBJ_C_IdentityFlag_AI) && (!(_ul_OldFlags & OBJ_C_IdentityFlag_AI)))
	{
		COL_AllocDetectionList(_pst_Object);
		OBJ_SetStatusFlag(_pst_Object, OBJ_C_StatusFlag_Detection);
	}

	if
	(
		(!(_ul_Flag & OBJ_C_IdentityFlag_AI))
	&&	(_ul_OldFlags & OBJ_C_IdentityFlag_AI)
	&&	!(_pst_Object->ul_StatusAndControlFlags & OBJ_C_ControlFlag_ForceDetectionList)
	)
	{
		COL_FreeDetectionList((struct COL_tdst_Base_ *) _pst_Object->pst_Extended->pst_Col);
		((struct COL_tdst_Base_ *) _pst_Object->pst_Extended->pst_Col)->pst_List = NULL;
		OBJ_ClearStatusFlag(_pst_Object, OBJ_C_StatusFlag_Detection);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_Ai flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_AI)
	{
		/* Can never add an AI alone */
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_AI))
		{
			_ul_Flag &= ~OBJ_C_IdentityFlag_AI;
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_AI)
	{
		AI_FreeInstance((AI_tdst_Instance *) _pst_Object->pst_Extended->pst_Ai);
		_pst_Object->pst_Extended->pst_Ai = NULL;
		EOT_UpdateIdentityFlagsOfTable(&(pst_CurrentWorld->st_ActivObjects));
		EOT_RemoveObjectFromTableIfBadFlag(&(pst_CurrentWorld->st_EOT.st_AI), OBJ_C_IdentityFlag_AI);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_Ai flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_Group)
	{
		/* Can never add an Group alone */
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_Group))
		{
			_ul_Flag &= ~OBJ_C_IdentityFlag_Group;
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_Group)
	{
		GRP_ObjDetachGroup(_pst_Object);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_Events flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_Events)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_Events))
		{
			EVE_AllocateMainStruct(_pst_Object);
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_Events)
	{
		EVE_FreeMainStruct(_pst_Object);
		EOT_UpdateIdentityFlagsOfTable(&(pst_CurrentWorld->st_ActivObjects));
		EOT_RemoveObjectFromTableIfBadFlag(&(pst_CurrentWorld->st_EOT.st_Events), OBJ_C_IdentityFlag_Events);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_Events flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_Sound)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_Sound))
		{
			_ul_Flag &= ~OBJ_C_IdentityFlag_Sound;
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_Sound)
	{
		_pst_Object->ul_IdentityFlags |= OBJ_C_IdentityFlag_Sound;
        SND_FreeMainStruct(_pst_Object);
        _pst_Object->ul_IdentityFlags &= ~OBJ_C_IdentityFlag_Sound;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_Links flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_Links)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_Links))
		{
			_pst_Object->pst_Extended->pst_Links = MEM_p_Alloc(sizeof(WAY_tdst_Struct));
			L_memset(_pst_Object->pst_Extended->pst_Links, 0, sizeof(WAY_tdst_Struct));
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_Links)
	{
		WAY_FreeStruct((WAY_tdst_Struct *) _pst_Object->pst_Extended->pst_Links);
		_pst_Object->pst_Extended->pst_Links = NULL;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the OBJ_C_IdentityFlag_Lights flag ?
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_Lights)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_Lights))
		{
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
			char *psz_Ext;
			char sz_NewName[BIG_C_MaxLenName];
			/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			pst_Gro = GRO_pst_Struct_Create(GRO_Light);
#ifdef ACTIVE_EDITORS
			//get GAO name and change extension
			strcpy(sz_NewName, _pst_Object->sz_Name);
			if(psz_Ext = L_strchr(sz_NewName, '.'))
				*psz_Ext = 0;
			L_strcat(sz_NewName, EDI_Csz_ExtGraphicLight);

			WOR_GetGroPath(pst_CurrentWorld, sz_Path);
			GRO_ul_Struct_FullSave(pst_Gro, sz_Path, sz_NewName, NULL);
#endif
			pst_PTable = &pst_CurrentWorld->st_GraphicObjectsTable;
			if(TAB_ul_Ptable_GetElemIndexWithPointer(pst_PTable, pst_Gro) == TAB_Cul_BadIndex)
			{
				TAB_Ptable_AddElemAndResize(pst_PTable, pst_Gro);
				pst_Gro->i->pfn_AddRef(pst_Gro, 1);
			}

			pst_Gro->i->pfn_AddRef(pst_Gro, 1);
			_pst_Object->pst_Extended->pst_Light = pst_Gro;
		}
	}
	else if(_ul_OldFlags & OBJ_C_IdentityFlag_Lights)
	{
		if((pst_Gro = _pst_Object->pst_Extended->pst_Light) != NULL)
		{
			pst_Gro->i->pfn_AddRef(pst_Gro, -1);
			pst_Gro->i->pfn_Destroy(pst_Gro);
		}

		_pst_Object->pst_Extended->pst_Light = NULL;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Change the Flags related to BV
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_ul_Flag & OBJ_C_IdentityFlag_OBBox)
	{
		if(!(_ul_OldFlags & OBJ_C_IdentityFlag_OBBox))
		{
			_pst_Object->pst_BV = MEM_p_VMRealloc(_pst_Object->pst_BV, sizeof(OBJ_tdst_ComplexBV));
			b_BV_HasChanged = TRUE;
		}
	}
	else
	{
		if(_ul_OldFlags & OBJ_C_IdentityFlag_OBBox)
		{
			_pst_Object->pst_BV = MEM_p_VMRealloc(_pst_Object->pst_BV, sizeof(OBJ_tdst_SingleBV));
			b_BV_HasChanged = TRUE;

#ifdef ACTIVE_EDITORS
			_pst_Object->ul_EditorFlags &= ~OBJ_C_EditFlags_ShowBV;
#endif

		}
	}
#ifdef JADEFUSION
    if (b_BV_HasChanged)
    {
        if (_pst_Object->ul_StatusAndControlFlags & OBJ_C_ControlFlag_EnableSnP)
        {
            INT_SnP_DetachObject(_pst_Object, pst_CurrentWorld);
            INT_SnP_InsertionSort(pst_CurrentWorld->pst_SnP, 0);
        }
    }
#endif
	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Finally we change the flags
	 -------------------------------------------------------------------------------------------------------------------
	 */

	OBJ_SetIdentityFlags(_pst_Object, _ul_Flag);

	/* Must stay after identity flags have been set */
	OBJ_UpdateStatusFlagRTL(_pst_Object);

	/* If the BV has changed, we recompute it */
	if(b_BV_HasChanged) OBJ_ComputeBV(_pst_Object, OBJ_C_BV_ForceComputation, OBJ_C_BV_CurrentType);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ChangeControlFlags(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Flags, ULONG _ul_OldFlags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World		*pst_CurrentWorld;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Avoid errors */
	if(!_pst_Object) return;
	pst_CurrentWorld = WOR_World_GetWorldOfObject(_pst_Object);

	OBJ_FlagsControlSet(_pst_Object, _ul_Flags);	/* Warning, this line must stay on top of the function, because
													 *
													 * WOR_Activator_AddObjectToRightTable needs the correct new flag */

	/*
	 * If object has the always active flag set and is not active, we look for it in
	 * the activators tables and remove it from all the activators tables before
	 * activating it and moving it to all the first tables of all the activators !
	 */
	if(_ul_Flags & OBJ_C_ControlFlag_AlwaysActive)
	{
		if(!OBJ_b_TestStatusFlag(_pst_Object, OBJ_C_StatusFlag_Active))
		{
			WOR_ActivateObjectIfFlagsAllow(_pst_Object, pst_CurrentWorld);
		}
	}

	if(_ul_Flags & OBJ_C_ControlFlag_ForceInactive)
	{
		/* Force the activation to be redone next frame */
		pst_CurrentWorld->b_ForceActivationRefresh = TRUE;
	}

	/* Sets the Control Flag Force Detection List and object has no detection list yet */
	if
	(
		(_ul_Flags & OBJ_C_ControlFlag_ForceDetectionList)
	&&	!(_pst_Object->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Detection)
	)
	{
		COL_AllocDetectionList(_pst_Object);
		OBJ_SetStatusFlag(_pst_Object, OBJ_C_StatusFlag_Detection);
	}

	/*
	 * Resets the Detection List if we have reset the ForceDetection List Flag and if
	 * the Object has no IA.
	 */
	if
	(
		!(_ul_Flags & OBJ_C_ControlFlag_ForceDetectionList)
	&&	(_pst_Object->ul_StatusAndControlFlags & OBJ_C_StatusFlag_Detection)
	&&	!(_pst_Object->ul_IdentityFlags & OBJ_C_IdentityFlag_AI)
	)
	{
		COL_FreeDetectionList((struct COL_tdst_Base_ *) _pst_Object->pst_Extended->pst_Col);
		((struct COL_tdst_Base_ *) _pst_Object->pst_Extended->pst_Col)->pst_List = NULL;
		OBJ_ClearStatusFlag(_pst_Object, OBJ_C_StatusFlag_Detection);
	}

	/* We update the status flag real-time lightened, according to new control flags */
	OBJ_UpdateStatusFlagRTL(_pst_Object);
}
#ifdef JADEFUSION
void OBJ_ChangeExtraFlags(OBJ_tdst_Extended * _pst_Extended, USHORT _uw_Flags, USHORT _uw_OldFlags)
{
    if(_uw_Flags & OBJ_C_ExtraFlag_AffectedByStaticWind )
    {
        if (!(_uw_OldFlags & OBJ_C_ExtraFlag_AffectedByStaticWind))
        {
    
            if( _pst_Extended->po_CurrentWind )
            {
                delete _pst_Extended->po_CurrentWind;
            }

            _pst_Extended->po_CurrentWind = new CCurrentStaticWind();
            _uw_Flags &= ~OBJ_C_ExtraFlag_AffectedByDynamicWind;
        }
    }
    else if (_uw_OldFlags & OBJ_C_ExtraFlag_AffectedByStaticWind )
    {
        if( _pst_Extended->po_CurrentWind )
        {
            delete _pst_Extended->po_CurrentWind;
            _pst_Extended->po_CurrentWind = NULL;
        }
    }

    if (_uw_Flags & OBJ_C_ExtraFlag_AffectedByDynamicWind)
    {
        if (!(_uw_OldFlags & OBJ_C_ExtraFlag_AffectedByDynamicWind))
        {
             if ( _pst_Extended->po_CurrentWind)
            {
                delete _pst_Extended->po_CurrentWind;
            }

            _pst_Extended->po_CurrentWind = new CCurrentDynamicWind();
            _uw_Flags &= ~OBJ_C_ExtraFlag_AffectedByStaticWind;
        }
    }
    else if (_uw_OldFlags & OBJ_C_ExtraFlag_AffectedByDynamicWind)
    {
        if (_pst_Extended->po_CurrentWind)
        {
            delete _pst_Extended->po_CurrentWind;
            _pst_Extended->po_CurrentWind = NULL;
        }
    }

    // finally, set the Misc flags
    _pst_Extended->uw_ExtraFlags = _uw_Flags;
}
#endif
/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL OBJ_b_HasBeenProcess(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Count, ULONG _ul_Mask)
{
	if(_ul_Count & 1) return _pst_Object->c_FixFlags & _ul_Mask ? FALSE : TRUE;
	return _pst_Object->c_FixFlags & _ul_Mask;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_SetProcess(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Count, ULONG _ul_Mask)
{
	if(_ul_Count & 1)
		_pst_Object->c_FixFlags &= ~_ul_Mask;
	else
		_pst_Object->c_FixFlags |= _ul_Mask;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ResetProcess(OBJ_tdst_GameObject *_pst_Object, ULONG _ul_Count, ULONG _ul_Mask)
{
	if(_ul_Count & 1)
		_pst_Object->c_FixFlags |= _ul_Mask;
	else
		_pst_Object->c_FixFlags &= ~_ul_Mask;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_HierarchyMainCall(WOR_tdst_World *_pst_World)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFtable	*pst_EOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	BOOL				b_Father;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_EOT = &(_pst_World->st_EOT.st_Hierarchy);
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_EOT);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_EOT);
	for(; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(TAB_b_IsAHole(pst_GAO)) continue;

		/* Hierarchy */
		if(OBJ_b_HasBeenProcess(pst_GAO, _pst_World->ul_ProcessCounterHie, OBJ_C_ProcessedHie)) continue;
		b_Father = FALSE;
		while
		(
			pst_GAO->pst_Base
		&&	pst_GAO->pst_Base->pst_Hierarchy
		&&	pst_GAO->pst_Base->pst_Hierarchy->pst_Father
		&&	!OBJ_b_HasBeenProcess
				(
					pst_GAO->pst_Base->pst_Hierarchy->pst_Father,
					_pst_World->ul_ProcessCounterHie,
					OBJ_C_ProcessedHie
				)
		)
		{
			pst_GAO = pst_GAO->pst_Base->pst_Hierarchy->pst_Father;
			b_Father = TRUE;
		}

		if(b_Father) pst_CurrentElem--;
		OBJ_SetProcess(pst_GAO, _pst_World->ul_ProcessCounterHie, OBJ_C_ProcessedHie);
		OBJ_ComputeGlobalWhenHie(pst_GAO);
	}

	_pst_World->ul_ProcessCounterHie++;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_ChangeFather(OBJ_tdst_GameObject *_pst_GO, OBJ_tdst_GameObject *_pst_NewFather)
{
	if(!OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Hierarchy))
	{
		OBJ_ChangeIdentityFlags
		(
			_pst_GO,
			OBJ_ul_FlagsIdentityGet(_pst_GO) | OBJ_C_IdentityFlag_Hierarchy,
			OBJ_ul_FlagsIdentityGet(_pst_GO)
		);
	}

	_pst_GO->pst_Base->pst_Hierarchy->pst_Father = _pst_NewFather;
	OBJ_ComputeLocalWhenHie(_pst_GO);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
    Aim:    Check, when an object is destroyed, all the references.
 =======================================================================================================================
 */
void OBJ_WhenDestroyAnObject(WOR_tdst_World *_pst_World, OBJ_tdst_GameObject *_pst_GO)
{
	WAY_WhenDestroyAnObject(_pst_World, _pst_GO);
}

#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
