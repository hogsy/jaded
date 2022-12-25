/*$T AIfunctions_ani.c GC! 1.081 11/14/02 18:19:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include <string.h>
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJconst.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJmain.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "ENGine/Sources/ANImation/ANIload.h"
#include "ENGine/Sources/ANImation/ANIinit.h"
#include "ENGine/Sources/ANImation/ANIplay.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "ENGine/Sources/ANImation/ANIaccess.h"
#include "ENGine/Sources/ACTions/ACTstruct.h"
#include "SDK/Sources/BASe/MEMory/MEM.h"
#include "SDK/Sources/BIGfiles/LOAding/LOAdefs.h"
#include "SDK/Sources/BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIpaths.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
/*$off*/
#ifdef PSX2_TARGET
/* only because cpp does not understand '\' in dos format */
#define TstIdFlg(a)		OBJ_b_TestIdentityFlag(a, OBJ_C_IdentityFlag_Anims)
#define TstBase(a)		a->pst_Base
#define TstAnim(a)		a->pst_Base->pst_GameObjectAnim
#define TstTrck(a,i)	(a->pst_Base->pst_GameObjectAnim->uc_AnimUsed & (1 << i))
#define TstTrckNbr(a,i) a->pst_Base->pst_GameObjectAnim->apst_Anim[i]
#define Test(a,i)		a && TstIdFlg(a) && TstBase(a) && TstAnim(a) && TstTrck(a,i) && TstTrckNbr(a,i)

#define M_CheckGameObjectHasAnim(pst_GO, iTrackNumber, pz_Message)		AI_Check(Test(pst_GO, iTrackNumber), pz_Message);

#else
#define M_CheckGameObjectHasAnim(pst_GO, iTrackNumber, pz_Message)						\
	AI_Check																			\
	(																					\
		pst_GO &&																		\
		OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims) &&						\
		pst_GO->pst_Base &&																\
		pst_GO->pst_Base->pst_GameObjectAnim &&											\
		(pst_GO->pst_Base->pst_GameObjectAnim->uc_AnimUsed & (1 << iTrackNumber)) &&	\
		pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[iTrackNumber],					\
		pz_Message																		\
);
/*$on*/
#endif
#ifdef JADEFUSION
void EVE_SetTracksRatio(EVE_tdst_Data *_pst_Data, float _f_Ratio);
#endif
/*$3
 =======================================================================================================================
 =======================================================================================================================
 */

int AI_EvalFunc_ANIFrequencyGet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber)
{
	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_FrequencyGet : Game object does not have an animation");
	return _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->uc_AnimFrequency;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIFrequencyGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ANIFrequencyGet_C(pst_GO, AI_PopInt()));

	return ++_pst_Node;
}

/*$3
 =======================================================================================================================
 =======================================================================================================================
 */

void AI_EvalFunc_ANIBitFieldLODSet_C(OBJ_tdst_GameObject *_pst_GO, char _c_LODBitField)
{
	if
	(
		_pst_GO
	&&	OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)
	&&	_pst_GO->pst_Base
	&&	_pst_GO->pst_Base->pst_GameObjectAnim
	) _pst_GO->pst_Base->pst_GameObjectAnim->c_LOD_Bitfield = _c_LODBitField;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIBitFieldLODSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				c_LODBitField;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	c_LODBitField = AI_PopInt();

	AI_EvalFunc_ANIBitFieldLODSet_C(pst_GO, c_LODBitField);

	return ++_pst_Node;
}

/*$3
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef JADEFUSION
void AI_EvalFunc_ANILODSet_C(OBJ_tdst_GameObject *_pst_GO, UCHAR _c_LOD)
#else
void AI_EvalFunc_ANILODSet_C(OBJ_tdst_GameObject *_pst_GO, char _c_LOD)
#endif
{
	if
	(
		_pst_GO
	&&	OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)
	&&	_pst_GO->pst_Base
	&&	_pst_GO->pst_Base->pst_GameObjectAnim
	) _pst_GO->pst_Base->pst_GameObjectAnim->c_LOD_IA = _c_LOD;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANILODSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	char				c_LOD;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	c_LOD = AI_PopInt();

	AI_EvalFunc_ANILODSet_C(pst_GO, c_LOD);

	return ++_pst_Node;
}


/*$3
 =======================================================================================================================
 =======================================================================================================================
 */

