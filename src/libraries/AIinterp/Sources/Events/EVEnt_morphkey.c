/*$T EVEnt_morphkey.c 1.00 03/08/06 09:30:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/MEMory/MEM.h"
#include "AIinterp/Sources/Events/EVEstruct.h"
#include "AIinterp/Sources/Events/EVEnt_morphkey.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEO.h"
#include "EVEplay.h"
#include "ENGine/Sources/ANImation/ANImain.h"
#include "BIGfiles/LOAding/LOAread.h"

#ifdef ACTIVE_EDITORS
#include "BIGfiles/SAVing/SAVdefs.h"
#endif




/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVE_tdst_MorphKeyParam *EVE_Event_MorphKey_GetParam( EVE_tdst_Event *_pst_MorphKey )
{
	if (_pst_MorphKey->p_Data == NULL) return NULL;
	return (EVE_tdst_MorphKeyParam *)( ((char *)_pst_MorphKey->p_Data) + 4 );
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EVE_Event_MorphKey_AllocData(EVE_tdst_Event *_pst_MorphKey,
								  INT _i_MorphBone,
								  INT _i_Channel,
								  INT _i_Target1,
								  INT _i_Target2,
								  FLOAT _f_Prog,
								  FLOAT _f_Factor)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_MorphKeyParam	*pst_Param;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if (_pst_MorphKey->p_Data)
	{
		MEM_Free(_pst_MorphKey->p_Data);
		_pst_MorphKey->p_Data = NULL;
	}

	_pst_MorphKey->p_Data = MEM_p_Alloc(4 + sizeof(EVE_tdst_MorphKeyParam));
	// Always put size of the data first. Use a short.
	*((short *)_pst_MorphKey->p_Data) = (short) (4 + sizeof(EVE_tdst_MorphKeyParam));
	// The first 4 byte of the data are used. The real parameters come after.
	pst_Param = EVE_Event_MorphKey_GetParam(_pst_MorphKey);

	// Set params
	pst_Param->i_MorphBone = _i_MorphBone;
	pst_Param->i_Channel = _i_Channel;
	pst_Param->i_Target1 = _i_Target1;
	pst_Param->i_Target2 = _i_Target2;
	pst_Param->f_Prog = _f_Prog;
	pst_Param->f_Factor = _f_Factor;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EVE_tdst_Event *EVE_Event_MorphKey_Play(EVE_tdst_Event *_pst_MorphKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_MorphGao;
	GEO_tdst_ModifierMorphing	*pst_MorphModif;
	EVE_tdst_MorphKeyParam		*pst_Param1, *pst_Param2;
	int							i_NextEvtIdx;
	EVE_tdst_Event				*pst_NextKey;
	float						f_Ratio;
	int							ai_ChIdx[4];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Find parameters of the current key
	pst_Param1 = EVE_Event_MorphKey_GetParam(_pst_MorphKey);

	// Find morph modifier
	pst_MorphModif = GEO_pst_ModifierMorphing_Get(EVE_gpst_CurrentGAO);
	if(!pst_MorphModif)
	{
		pst_MorphGao = ANI_pst_GetObjectByAICanal(EVE_gpst_CurrentGAO, (UCHAR) pst_Param1->i_MorphBone);
		if(!pst_MorphGao) return _pst_MorphKey;
		pst_MorphModif = GEO_pst_ModifierMorphing_Get(pst_MorphGao);
		if(!pst_MorphModif) return _pst_MorphKey;
	}

	// Find next morph key and store its parameters too. We assume it's on the same morph modifier
	i_NextEvtIdx = EVE_gpst_CurrentParam->uw_CurrentEvent + 1;
	if(i_NextEvtIdx == EVE_gpst_CurrentTrack->uw_NumEvents)
		i_NextEvtIdx = 0;
	pst_NextKey = EVE_gpst_CurrentTrack->pst_AllEvents + i_NextEvtIdx;
	while ((pst_NextKey->w_Flags & EVE_C_EventFlag_Type) != EVE_C_EventFlag_MorphKey) {
		i_NextEvtIdx++;
		if(i_NextEvtIdx == EVE_gpst_CurrentTrack->uw_NumEvents)
			i_NextEvtIdx = 0;
		pst_NextKey = EVE_gpst_CurrentTrack->pst_AllEvents + i_NextEvtIdx;
	}
	if (pst_NextKey == _pst_MorphKey)
		return _pst_MorphKey;
	pst_Param2 = EVE_Event_MorphKey_GetParam(pst_NextKey);

	// Check the parameters
	if (pst_Param1->i_Target1 > pst_MorphModif->l_NbMorphData || pst_Param1->i_Target1 < 0 ||
		pst_Param1->i_Target2 > pst_MorphModif->l_NbMorphData || pst_Param1->i_Target2 < 0 ||
		pst_Param2->i_Target1 > pst_MorphModif->l_NbMorphData || pst_Param2->i_Target1 < 0 ||
		pst_Param2->i_Target2 > pst_MorphModif->l_NbMorphData || pst_Param2->i_Target2 < 0)
	{
#ifdef ACTIVE_EDITORS
		// WARNING !
		ERR_X_Warning(0, "Some morph targets are invalid !", NULL);
#endif
		return _pst_MorphKey;
	}

	// Compute ratio (time elapsed since beginning of this key) / (time until next key)
	f_Ratio = EVE_gpst_CurrentParam->f_Time / EVE_FrameToTime(EVE_NumFrames(_pst_MorphKey));
	if (f_Ratio > 1.0f) f_Ratio = 1.0f;

	// Find index of the 3 dummy channels associated to this one.
	// We assume the dummy channels have been created with the
	// GEO_ModifierMorphing_CreateDummyChannels function. So we know their location.
	ai_ChIdx[0] = pst_Param1->i_Channel;
	ai_ChIdx[1] = pst_MorphModif->l_NbChannel / 4 + 3 * pst_Param1->i_Channel;
	ai_ChIdx[2] = ai_ChIdx[1] + 1;
	ai_ChIdx[3] = ai_ChIdx[1] + 2;

	// If the dummies have been created with GEO_ModifierMorphing_CreateDummyChannels,
	// their data chain has the right length. We just have to set the targets.
	// WE ASSUME THE NEUTRAL POSE IS DATA 0 !
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[0]].dl_DataIndex[0] = pst_Param1->i_Target1;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[0]].dl_DataIndex[1] = 0;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[1]].dl_DataIndex[0] = pst_Param1->i_Target2;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[1]].dl_DataIndex[1] = 0;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[2]].dl_DataIndex[0] = 0;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[2]].dl_DataIndex[1] = pst_Param2->i_Target1;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[3]].dl_DataIndex[0] = 0;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[3]].dl_DataIndex[1] = pst_Param2->i_Target2;

	// Set the progression
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[0]].f_Blend = pst_Param1->f_Factor * (1 - pst_Param1->f_Prog);
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[0]].f_ChannelBlend = 1.0f + f_Ratio;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[1]].f_Blend = pst_Param1->f_Factor * pst_Param1->f_Prog;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[1]].f_ChannelBlend = 1.0f + f_Ratio;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[2]].f_Blend = pst_Param2->f_Factor * (1 - pst_Param2->f_Prog);
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[2]].f_ChannelBlend = 1.0f + f_Ratio;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[3]].f_Blend = pst_Param2->f_Factor * pst_Param2->f_Prog;
	pst_MorphModif->dst_MorphChannel[ai_ChIdx[3]].f_ChannelBlend = 1.0f + f_Ratio;

	return _pst_MorphKey;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
#ifdef ACTIVE_EDITORS
void EVE_Event_MorphKey_Save(EVE_tdst_Event *_pst_MorphKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_MorphKeyParam	*pst_Param;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Param = EVE_Event_MorphKey_GetParam( _pst_MorphKey );

	SAV_Buffer(&pst_Param->i_MorphBone, sizeof(INT));
	SAV_Buffer(&pst_Param->i_Channel, sizeof(INT));
	SAV_Buffer(&pst_Param->i_Target1, sizeof(INT));
	SAV_Buffer(&pst_Param->i_Target2, sizeof(INT));
	SAV_Buffer(&pst_Param->f_Prog, sizeof(FLOAT));
	SAV_Buffer(&pst_Param->f_Factor, sizeof(FLOAT));
	
	return;
}

// The buffer length should be, at least, 256.
// _pst_MorphGao must not be NULL 
BOOL EVE_Event_MorphKey_Check(
	EVE_tdst_Event *_pst_MorphKey,
	OBJ_tdst_GameObject *_pst_MorphGao,
	char *_pz_Buff,
	int *_pi_ChIdx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pst_MorphGao;
	GEO_tdst_ModifierMorphing	*pst_MorphModif;
	EVE_tdst_MorphKeyParam		*pst_Param;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	// Find parameters of the current key
	pst_Param = EVE_Event_MorphKey_GetParam(_pst_MorphKey);
	*_pi_ChIdx = pst_Param->i_Channel;

	// Find morph modifier
	pst_MorphModif = GEO_pst_ModifierMorphing_Get(_pst_MorphGao);
	if(!pst_MorphModif)
	{
		pst_MorphGao = ANI_pst_GetObjectByAICanal(_pst_MorphGao, (UCHAR) pst_Param->i_MorphBone);
		if(!pst_MorphGao)
		{
			sprintf(
				_pz_Buff,
				"No morph modifier in gao %s",
				_pst_MorphGao->sz_Name);
			return FALSE;
		}
		pst_MorphModif = GEO_pst_ModifierMorphing_Get(pst_MorphGao);
		if(!pst_MorphModif)
		{
			sprintf(
				_pz_Buff,
				"No morph modifier in gao %s",
				_pst_MorphGao->sz_Name);
			return FALSE;
		}
	}

	// Check dummy channels
	if (!GEO_b_ModifierMorphing_CheckDummyChannels( pst_MorphModif ))
	{
		sprintf(
			_pz_Buff,
			"Morph modifier of gao %s doesn't have correct dummy channels",
			_pst_MorphGao->sz_Name);
		return FALSE;
	}
	return TRUE;
}
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EVE_Event_MorphKey_Load(EVE_tdst_Event *_pst_MorphKey, char *_pc_Buffer)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char					*pc_Buf;
	EVE_tdst_MorphKeyParam	st_Param;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pc_Buf = _pc_Buffer;
	st_Param.i_MorphBone = LOA_ReadInt(&pc_Buf);
	st_Param.i_Channel = LOA_ReadInt(&pc_Buf);
	st_Param.i_Target1 = LOA_ReadInt(&pc_Buf);
	st_Param.i_Target2 = LOA_ReadInt(&pc_Buf);
	st_Param.f_Prog = LOA_ReadFloat(&pc_Buf);
	st_Param.f_Factor = LOA_ReadFloat(&pc_Buf);
	EVE_Event_MorphKey_AllocData(
		_pst_MorphKey,
		st_Param.i_MorphBone,
		st_Param.i_Channel,
		st_Param.i_Target1,
		st_Param.i_Target2,
		st_Param.f_Prog,
		st_Param.f_Factor);

	return(pc_Buf - _pc_Buffer);
}