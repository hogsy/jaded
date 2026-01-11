/*$T AIfunctions_dyn.c GC! 1.100 05/31/01 10:16:25 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/DYNamics/DYNaccess.h"
#include "ENGine/Sources/DYNamics/DYNstruct.h"
#include "ENGine/Sources/DYNamics/DYNBasic.h"
#include "ENGine/Sources/DYNamics/DYNconst.h"
#include "ENGine/Sources/DYNamics/DYNsolid.h"
#include "ENGine/Sources/DYNamics/DYNcol.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/DYNamics/DYNConstraint.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "LINks/LINKstruct.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#define AI_Csz_ERRBasicForceNotAllocated	"Basic Forces not allocated"
#define AI_Csz_ERRDynamicsNotAllocated		"Dynamics not allocated"
#define AI_Csz_ERRNoObject					"No object associated"
#define AI_Csz_ERRConstraintNotAllocated	"Constraints not allocated"

extern ULONG				AI_C_Callback;

#ifdef ODE_INSIDE
#include "ode_jade.h"
#endif

/*
 =======================================================================================================================
    Aim:    Set the forces so that the speed reaches the given speed vetor (first parameter) in the given time (second
            parameter)

    Note:   The time given is the time needed to reach 90% of the final speed
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNSpeedReachInTime(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_K;	/* Calculated friction */
	MATH_tdst_Vector	st_T;	/* Calculated traction to apply */
	DYN_tdst_Dyna		*pst_Dyna;
	float				f_dt;	/* Time (given as last parameter) */
	MATH_tdst_Vector	st_V2;	/* Wanted speed vector (given as first parameter) */
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_dt = AI_PopFloat();		/* Read wanted time */
	AI_PopVector(&st_V2);		/* Read wanted speed vector */

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	if(pst_Dyna)
	{
		AI_PushInt
		(
			/* Call the magic function */DYN_b_SpeedReachInTime
				(
					pst_Dyna,
					&f_K,	/* Return friction */
					&st_T,	/* Return traction vector */
					&st_V2, /* Wanted final speed */
					f_dt	/* Time to reach 90% of this speed */
				)
		);
	}
	else
		/* No dyna allocated */
		AI_PushInt(0);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Set the Current Speed of an object
 =======================================================================================================================
 */
int AI_EvalFunc_DYNSpeedSetVector_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *speed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna, "Dynamic not allocated");
	DYN_SetSpeedVector(pst_Dyna, speed);
	return 1;
}
/**/
int AI_EvalFunc_DYNSpeedSetVector_C_CURRENT(MATH_tdst_Vector *speed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(AI_gpst_CurrentGameObject);
	DYN_SetSpeedVector(pst_Dyna, speed);
	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNSpeedSetVector(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_WantedSpeed;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_WantedSpeed);	/* Read wanted speed vector */
	AI_PushInt(AI_EvalFunc_DYNSpeedSetVector_C(pst_GO, &st_WantedSpeed));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Get the Current Speed of an object
 =======================================================================================================================
 */
void AI_EvalFunc_DYNSpeedGetVector_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Dest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	if(pst_Dyna)
		DYN_GetSpeedVector(pst_Dyna, pst_Dest);
	else
		MATH_CopyVector(pst_Dest, &MATH_gst_NulVector);
}
/**/
void AI_EvalFunc_DYNSpeedGetVector_C_CURRENT(MATH_tdst_Vector *pst_Dest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(AI_gpst_CurrentGameObject);
	if(pst_Dyna)
		DYN_GetSpeedVector(pst_Dyna, pst_Dest);
	else
		MATH_CopyVector(pst_Dest, &MATH_gst_NulVector);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNSpeedGetVector(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_CurrentSpeed;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DYNSpeedGetVector_C(pst_GO, &st_CurrentSpeed);
	AI_PushVector(&st_CurrentSpeed);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Set the Current Mass of an object
 =======================================================================================================================
 */
int AI_EvalFunc_DYNMassSet_C(OBJ_tdst_GameObject *pst_GO, float f_Mass)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	if(pst_Dyna)
	{
		/* Set the current speed of the object */
		DYN_SetMass(pst_Dyna, f_Mass);

		/* Return OK */
		return 1;
	}

	/* No dyna allocated */
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNMassSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Mass;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Mass = AI_PopFloat(); /* Read wanted speed vector */
	AI_PushInt(AI_EvalFunc_DYNMassSet_C(pst_GO, f_Mass));
	return ++_pst_Node;
}

void AI_EvalFunc_DYNMaxStepSet_C(OBJ_tdst_GameObject *_pst_GO, float _f_MaxStep)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna		*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(_pst_GO);
	if(pst_Dyna) DYN_SetMaxStepSize(pst_Dyna, _f_MaxStep);
}
/*
 =======================================================================================================================
    Aim:    Set Max Step Size of an object
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNMaxStepSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_MaxStepSize;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_MaxStepSize = AI_PopFloat();

	AI_EvalFunc_DYNMaxStepSet_C(pst_GO, f_MaxStepSize);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Get the Current Mass of an object
 =======================================================================================================================
 */
float AI_EvalFunc_DYNMassGet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	if(pst_Dyna) return DYN_f_GetMass(pst_Dyna);

	/* No dyna allocated, so we return a null vector */
	return Cf_Zero;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNMassGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_DYNMassGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Allocate the dynamics
 =======================================================================================================================
 */
int AI_EvalFunc_DYNOn_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	LONG			l_IdentityFlags;
	int				ires;
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_IdentityFlags = OBJ_ul_FlagsIdentityGet(pst_GO);
	l_IdentityFlags |= OBJ_C_IdentityFlag_Dyna;

	/*
	 * We allocate the Dyna structure (OBJ_C_UnvalidFlags means that no old flags are
	 * specified)
	 */
	OBJ_ChangeIdentityFlags(pst_GO, l_IdentityFlags, OBJ_C_UnvalidFlags);

	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	/* If we are in the AfterBlend Callback, we remove the object from Dyna Table. */
	if(AI_C_Callback == 1)
	{
//#ifdef _DEBUG	
#if 0
		/*~~~~~~~~~~~~~~~~~~*/
		char	asz_Log[512];
		/*~~~~~~~~~~~~~~~~~~*/

		sprintf(asz_Log, "A DYN_On has been called for %s in a AfterBlend Callback ==> Evil procedure !!!", pst_GO->sz_Name);
		LINK_PrintStatusMsg(asz_Log);
#endif		

		TAB_PFtable_AddElemWithData(&pst_World->st_EOT.st_Dyna, (void *) pst_GO, pst_GO->ul_IdentityFlags);
	}
	else
	{
		/* Force rebuild dyn table */
		ENG_gb_MustRebuildDyn = TRUE;
	}

	/* Everything went OK ? */
	if((pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO)))
		ires = 1;
	else
		/* Oh no! something went wrong... */
		ires = 0;

#ifdef ACTIVE_EDITORS
	LINK_UpdatePointer(pst_GO);