void AI_EvalFunc_ANIPelvisOffsetSet_C(OBJ_tdst_GameObject *_pst_GO, float _z_Offset)
{
	if
	(
		_pst_GO
	&&	OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Anims)
	&&	_pst_GO->pst_Base
	&&	_pst_GO->pst_Base->pst_GameObjectAnim
	) 
	_pst_GO->pst_Base->pst_GameObjectAnim->f_Z_Offset = _z_Offset;

}
/**/
AI_tdst_Node *AI_EvalFunc_ANIPelvisOffsetSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				z_Offset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	z_Offset = AI_PopFloat();

	AI_EvalFunc_ANIPelvisOffsetSet_C(pst_GO, z_Offset);

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANIFrequencySet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber, int _iAnimFrequency)
{
	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_FrequencySet : Game object does not have an animation");
	_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->uc_AnimFrequency = (UCHAR) (((float) _iAnimFrequency * 60.0f / (float) ACT_C_DefaultAnimFrequency) + 0.5f);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIFrequencySet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					iAnimFrequency;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	iAnimFrequency = AI_PopInt();
	AI_Check(iAnimFrequency > 0 && iAnimFrequency < 256, "ANI_FrequencySet : frequency must be between 1 and 255");
	AI_EvalFunc_ANIFrequencySet_C(pst_GO, AI_PopInt(), iAnimFrequency);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANICurrentFrameGet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*p_Data;
	float			f_Cur, f_Tot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_CurrentFrameGet : Game object does not have an animation");
	p_Data = _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->pst_Data;
	EVE_GetTracksTime(p_Data, 0, &f_Cur, &f_Tot);
	return (int) (f_Cur * 60.0f);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANICurrentFrameGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ANICurrentFrameGet_C(pst_GO, AI_PopInt()));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_ANIRatioGet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*p_Data;
	float			f_Cur, f_Tot;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_CurrentFrameGet : Game object does not have an animation");
	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	p_Data = pst_GOAnim->apst_Anim[_iTrackNumber]->pst_Data;
	
	
#ifdef ANIMS_USE_ARAM
	/* If data is not in Cache (and should be), Load it From ARAM */
	{
		ACT_st_ActionKit		*pst_ActionKit;
		EVE_tdst_ListTracks		*pst_TrackList;
	
		pst_ActionKit = pst_GOAnim->pst_ActionKit;	
		pst_TrackList = p_Data->pst_ListTracks;
		if((pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_UseARAM))
		{		
			ACT_i_Cache_LoadAnim(pst_ActionKit, pst_TrackList);
		}
	}
#endif
	
	EVE_GetTracksTime(p_Data, 0, &f_Cur, &f_Tot);
	return (f_Cur / f_Tot);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIRatioGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_ANIRatioGet_C(pst_GO, AI_PopInt()));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANIPartialCurrentFrameGet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*p_Data;
	float			f_Cur, f_Tot;
	int				track, i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO->pst_Base || !_pst_GO->pst_Base->pst_GameObjectAnim || !_pst_GO->pst_Base->pst_GameObjectAnim->apst_PartialAnim[_iTrackNumber]) 
		return -1;

	p_Data = _pst_GO->pst_Base->pst_GameObjectAnim->apst_PartialAnim[_iTrackNumber]->pst_Data;

	track = 0;
	if
	(
		(_pst_GO->pst_Base->pst_GameObjectAnim->aul_PartialMask[0])
	||	(_pst_GO->pst_Base->pst_GameObjectAnim->aul_PartialMask[1])
	)
	{
		for(i = 0; i < (int) p_Data->pst_ListTracks->uw_NumTracks; i++)
		{
			if(ANI_b_GizmoInMask(_pst_GO->pst_Base->pst_GameObjectAnim->aul_PartialMask, p_Data->pst_ListTracks->pst_AllTracks[i].uw_Gizmo))
			{
				track = i;
				break;
			}
		}
	}

