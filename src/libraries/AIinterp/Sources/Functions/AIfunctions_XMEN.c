/*$T AIfunctions_MAT.c GC! 1.100 03/30/01 12:25:02 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "LINks/LINKmsg.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "ENGine/Sources/ANImation/ANIstruct.h"
#include "ENGine/Sources/MODifier/MDFstruct.h"
#include "ENGine/Sources/MODifier/MDFmodifier_XMEN.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void XMEC_OnOff(OBJ_tdst_GameObject *pst_GO,ULONG UserID, ULONG OnOff)
{
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_GameObject		*pst_BoneGO;
	OBJ_tdst_Group			*pst_Skeleton;

	if (OnOff) OnOff = 0;
	else OnOff = MDF_C_Modifier_Inactive;

	if ((pst_GO->pst_Extended ) && (pst_GO->pst_Extended->pst_Modifiers))
	{
		MDF_tdst_Modifier *p_Mod;
		p_Mod = pst_GO->pst_Extended->pst_Modifiers;
		while (p_Mod)
		{
			if (p_Mod ->i ->ul_Type == MDF_C_Modifier_XMEC)
			{
				p_Mod->ul_Flags &= ~MDF_C_Modifier_Inactive;
				p_Mod->ul_Flags |= OnOff;
			}
			p_Mod = p_Mod->pst_Next;
		}
	}


	pst_Skeleton = NULL;
	pst_EndBone = pst_CurrentBone = NULL ;

	if (!pst_GO) return;
	if (!pst_GO->pst_Base) goto GO_SET;
	if (!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) goto GO_SET;
	if (!pst_GO->pst_Base->pst_GameObjectAnim) goto GO_SET;
	if (!((ANI_st_GameObjectAnim *)(pst_GO->pst_Base->pst_GameObjectAnim))->pst_Skeleton) goto GO_SET;


	pst_Skeleton = pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
GO_SET: 
	for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	{
		if (pst_CurrentBone != NULL)
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		else
		{
			pst_BoneGO = pst_GO;
			if (pst_EndBone) 
			{
				pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
				pst_CurrentBone--;
			}
		}

		if(TAB_b_IsAHole(pst_BoneGO))					continue;
		if (!pst_BoneGO->pst_Extended)					continue;
		if (!pst_BoneGO->pst_Extended->pst_Modifiers)	continue;
		{
			MDF_tdst_Modifier *p_Mod;
			p_Mod = pst_BoneGO->pst_Extended->pst_Modifiers;
			while (p_Mod)
			{
				if (p_Mod ->i ->ul_Type == MDF_C_Modifier_XMEC)
				{
					p_Mod->ul_Flags &= ~MDF_C_Modifier_Inactive;
					p_Mod->ul_Flags |= OnOff;
				}
				p_Mod = p_Mod->pst_Next;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void XMEN_OnOff(OBJ_tdst_GameObject *pst_GO,ULONG UserID, ULONG OnOff)
{
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_GameObject		*pst_BoneGO;
	OBJ_tdst_Group			*pst_Skeleton;

	pst_Skeleton = NULL;
	pst_EndBone = pst_CurrentBone = NULL ;
	
	if (OnOff) OnOff = 0;
	else OnOff = MDF_C_Modifier_Inactive;

	if (!pst_GO) return;
	if (!pst_GO->pst_Base) goto GO_SET;
	if (!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) goto GO_SET;
	if (!pst_GO->pst_Base->pst_GameObjectAnim) goto GO_SET;
	if (!((ANI_st_GameObjectAnim *)(pst_GO->pst_Base->pst_GameObjectAnim))->pst_Skeleton) goto GO_SET;


	pst_Skeleton = pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
GO_SET: 
	for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	{
		if (pst_CurrentBone != NULL)
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		else
		{
			pst_BoneGO = pst_GO;
			if (pst_EndBone) 
			{
				pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
				pst_CurrentBone--;
			}
		}

		if(TAB_b_IsAHole(pst_BoneGO))					continue;
		if (!pst_BoneGO->pst_Extended)					continue;
		if (!pst_BoneGO->pst_Extended->pst_Modifiers)	continue;
		{
			MDF_tdst_Modifier *p_Mod;
			p_Mod = pst_BoneGO->pst_Extended->pst_Modifiers;
			while (p_Mod)
			{
				if (p_Mod ->i ->ul_Type == MDF_C_Modifier_XMEN)
				{
					GAO_tdst_ModifierXMEN *p_XMEN;
					p_XMEN = (GAO_tdst_ModifierXMEN *)p_Mod ->p_Data;
					if ((p_XMEN->ulUserID == UserID) || (0xffffffff == UserID))
					{
						p_Mod->ul_Flags &= ~MDF_C_Modifier_Inactive;
						p_Mod->ul_Flags |= OnOff;
					}
				}
				p_Mod = p_Mod->pst_Next;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void XMEN_SetSize(OBJ_tdst_GameObject *pst_GO,ULONG UserID,ULONG  BoneNum ,float NewSize)
{
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	OBJ_tdst_GameObject		*pst_BoneGO;
	OBJ_tdst_Group			*pst_Skeleton;
	
	pst_Skeleton = NULL;
	pst_EndBone = pst_CurrentBone = NULL ;
	
	if (!pst_GO) return;
	if (!pst_GO->pst_Base) goto GO_SET;
	if (!pst_GO->pst_Base->pst_GameObjectAnim) goto GO_SET;
	if (!(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)) goto GO_SET;
	if (!((ANI_st_GameObjectAnim *)(pst_GO->pst_Base->pst_GameObjectAnim))->pst_Skeleton) goto GO_SET;

	pst_Skeleton = pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;

	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);

GO_SET: 

	for(; pst_CurrentBone <= pst_EndBone; pst_CurrentBone++)
	{
		if (pst_CurrentBone != NULL)
			pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		else
		{
			pst_BoneGO = pst_GO;
			if (pst_EndBone) 
			{
				pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
				pst_CurrentBone--;
			}
		}



		if(TAB_b_IsAHole(pst_BoneGO))					continue;
		if (!pst_BoneGO->pst_Extended)					continue;
		if (!pst_BoneGO->pst_Extended->pst_Modifiers)	continue;
		{
			MDF_tdst_Modifier *p_Mod;
			p_Mod = pst_BoneGO->pst_Extended->pst_Modifiers;
			while (p_Mod)
			{
				if (p_Mod ->i ->ul_Type == MDF_C_Modifier_XMEN)
				{
					GAO_tdst_ModifierXMEN *p_XMEN;
					p_XMEN = (GAO_tdst_ModifierXMEN *)p_Mod ->p_Data;
					if ((p_XMEN->ulUserID == UserID) || (0xffffffff == UserID))
					{
						ULONG BoneCounter;
						for (BoneCounter = 0 ; BoneCounter < p_XMEN -> ulNumber_Of_Chhlaahhh ; BoneCounter ++)
						{
							if ((BoneNum == 0xffffffff) || (p_XMEN->p_st_Chhlaahhh[BoneCounter].ulBonesNum == BoneNum))
							{
								p_XMEN->p_st_Chhlaahhh[BoneCounter].fLenght = NewSize;
							}
						}
					}
				}
				p_Mod = p_Mod->pst_Next;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_XMEN_OnOff(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					UserId;
	ULONG					OnOff;
	OBJ_tdst_GameObject		*pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	OnOff =  AI_PopInt();
	UserId = AI_PopInt();
	XMEN_OnOff(pst_GO,UserId , OnOff );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_XMEC_OnOff(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					UserId;
	ULONG					OnOff;
	OBJ_tdst_GameObject		*pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	OnOff =  AI_PopInt();
	UserId = AI_PopInt();
	XMEC_OnOff(pst_GO,UserId , OnOff );
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_XMEN_SetSize(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG					UserId;
	ULONG					BoneNum;
	float					NewSize;
	OBJ_tdst_GameObject		*pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	NewSize=  AI_PopFloat();
	BoneNum = AI_PopInt();
	UserId = AI_PopInt();
	XMEN_SetSize(pst_GO,UserId ,BoneNum ,  NewSize);
	return ++_pst_Node;
}

void SPG2_AddSphere_C(MATH_tdst_Vector	*Pos , float Radius)
{
	extern void SPG2_AddASphere(MATH_tdst_Vector	*Pos , float Radius);
	SPG2_AddASphere(Pos , Radius);
}



AI_tdst_Node *AI_EvalFunc_SPG2_AddSphere(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		Center;
	float					Radius;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	Radius = AI_PopFloat();
	AI_PopVector(&Center);
	SPG2_AddSphere_C(&Center,Radius);
	return ++_pst_Node;
}

void GFX_AddSphere_C(MATH_tdst_Vector	*Pos , MATH_tdst_Vector	*Speed , float Radius , u32 Mode )
{
#ifdef JADEFUSION
	extern void FOGDyn_AddASphere(MATH_tdst_Vector	*Pos , MATH_tdst_Vector	*Speed , float Radius, u32 Mode, OBJ_tdst_GameObject* ptr );
	FOGDyn_AddASphere(Pos , Speed , Radius , Mode, NULL);
#else
	extern void FOGDyn_AddASphere(MATH_tdst_Vector	*Pos , MATH_tdst_Vector	*Speed , float Radius, u32 Mode );
	FOGDyn_AddASphere(Pos , Speed , Radius , Mode);
#endif
}

AI_tdst_Node *AI_EvalFunc_GFX_AddSphere(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector		Center,Speed;
	float					Radius;
	u32						Mode;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	Mode = AI_PopInt();
	Radius = AI_PopFloat();
	AI_PopVector(&Speed);
	AI_PopVector(&Center);
	GFX_AddSphere_C(&Center,&Speed,Radius,Mode);
	return ++_pst_Node;
}


#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