#endif
	return ires;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNOn(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_DYNOn_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Free the dynamics
 =======================================================================================================================
 */
int AI_EvalFunc_DYNOff_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	LONG			l_IdentityFlags;
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	l_IdentityFlags = OBJ_ul_FlagsIdentityGet(pst_GO);

	/* Clear the flag dynamics */
	l_IdentityFlags &= (~OBJ_C_IdentityFlag_Dyna);

	/*
	 * We free the Dyna structure (OBJ_C_UnvalidFlags means that no old flags are
	 * specified)
	 */
	OBJ_ChangeIdentityFlags(pst_GO, l_IdentityFlags, OBJ_C_UnvalidFlags);

	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	/* If we are in the AfterBlend Callback, we remove the object from Dyna Table. */
	if(AI_C_Callback == 1)
	{
#if 0	
		/*~~~~~~~~~~~~~~~~~~*/
		char	asz_Log[512];
		/*~~~~~~~~~~~~~~~~~~*/

		sprintf(asz_Log, "A DYN_Off has been called for %s in a AfterBlend Callback ==> Evil procedure !!!", pst_GO->sz_Name);
		LINK_PrintStatusMsg(asz_Log);
#endif		

		TAB_PFtable_RemoveElemWithPointer(&pst_World->st_EOT.st_Dyna, pst_GO);
	}
	else
	{
		/* Force rebuild dyn table */
		ENG_gb_MustRebuildDyn = TRUE;
	}

	/* Everything went OK ? */
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	if(!pst_Dyna)	/* Dyna was cleared without problems */
		return 1;

	/* Oh no! something went wrong... */
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNOff(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_DYNOff_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets/Clears the dynamic flags
 =======================================================================================================================
 */
int AI_EvalFunc_DYNFlagsSet_C(OBJ_tdst_GameObject *pst_GO, LONG l_DynaFlagsOn, LONG l_DynaFlagsOff)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	LONG			l_IdentityFlags;
	LONG			l_DynaFlags;
	LONG			l_OldDynaFlags;
	int				ires;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_GO)
	{
		pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
		if(!pst_Dyna)
		{
			/* Dynamics is not already allocated, so let's do it ! */
			l_IdentityFlags = OBJ_ul_FlagsIdentityGet(pst_GO);
			l_IdentityFlags |= OBJ_C_IdentityFlag_Dyna;

			/*
			 * We allocate the Dyna structure (OBJ_C_UnvalidFlags means that no old flags are
			 * specified)
			 */
			OBJ_ChangeIdentityFlags(pst_GO, l_IdentityFlags, OBJ_C_UnvalidFlags);
			pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
		}

		/* Here we have an object with dynamics, now we change the Flags */
		l_OldDynaFlags = DYN_ul_GetDynFlags(pst_Dyna);
		l_DynaFlags = (l_OldDynaFlags & (~l_DynaFlagsOff)) | l_DynaFlagsOn;
		DYN_ChangeDynFlags(pst_Dyna, l_DynaFlags, l_OldDynaFlags);

		/* We return the new dyna flags */
		ires = l_DynaFlags;
	}
	else
		/* Shoudn't happen */
		ires = -1;

	return ires;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNFlagsSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	LONG				l_DynaFlagsOn;
	LONG				l_DynaFlagsOff;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	l_DynaFlagsOff = AI_PopInt();
	l_DynaFlagsOn = AI_PopInt();
	AI_PushInt(AI_EvalFunc_DYNFlagsSet_C(pst_GO, l_DynaFlagsOn, l_DynaFlagsOff));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Gets the dynamic flags
 =======================================================================================================================
 */
int AI_EvalFunc_DYNFlagsGet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_GO)
	{
		pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
		if(!pst_Dyna) return(0);
		return((int) DYN_ul_GetDynFlags(pst_Dyna));
	}

	return(0);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNFlagsGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_DYNFlagsGet_C(pst_GO));
	return ++_pst_Node;
}
/*
 =======================================================================================================================
    Aim:    Sets the gravity vector of an actor
 =======================================================================================================================
 */
void AI_EvalFunc_DYNGravitySet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_GravityWanted)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, AI_Csz_ERRNoObject);
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, AI_Csz_ERRDynamicsNotAllocated);
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces), AI_Csz_ERRBasicForceNotAllocated);
	DYN_SetGravity(pst_Dyna, pst_GravityWanted);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNGravitySet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_GravityWanted;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_GravityWanted);
	AI_EvalFunc_DYNGravitySet_C(pst_GO, &st_GravityWanted);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DYNTractionSet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Tract)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, AI_Csz_ERRNoObject);
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, AI_Csz_ERRDynamicsNotAllocated);
	if(!pst_Dyna) return;
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces), AI_Csz_ERRBasicForceNotAllocated);
	if(!pst_Dyna->pst_Forces) return;
	DYN_SetTraction(pst_Dyna, pst_Tract);
}
/**/
void AI_EvalFunc_DYNTractionSet_C_CURRENT(MATH_tdst_Vector *pst_Tract)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(AI_gpst_CurrentGameObject);
	if(!pst_Dyna) return;
	if(!pst_Dyna->pst_Forces) return;
	DYN_SetTraction(pst_Dyna, pst_Tract);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNTractionSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_TractionWanted;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_TractionWanted);
	AI_EvalFunc_DYNTractionSet_C(pst_GO, &st_TractionWanted);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the traction vector of an actor
 =======================================================================================================================
 */
void AI_EvalFunc_DYNStreamSet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *_pst_Stream)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, AI_Csz_ERRDynamicsNotAllocated);
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces), AI_Csz_ERRBasicForceNotAllocated);
	DYN_SetStream(pst_Dyna, _pst_Stream);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNStreamSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_StreamWanted;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, AI_Csz_ERRNoObject);
	AI_PopVector(&st_StreamWanted);
	AI_EvalFunc_DYNStreamSet_C(pst_GO, &st_StreamWanted);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets friction (uniform on all axis)
 =======================================================================================================================
 */
int AI_EvalFunc_DYNFrictionSet_C(OBJ_tdst_GameObject *pst_GO, float f_FrictionWanted)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna, "No dynamic allocated");
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces), "No basic forces allocated");
	DYN_FrictionSet(pst_Dyna, f_FrictionWanted);
	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNFrictionSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_DYNFrictionSet_C(pst_GO, AI_PopFloat()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Gets friction (uniform on all axis)
 =======================================================================================================================
 */
float AI_EvalFunc_DYNFrictionGet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	if(pst_GO)
	{
		pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
		if(!pst_Dyna)
		{
			/* If no dyna, nothing is done and 0 is returned */
			return 0;
		}
		else
		{
			if(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces))
			{
				return DYN_f_FrictionGet(pst_Dyna);
			}
			else
				/* If no basic forces, nothing is done and 0 is returned */
				return 0;
		}
	}
	else
		/* Shoudn't happen */
		return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNFrictionGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PushFloat(AI_EvalFunc_DYNFrictionGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets friction (friction vector)
 =======================================================================================================================
 */
int AI_EvalFunc_DYNFrictionVectorSet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *vec)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO, "Invalid object");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna, "Dynamic not allocated");
	if(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces))
	{
		DYN_FrictionVectorSet(pst_Dyna, vec);
		return 1;
	}

	/* If no basic forces, nothing is done and 0 is returned */
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNFrictionVectorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_FrictionWanted;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_FrictionWanted);
	AI_PushInt(AI_EvalFunc_DYNFrictionVectorSet_C(pst_GO, &st_FrictionWanted));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Gets friction (vector)
 =======================================================================================================================
 */