#ifdef ANIMS_USE_ARAM
	/* If data is not in Cache (and should be), Load it From ARAM */
	{
		ACT_st_ActionKit		*pst_ActionKit;
		EVE_tdst_ListTracks		*pst_TrackList;
	
		pst_ActionKit = _pst_GO->pst_Base->pst_GameObjectAnim->pst_ActionKit;	
		pst_TrackList = p_Data->pst_ListTracks;
		if((pst_TrackList->ul_GC_Flags & EVE_C_ListTracks_UseARAM))
		{		
			ACT_i_Cache_LoadAnim(pst_ActionKit, pst_TrackList);
		}
	}
#endif

	EVE_GetTracksTime(p_Data, track, &f_Cur, &f_Tot);
	return (int) (f_Cur * 60.0f);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIPartialCurrentFrameGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ANIPartialCurrentFrameGet_C(pst_GO, AI_PopInt()));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANICurrentFrameSet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber, int _iCurrentFrame)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*p_Data;
	/*~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_CurrentFrameSet : Game object does not have an animation");
	p_Data = _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->pst_Data;
	EVE_SetTracksTime(p_Data, _iCurrentFrame * (1.0f / 60.0f));
}
/**/
AI_tdst_Node *AI_EvalFunc_ANICurrentFrameSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					iCurrentFrame;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	iCurrentFrame = AI_PopInt();
	AI_EvalFunc_ANICurrentFrameSet_C(pst_GO, AI_PopInt(), iCurrentFrame);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANIRatioSet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber, float _f_Ratio)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*p_Data;
	/*~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_CurrentFrameSet : Game object does not have an animation");
	p_Data = _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->pst_Data;
	EVE_SetTracksRatio(p_Data, _f_Ratio);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIRatioSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				fRatio;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	fRatio = AI_PopFloat();
	AI_EvalFunc_ANIRatioSet_C(pst_GO, AI_PopInt(), fRatio);	
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANISetEnd_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	int				i, i_Evt;
	EVE_tdst_Data	*p_Data;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectHasAnim(pst_GO, 0, "ANI_SetEnd : Game object does not have an animation");

	p_Data = pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data;
	for(i = 0; i < p_Data->pst_ListTracks->uw_NumTracks; i++)
	{
		if(p_Data->pst_ListTracks->pst_AllTracks[i].uw_NumEvents)
		{
			i_Evt = p_Data->pst_ListTracks->pst_AllTracks[i].uw_NumEvents - 1;
			p_Data->pst_ListParam[i].uw_CurrentEvent = i_Evt;
			p_Data->pst_ListParam[i].f_Time = EVE_FrameToTime(p_Data->pst_ListTracks->pst_AllTracks[i].pst_AllEvents[i_Evt].uw_NumFrames & 0x7FFF);
		}
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_ANISetEnd(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_ANISetEnd_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANINbFrameGet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*p_Data;
	float			f_Cur, f_Tot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_NbFrameGet : Game object does not have an animation");
	p_Data = _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->pst_Data;

	EVE_GetTracksTime(p_Data, 0, &f_Cur, &f_Tot);
	return (int) (f_Tot * 60.0f);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANINbFrameGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ANINbFrameGet_C(pst_GO, AI_PopInt()));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANIPartialNbFrameGet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*p_Data;
	float			f_Cur, f_Tot;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_GO->pst_Base || !_pst_GO->pst_Base->pst_GameObjectAnim || !_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber])
		return -1;

	p_Data = _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->pst_Data;

	EVE_GetTracksTime(p_Data, 0, &f_Cur, &f_Tot);
	return (int) (f_Tot * 60.0f);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIPartialNbFrameGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ANIPartialNbFrameGet_C(pst_GO, AI_PopInt()));

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANIAnimGet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifdef ACTIVE_EDITORS
	EVE_tdst_ListTracks *pst_TrackList;
	BIG_KEY				ul_Key;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_AnimGet : Game object does not have an animation");
	pst_TrackList = _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->pst_Data->pst_ListTracks;
	ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_TrackList);
	AI_Check(ul_Key != BIG_C_InvalidIndex, "ANI_AnimGet : Animation is not referenced");
	return (int) ul_Key;
#else
	return BIG_C_InvalidKey;
#endif
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIAnimGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ANIAnimGet_C(pst_GO, AI_PopInt()));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANIFlagGet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber)
{
	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_FlagGet : Game object does not have an animation");
	return _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->uw_Flag;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIFlagGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ANIFlagGet_C(pst_GO, AI_PopInt()));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANIFlagSet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber, int _iFlagToSet, int _iFlagToReset)
{
	/*~~~~~~~~~~~~~~*/
	USHORT	*puw_Flag;
	/*~~~~~~~~~~~~~~*/

	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_FlagSet : Game object does not have an animation");
	puw_Flag = &_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->uw_Flag;
	*puw_Flag |= _iFlagToSet;
	*puw_Flag &= ~_iFlagToReset;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIFlagSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					iFlagToSet, iFlagToReset;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	iFlagToReset = AI_PopInt();
	iFlagToSet = AI_PopInt();

	AI_EvalFunc_ANIFlagSet_C(pst_GO, AI_PopInt(), iFlagToSet, iFlagToReset);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANIFlagTest_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber, int _iFlagToTest)
{
	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_FlagTest : Game object does not have an animation");
	return _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->uw_Flag & _iFlagToTest;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIFlagTest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					iFlagToTest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	iFlagToTest = AI_PopInt();
	AI_PushInt(AI_EvalFunc_ANIFlagTest_C(pst_GO, AI_PopInt(), iFlagToTest));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANIIsInBlend_C(OBJ_tdst_GameObject *pst_GO)
{
	M_CheckGameObjectHasAnim(pst_GO, 0, "ANI_IsInBlend : Game object does not have an animation");
	return pst_GO->pst_Base->pst_GameObjectAnim->uc_AnimUsed & 2 ? 1 : 0;
}
/**/
int AI_EvalFunc_ANIIsInBlend_C_CURRENT(void)
{
	return AI_gpst_CurrentGameObject->pst_Base->pst_GameObjectAnim->uc_AnimUsed & 2 ? 1 : 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIIsInBlend(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ANIIsInBlend_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANIModeGet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber)
{
	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_ModeGet : Game object does not have an animation");
	return (_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->uw_Flag &ANI_C_AnimFlag_PlayModeMask) >> 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIModeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_ANIModeGet_C(pst_GO, AI_PopInt()));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANIModeSet_C(OBJ_tdst_GameObject *_pst_GO, int _iTrackNumber, int _iModeToSet)
{
	/*~~~~~~~~~~~~~~*/
	USHORT	*puw_Flag;
	/*~~~~~~~~~~~~~~*/

	M_CheckGameObjectHasAnim(_pst_GO, _iTrackNumber, "ANI_ModeSet : Game object does not have an animation");
	puw_Flag = &_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[_iTrackNumber]->uw_Flag;
	*puw_Flag &= ~ANI_C_AnimFlag_PlayModeMask;
	*puw_Flag |= _iModeToSet << 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIModeSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					iModeToSet;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	iModeToSet = AI_PopInt();

	AI_EvalFunc_ANIModeSet_C(pst_GO, AI_PopInt(), iModeToSet);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANIShapeSelect_C(OBJ_tdst_GameObject *pst_GO, int canal, int pos)
{
	ANI_ChangeBoneShape(pst_GO, (UCHAR) canal, (UCHAR) pos, TRUE);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIShapeSelect(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	UCHAR				uc_Canal;
	UCHAR				uc_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	uc_Pos = AI_PopInt();
	uc_Canal = AI_PopInt();
	AI_EvalFunc_ANIShapeSelect_C(pst_GO, uc_Canal, uc_Pos);
	return ++_pst_Node;
}

extern UCHAR	ANI_uc_GetEngineCanalByAICanal(OBJ_tdst_GameObject *, UCHAR);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_ANIShapeObjectGet_C(OBJ_tdst_GameObject *pst_GO, UCHAR canal, UCHAR visual)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_BoneGO;
	OBJ_tdst_GameObject *pst_GrpGO;
	UCHAR				uc_ENG_Canal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if
	(
		pst_GO
	&&	(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	&&	pst_GO->pst_Base
	&&	pst_GO->pst_Base->pst_GameObjectAnim
	&&	pst_GO->pst_Base->pst_GameObjectAnim->pst_Shape
	)
	{
		pst_BoneGO = ANI_pst_GetObjectByAICanal(pst_GO, canal);
		if(!pst_BoneGO) return NULL;

		uc_ENG_Canal = ANI_uc_GetEngineCanalByAICanal(pst_GO, canal);

		if(uc_ENG_Canal == 255)
		{
			ERR_X_Warning(0, "[Shape] ANI_ShapeObjectGet", pst_GO->sz_Name);
			return NULL;
		}

		/* If visual == 0xFF, we want to get the Object of the current Visual. */
		if(visual == 0xFF)
		{
			visual = pst_GO->pst_Base->pst_GameObjectAnim->auc_DefaultVisu[uc_ENG_Canal];
			if(visual == 0xFF) return NULL;
		}

		if
		(
			pst_BoneGO
		&&	pst_BoneGO->pst_Extended
		&&	pst_BoneGO->pst_Extended->pst_Group
		&&	(pst_BoneGO->pst_Extended->pst_Group->pst_AllObjects->ul_NbElems > visual)
		)
		{
			pst_GrpGO = (OBJ_tdst_GameObject *) ((pst_BoneGO->pst_Extended->pst_Group->pst_AllObjects->p_Table + visual)->p_Pointer);
			return pst_GrpGO;
		}
		else
			return NULL;
	}
	else
		return NULL;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIShapeObjectGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	UCHAR				uc_Canal, uc_Visual;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	uc_Visual = AI_PopInt();
	uc_Canal = AI_PopInt();
	AI_PushGameObject(AI_EvalFunc_ANIShapeObjectGet_C(pst_GO, uc_Canal, uc_Visual));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANIShapeGet_C(OBJ_tdst_GameObject *pst_GO, int canal)
{
	if
	(
		pst_GO
	&&	(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Anims)
	&&	pst_GO->pst_Base
	&&	pst_GO->pst_Base->pst_GameObjectAnim
	&&	pst_GO->pst_Base->pst_GameObjectAnim->pst_Shape
	)
	{
		/*~~~~~~~~~~~~~~~~~*/
		int		visual;
		UCHAR	uc_ENG_Canal;
		/*~~~~~~~~~~~~~~~~~*/

		uc_ENG_Canal = ANI_uc_GetEngineCanalByAICanal(pst_GO, (UCHAR) canal);

		if(uc_ENG_Canal == 255)
		{
			ERR_X_Warning(0, "[Shape] ANI_ShapeGet", pst_GO->sz_Name);
			return -1;
		}

		visual = pst_GO->pst_Base->pst_GameObjectAnim->auc_DefaultVisu[uc_ENG_Canal];
		return visual;
	}

	return -1;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIShapeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	UCHAR				uc_Canal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	uc_Canal = AI_PopInt();
	AI_PushInt(AI_EvalFunc_ANIShapeGet_C(pst_GO, uc_Canal));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *AI_EvalFunc_ANICanalGaoGet_C(OBJ_tdst_GameObject *pst_GO, int canal)
{
	return ANI_pst_GetObjectByAICanal(pst_GO, (UCHAR) canal);
}
/**/
OBJ_tdst_GameObject *AI_EvalFunc_ANICanalGaoGet_C_CURRENT(int canal)
{
	return ANI_pst_GetObjectByAICanal(AI_gpst_CurrentGameObject, (UCHAR) canal);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANICanalGaoGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	UCHAR				uc_Canal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	uc_Canal = AI_PopInt();
	AI_PushGameObject(AI_EvalFunc_ANICanalGaoGet_C(pst_GO, uc_Canal));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_ANICanalClosestGaoGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Vector	st_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PopVector(&st_Pos);
	AI_PushGameObject(ANI_pst_GetClosestObjCanal(pst_GO, &st_Pos));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANISkeletonChange_C(OBJ_tdst_GameObject *pst_GO1, OBJ_tdst_GameObject *pst_GO2)
{
	ANI_ChangeDefaultSkeleton(pst_GO1, pst_GO2);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANISkeletonChange(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO1, *pst_GO2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO1);
	pst_GO2 = AI_PopGameObject();

	AI_EvalFunc_ANISkeletonChange_C(pst_GO1, pst_GO2);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANICloneSet_C(OBJ_tdst_GameObject *pst_GO1, OBJ_tdst_GameObject *pst_GO2, int _i_NumAction)
{
	ANI_CloneSet(pst_GO1, pst_GO2, (ULONG) _i_NumAction);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANICloneSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO1, *pst_GO2;
	int					i_NumAction;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO1);
	i_NumAction = AI_PopInt();
	pst_GO2 = AI_PopGameObject();

	AI_EvalFunc_ANICloneSet_C(pst_GO1, pst_GO2, i_NumAction);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANISkeletonRestore_C(OBJ_tdst_GameObject *pst_GO1, OBJ_tdst_GameObject *pst_GO2)
{
	ANI_RestoreDefaultSkeleton(pst_GO1, pst_GO2);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANISkeletonRestore(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO1, *pst_GO2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO1);
	pst_GO2 = AI_PopGameObject();

	AI_EvalFunc_ANISkeletonRestore_C(pst_GO1, pst_GO2);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANIIsBoneAnimed_C(OBJ_tdst_GameObject *_pst_GO, int _i_BoneIndex)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_BoneGO;
	EVE_tdst_ListTracks *pst_ListTracks;
	OBJ_tdst_Group		*pst_Skeleton;
	int					i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_BoneGO = ANI_pst_GetObjectByAICanal(_pst_GO,(UCHAR) _i_BoneIndex);
	if(!pst_BoneGO) return 0;

	if
	(
		!_pst_GO
	||	!_pst_GO->pst_Base
	||	!_pst_GO->pst_Base->pst_GameObjectAnim
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton
	||	!_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]
	) return 0;

	pst_ListTracks = _pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[0]->pst_Data->pst_ListTracks;
	pst_Skeleton = _pst_GO->pst_Base->pst_GameObjectAnim->pst_Skeleton;
	for(i = 0; i < pst_ListTracks->uw_NumTracks; i++)
	{
		if(pst_BoneGO == (OBJ_tdst_GameObject *)(pst_Skeleton->pst_AllObjects->p_Table + pst_ListTracks->pst_AllTracks[i].uw_Gizmo)->p_Pointer) 
			return 1;
	}

	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIIsBoneAnimed(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	LONG				l_BoneIndex;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	l_BoneIndex = AI_PopInt();

	AI_PushInt(AI_EvalFunc_ANIIsBoneAnimed_C(pst_GO, l_BoneIndex));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANIBlendFrameMaxSet_C(OBJ_tdst_GameObject *_pst_GO, int _iMaxBlendFrames)
{
	float	f_TotalBlend, f_CurrentBlend, f_RemainingBlendTime;	
	float	f_NewTotal, f_NewCurrent;
	UCHAR	uc_RemainingBlend;

	if(!_pst_GO || !_pst_GO->pst_Base || !_pst_GO->pst_Base->pst_GameObjectAnim || !_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]) return;

	f_TotalBlend = (_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->uc_BlendTime + 1) / (60.0f);
	f_CurrentBlend = (_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->uw_BlendCurTime) / ((float) 60 * 256);
	f_RemainingBlendTime = f_TotalBlend - f_CurrentBlend;

	uc_RemainingBlend = (UCHAR) EVE_TimeToFrame(f_RemainingBlendTime);

	if(uc_RemainingBlend > (UCHAR) _iMaxBlendFrames)
	{
		f_NewCurrent = ((f_CurrentBlend / f_TotalBlend) / (1.0f - (f_CurrentBlend / f_TotalBlend))) * EVE_FrameToTime(_iMaxBlendFrames);
		f_NewTotal = f_NewCurrent + EVE_FrameToTime(_iMaxBlendFrames);

		_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->uc_BlendTime = (UCHAR) EVE_TimeToFrame(f_NewTotal);
		_pst_GO->pst_Base->pst_GameObjectAnim->apst_Anim[1]->uw_BlendCurTime = ((USHORT) EVE_TimeToFrame(f_NewCurrent)) * 256;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIBlendFrameMaxSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_ANIBlendFrameMaxSet_C(pst_GO, AI_PopInt());

	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANISignalMaskSet_C(OBJ_tdst_GameObject *pst_GO, int on, int off)
{
    UCHAR ucOn, ucOff;

	if
	(																					
		!pst_GO 
	|| !OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims)
	|| !pst_GO->pst_Base 
	|| !pst_GO->pst_Base->pst_GameObjectAnim
	)
	return;

    ucOn = 0xFF & on;
    ucOff = 0xFF & off;

	pst_GO->pst_Base->pst_GameObjectAnim->uc_Signal |= ucOn;
	pst_GO->pst_Base->pst_GameObjectAnim->uc_Signal &= ~ucOff;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANISignalMaskSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					on, off;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	off = AI_PopInt();
	on = AI_PopInt();
	AI_EvalFunc_ANISignalMaskSet_C(pst_GO, on, off);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANISignalSet_C(OBJ_tdst_GameObject *pst_GO, int Signal)
{

	if
	(																					
		!pst_GO 
	|| !OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims)
	|| !pst_GO->pst_Base 
	|| !pst_GO->pst_Base->pst_GameObjectAnim
	)
	return;

	pst_GO->pst_Base->pst_GameObjectAnim->uc_Signal = 0xFF & Signal;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANISignalSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					signal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	signal = AI_PopInt();

	AI_EvalFunc_ANISignalSet_C(pst_GO, signal);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_ANISignalGet_C(OBJ_tdst_GameObject *pst_GO)
{

	if
	(																					
		!pst_GO 
	|| !OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims)
	|| !pst_GO->pst_Base 
	|| !pst_GO->pst_Base->pst_GameObjectAnim
	)
	return 0;

	return (0xFF & pst_GO->pst_Base->pst_GameObjectAnim->uc_Signal);
}
/**/
AI_tdst_Node *AI_EvalFunc_ANISignalGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_ANISignalGet_C(pst_GO));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANIStockMatrix_C(OBJ_tdst_GameObject *_pst_GO)
{
	TAB_tdst_PFelem			*pst_CurrentBone, *pst_EndBone;
	MATH_tdst_Matrix		*M;
	ANI_st_GameObjectAnim	*pst_GOAnim;
	OBJ_tdst_Group			*pst_Skeleton;
	OBJ_tdst_GameObject		*pst_BoneGO;
	ULONG					i;

	pst_GOAnim = _pst_GO->pst_Base->pst_GameObjectAnim;
	pst_Skeleton = pst_GOAnim->pst_Skeleton;

	/* Stock Matrixes */
	pst_CurrentBone = TAB_pst_PFtable_GetFirstElem(pst_Skeleton->pst_AllObjects);
	pst_EndBone = TAB_pst_PFtable_GetLastElem(pst_Skeleton->pst_AllObjects);
	for(i = 0; pst_CurrentBone <= pst_EndBone; i++, pst_CurrentBone++)
	{
		pst_BoneGO = (OBJ_tdst_GameObject *) pst_CurrentBone->p_Pointer;
		ERR_X_Assert(!TAB_b_IsAHole(pst_BoneGO));
			
		M = OBJ_pst_GetLocalMatrix(pst_BoneGO);
		MATH_CopyMatrix(&pst_GOAnim->dpst_Stock[i], M);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIStockMatrix(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_ANIStockMatrix_C(pst_GO);

	return ++_pst_Node;
}



/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_ANIPause_C(OBJ_tdst_GameObject *pst_GO, int Pause)
{
	if
	(																					
		!pst_GO 
	|| !OBJ_b_TestIdentityFlag(pst_GO, OBJ_C_IdentityFlag_Anims)
	|| !pst_GO->pst_Base 
	|| !pst_GO->pst_Base->pst_GameObjectAnim
	)
	return;

	pst_GO->pst_Base->pst_GameObjectAnim->uc_PauseAnim = Pause;
}
/**/
AI_tdst_Node *AI_EvalFunc_ANIPause(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					Pause;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	Pause = AI_PopInt();
	AI_EvalFunc_ANIPause_C(pst_GO, Pause);
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
