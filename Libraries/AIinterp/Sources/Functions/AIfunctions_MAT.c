/*$T AIfunctions_MAT.c GC! 1.081 07/02/02 08:38:43 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "LINks/LINKmsg.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "TEXture/TEXprocedural.h"
#include "TEXture/TEXanimated.h"
#include "BIGfiles/LOAding/LOAdefs.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
#define MAT_VALIDATEMTRL()	(_pst_GRO)
#define MAT_ValidateSUBMAT() \
	{ \
		OBJ_tdst_GameObject *pst_GO; \
		AI_M_GetCurrentObject(pst_GO); \
		_pst_GRO = MAT_ValidateSUBMAT_C(pst_GO, AI_PopInt()); \
	}
#define MAT_ValidateSUBMAT_AI2C(a, b) \
	{ \
		_pst_GRO = MAT_ValidateSUBMAT_C(a, b); \
	}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GRO_tdst_Struct *MAT_ValidateSUBMAT_C(OBJ_tdst_GameObject *pst_GO, ULONG ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct *pst_GRO;
	ULONG			ul_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	pst_GRO = NULL;
	if(ul_Key & 0xff000000)
	{
		/* It's a Key */
		ul_Pos = BIG_ul_SearchKeyToPos(ul_Key);
		if(ul_Pos != (ULONG) - 1)
		{
			pst_GRO = (GRO_tdst_Struct *) LOA_ul_SearchAddress(ul_Pos);
			if((ULONG) pst_GRO == (ULONG) - 1) pst_GRO = NULL;
		}
	}
	else
	{
		/* It's a submaterial number */
		if(OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Visu))
		{
			if(pst_GO->pst_Base->pst_Visu)
			{
				if(pst_GO->pst_Base->pst_Visu->pst_Material)
				{
					if(pst_GO->pst_Base->pst_Visu->pst_Material->i->ul_Type == GRO_MaterialMulti)
					{
						pst_GRO = (GRO_tdst_Struct *) pst_GO->pst_Base->pst_Visu->pst_Material;
						pst_GRO = (GRO_tdst_Struct *) ((MAT_tdst_Multi *) pst_GRO)->dpst_SubMaterial[ul_Key % ((MAT_tdst_Multi *) pst_GRO)->l_NumberOfSubMaterials];
					}
					else
						pst_GRO = pst_GO->pst_Base->pst_Visu->pst_Material;
				}
			}
		}
	}

	return pst_GRO;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATSetDiffuse_C(OBJ_tdst_GameObject *pst_GO, int Color, int Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			*pul_Color;
	GRO_tdst_Struct *_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MAT_ValidateSUBMAT_AI2C(pst_GO, Key);
	pul_Color = NULL;
	if(MAT_VALIDATEMTRL()) MAT_GetColorPtr(_pst_GRO, &pul_Color, 0, 0);
	if(pul_Color) *pul_Color = Color;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATSetDiffuse(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Color;
	int					Key;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Key = AI_PopInt();
	Color = AI_PopInt();
	AI_EvalFunc_MATSetDiffuse_C(pst_GO, Color, Key);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MATGetDiffuse_C(OBJ_tdst_GameObject *pst_GO, int key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				Color;
	ULONG			*pul_Color;
	GRO_tdst_Struct *_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MAT_ValidateSUBMAT_AI2C(pst_GO, key);
	Color = 0;
	pul_Color = NULL;
	if(MAT_VALIDATEMTRL()) MAT_GetColorPtr(_pst_GRO, &pul_Color, 0, 0);
	if(pul_Color) Color = *pul_Color;
	return Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_MATGetDiffuse(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Key;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Key = AI_PopInt();
	AI_PushInt(AI_EvalFunc_MATGetDiffuse_C(pst_GO, Key));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATSetConstant_C(OBJ_tdst_GameObject *pst_GO, int Color, int key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG			*pul_Color;
	GRO_tdst_Struct *_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MAT_ValidateSUBMAT_AI2C(pst_GO, key);
	pul_Color = NULL;
	if(MAT_VALIDATEMTRL()) MAT_GetColorPtr(_pst_GRO, &pul_Color, 0, 1);
	if(pul_Color) *pul_Color = Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_MATSetConstant(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Color;
	int					Key;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Key = AI_PopInt();
	Color = AI_PopInt();
	AI_EvalFunc_MATSetConstant_C(pst_GO, Color, Key);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MATGetConstant_C(OBJ_tdst_GameObject *pst_GO, int key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				Color;
	ULONG			*pul_Color;
	GRO_tdst_Struct *_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MAT_ValidateSUBMAT_AI2C(pst_GO, key);
	Color = 0;
	pul_Color = NULL;
	if(MAT_VALIDATEMTRL()) MAT_GetColorPtr(_pst_GRO, &pul_Color, 0, 1);
	if(pul_Color) Color = *pul_Color;
	return Color;
}
/**/
AI_tdst_Node *AI_EvalFunc_MATGetConstant(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	key = AI_PopInt();
	AI_PushInt(AI_EvalFunc_MATGetConstant_C(pst_GO, key));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_MATSetAmbient(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				Color;
	ULONG			*pul_Color;
	GRO_tdst_Struct *_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MAT_ValidateSUBMAT();
	Color = AI_PopInt();
	pul_Color = NULL;
	if(MAT_VALIDATEMTRL()) MAT_GetColorPtr(_pst_GRO, &pul_Color, 0, 2);
	if(pul_Color) *pul_Color = Color;
	return ++_pst_Node;
}
/**/
AI_tdst_Node *AI_EvalFunc_MATGetAmbient(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int				Color;
	ULONG			*pul_Color;
	GRO_tdst_Struct *_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	MAT_ValidateSUBMAT();
	Color = 0;
	pul_Color = NULL;
	if(MAT_VALIDATEMTRL()) MAT_GetColorPtr(_pst_GRO, &pul_Color, 0, 2);
	if(pul_Color) Color = *pul_Color;
	AI_PushInt(Color);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GetFlag_C(OBJ_tdst_GameObject *pst_GO, int key, int i_Level)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct		*_pst_GRO;
	MAT_tdst_MTLevel	*p_MtxLvl;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	MAT_ValidateSUBMAT_AI2C(pst_GO, key);
	if(MAT_VALIDATEMTRL()) 
	{
		if (i_Level == -1)
			return(((MAT_tdst_MultiTexture *) _pst_GRO)->ul_Flags);

		if (i_Level >= 1000 )
		{
			MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, i_Level - 1000);
			if(p_MtxLvl)
				return p_MtxLvl->s_AditionalFlags;
		}
		
		MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, i_Level);
		if(p_MtxLvl)
			return p_MtxLvl->ul_Flags;
	}
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_GetFlag(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					key, i_Level;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Level = AI_PopInt();
	key = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_GetFlag_C(pst_GO, key, i_Level));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SetFlag_C(OBJ_tdst_GameObject *pst_GO, int key, int i_Level, int i_One, int i_Zero)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GRO_tdst_Struct		*_pst_GRO;
	MAT_tdst_MTLevel	*p_MtxLvl;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	MAT_ValidateSUBMAT_AI2C(pst_GO, key);
	if(MAT_VALIDATEMTRL())
	{
		if (i_Level == -1)
		{
			((MAT_tdst_MultiTexture *) _pst_GRO)->ul_Flags |= i_One;
			((MAT_tdst_MultiTexture *) _pst_GRO)->ul_Flags &= ~i_Zero;
		}
		else
		{
			if (i_Level >= 1000 )
			{
				MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, i_Level - 1000);
				if(p_MtxLvl)
				{
					p_MtxLvl->s_AditionalFlags |= i_One;
					p_MtxLvl->s_AditionalFlags &= ~i_Zero; 
				}
			}
			else
			{
				MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, i_Level);
				if(p_MtxLvl)
				{
					p_MtxLvl->ul_Flags |= i_One;
					p_MtxLvl->ul_Flags &= ~i_Zero; 
				}
			}
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_SetFlag(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_One, i_Zero, i_Level;
	OBJ_tdst_GameObject *pst_GO;
	int					key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Zero = AI_PopInt();
	i_One = AI_PopInt();
	i_Level = AI_PopInt();
	key = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_SetFlag_C(pst_GO, key, i_Level, i_One, i_Zero);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
//void AI_EvalFunc_MatMTL_SetFlag_C(OBJ_tdst_GameObject *pst_GO, int id, int level, int i_One, int i_Zero)
//{
//	/*~~~~~~~~~~~~~~~~~~~~~~*/
//    MAT_tdst_MTLevel	*p_MtxLvl;
//	GRO_tdst_Struct		*_pst_GRO;
//    /*~~~~~~~~~~~~~~~~~~~~~~*/
//	
//    p_MtxLvl = NULL;
//    _pst_GRO = MAT_ValidateSUBMAT_C(pst_GO, id); 
//	if(MAT_VALIDATEMTRL()) MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, level);
//	if(p_MtxLvl)
//	{
//		/*~~~~~~~~~~~~~*/
//		ULONG	ul_Flags;
//		/*~~~~~~~~~~~~~*/
//
//		ul_Flags = MAT_GET_FLAG(p_MtxLvl->ul_Flags);
//        ul_Flags |= i_One;
//        ul_Flags &= ~i_Zero; 
//		MAT_SET_FLAG(p_MtxLvl->ul_Flags, ul_Flags);
//	}
//}
///**/
//AI_tdst_Node *AI_EvalFunc_MatMTL_SetFlag(AI_tdst_Node *_pst_Node)
//{
//	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//	int					i_One, i_Zero;
//	OBJ_tdst_GameObject *pst_GO;
//	int					id, level;
//	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//
//	i_Zero = AI_PopInt();
//	i_One = AI_PopInt();
//    level = AI_PopInt();
//	id = AI_PopInt();
//	AI_M_GetCurrentObject(pst_GO);
//	AI_EvalFunc_MatMTL_SetFlag_C(pst_GO, id, level, i_One, i_Zero);
//	return ++_pst_Node;
//}
//
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATEnableMltxtLvl_C(OBJ_tdst_GameObject*pGO, int id ,int Level, int bEnabld)
{
	MAT_tdst_MTLevel	*p_MtxLvl;
	GRO_tdst_Struct		*_pst_GRO;
	
    p_MtxLvl = NULL;
    _pst_GRO = MAT_ValidateSUBMAT_C(pGO, id); 
	if(MAT_VALIDATEMTRL()) MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, Level);
	if(p_MtxLvl)
	{
		/*~~~~~~~~~~~~~*/
		ULONG	ul_Flags;
		/*~~~~~~~~~~~~~*/

		ul_Flags = MAT_GET_FLAG(p_MtxLvl->ul_Flags);
		if(!bEnabld)
			ul_Flags |= MAT_Cul_Flag_InActive;
		else
			ul_Flags &= ~MAT_Cul_Flag_InActive;
		MAT_SET_FLAG(p_MtxLvl->ul_Flags, ul_Flags);
	}
}
AI_tdst_Node *AI_EvalFunc_MATEnableMltxtLvl(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Level, bEnabld;
    OBJ_tdst_GameObject *pst_GO;
    int id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/	

	bEnabld = AI_PopInt();
	Level = AI_PopInt();
    id = AI_PopInt();

	AI_M_GetCurrentObject(pst_GO); 
    AI_EvalFunc_MATEnableMltxtLvl_C(pst_GO, id, Level, bEnabld);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATEnableMltxtLvlMT_C(OBJ_tdst_GameObject *pst_GO, int key, int Level, int SubMat, int bEnabld)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_MTLevel	*p_MtxLvl;
	GRO_tdst_Struct		*_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_MtxLvl = NULL;

	MAT_ValidateSUBMAT_AI2C(pst_GO, key);
	if(MAT_VALIDATEMTRL()) MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, SubMat, Level);
	if(p_MtxLvl)
	{
		/*~~~~~~~~~~~~~*/
		ULONG	ul_Flags;
		/*~~~~~~~~~~~~~*/

		ul_Flags = MAT_GET_FLAG(p_MtxLvl->ul_Flags);
		if(!bEnabld)
			ul_Flags |= MAT_Cul_Flag_InActive;
		else
			ul_Flags &= ~MAT_Cul_Flag_InActive;
		MAT_SET_FLAG(p_MtxLvl->ul_Flags, ul_Flags);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MATEnableMltxtLvlMT(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Level, bEnabld;
	int					SubMat;
	int					key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	bEnabld = AI_PopInt();
	SubMat = AI_PopInt();
	Level = AI_PopInt();
	key = AI_PopInt();
	AI_EvalFunc_MATEnableMltxtLvlMT_C(pst_GO, key, Level, SubMat, bEnabld);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GetLocalAlpha_C(OBJ_tdst_GameObject *pst_GO, int key, int Level)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					LocalAlpha;
	MAT_tdst_MTLevel	*p_MtxLvl;
	GRO_tdst_Struct		*_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_MtxLvl = NULL;

	MAT_ValidateSUBMAT_AI2C(pst_GO, key);
	if(MAT_VALIDATEMTRL()) MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, Level);
	if(p_MtxLvl)
		LocalAlpha = MAT_GET_LocalAlpha(p_MtxLvl->s_AditionalFlags);
	else
		LocalAlpha = 0;
	return LocalAlpha << 3;
}
/**/
AI_tdst_Node *AI_EvalFunc_GetLocalAlpha(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Level, Key;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Level = AI_PopInt();
	Key = AI_PopInt();
	AI_PushInt(AI_EvalFunc_GetLocalAlpha_C(pst_GO, Key, Level));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SetLocalAlpha_C(OBJ_tdst_GameObject *pst_GO, int Key, int Level, int LocalAlpha)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_MTLevel	*p_MtxLvl;
	GRO_tdst_Struct		*_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_MtxLvl = NULL;
	MAT_ValidateSUBMAT_AI2C(pst_GO, Key);
	if(MAT_VALIDATEMTRL()) MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, Level);
	if(p_MtxLvl) MAT_SET_LocalAlpha(p_MtxLvl->s_AditionalFlags, LocalAlpha >> 3);
}
/**/
AI_tdst_Node *AI_EvalFunc_SetLocalAlpha(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Level, LocalAlpha;
	int					Key;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	LocalAlpha = AI_PopInt();
	Level = AI_PopInt();
	Key = AI_PopInt();
	AI_EvalFunc_SetLocalAlpha_C(pst_GO, Key, Level, LocalAlpha);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_GetALPHATRESH_C(OBJ_tdst_GameObject *pst_GO, int key, int Level)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					LocalAlpha;
	MAT_tdst_MTLevel	*p_MtxLvl;
	GRO_tdst_Struct		*_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_MtxLvl = NULL;
	_pst_GRO = MAT_ValidateSUBMAT_C(pst_GO, key); 
	
	if(MAT_VALIDATEMTRL()) MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, Level);

	if(p_MtxLvl)
		LocalAlpha = MAT_GET_AlphaTresh(p_MtxLvl->ul_Flags);
	else
		LocalAlpha = 0;

	return LocalAlpha;
}

AI_tdst_Node *AI_EvalFunc_GetALPHATRESH(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int key,Level;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


	Level = AI_PopInt();
 	key = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO); 

	AI_PushInt(AI_EvalFunc_GetALPHATRESH_C(pst_GO, key, Level));
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SetALPHATRESH_C(OBJ_tdst_GameObject *pst_GO, int key, int Level, int LocalAlpha )
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_MTLevel	*p_MtxLvl;
	GRO_tdst_Struct		*_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_MtxLvl = NULL;
	_pst_GRO = MAT_ValidateSUBMAT_C(pst_GO, key); 
	
	if(MAT_VALIDATEMTRL()) MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, Level);
	if(p_MtxLvl) MAT_SET_AlphaTresh(p_MtxLvl->ul_Flags, LocalAlpha);
}

AI_tdst_Node *AI_EvalFunc_SetALPHATRESH(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Level, LocalAlpha, key;
	OBJ_tdst_GameObject *pst_GO; 
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	LocalAlpha = AI_PopInt();
	Level = AI_PopInt();
	key = AI_PopInt();	 
	AI_M_GetCurrentObject(pst_GO); 

	AI_EvalFunc_SetALPHATRESH_C(pst_GO, key, Level, LocalAlpha);

	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Animated textures functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MATAntGet_C(OBJ_tdst_GameObject *pst_GO, int key, int Level)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Result;
	GRO_tdst_Struct		*_pst_GRO;
	MAT_tdst_MTLevel	*p_MtxLvl;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Result = 0;
	MAT_ValidateSUBMAT_AI2C(pst_GO, key);
	if(MAT_VALIDATEMTRL())
	{
		MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, Level);
		if(p_MtxLvl)
		{
			Result = ((int) TEX_pst_Anim_Get(p_MtxLvl->s_TextureId));
		}
	}

	return Result;
}
/**/
AI_tdst_Node *AI_EvalFunc_MATAntGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					key, Level;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Level = AI_PopInt();
	key = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_MATAntGet_C(pst_GO, key, Level));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATAntSeti_C(int ant, int param, int value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Animated *pst_Ant;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Ant = (TEX_tdst_Animated *) ant;
	if( !pst_Ant ) return;

    if (param == 1)
    {
        if (value == 0)
            pst_Ant->uc_AniFlags &= ~TEXANI_Flags_Frozen; 
        else
            pst_Ant->uc_AniFlags |= TEXANI_Flags_Frozen; 
    }
    else if (param == 2)
    {
        if (value == -1)
            pst_Ant->uc_AniFlags &= ~TEXANI_Flags_UseRequested; 
        else
        {
            pst_Ant->uc_AniFlags |= TEXANI_Flags_UseRequested; 
            pst_Ant->uc_Requested = (char) value;
        }
    }
}
/**/
AI_tdst_Node *AI_EvalFunc_MATAntSeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	int param, value, i0;
	/*~~~~~~~~~~~~~~~~~*/

	value = AI_PopInt();
	param = AI_PopInt();
	i0 = AI_PopInt();
	AI_EvalFunc_MATAntSeti_C(i0, param, value);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    Procedural textures functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MATGetPro_C(OBJ_tdst_GameObject *pst_GO, int key, int Level)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Result;
	GRO_tdst_Struct		*_pst_GRO;
	MAT_tdst_MTLevel	*p_MtxLvl;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Result = 0;
	MAT_ValidateSUBMAT_AI2C(pst_GO, key);
	if(MAT_VALIDATEMTRL())
	{
		MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, Level);
		if(p_MtxLvl)
		{
			Result = ((int) TEX_pst_Procedural_Get(p_MtxLvl->s_TextureId));
		}
	}

	return Result;
}
/**/
AI_tdst_Node *AI_EvalFunc_MATGetPro(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					key, Level;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Level = AI_PopInt();
	key = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_MATGetPro_C(pst_GO, key, Level));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_MATProSeti_C(int pro, int param, int value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Procedural *pst_Pro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Pro = (TEX_tdst_Procedural *) pro;
	if(!pst_Pro) return;

	if(pst_Pro->i->ul_Type == TEXPRO_Photo)
	{
		if(param == 0)
			((TEXPRO_tdst_Photo *) pst_Pro->p_Data)->c_AskedPhoto = value;
		else if(param == 1) ((TEXPRO_tdst_Photo *) pst_Pro->p_Data)->c_Update = 1;
        else if (param == 2) TEXPRO_Photo_SetMapGao( pst_Pro, value ); 
        else if (param == 3) 
        {
            if (((TEXPRO_tdst_Photo *) pst_Pro->p_Data)->ul_CarteObj == 0x10000000)
            {
                ((TEXPRO_tdst_Photo *) pst_Pro->p_Data)->ul_CarteObj = 128;
                ((TEXPRO_tdst_Photo *) pst_Pro->p_Data)->f_Time = ((float) value) / 1000.0f;
                ((TEXPRO_tdst_Photo *) pst_Pro->p_Data)->f_TimeLeft = ((TEXPRO_tdst_Photo *) pst_Pro->p_Data)->f_Time;
            }
        }
    }
	else if(pst_Pro->i->ul_Type == TEXPRO_Mpeg)
	{
		if(param == 0)
		{
			if(value)
				((TEXPRO_tdst_Mpeg *) pst_Pro->p_Data)->c_Flags |= TEXPRO_C_Mpeg_Pause;
			else
				((TEXPRO_tdst_Mpeg *) pst_Pro->p_Data)->c_Flags &= ~TEXPRO_C_Mpeg_Pause;
		}
		else if(param == 1)
		{
			if(value)
				((TEXPRO_tdst_Mpeg *) pst_Pro->p_Data)->c_Flags |= TEXPRO_C_Mpeg_StopAtEnd;
			else
				((TEXPRO_tdst_Mpeg *) pst_Pro->p_Data)->c_Flags &= ~TEXPRO_C_Mpeg_StopAtEnd;
		}
		else if(param == 10)
		{
			((TEXPRO_tdst_Mpeg *) pst_Pro->p_Data)->c_Request = value;
		}
		else if (param == 11)
		{
			TEX_Procedural_UpdateWithAnother( pst_Pro, (TEX_tdst_Procedural *) value );
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_MATProSeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~*/
	int param, value, i0;
	/*~~~~~~~~~~~~~~~~~*/

	value = AI_PopInt();
	param = AI_PopInt();
	i0 = AI_PopInt();
	AI_EvalFunc_MATProSeti_C(i0, param, value);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_MATProGeti_C(int pro, int param)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TEX_tdst_Procedural *pst_Pro;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Pro = (TEX_tdst_Procedural *) pro;
	if(!pst_Pro) return 0;

	if(pst_Pro->i->ul_Type == TEXPRO_Photo)
	{
		if (param == 0)
		{
			if ( 
					( ((TEXPRO_tdst_Photo *) pst_Pro->p_Data)->c_Photo == ((TEXPRO_tdst_Photo *) pst_Pro->p_Data)->c_AskedPhoto ) &&
					( ((TEXPRO_tdst_Photo *) pst_Pro->p_Data)->c_Update == 0)
				)
				return 1;
			else
				return 0;

		}
	}
	else if(pst_Pro->i->ul_Type == TEXPRO_Mpeg)
	{
		if(param == 0)
		{
			return (((TEXPRO_tdst_Mpeg *) pst_Pro->p_Data)->c_Flags &TEXPRO_C_Mpeg_Pause) ? 1 : 0;
		}
	}

	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_MATProGeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~*/
	int param, i0;
	/*~~~~~~~~~~*/

	param = AI_PopInt();
	i0 = AI_PopInt();
	AI_PushInt(AI_EvalFunc_MATProGeti_C(i0, param));
	return ++_pst_Node;
}

/* UV */
#define MAT_GetPos	MAT_GetSpeed

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_GetUV_C(OBJ_tdst_GameObject *pst_GO, int key, int Level, int mode)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_MTLevel	*p_MtxLvl;
	GRO_tdst_Struct		*_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	p_MtxLvl = NULL;

	MAT_ValidateSUBMAT_AI2C(pst_GO, key);
	if(MAT_VALIDATEMTRL()) MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, Level);
	if(p_MtxLvl)
	{
		switch(mode)
		{
		case 0:
			return MAT_GetPos(p_MtxLvl->ScaleSPeedPosU);
		case 1:
			return MAT_GetPos(p_MtxLvl->ScaleSPeedPosV);
		case 2:
			return MAT_GetScale(p_MtxLvl->ScaleSPeedPosU);
		case 3:
			return MAT_GetScale(p_MtxLvl->ScaleSPeedPosV);
		case 4:
			return MAT_GetRotation( p_MtxLvl->ScaleSPeedPosU, p_MtxLvl->ScaleSPeedPosV );
		}
	}

	return 0.0f;
}
/**/
AI_tdst_Node *AI_EvalFunc_GetUV(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Level, Key, mode;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	mode = AI_PopInt();
	Level = AI_PopInt();
	Key = AI_PopInt();
	AI_PushFloat(AI_EvalFunc_GetUV_C(pst_GO, Key, Level, mode));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SetUV_C(OBJ_tdst_GameObject *pst_GO, int Key, int Level, int mode, float Value)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MAT_tdst_MTLevel	*p_MtxLvl;
	GRO_tdst_Struct		*_pst_GRO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	p_MtxLvl = NULL;
	MAT_ValidateSUBMAT_AI2C(pst_GO, Key);
	if(MAT_VALIDATEMTRL()) MAT_GetMtxLvlPtr(_pst_GRO, &p_MtxLvl, 0, Level);
	if(p_MtxLvl)
	{
		switch(mode)
		{
		case 0:
			MAT_SetPos(&p_MtxLvl->ScaleSPeedPosU, Value);
			break;
		case 1:
			MAT_SetPos(&p_MtxLvl->ScaleSPeedPosV, Value);
			break;
		case 2:
			MAT_SetScale(&p_MtxLvl->ScaleSPeedPosU, Value);
			break;
		case 3:
			MAT_SetScale(&p_MtxLvl->ScaleSPeedPosV, Value);
			break;
		case 4:
			MAT_SetRotation( &p_MtxLvl->ScaleSPeedPosU, &p_MtxLvl->ScaleSPeedPosV, Value );
			break;
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_SetUV(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					Level, mode;
	int					Key;
	float				fValue;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	fValue = AI_PopFloat();
	mode = AI_PopInt();
	Level = AI_PopInt();
	Key = AI_PopInt();
	AI_EvalFunc_SetUV_C(pst_GO, Key, Level, mode, fValue);
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