void AI_EvalFunc_DYNFrictionVectorGet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *_pst_Dest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna, "Dynamic not allocated");
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces), "Basic forces not allocated");
	MATH_CopyVector(_pst_Dest, DYN_pst_FrictionVectorGet(pst_Dyna));
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNFrictionVectorGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DYNFrictionVectorGet_C(pst_GO, &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Gets friction (vector)
 =======================================================================================================================
 */
void AI_EvalFunc_DYNGravityVectorGet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *_pst_Dest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna, "Dynamic not allocated");
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces), "Basic forces not allocated");
	MATH_CopyVector(_pst_Dest, DYN_pst_GetGravity(pst_Dyna));
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNGravityVectorGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DYNGravityVectorGet_C(pst_GO, &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DYNStreamVectorGet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *_pst_Dest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	MATH_tdst_Vector	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna, "Dynamic not allocated");
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces), "Basic forces not allocated");
	pst_Stream = DYN_pst_GetStream(pst_Dyna);
	MATH_CopyVector(_pst_Dest, pst_Stream);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNStreamVectorGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DYNStreamVectorGet_C(pst_GO, &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DYNTractionVectorGet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *_pst_Dest)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	MATH_tdst_Vector	*pst_Stream;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna, "Dynamic not allocated");
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces), "Basic forces not allocated");
	pst_Stream = DYN_pst_GetTraction(pst_Dyna);
	MATH_CopyVector(_pst_Dest, pst_Stream);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNTractionVectorGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	v;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DYNTractionVectorGet_C(pst_GO, &v);
	AI_PushVector(&v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the friction to limit the speed to the given Speed value(or friction)
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNFrictionSetForMaxSpeed(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	float				f_SpeedWanted;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_SpeedWanted = AI_PopFloat();

	if(pst_GO)
	{
		pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
		if(!pst_Dyna)
		{
			/* If no dyna, nothing is done and 0 is returned */
			AI_PushInt(0);
		}
		else
		{
			if(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_BasicForces))
			{
				DYN_FrictionSet(pst_Dyna, DYN_f_FindFrictionForMaxSpeed(pst_Dyna, f_SpeedWanted));
				AI_PushInt(1);
			}
			else
				/* If no basic forces, nothing is done and 0 is returned */
				AI_PushInt(0);
		}
	}
	else
		/* Shoudn't happen */
		AI_PushInt(0);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim returns the norm of the speed vector
 =======================================================================================================================
 */
float AI_EvalFunc_DYNSpeedGet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	if(pst_Dyna) return DYN_f_GetNormSpeed(pst_Dyna);
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNSpeedGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_DYNSpeedGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the norm of the speed

    Note:   You can't change the norm of a nul vector
 =======================================================================================================================
 */
void AI_EvalFunc_DYNSpeedSet_C(OBJ_tdst_GameObject *pst_GO, float f_Speed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	AI_Check(!MATH_b_NulVectorWithEpsilon(&pst_Dyna->st_P, Cf_EpsilonLow), "Changing the norm of a nul vector");
	DYN_SetNormSpeed(pst_Dyna, f_Speed);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNSpeedSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Speed;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Speed = AI_PopFloat();
	AI_EvalFunc_DYNSpeedSet_C(pst_GO, f_Speed);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the Elastical Factor of the Physical algorithm.

 =======================================================================================================================
 */
void AI_EvalFunc_DYNElasticSet_C(OBJ_tdst_GameObject *pst_GO, float f_Elastic)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	AI_Check(pst_Dyna->pst_Solid != NULL, "Solid not allocated");
	pst_Dyna->pst_Solid->f_Factor = f_Elastic;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNElasticSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Elastic;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Elastic = AI_PopFloat();
	AI_EvalFunc_DYNElasticSet_C(pst_GO, f_Elastic);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the norm of the speed limit
 =======================================================================================================================
 */
float AI_EvalFunc_DYNSpeedLimitGet_C(OBJ_tdst_GameObject*pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna		*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	return (DYN_f_GetSpeedLimitHoriz(pst_Dyna));	
}

AI_tdst_Node *AI_EvalFunc_DYNSpeedLimitGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_DYNSpeedLimitGet_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the norm of the Horizontal speed limit
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNSpeedLimitHorizGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	AI_PushFloat(DYN_f_GetSpeedLimitHoriz(pst_Dyna));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the norm of the Vertical speed limit
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNSpeedLimitVertGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	AI_PushFloat(DYN_f_GetSpeedLimitVert(pst_Dyna));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the limit (horizontal & vertical) of the speed
 =======================================================================================================================
 */
void AI_EvalFunc_DYNSpeedLimitSet_C(OBJ_tdst_GameObject *pst_GO, float f_SpeedLimit)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");

	DYN_SetSpeedLimitHoriz(pst_Dyna, f_SpeedLimit);
	DYN_SetSpeedLimitVert(pst_Dyna, f_SpeedLimit);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNSpeedLimitSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_SpeedLimit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_SpeedLimit = AI_PopFloat();
	AI_EvalFunc_DYNSpeedLimitSet_C(pst_GO, f_SpeedLimit);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the limit (horizontal) of the speed
 =======================================================================================================================
 */
void AI_EvalFunc_DYNSpeedLimitHorizSet_C(OBJ_tdst_GameObject *pst_GO, float f_SpeedLimit)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");

	DYN_SetSpeedLimitHoriz(pst_Dyna, f_SpeedLimit);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNSpeedLimitHorizSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_SpeedLimit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_SpeedLimit = AI_PopFloat();
	AI_EvalFunc_DYNSpeedLimitHorizSet_C(pst_GO, f_SpeedLimit);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the limit (vertical) of the speed
 =======================================================================================================================
 */
void AI_EvalFunc_DYNSpeedLimitVertSet_C(OBJ_tdst_GameObject *pst_GO, float f_SpeedLimit)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");

	DYN_SetSpeedLimitVert(pst_Dyna, f_SpeedLimit);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNSpeedLimitVertSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_SpeedLimit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_SpeedLimit = AI_PopFloat();
	AI_EvalFunc_DYNSpeedLimitVertSet_C(pst_GO, f_SpeedLimit);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Set inertia for auto orient
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNAutoOrientInertiaSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	LONG				l_InertiaLevel;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	l_InertiaLevel = AI_PopInt();

	if(pst_GO)
	{
		pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
		if(pst_Dyna)
		{
			DYN_AutoOrientInertiaSet(pst_Dyna, l_InertiaLevel);
		}
	}

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Returns the inertia for auto orient
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNAutoOrientInertiaGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	if(pst_GO)
	{
		pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
		if(pst_Dyna)
		{
			AI_PushInt(DYN_l_AutoOrientInertiaGet(pst_Dyna));
		}
		else
			AI_PushInt(0);
	}
	else
		AI_PushInt(0);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Constraint an object to a segment
 =======================================================================================================================
 */
void AI_EvalFunc_DYNConstraintObjectToSegment_C(OBJ_tdst_GameObject*pst_GO,MATH_tdst_Vector	*pst_VA, MATH_tdst_Vector	*pst_VB )
{
	DYN_tdst_Dyna		*pst_Dyna;

    AI_Check(pst_GO != NULL, AI_Csz_ERRNoObject);
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, AI_Csz_ERRDynamicsNotAllocated);
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_Constraint), AI_Csz_ERRConstraintNotAllocated);
	DYN_ConstraintInit(DYN_pst_ConstraintGet(pst_Dyna), DYN_C_ConstraintSegment, pst_VA, pst_VB, &MATH_gst_NulVector, 0);
}
AI_tdst_Node *AI_EvalFunc_DYNConstraintObjectToSegment(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_A;
	MATH_tdst_Vector	st_B;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_B);
	AI_PopVector(&st_A);
    AI_EvalFunc_DYNConstraintObjectToSegment_C(pst_GO, &st_A, &st_B);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Constraint to a parabol

    Note:   The parabol is defined by 3 vectors (point A, point B, origin) and one float (distance OA)
 =======================================================================================================================
 */
void AI_EvalFunc_DYNConstraintObjectToParabol_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector*pst_A, MATH_tdst_Vector*pst_B, MATH_tdst_Vector*pst_O, float f)
{
	DYN_tdst_Dyna		*pst_Dyna;

	AI_Check(pst_GO != NULL, AI_Csz_ERRNoObject);
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, AI_Csz_ERRDynamicsNotAllocated);
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_Constraint), AI_Csz_ERRConstraintNotAllocated);
	DYN_ConstraintInit(DYN_pst_ConstraintGet(pst_Dyna), DYN_C_ConstraintParabol, pst_A, pst_B, pst_O, f);
}
AI_tdst_Node *AI_EvalFunc_DYNConstraintObjectToParabol(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_A;
	MATH_tdst_Vector	st_B;
	MATH_tdst_Vector	st_O;
	float				f_Rayon;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Rayon = AI_PopFloat();
	AI_PopVector(&st_O);
	AI_PopVector(&st_B);
	AI_PopVector(&st_A);
    AI_EvalFunc_DYNConstraintObjectToParabol_C(pst_GO, &st_A, &st_B, &st_O, f_Rayon);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Changes the speed and clears the current traction force in order to reach a given position (real c
            function)

    Note:   T= total time to go from A to B g = norm of gravity vector
 =======================================================================================================================
 */
void AI_EvalFunc_DYNImpulsion_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *_pst_B, float _f_MaxHeight)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*pst_gravity;
	MATH_tdst_Vector	st_UnitG;
	MATH_tdst_Vector	st_InvUnitG;
	MATH_tdst_Vector	*pst_A;
	MATH_tdst_Vector	st_Impulse, st_ImpulseLoc;
	MATH_tdst_Vector	st_M, st_MA, st_MB, st_AB, st_ABLoc;
	MATH_tdst_Matrix	st_Mat;
	float				h0, h1, g, t, f_V0, fInvt;
	DYN_tdst_Dyna		*_pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	_pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(_pst_Dyna != NULL, "Dynamics not allocated");
	AI_Check(_pst_Dyna->pst_Forces != NULL, "Calling DYNImpulsion: basic forces not allocated ");
	AI_Check(!MATH_b_NulVector(DYN_pst_GetGravity(_pst_Dyna)), "Calling DYNImpulsion: Object without gravity");

	/* Clears the traction force and the friction */
	DYN_SetTraction(_pst_Dyna, &MATH_gst_NulVector);
	DYN_FrictionVectorSet(_pst_Dyna, &MATH_gst_NulVector);

	/* Clears the SpeedLimit */
	DYN_SetSpeedLimitHoriz(_pst_Dyna, Cf_Infinit);
	DYN_SetSpeedLimitVert(_pst_Dyna, Cf_Infinit);

	/* Gets the gravity vector of actor */
	pst_gravity = DYN_pst_GetGravity(_pst_Dyna);
	MATH_NormalizeVector(&st_UnitG, pst_gravity);
	MATH_NegVector(&st_InvUnitG, &st_UnitG);
	g = MATH_f_NormVector(pst_gravity);

	/* Gets the Initial Position of actor */
	pst_A = DYN_pst_GetPosition(_pst_Dyna);
	MATH_SubVector(&st_AB, _pst_B, pst_A);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Calculate position of MaxHeight Point (M): This point is located _f_MaxHeight above the highest of points A and B
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(_f_MaxHeight < 0) _f_MaxHeight = 0;
	MATH_InitVectorToZero(&st_M);
	MATH_MulVector(&st_M, &st_InvUnitG, _f_MaxHeight);

	/* Find highest of points A and B. If OM.AB > 0 then B is higher than A */
	if(MATH_f_DotProduct(&st_InvUnitG, &st_AB) > 0)
		MATH_AddEqualVector(&st_M, _pst_B);
	else
		MATH_AddEqualVector(&st_M, pst_A);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Calculate h0 (height between A and M), h1 (height between B and M) and t (total time )
	 -------------------------------------------------------------------------------------------------------------------
	 */

	MATH_SubVector(&st_MB, _pst_B, &st_M);
	MATH_SubVector(&st_MA, pst_A, &st_M);
	h0 = MATH_f_DotProduct(&st_MA, &st_UnitG);
	h1 = MATH_f_DotProduct(&st_MB, &st_UnitG);
	t = fSqrt(Cf_Two / g) * (fSqrt(h0) + fSqrt(h1));

	/* Calculate V0 */
	f_V0 = fSqrt(Cf_Two * g * h0);
	fInvt = fInv(t);

	/* If gravity is vertical, no need of changing coordinates */
	if(MATH_b_EqVectorWithEpsilon(&st_InvUnitG, &MATH_gst_BaseVectorK, Cf_EpsilonBig))
	{
		st_Impulse.z = f_V0;
		st_Impulse.x = (_pst_B->x - pst_A->x) * fInvt;
		st_Impulse.y = (_pst_B->y - pst_A->y) * fInvt;
	}
	else
	{
		/* Build Matrix to change coordinates */
		MATH_MakeRotationMatrix_UsingBanking(&st_Mat, &st_AB, &st_InvUnitG, 1);

		/* Calculates local vector AB */
		MATH_VectorGlobalToLocal(&st_ABLoc, &st_Mat, &st_AB);
		st_ImpulseLoc.z = f_V0;
		st_ImpulseLoc.x = st_ABLoc.x * fInvt;
		st_ImpulseLoc.y = st_ABLoc.y * fInvt;
		MATH_VectorLocalToGlobal(&st_Impulse, &st_Mat, &st_ImpulseLoc);
	}

	DYN_SetSpeedVector(_pst_Dyna, &st_Impulse);
}

/*
 =======================================================================================================================
    Aim:    Changes the speed and clears the current traction force in order to reach a given position

    Ex:     DYN_Impulsion(v_DstPosition, f_MaxHeight)
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNImpulsion(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_DstPos;
	float				f_MaxHeight;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_MaxHeight = AI_PopFloat();
	AI_PopVector(&st_DstPos);

	/* Object must have a gravity */
	AI_EvalFunc_DYNImpulsion_C(pst_GO, &st_DstPos, f_MaxHeight);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Set the torque of a solid object

    Ex:     DYN_TorqueVectorSet(cvector(0,0,1)) creates a torque (rotation acceleration) around vertical axis
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNTorqueVectorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	MATH_tdst_Vector	st_T;
	DYN_tdst_Solid		*pst_Solid;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_T);

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Solid = DYN_pst_SolidGet(pst_Dyna);
	AI_Check(pst_Solid != NULL, "Solid structure not allocated (check Solid flag in Dynamics flags) ");
	MATH_CopyVector(DYN_pst_TGet(pst_Solid), &st_T);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Get the torque of a solid object
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNTorqueVectorGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	DYN_tdst_Solid		*pst_Solid;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Solid = DYN_pst_SolidGet(pst_Dyna);
	AI_Check(pst_Solid != NULL, "Solid structure not allocated (check Solid flag in Dynamics flags) ");
	AI_PushVector(DYN_pst_TGet(pst_Solid));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNRotSpeedVectorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	MATH_tdst_Vector	st_W;
	DYN_tdst_Solid		*pst_Solid;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, "No object associated");
	AI_PopVector(&st_W);
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Solid = DYN_pst_SolidGet(pst_Dyna);
	AI_Check(pst_Solid != NULL, "Solid structure not allocated (check Solid flag in Dynamics flags) ");
	MATH_CopyVector(DYN_pst_WGet(pst_Solid), &st_W);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNRotSpeedVectorGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	DYN_tdst_Solid		*pst_Solid;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Solid = DYN_pst_SolidGet(pst_Dyna);
	AI_Check(pst_Solid != NULL, "Solid structure not allocated (check Solid flag in Dynamics flags) ");
	AI_PushVector(DYN_pst_WGet(pst_Solid));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNRotSpeedLimitGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	DYN_tdst_Solid		*pst_Solid;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Solid = DYN_pst_SolidGet(pst_Dyna);
	AI_Check(pst_Solid != NULL, "Solid structure not allocated (check Solid flag in Dynamics flags) ");
	AI_PushFloat(DYN_f_wLimitGet(pst_Solid));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNRotSpeedLimitSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	float				f_RotSpeedLimit;
	DYN_tdst_Solid		*pst_Solid;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, "No object associated");
	f_RotSpeedLimit = AI_PopFloat();
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Solid = DYN_pst_SolidGet(pst_Dyna);
	AI_Check(pst_Solid != NULL, "Solid structure not allocated (check Solid flag in Dynamics flags) ");
	DYN_wLimitSet(pst_Solid, f_RotSpeedLimit);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DYNSlideSet_C(OBJ_tdst_GameObject *pst_GO, float f_Slide)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	DYN_tdst_Col	*pst_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Col = DYN_pst_ColGet(pst_Dyna);
	AI_Check(pst_Col != NULL, "Col structure not allocated (check Col flag in Dynamics flags) ");
	DYN_SlideSet(pst_Col, f_Slide);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNSlideSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_DYNSlideSet_C(pst_GO, AI_PopFloat());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNSlideGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	DYN_tdst_Col		*pst_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Col = DYN_pst_ColGet(pst_Dyna);
	AI_Check(pst_Col != NULL, "Col structure not allocated (check Col flag in Dynamics flags) ");
	AI_PushFloat(DYN_f_SlideGet(pst_Col));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DYNReboundSet_C(OBJ_tdst_GameObject *pst_GO, float f_Rebound)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	DYN_tdst_Col	*pst_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Col = DYN_pst_ColGet(pst_Dyna);
	AI_Check(pst_Col != NULL, "Col structure not allocated (check Col flag in Dynamics flags) ");
	DYN_ReboundSet(pst_Col, f_Rebound);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNReboundSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Rebound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Rebound = AI_PopFloat();
	AI_EvalFunc_DYNReboundSet_C(pst_GO, f_Rebound);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DYNApplyRectFactorSet_C(OBJ_tdst_GameObject *pst_GO, float f_Rebound)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	DYN_tdst_Col	*pst_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Col = DYN_pst_ColGet(pst_Dyna);
	AI_Check(pst_Col != NULL, "Col structure not allocated (check Col flag in Dynamics flags) ");
	pst_Col->f_ApplyRecFactor = f_Rebound;
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNApplyRectFactorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Rebound;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Rebound = AI_PopFloat();
	AI_EvalFunc_DYNApplyRectFactorSet_C(pst_GO, f_Rebound);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_DYNReboundGet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	DYN_tdst_Col	*pst_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Col = DYN_pst_ColGet(pst_Dyna);
	AI_Check(pst_Col != NULL, "Col structure not allocated (check Col flag in Dynamics flags) ");
	return DYN_f_ReboundGet(pst_Col);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNReboundGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, "No object associated");
	AI_PushFloat(AI_EvalFunc_DYNReboundGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNReboundHorizCosAngleSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	float				f_ReboundHorizCosAngle;
	DYN_tdst_Col		*pst_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, "No object associated");
	f_ReboundHorizCosAngle = AI_PopFloat();
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Col = DYN_pst_ColGet(pst_Dyna);
	AI_Check(pst_Col != NULL, "Col structure not allocated (check Col flag in Dynamics flags) ");
	DYN_ReboundHorizCosAngleSet(pst_Col, f_ReboundHorizCosAngle);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNReboundHorizCosAngleGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	DYN_tdst_Col		*pst_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Col = DYN_pst_ColGet(pst_Dyna);
	AI_Check(pst_Col != NULL, "Col structure not allocated (check Col flag in Dynamics flags) ");
	AI_PushFloat(DYN_f_ReboundHorizCosAngleGet(pst_Col));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DYNSlideHorizCosAngleSet_C(OBJ_tdst_GameObject *pst_GO, float f_SlideHorizCosAngle)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	DYN_tdst_Col	*pst_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Col = DYN_pst_ColGet(pst_Dyna);
	AI_Check(pst_Col != NULL, "Col structure not allocated (check Col flag in Dynamics flags) ");
	DYN_SlideHorizCosAngleSet(pst_Col, f_SlideHorizCosAngle);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNSlideHorizCosAngleSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_SlideHorizCosAngle;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_SlideHorizCosAngle = AI_PopFloat();
	AI_EvalFunc_DYNSlideHorizCosAngleSet_C(pst_GO, f_SlideHorizCosAngle);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_DYNSlideHorizCosAngleGet_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	DYN_tdst_Col	*pst_Col;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, "No object associated");
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, "Dynamics not allocated");
	pst_Col = DYN_pst_ColGet(pst_Dyna);
	AI_Check(pst_Col != NULL, "Col structure not allocated (check Col flag in Dynamics flags) ");
	return DYN_f_SlideHorizCosAngleGet(pst_Col);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNSlideHorizCosAngleGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_DYNSlideHorizCosAngleGet_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Change the horizontal speed only (project the input vector on the horizontal plane before applying it)
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_DYNSpeedVectorHorizSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	DYN_tdst_Dyna		*pst_Dyna;
	MATH_tdst_Vector	st_Speed;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, AI_Csz_ERRNoObject);
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, AI_Csz_ERRDynamicsNotAllocated);

	AI_PopVector(&st_Speed);
	DYN_SetSpeedVector_WithoutChangingGravity(pst_Dyna, &st_Speed);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DYNMaxPosSet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *pst_Speed)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna	*pst_Dyna;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO != NULL, AI_Csz_ERRNoObject);
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, AI_Csz_ERRDynamicsNotAllocated);

	MATH_CopyVector(&pst_Dyna->st_MaxPos, pst_Speed);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNMaxPosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	st_Speed;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Speed);
	AI_EvalFunc_DYNMaxPosSet_C(pst_GO, &st_Speed);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DYNConstraintGetTangent_C(OBJ_tdst_GameObject *pst_GO,MATH_tdst_Vector*pst_V)
{
    DYN_tdst_Dyna		*pst_Dyna;

    AI_Check(pst_GO != NULL, AI_Csz_ERRNoObject);
	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, AI_Csz_ERRDynamicsNotAllocated);
	AI_Check(DYN_ul_TestDynFlags(pst_Dyna, DYN_C_Constraint), AI_Csz_ERRConstraintNotAllocated);

    MATH_CopyVector(pst_V, &pst_Dyna->pst_Constraint->st_Tangent);
}
AI_tdst_Node *AI_EvalFunc_DYNConstraintGetTangent(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;	
    MATH_tdst_Vector	v;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	
    AI_EvalFunc_DYNConstraintGetTangent_C(pst_GO, &v);
    AI_PushVector(&v);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_DYNAlignSight_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_Dyna		*pst_Dyna;
	MATH_tdst_Vector	st_Speed;
	float				f;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Dyna = OBJ_pst_GetDynaStruct(pst_GO);
	AI_Check(pst_Dyna != NULL, AI_Csz_ERRDynamicsNotAllocated);
	DYN_GetSpeedVector(pst_Dyna, &st_Speed);
	f = MATH_f_NormVector(&st_Speed);
	OBJ_SightGet(pst_GO, &st_Speed);
	MATH_SetNormVector(&st_Speed, &st_Speed, f);
	DYN_SetSpeedVector(pst_Dyna, &st_Speed);
}
/**/
AI_tdst_Node *AI_EvalFunc_DYNAlignSight(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_Check(pst_GO != NULL, AI_Csz_ERRNoObject);
	AI_EvalFunc_DYNAlignSight_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================

															ODE
	
 =======================================================================================================================
 */
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void  AI_EvalFunc_ODEGravitySet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *_pst_Gravity)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO || !_pst_Gravity)
		return;

	pst_World = WOR_World_GetWorldOfObject(pst_GO);

	if(!pst_World || !pst_World->ode_id_world)
		return;

	dWorldSetGravity (pst_World->ode_id_world, _pst_Gravity->x, _pst_Gravity->y, _pst_Gravity->z);
#else
	return;
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_ODEGravitySet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_G;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_G);

	AI_EvalFunc_ODEGravitySet_C(pst_GO, &st_G);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ODEEnable_C(OBJ_tdst_GameObject *pst_GO, int Enable)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		Enabled;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE))
		return 0;

	if(pst_GO->pst_Base->pst_ODE->ode_id_body)
		Enabled = (pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_ENABLE);
	else
		Enabled = 0;

	if(Enable == -1)
		return Enabled;

	if(Enable)
	{
		pst_GO->pst_Base->pst_ODE->uc_Flags |= ODE_FLAGS_ENABLE;
		if(pst_GO->pst_Base->pst_ODE->ode_id_body)
			dBodyEnable(pst_GO->pst_Base->pst_ODE->ode_id_body);
	}
	else
	{
		pst_GO->pst_Base->pst_ODE->uc_Flags &= ~ODE_FLAGS_ENABLE;
		if(pst_GO->pst_Base->pst_ODE->ode_id_body)
			dBodyDisable(pst_GO->pst_Base->pst_ODE->ode_id_body);
	}

	return Enabled;
#else
	return 0;
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_ODEEnable(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Enable;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	Enable = AI_PopInt();

	AI_PushInt(AI_EvalFunc_ODEEnable_C(pst_GO, Enable));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ODEPause_C(OBJ_tdst_GameObject *pst_GO, int Pause)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		Paused;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE))
		return 0;

	if(pst_GO->pst_Base->pst_ODE->ode_id_body)
		Paused = ! (dBodyIsEnabled(pst_GO->pst_Base->pst_ODE->ode_id_body));
	else
		Paused = FALSE;

	if(Pause == -1)
		return Paused;

	if(Pause)
	{
		if(pst_GO->pst_Base->pst_ODE->ode_id_body)
			dBodyDisable(pst_GO->pst_Base->pst_ODE->ode_id_body);
	}
	else
	{
		if(pst_GO->pst_Base->pst_ODE->ode_id_body)
			dBodyEnable(pst_GO->pst_Base->pst_ODE->ode_id_body);
	}
	return Paused;
#else
	return 0;
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_ODEPause(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Pause;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	Pause = AI_PopInt();

	AI_PushInt(AI_EvalFunc_ODEPause_C(pst_GO, Pause));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ODEForcePosSet_C(OBJ_tdst_GameObject *pst_GO, MATH_tdst_Vector *_pst_Force, MATH_tdst_Vector *_pst_Pos)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	float				rot[12];
	DYN_tdst_ODE		*pst_ODE;
	dBodyID				ode_id_body;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) || !(pst_GO->pst_Base->pst_ODE->ode_id_body))
		return;

	pst_ODE = pst_GO->pst_Base->pst_ODE;
	ode_id_body = pst_ODE->ode_id_body;

	dBodyEnable(ode_id_body);

	ODE_ComputeODEMatrixFromJade(pst_GO->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot); 
	dBodySetPosition(ode_id_body, pst_GO->pst_GlobalMatrix->T.x + pst_ODE->st_Offset.x, pst_GO->pst_GlobalMatrix->T.y + pst_ODE->st_Offset.y, pst_GO->pst_GlobalMatrix->T.z + pst_ODE->st_Offset.z);
	dBodySetRotation(ode_id_body, &rot[0]);

	dBodyAddForceAtPos(pst_GO->pst_Base->pst_ODE->ode_id_body, _pst_Force->x, _pst_Force->y, _pst_Force->z, _pst_Pos->x, _pst_Pos->y, _pst_Pos->z);
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_ODEForcePosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Force;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PopVector(&st_Pos);
	AI_PopVector(&st_Force);

	AI_EvalFunc_ODEForcePosSet_C(pst_GO, &st_Force, &st_Pos);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ODESeti_C(OBJ_tdst_GameObject *pst_GO, int Id, int Param)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE	*pst_ODE;
	WOR_tdst_World	*pst_World;
	dMass			Mass;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) || !(pst_GO->pst_Base->pst_ODE))
		return -1;

	pst_ODE = pst_GO->pst_Base->pst_ODE;
	switch(Id)
	{
	case 0:		// ControlFlags ODE
		{
			if(pst_ODE->ode_id_body)
			{
				dBodySetGravityMode(pst_ODE->ode_id_body, (Param & ODE_FLAGS_NO_GRAVITY) ? 0 : 1);
			}

			/* Flag Immovable Set */
			if(Param & ODE_FLAGS_FORCEIMMOVABLE)
			{
				if(!(pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_FORCEIMMOVABLE)  && pst_ODE->ode_id_body && (pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY))
				{
					dBodyDestroy(pst_ODE->ode_id_body);
					pst_ODE->ode_id_body = 0;
				}
			}

			/* Flag Immovable Reset */
			if(!(Param & ODE_FLAGS_FORCEIMMOVABLE))
			{
				if((pst_GO->pst_Base->pst_ODE->uc_Flags & ODE_FLAGS_FORCEIMMOVABLE) && (pst_ODE->uc_Flags & ODE_FLAGS_RIGIDBODY))
				{
					pst_World = WOR_World_GetWorldOfObject(pst_GO);

					pst_ODE->ode_id_body = dBodyCreate(pst_World->ode_id_world);
					dBodySetData(pst_ODE->ode_id_body, pst_GO);

					switch(pst_ODE->uc_Type)
					{
					case ODE_TYPE_SPHERE:
						{
							dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
							dMassSetSphereTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X);
							dBodySetMass(pst_ODE->ode_id_body, &Mass);
						}
						break;

					case ODE_TYPE_BOX:
						{
							dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
							dMassSetBoxTotal(&Mass, pst_ODE->mass_init, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
							dBodySetMass(pst_ODE->ode_id_body, &Mass);
						}
						break;

					case ODE_TYPE_CYLINDER:
						{
							dGeomSetBody(pst_ODE->ode_id_geom, pst_ODE->ode_id_body);
							dMassSetCylinderTotal(&Mass, pst_ODE->mass_init, 2, pst_ODE->f_X, pst_ODE->f_Y);
							dBodySetMass(pst_ODE->ode_id_body, &Mass);
						}
						break;
					}

					if(pst_ODE->uc_Flags & ODE_FLAGS_AUTODISABLE)
					{
						dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 1);
						dBodySetAutoDisableLinearThreshold(pst_ODE->ode_id_body, pst_ODE->f_LinearThres);
						dBodySetAutoDisableAngularThreshold(pst_ODE->ode_id_body, pst_ODE->f_AngularThres);
						dBodySetAutoDisableSteps(pst_ODE->ode_id_body, 10);
					}
					else 
					{
						dBodySetAutoDisableFlag(pst_ODE->ode_id_body, 0);
					}
				}
			}

			if((pst_ODE->uc_Flags & ODE_FLAGS_MATHCOLMAP) && (pst_ODE->uc_Type != ODE_TYPE_PLAN))
			{
				float				rot[12];
				MATH_tdst_Vector	st_Offset;

				MATH_TransformVector(&st_Offset, pst_GO->pst_GlobalMatrix, &pst_ODE->st_Offset);

				ODE_ComputeODEMatrixFromJade(pst_GO->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot); 

				dGeomSetPositionNoDirty(pst_ODE->ode_id_geom, pst_GO->pst_GlobalMatrix->T.x + st_Offset.x, pst_GO->pst_GlobalMatrix->T.y + st_Offset.y, pst_GO->pst_GlobalMatrix->T.z + st_Offset.z); 
				dGeomSetRotationNoDirty(pst_ODE->ode_id_geom, &rot[0]);
			}



			pst_GO->pst_Base->pst_ODE->uc_Flags = Param;
		}
		break;

	}
#endif
	return -1;

}
/**/
AI_tdst_Node *AI_EvalFunc_ODESeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int			Id, Param;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	Param = AI_PopInt();
	Id = AI_PopInt();

	AI_PushInt(AI_EvalFunc_ODESeti_C(pst_GO, Id, Param));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ODESetf_C(OBJ_tdst_GameObject *pst_GO, int Id, float Param)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE	*pst_ODE;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) || !(pst_GO->pst_Base->pst_ODE))
		return -1;

	pst_ODE = pst_GO->pst_Base->pst_ODE;

	switch(Id)
	{
	case 0:	/* Mass */
		{
			if(pst_GO->pst_Base->pst_ODE->ode_id_body)
			{
				/*~~~~~~~~~~~~~~~~~~~*/
				dMass			Mass; 
				/*~~~~~~~~~~~~~~~~~~~*/

				switch(pst_ODE->uc_Type)
				{
					case ODE_TYPE_SPHERE:
						dMassSetSphereTotal(&Mass, Param, pst_ODE->f_X);
						dBodySetMass(pst_ODE->ode_id_body, &Mass);
						break;

					case ODE_TYPE_BOX:
						dMassSetBoxTotal(&Mass, Param, pst_ODE->f_X, pst_ODE->f_Y, pst_ODE->f_Z);
						dBodySetMass(pst_ODE->ode_id_body, &Mass);
						break;

					case ODE_TYPE_CYLINDER:
						dMassSetCylinderTotal(&Mass, Param, 2, pst_ODE->f_X, pst_ODE->f_Y);
						dBodySetMass(pst_ODE->ode_id_body, &Mass);
						break;

				}
				return 1;
			}
			else
				return -1;
		}

	case 1: // CFM
		{
			if(pst_GO->pst_Base->pst_ODE)
			{
				pst_GO->pst_Base->pst_ODE->soft_cfm = Param;
				return 1;
			}
			else
				return -1;

		}
	}
#endif
	return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ODESetf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int			Id;
	float		Param;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	Param = AI_PopFloat();
	Id = AI_PopInt();

	AI_PushInt(AI_EvalFunc_ODESetf_C(pst_GO, Id, Param));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ODESetv_C(OBJ_tdst_GameObject *pst_GO, int Id, MATH_tdst_Vector *Vector)
{
#ifdef ODE_INSIDE
	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) || !(pst_GO->pst_Base->pst_ODE))
		return -1;

	switch(Id)
	{
	case 0: // Linear Velocity
		{
			if(pst_GO->pst_Base->pst_ODE->ode_id_body)
				dBodySetLinearVel(pst_GO->pst_Base->pst_ODE->ode_id_body, Vector->x, Vector->y, Vector->z);
		}
		break;

	case 1:	// Offset
		{
			MATH_CopyVector(&pst_GO->pst_Base->pst_ODE->st_Offset, Vector);
		}
		break;


	case 2:	// Force on the gravity center
		{
		}
		break;

	case 3:	// Angular velocity
		{
			if(pst_GO->pst_Base->pst_ODE->ode_id_body)
				dBodySetAngularVel(pst_GO->pst_Base->pst_ODE->ode_id_body, Vector->x, Vector->y, Vector->z);

		}
		break;


	case 4:	// Mass translation
		{
			if(pst_GO->pst_Base->pst_ODE->ode_id_body)
			{
				dMass	mass;

				dBodyGetMass(pst_GO->pst_Base->pst_ODE->ode_id_body, &mass);
				dMassTranslate(&mass, Vector->x, Vector->y, Vector->z);
				dBodySetMass(pst_GO->pst_Base->pst_ODE->ode_id_body, &mass);
			}
		}


	}
#endif
	return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ODESetv(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Id;
	MATH_tdst_Vector	*Vector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	Vector = AI_PopVectorPtr();
	Id = AI_PopInt();

	AI_PushInt(AI_EvalFunc_ODESetv_C(pst_GO, Id, Vector));
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ODEGeti_C(OBJ_tdst_GameObject *pst_GO, int Id)
{
#ifdef ODE_INSIDE
	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) || !(pst_GO->pst_Base->pst_ODE))
		return -1;

	switch(Id)
	{
		case 0:
			{
				return pst_GO->pst_Base->pst_ODE->uc_Flags;
			}
			break;

	}

#endif
	return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ODEGeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int			Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	Id = AI_PopInt();

	AI_PushInt(AI_EvalFunc_ODEGeti_C(pst_GO, Id));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_ODEGetf_C(OBJ_tdst_GameObject *pst_GO, int Id)
{
#ifdef ODE_INSIDE
	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) || !(pst_GO->pst_Base->pst_ODE))
		return -1.0f;

	switch(Id)
	{
	case 0:
		{
			if(pst_GO->pst_Base->pst_ODE->ode_id_body)
			{
				/*~~~~~~~~~~*/
				dMass	Mass; 
				/*~~~~~~~~~~*/

				dBodyGetMass(pst_GO->pst_Base->pst_ODE->ode_id_body, &Mass);
				return Mass.mass;
			}
			return 0.0f;
		}

	case 1:		// CFM
		{
			return pst_GO->pst_Base->pst_ODE->soft_cfm;
		}
		break;


	case 2:		// Linear Velocity
		{
			if(pst_GO->pst_Base->pst_ODE->ode_id_body)
			{
				const float	*pf_Vel;
				float	f_Vel;

				pf_Vel = dBodyGetLinearVel (pst_GO->pst_Base->pst_ODE->ode_id_body);
				f_Vel = pf_Vel[0] * pf_Vel[0] + pf_Vel[1] * pf_Vel[1] + pf_Vel[2] * pf_Vel[2];

				return f_Vel;
			}
			return 0.0f;

		}

	case 3:		// Angular Velocity
		{
			if(pst_GO->pst_Base->pst_ODE->ode_id_body)
			{
				const float	*pf_Ang;
				float	f_Ang;

				pf_Ang = dBodyGetAngularVel (pst_GO->pst_Base->pst_ODE->ode_id_body);
				f_Ang = pf_Ang[0] * pf_Ang[0] + pf_Ang[1] * pf_Ang[1] + pf_Ang[2] * pf_Ang[2];

				return f_Ang;
			}
			else
				return 0.0f;
		}


	}

#endif
	return -1.0f;

}
/**/
AI_tdst_Node *AI_EvalFunc_ODEGetf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int			Id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	Id = AI_PopInt();

	AI_PushFloat(AI_EvalFunc_ODEGetf_C(pst_GO, Id));
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ODEGetv_C(OBJ_tdst_GameObject *pst_GO, int Id, MATH_tdst_Vector *Vector)
{
	if(Vector)
		MATH_InitVector(Vector, 0.0f, 0.0f, 0.0f);

#ifdef ODE_INSIDE
	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE) || !(pst_GO->pst_Base->pst_ODE))
		return;

	switch(Id)
	{
		case 0: // Linear Velocity
			{
				if(pst_GO->pst_Base->pst_ODE->ode_id_body)
				{
					L_memcpy(Vector, (float *) dBodyGetLinearVel(pst_GO->pst_Base->pst_ODE->ode_id_body), 3 * sizeof(float) );
				}
			}
			break;

		case 1:	// Offset
			{
				MATH_CopyVector(Vector, &pst_GO->pst_Base->pst_ODE->st_Offset);
			}
			break;
			
		case 2:
			{	
			
			}
			break;

		case 3: // Angular Velocity
			{
				if(pst_GO->pst_Base->pst_ODE->ode_id_body)
				{
					L_memcpy(Vector, (float *) dBodyGetAngularVel(pst_GO->pst_Base->pst_ODE->ode_id_body), 3 * sizeof(float) );
				}
			}
			break;


	}

#endif
	return;

}
/**/
AI_tdst_Node *AI_EvalFunc_ODEGetv(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Id;
	MATH_tdst_Vector	Vector;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	Id = AI_PopInt();

	AI_EvalFunc_ODEGetv_C(pst_GO, Id, &Vector);

	AI_PushVector(&Vector);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ODEJointCreate_C(OBJ_tdst_GameObject *_pst_GO1, OBJ_tdst_GameObject *_pst_GO2, MATH_tdst_Vector *_pst_Pos, int _i_Type)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	WOR_tdst_World	*pst_World;
	dJointID		ode_joint_id;
	dBodyID			ode_body_1, ode_body_2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	

	if(!_pst_GO1 && !_pst_GO2) return 0;

	pst_World = _pst_GO1 ? WOR_World_GetWorldOfObject(_pst_GO1) : WOR_World_GetWorldOfObject(_pst_GO2);

	ode_body_1 = (_pst_GO1 && _pst_GO1->pst_Base && _pst_GO1->pst_Base->pst_ODE) ? _pst_GO1->pst_Base->pst_ODE->ode_id_body : 0;
	ode_body_2 = (_pst_GO2 && _pst_GO2->pst_Base && _pst_GO2->pst_Base->pst_ODE) ? _pst_GO2->pst_Base->pst_ODE->ode_id_body : 0;

	switch(_i_Type)
	{
	case ODE_JointTypeBall:
		{
			ode_joint_id = dJointCreateBall(pst_World->ode_id_world, pst_World->ode_joint_ode);
			dJointAttach(ode_joint_id, ode_body_1, ode_body_2);
			break;
		}

	case ODE_JointTypeHinge:
		{
			ode_joint_id = dJointCreateHinge(pst_World->ode_id_world, pst_World->ode_joint_ode);
			dJointAttach(ode_joint_id, ode_body_1, ode_body_2);
			break;
		}

	case ODE_JointTypeFixed:
		{
			float			rot[12];
			DYN_tdst_ODE	*pst_ODE;

			if(_pst_GO1)
			{
				pst_ODE = _pst_GO1->pst_Base->pst_ODE;
				ODE_ComputeODEMatrixFromJade(_pst_GO1->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot); 
				dBodySetPosition(ode_body_1, _pst_GO1->pst_GlobalMatrix->T.x + pst_ODE->st_Offset.x, _pst_GO1->pst_GlobalMatrix->T.y + pst_ODE->st_Offset.y, _pst_GO1->pst_GlobalMatrix->T.z + pst_ODE->st_Offset.z);
				dBodySetRotation(ode_body_1, &rot[0]);
			}

			if(_pst_GO2)
			{
				pst_ODE = _pst_GO2->pst_Base->pst_ODE;
				ODE_ComputeODEMatrixFromJade(_pst_GO2->pst_GlobalMatrix, &pst_ODE->st_RotMatrix, rot); 
				dBodySetPosition(ode_body_2, _pst_GO2->pst_GlobalMatrix->T.x + pst_ODE->st_Offset.x, _pst_GO2->pst_GlobalMatrix->T.y + pst_ODE->st_Offset.y, _pst_GO2->pst_GlobalMatrix->T.z + pst_ODE->st_Offset.z);
				dBodySetRotation(ode_body_2, &rot[0]);
			}

			ode_joint_id = dJointCreateFixed (pst_World->ode_id_world, pst_World->ode_joint_ode);
			dJointAttach(ode_joint_id, ode_body_1, ode_body_2);
#ifdef JADEFUSION
			dJointSetFixed((dxJointFixed*)ode_joint_id);
#else
			dJointSetFixed(ode_joint_id);
#endif
			break;
		}

	default:
		return 0;
	}

	return (int) ode_joint_id;
#else
	return 0;
#endif
}

/**/
AI_tdst_Node *AI_EvalFunc_ODEJointCreate(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_GO1;	
	OBJ_tdst_GameObject *pst_GO2;	
	int					i_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	i_Type = AI_PopInt();

	pst_GO2 = AI_PopGameObject();
	pst_GO1 = AI_PopGameObject();


	AI_PushInt(AI_EvalFunc_ODEJointCreate_C(pst_GO1, pst_GO2, &pst_GO->pst_GlobalMatrix->T, i_Type));
	return ++_pst_Node;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ODEJointAxisSet_C(int i_ode_joint_id, MATH_tdst_Vector *_pst_Axis)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	dJointID		ode_joint_id;
	int				i_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ode_joint_id = (dJointID)i_ode_joint_id;
	i_Type = dJointGetType(ode_joint_id);

	if(!ode_joint_id) 
		return;

	switch(i_Type)
	{
	case ODE_JointTypeBall:
		{
			return;
		}

	case ODE_JointTypeHinge:
		{
#ifdef JADEFUSION
			dJointSetHingeAxis((dxJointHinge*)ode_joint_id, _pst_Axis->x, _pst_Axis->y, _pst_Axis->z);
#else
			dJointSetHingeAxis(ode_joint_id, _pst_Axis->x, _pst_Axis->y, _pst_Axis->z);
#endif
			break;
		}

	default:
		return;
	}
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_ODEJointAxisSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Axis;
	int					i_ode_joint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Axis);
	i_ode_joint= AI_PopInt();


	AI_EvalFunc_ODEJointAxisSet_C(i_ode_joint, &st_Axis);

	return ++_pst_Node;

}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ODEJointAnchorSet_C(int i_ode_joint_id, MATH_tdst_Vector *_pst_Anchor)
{
#ifdef ODE_INSIDE
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	dJointID		ode_joint_id;
	int				i_Type;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!i_ode_joint_id) return;

	ode_joint_id = (dJointID)i_ode_joint_id;
	i_Type = dJointGetType(ode_joint_id);

	if(!ode_joint_id) 
		return;

	switch(i_Type)
	{
	case ODE_JointTypeBall:
		{
#ifdef JADEFUSION
			dJointSetBallAnchor((dxJointBall*)ode_joint_id, _pst_Anchor->x, _pst_Anchor->y, _pst_Anchor->z);
#else
			dJointSetBallAnchor(ode_joint_id, _pst_Anchor->x, _pst_Anchor->y, _pst_Anchor->z);
#endif
			break;
		}

	case ODE_JointTypeHinge:
		{
#ifdef JADEFUSION
			dJointSetHingeAnchor((dxJointHinge*)ode_joint_id, _pst_Anchor->x, _pst_Anchor->y, _pst_Anchor->z);
#else
			dJointSetHingeAnchor(ode_joint_id, _pst_Anchor->x, _pst_Anchor->y, _pst_Anchor->z);
#endif
			break;
		}

	default:
		return;
	}
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_ODEJointAnchorSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Anchor;
	int					i_ode_joint;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PopVector(&st_Anchor);
	i_ode_joint= AI_PopInt();

	AI_EvalFunc_ODEJointAnchorSet_C(i_ode_joint, &st_Anchor);

	return ++_pst_Node;

}

void AI_EvalFunc_ODE_DesactiveAllJoints_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	DYN_tdst_ODE		*pst_ODE;
	dJointID			joint_id;
	int					i, numlinks;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)) return;

	pst_ODE = pst_GO->pst_Base->pst_ODE;

	if(!pst_ODE->ode_id_body) return;

	numlinks = dBodyGetNumJoints(pst_ODE->ode_id_body);

	for(i = 0; i < numlinks; i++)
	{
		joint_id = dBodyGetJoint(pst_ODE->ode_id_body, i);

		if(!joint_id) continue;

		dJointAttach(joint_id, 0, 0);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_ODE_DesactiveAllJoints(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_ODE_DesactiveAllJoints_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ODE_LinkedObjectsGet_C(OBJ_tdst_GameObject *pst_GO, OBJ_tdst_GameObject **ppst_Array, int maxnum)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject	*pst_A, *pst_B;
	DYN_tdst_ODE		*pst_ODE;
	dJointID			joint_id;
	dBodyID				body_A, body_B;
	int					i, numlinks, num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!pst_GO || !(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_ODE)) return;

	pst_ODE = pst_GO->pst_Base->pst_ODE;

	if(!pst_ODE->ode_id_body) return;

	numlinks = dBodyGetNumJoints(pst_ODE->ode_id_body);

	num = 0;
	for(i = 0; i < numlinks; i++)
	{
		if(num + 1 > maxnum)
			return;

		joint_id = dBodyGetJoint(pst_ODE->ode_id_body, i);

		if(!joint_id) continue;

		body_A = dJointGetBody(joint_id, 0);
		body_B = dJointGetBody(joint_id, 1);

		pst_A = NULL;
		pst_B = NULL;

		if(body_A)
			pst_A = (OBJ_tdst_GameObject *) dBodyGetData(body_A);

		if(body_B)
			pst_B = (OBJ_tdst_GameObject *) dBodyGetData(body_B);

		if((pst_A == pst_GO) && pst_B)
		{
			if(ppst_Array) ppst_Array[num] = pst_B;
			num++;
		}

		if((pst_B == pst_GO) && pst_A)
		{
			if(ppst_Array) ppst_Array[num] = pst_A;
			num++;
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_ODE_LinkedObjectsGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	AI_tdst_PushVar		st_Var;
	AI_tdst_UnionVar	Val;
	int					maxnum;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	maxnum = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PopVar(&Val, &st_Var);

	AI_EvalFunc_ODE_LinkedObjectsGet_C(pst_GO, (OBJ_tdst_GameObject **) st_Var.pv_Addr, maxnum);
	return ++_pst_Node;
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
