/*$T AIfunctions_snd.c GC 1.138 04/04/05 15:27:04 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/AIstack.h"

#include "SouND/Sources/SND.h"
#include "SouND/Sources/SNDwave.h"
#include "SouND/Sources/SNDstruct.h"
#include "SouND/Sources/SNDvolume.h"
#include "SouND/Sources/SNDload.h"
#include "SouND/Sources/SNDmodifier.h"
#include "SouND/Sources/SNDconst.h"
#include "SouND/Sources/SNDfx.h"
#include "SouND/Sources/SNDtrack.h"
#include "SouND/Sources/SNDstream.h"
#include "SouND/Sources/SNDdirect.h"
#include "SouND/Sources/SNDinterface.h"
#include "SouND/Sources/SNDmusic.h"
#include "SouND/Sources/SNDambience.h"
#include "SouND/Sources/SNDdialog.h"
#include "SouND/Sources/SNDloadingsound.h"
#include "SouND/Sources/SNDbank.h"

#include "AIinterp/Sources/Events/EVEplay.h"

#include "ENGine/Sources/OBJects/OBJorient.h"
#include "ENGine/Sources/MoDiFier/MDFstruct.h"
#include "ENGine/Sources/MoDiFier/MDFmodifier_GEN.h"
#ifdef ACTIVE_EDITORS
#include "EDItors/Sources/SOuNd/SONmsg.h"
#endif
#include "LINks/LINKtoed.h"

#include "BASe/BENch/BENch.h"

#ifdef _FINAL_
#define M_StartRaster()
#define M_StopRaster()
#else
#define M_StartRaster() _GSP_BeginRaster(39)
#define M_StopRaster()	_GSP_EndRaster(39)
#endif


/*$4
 ***********************************************************************************************************************
    prototypes
 ***********************************************************************************************************************
 */

AI_tdst_Node	*AI_EvalFunc_SND_SoundDurationGet(AI_tdst_Node *_pst_Node);
float			SND_SoundDurationGet(OBJ_tdst_GameObject *pst_GO, int idx);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

AI_tdst_Node	*AI_EvalFunc_SND_SetObject(AI_tdst_Node *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

extern float	AI_EvalFunc_MATHRandFloat_C(float _f1, float _f2);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

AI_tdst_Node	*AI_EvalFunc_SNDInstanceSeti(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDInstanceGeti(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDInstanceSetf(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDInstanceGetf(AI_tdst_Node *);

/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

void			AI_EvalFunc_SNDPrefetchModifier_C(OBJ_tdst_GameObject *, int _i_id);
AI_tdst_Node	*AI_EvalFunc_SNDPrefetchModifier(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDPrefetch(AI_tdst_Node *);
void			AI_EvalFunc_SNDFlushModifier_C(OBJ_tdst_GameObject *, int _i_id);
AI_tdst_Node	*AI_EvalFunc_SNDFlushModifier(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDFlush(AI_tdst_Node *);

void			AI_EvalFunc_SNDInstPosVSet_C(int _l_Index, MATH_tdst_Vector *);
AI_tdst_Node	*AI_EvalFunc_SNDInstPosVSet(AI_tdst_Node *);

AI_tdst_Node	*AI_EvalFunc_SNDBankRestore(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDBankMergeOver(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDBankMerge(AI_tdst_Node *);
void			SND_MergeBankNoOver(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *);
void			SND_MergeBankOver(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *);

AI_tdst_Node	*AI_EvalFunc_SNDMicroPosSet(AI_tdst_Node *);
void			AI_EvalFunc_SNDMicroPosSet_C(OBJ_tdst_GameObject *);
int				AI_EvalFunc_SNDGetInstanceModifier_C(OBJ_tdst_GameObject *, int _i_id);
AI_tdst_Node	*AI_EvalFunc_SNDSetGlobalVol(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDGetGlobalVol(AI_tdst_Node *);

int				AI_EvalFunc_SNDRequest_C(OBJ_tdst_GameObject *, int _i_Sound, ULONG _ul_Flags);
AI_tdst_Node	*AI_EvalFunc_SNDRequest(AI_tdst_Node *);
int				AI_EvalFunc_SNDRequestPlay_C(OBJ_tdst_GameObject *, int _i_Snd);
AI_tdst_Node	*AI_EvalFunc_SNDRequestPlay(AI_tdst_Node *);

void			AI_EvalFunc_SNDSoundPlayerForAnimators_C(int _i_CurSoundIndex);
AI_tdst_Node	*AI_EvalFunc_SNDSoundPlayerForAnimators(AI_tdst_Node *);

AI_tdst_Node	*AI_EvalFunc_SNDDestroy(AI_tdst_Node *);

AI_tdst_Node	*AI_EvalFunc_SNDSetPan(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDGetPan(AI_tdst_Node *);

AI_tdst_Node	*AI_EvalFunc_SNDSetFreq(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDGetFreq(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDSetVol(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDGetVol(AI_tdst_Node *);
void			AI_EvalFunc_SNDUltraPlaySoundLooped_C(OBJ_tdst_GameObject *, int index, int nb);
AI_tdst_Node	*AI_EvalFunc_SNDUltraPlaySoundLooped(AI_tdst_Node *);
void			AI_EvalFunc_SNDUltraPlaySound_C(OBJ_tdst_GameObject *, int index);
AI_tdst_Node	*AI_EvalFunc_SNDUltraPlaySound(AI_tdst_Node *);

AI_tdst_Node	*AI_EvalFunc_SNDStopSound(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDPauseSound(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDIsPlaying(AI_tdst_Node *);

AI_tdst_Node	*AI_EvalFunc_SNDFxSeti(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDFxSetf(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDFxGeti(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDFxGetf(AI_tdst_Node *);

AI_tdst_Node	*AI_EvalFunc_SNDSetf(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDGetf(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDSeti(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDGeti(AI_tdst_Node *);

void			AI_EvalFunc_SNDPlayModifier_C(OBJ_tdst_GameObject *, int _i_id);
void			AI_EvalFunc_SNDPauseModifier_C(OBJ_tdst_GameObject *, int _i_id);
void			AI_EvalFunc_SNDStopModifier_C(OBJ_tdst_GameObject *, int _i_id);
AI_tdst_Node	*AI_EvalFunc_SNDGetInstanceModifier(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDPlayModifier(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDPauseModifier(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDStopModifier(AI_tdst_Node *);
AI_tdst_Node	*AI_EvalFunc_SNDGetStatusModifier(AI_tdst_Node *);
int				AI_EvalFunc_SNDGetStatusModifier_C(OBJ_tdst_GameObject *, int _i_id);

AI_tdst_Node	*AI_EvalFunc_SNDPrefetchDialog(AI_tdst_Node *_pst_Node);
void			AI_EvalFunc_SNDPrefetchDialog_C(TEXT_tdst_Eval *_pst_Text, float _f_TimeOut);

// Needed
extern int		AI_EvalFunc_TEXTToSound_C(TEXT_tdst_Eval *_pst_Text);

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SNDRequest_C(OBJ_tdst_GameObject *_pst_GO, int _i_Sound, ULONG _ul_Flags)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	LONG				i;
	MATH_tdst_Matrix	*pst_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;

	AI_Check((_ul_Flags != -1), "Invalid parameter");

	M_StartRaster();
	pst_Pos = _pst_GO->pst_GlobalMatrix;

	if(SND_Cul_GlobalSoundReferencing & _ul_Flags)
		SND_gst_Params.ul_Flags |= SND_Cte_UseGlobalIndex;

	i = SND_l_Request((void *) _pst_GO, _i_Sound);


	SND_gst_Params.ul_Flags &= ~SND_Cte_UseGlobalIndex;
	_ul_Flags &= ~SND_Cul_GlobalSoundReferencing;

	if(i != -1)
	{
		SND_SetFlags(_pst_GO, i, _ul_Flags);
		SND_SetPos((void *) _pst_GO, i, pst_Pos);
		if(_ul_Flags & SND_Cul_DSF_AutoPlay) SND_PlaySound(i);
	}

	M_StopRaster();
	return i;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDRequest(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG				ul_Flags;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Flags = (ULONG) AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_SNDRequest_C(pst_GO, AI_PopInt(), ul_Flags));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SNDRequestPlayLoop_C(OBJ_tdst_GameObject *_pst_GO, int _i_Snd)
{
	if(!SND_gst_Params.l_Available) return -1;
	_i_Snd = AI_EvalFunc_SNDRequest_C(_pst_GO, _i_Snd, SND_Cul_DSF_DestroyWhenFinished);
	SND_PlaySoundLooping(_i_Snd, -1);
	return _i_Snd;
}
/**/
AI_tdst_Node *AI_EvalFunc_SNDRequestPlayLoop(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Snd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Snd = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_SNDRequestPlayLoop_C(pst_GO, i_Snd));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SNDRequestPlay_C(OBJ_tdst_GameObject *_pst_GO, int _i_Snd)
{
	if(!SND_gst_Params.l_Available) return -1;
	_i_Snd = AI_EvalFunc_SNDRequest_C(_pst_GO, _i_Snd, SND_Cul_DSF_DestroyWhenFinished);
	SND_PlaySound(_i_Snd);
	return _i_Snd;
}
/**/
AI_tdst_Node *AI_EvalFunc_SNDRequestPlay(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Snd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Snd = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_SNDRequestPlay_C(pst_GO, i_Snd));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SNDRequestPlayDialog_C(OBJ_tdst_GameObject *_pst_GO, TEXT_tdst_Eval *_pst_Text)
{
	int				i_SndIdx;
#ifdef JADEFUSION
	bool			b_GlobIdx;
#else
	BOOL			b_GlobIdx;
#endif

	if(!SND_gst_Params.l_Available) return -1;

	i_SndIdx = AI_EvalFunc_TEXTToSound_C(_pst_Text);
	if((i_SndIdx < 0) || (i_SndIdx >= SND_gst_Params.l_SoundNumber)) return -1;

	b_GlobIdx = (SND_gst_Params.ul_Flags & SND_Cte_UseGlobalIndex);
	SND_gst_Params.ul_Flags |= SND_Cte_UseGlobalIndex;
	i_SndIdx = AI_EvalFunc_SNDRequest_C(_pst_GO, i_SndIdx, SND_Cul_DSF_DestroyWhenFinished);
	if (!b_GlobIdx)
		SND_gst_Params.ul_Flags &= ~SND_Cte_UseGlobalIndex;
	SND_PlaySound(i_SndIdx);
	return i_SndIdx;
}
/**/
AI_tdst_Node *AI_EvalFunc_SNDRequestPlayDialog(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	TEXT_tdst_Eval	st_Text;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	st_Text.i_Id = AI_PopInt();
	st_Text.i_FileKey= AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_SNDRequestPlayDialog_C(pst_GO, &st_Text));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDSoundPlayerForAnimators_C(int _i_CurSoundIndex) {
	
#ifdef ACTIVE_EDITORS

	static float			sf_CurRatio = 0.0f;
	static int				si_CurInstanceIndex = -1;
	static int				si_OldSoundIndex = -1;
	static BOOL				sb_IsPlaying = FALSE;
	SND_tdst_SoundInstance	*pst_SI;
    SND_tdst_OneSound	    *pst_Sound;

	sf_CurRatio += EVE_f_GetCurrentRatio();
	if (sf_CurRatio > 1) sf_CurRatio = 1; // To correct little errors
	if (sf_CurRatio < 0) sf_CurRatio = 0;

	if(!ENG_gb_EVERunning) {
		if (sb_IsPlaying) {
			// Force reloading of instance. Necessary for streams : we must get a new instance so that
			// it will resume playing from the correct position (depending on the current time of event)
			si_OldSoundIndex = -2;
			SND_Release(si_CurInstanceIndex);
			sb_IsPlaying = FALSE;
		}
		return;
	} else {
		if (sb_IsPlaying) return;
		sb_IsPlaying = TRUE;
	}

	if(!SND_gst_Params.l_Available) return;

	if (si_OldSoundIndex != _i_CurSoundIndex) {          // Update of variables needed
		si_OldSoundIndex = _i_CurSoundIndex;
		if (_i_CurSoundIndex == -1) {           // Invalid sound
			si_CurInstanceIndex = -1;
			return;
		}
		for (si_CurInstanceIndex=0; si_CurInstanceIndex<SND_gst_Params.l_InstanceNumber; si_CurInstanceIndex++)
			if (SND_gst_Params.dst_Instance[si_CurInstanceIndex].l_Sound == _i_CurSoundIndex) break;
		if(si_CurInstanceIndex >= SND_gst_Params.l_InstanceNumber) {
			// No instance of this sound
			// Load it
			si_CurInstanceIndex = SND_l_Request(NULL, _i_CurSoundIndex);
		}
	}

	SND_M_GetInstanceOrReturn(si_CurInstanceIndex, pst_SI, ;);
    SND_M_GetSoundOrReturn(_i_CurSoundIndex, pst_Sound, ;);
	if (!pst_Sound->pst_Wave) return;
	SND_PlayRq(pst_SI, 0, sf_CurRatio*pst_Sound->pst_Wave->ul_DataSize, -1, 0);
	
#endif // ACTIVE_EDITORS

}

AI_tdst_Node *AI_EvalFunc_SNDSoundPlayerForAnimators(AI_tdst_Node *_pst_Node) {
	AI_EvalFunc_SNDSoundPlayerForAnimators_C(AI_PopInt());
	return ++_pst_Node;
}

extern OBJ_tdst_GameObject	*AI_EvalFunc_ANICanalGaoGet_C(OBJ_tdst_GameObject *, int);

extern OBJ_tdst_GameObject	*EVE_gpst_CurrentGAO;
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SNDRequestPlayOnCanal_C(OBJ_tdst_GameObject *_pst_GO, int _i_Snd, int _i_Canal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pBone;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;

	if(!_pst_GO && ENG_gb_ANIRunning)
	{
		_pst_GO = EVE_gpst_CurrentGAO;
	}

	/* get the bone */
	pBone = AI_EvalFunc_ANICanalGaoGet_C(_pst_GO, _i_Canal);
	if(!pBone) pBone = _pst_GO;

	/* get the instance */
	_i_Snd = AI_EvalFunc_SNDRequest_C(_pst_GO, _i_Snd, SND_Cul_DSF_DestroyWhenFinished);

	/* set the obj for instance */
	SND_SetObject(pBone, _i_Snd);

	/* play instance */
	SND_PlaySound(_i_Snd);

	return _i_Snd;
}
/**/
AI_tdst_Node *AI_EvalFunc_SNDRequestPlayOnObjCanal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Snd;
	int					i_Canal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Canal = AI_PopInt();
	i_Snd = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_SNDRequestPlayOnCanal_C(pst_GO, i_Snd, i_Canal));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SNDRequestPlayLoopOnCanal_C(OBJ_tdst_GameObject *_pst_GO, int _i_Snd, int _i_Canal)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject			*pBone;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return -1;

	if(!_pst_GO && ENG_gb_ANIRunning)
	{
		_pst_GO = EVE_gpst_CurrentGAO;
	}

	/* get the bone */
	pBone = AI_EvalFunc_ANICanalGaoGet_C(_pst_GO, _i_Canal);
	if(!pBone) pBone = _pst_GO;

	/* get the instance */
	_i_Snd = AI_EvalFunc_SNDRequest_C(_pst_GO, _i_Snd, SND_Cul_DSF_DestroyWhenFinished);

	/* set the obj for instance */
	SND_SetObject(pBone, _i_Snd);

	/* play instance */
	SND_PlaySoundLooping(_i_Snd, -1);

	return _i_Snd;
}
/**/
AI_tdst_Node *AI_EvalFunc_SNDRequestPlayLoopOnObjCanal(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Snd;
	int					i_Canal;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_Canal = AI_PopInt();
	i_Snd = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_SNDRequestPlayLoopOnCanal_C(pst_GO, i_Snd, i_Canal));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDDestroy(AI_tdst_Node *_pst_Node)
{
	SND_Release(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDSetPan(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~*/
	int i_Pan, i_Front;
	/*~~~~~~~~~~~~~~~*/

	i_Front = AI_PopInt();
	i_Pan = AI_PopInt();
	SND_SetPan(AI_PopInt(), i_Pan, i_Front);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDGetPan(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(SND_l_GetPan(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDSetFreq(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~*/
	int i_Sound, i_Freq;
	/*~~~~~~~~~~~~~~~~*/

	i_Freq = AI_PopInt();
	i_Sound = AI_PopInt();
	M_StartRaster();
	SND_SetFreq(i_Sound, i_Freq);
	M_StopRaster();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDGetFreq(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(SND_l_GetFreq(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDSetVol(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~*/
	int		i_Sound;
	float	f_Vol;
	/*~~~~~~~~~~~~*/

	f_Vol = AI_PopFloat();
	i_Sound = AI_PopInt();
	SND_SetVol(i_Sound, f_Vol);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDGetVol(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(SND_f_GetVol(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDUltraPlaySoundLooped_C(OBJ_tdst_GameObject *pst_GO, int index, int nb)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix		*pst_Pos;
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	SND_M_GetInstanceOrReturn(index, pst_SI, ;);

	pst_SI->ul_Flags |= SND_Cul_OSF_Loop;
	pst_SI->i_LoopNb = nb;

	if(SND_i_IsPlaying(index)) return;

	M_StartRaster();
	SND_i_CheckOrInitGAO(pst_GO);
	pst_SI->p_GameObject = (void *) (pst_GO);
	pst_Pos = pst_GO->pst_GlobalMatrix;
	SND_SetPos((void *) pst_GO, index, pst_Pos);
	SND_PlaySoundLooping(index, nb);
	M_StopRaster();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDUltraPlaySoundLooped(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					index, nb;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	nb = AI_PopInt();
	index = AI_PopInt();
	AI_EvalFunc_SNDUltraPlaySoundLooped_C(pst_GO, index, nb);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDUltraPlaySound_C(OBJ_tdst_GameObject *pst_GO, int index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Matrix	*pst_Pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(SND_gst_Params.l_InstanceNumber <= index) return;
	if(index < 0) return;
	if(SND_i_IsPlaying(index)) return;

	M_StartRaster();
	SND_i_CheckOrInitGAO(pst_GO);
	SND_gst_Params.dst_Instance[index].p_GameObject = (void *) (pst_GO);
	pst_Pos = pst_GO->pst_GlobalMatrix;
	SND_SetPos((void *) pst_GO, index, pst_Pos);
	SND_PlaySound(index);
	M_StopRaster();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDUltraPlaySound(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	index = AI_PopInt();
	AI_EvalFunc_SNDUltraPlaySound_C(pst_GO, index);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDStopSound(AI_tdst_Node *_pst_Node)
{
	SND_StopRq(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDPauseSound(AI_tdst_Node *_pst_Node)
{
	SND_Pause(AI_PopInt());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDIsPlaying(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(SND_i_IsPlaying(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDFxSeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~*/
	int rq, val;
	/*~~~~~~~~*/

	val = AI_PopInt();
	rq = AI_PopInt();
	AI_PushInt(SND_i_FxSeti(rq, val));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDSeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~*/
	int rq, val;
	/*~~~~~~~~*/

	val = AI_PopInt();
	rq = AI_PopInt();
	AI_PushInt(SND_i_Seti(rq, val));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDFxSetf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~*/
	int		rq;
	float	val;
	/*~~~~~~~~*/

	val = AI_PopFloat();
	rq = AI_PopInt();
	AI_PushInt(SND_i_FxSetf(rq, val));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDFxGeti(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int rq;
	/*~~~*/

	rq = AI_PopInt();
	AI_PushInt(SND_i_FxGeti(rq));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDGeti(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int rq;
	/*~~~*/

	rq = AI_PopInt();
	AI_PushInt(SND_i_Geti(rq));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDFxGetf(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int rq;
	/*~~~*/

	rq = AI_PopInt();
	AI_PushFloat(SND_f_FxGetf(rq));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDSetf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~*/
	int		rq;
	float	val;
	/*~~~~~~~~*/

	val = AI_PopFloat();
	rq = AI_PopInt();
	AI_PushInt(SND_i_Setf(rq, val));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDGetf(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int rq;
	/*~~~*/

	rq = AI_PopInt();
	AI_PushFloat(SND_f_Getf(rq));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDFlagSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id, flag;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	flag = AI_PopInt();
	id = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	SND_SetFlags(pst_GO, id, (unsigned int) flag);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDFlagReset(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id, flag;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	flag = AI_PopInt();
	id = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	SND_ResetFlags(pst_GO, id, (unsigned int) flag);

	return ++_pst_Node;
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    sound modifiers
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDGetInstanceModifier(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	id = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_SNDGetInstanceModifier_C(pst_GO, id));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDPlayModifier(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	id = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_SNDPlayModifier_C(pst_GO, id);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SNDGetInstanceModifier_C(OBJ_tdst_GameObject *_pst_GAO, int _i_id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier		*pst_MdF;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_id == -10) return -1;
	if(!SND_gst_Params.l_Available) return -1;
	if(_pst_GAO == NULL) return -1;
	if(_pst_GAO->pst_Extended == NULL) return -1;
	if(_pst_GAO->pst_Extended->pst_Modifiers == NULL) return -1;
	if(_i_id < 0) return -1;

	M_StartRaster();
	pst_MdF = _pst_GAO->pst_Extended->pst_Modifiers;

	while(pst_MdF)
	{
		if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
		{
			pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
			if(pst_SndMdF->ui_Id == (unsigned int) _i_id)
#ifdef ACTIVE_EDITORS
				return((pst_SndMdF->i_SndInstance < 0) ? -1 : pst_SndMdF->i_SndInstance);
#else
			return pst_SndMdF->i_SndInstance;
#endif
		}

		pst_MdF = pst_MdF->pst_Next;
	}

	M_StopRaster();
	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDPlayModifier_C(OBJ_tdst_GameObject *_pst_GAO, int _i_id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier		*pst_MdF;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(_pst_GAO == NULL) return;
	if(_pst_GAO->pst_Extended == NULL) return;
	if(_pst_GAO->pst_Extended->pst_Modifiers == NULL) return;
	if(_i_id == -10) return;

	M_StartRaster();
	pst_MdF = _pst_GAO->pst_Extended->pst_Modifiers;

	if(_i_id == -1)
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				SND_PlayMdF(pst_SndMdF);
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}
	else
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				if(pst_SndMdF->ui_Id == (unsigned int) _i_id)
				{
					SND_PlayMdF(pst_SndMdF);
					break;
				}
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}

	M_StopRaster();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDPauseModifier(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	id = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_SNDPauseModifier_C(pst_GO, id);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDPauseModifier_C(OBJ_tdst_GameObject *_pst_GAO, int _i_id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier		*pst_MdF;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(_pst_GAO == NULL) return;
	if(_pst_GAO->pst_Extended == NULL) return;
	if(_pst_GAO->pst_Extended->pst_Modifiers == NULL) return;
	if(_i_id == -10) return;

	M_StartRaster();
	pst_MdF = _pst_GAO->pst_Extended->pst_Modifiers;

	if(_i_id == -1)
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				SND_PauseMdF(pst_SndMdF);
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}
	else
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				if(pst_SndMdF->ui_Id == (unsigned int) _i_id)
				{
					SND_PauseMdF(pst_SndMdF);
					break;
				}
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}

	M_StopRaster();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDStopModifier(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	id = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_SNDStopModifier_C(pst_GO, id);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDStopModifier_C(OBJ_tdst_GameObject *_pst_GAO, int _i_id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier		*pst_MdF;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(_pst_GAO == NULL) return;
	if(_pst_GAO->pst_Extended == NULL) return;
	if(_pst_GAO->pst_Extended->pst_Modifiers == NULL) return;
	if(_i_id == -10) return;

	M_StartRaster();
	pst_MdF = _pst_GAO->pst_Extended->pst_Modifiers;

	if(_i_id == -1)
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				SND_StopMdF(pst_SndMdF);
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}
	else
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				if(pst_SndMdF->ui_Id == (unsigned int) _i_id)
				{
					SND_StopMdF(pst_SndMdF);
					break;
				}
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}

	M_StopRaster();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDActiveInsertModifier_C
(
	OBJ_tdst_GameObject *_pst_GAO,
	int					_i_id,
	int					_i_insert,
	int					_i_active,
	int					_i_reinit
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier		*pst_MdF;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return;
	if(_pst_GAO == NULL) return;
	if(_pst_GAO->pst_Extended == NULL) return;
	if(_pst_GAO->pst_Extended->pst_Modifiers == NULL) return;
	if(_i_id == -10) return;

	M_StartRaster();
	pst_MdF = _pst_GAO->pst_Extended->pst_Modifiers;

	if(_i_id == -1)
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				SND_l_ActiveInstanceInsert(pst_SndMdF->i_SndInstance, _i_insert, _i_active, _i_reinit);
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}
	else
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				if(pst_SndMdF->ui_Id == (unsigned int) _i_id)
				{
					SND_l_ActiveInstanceInsert(pst_SndMdF->i_SndInstance, _i_insert, _i_active, _i_reinit);
					break;
				}
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}

	M_StopRaster();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDActiveInsertModifier(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id, active, insert, reinit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	reinit = AI_PopInt();
	active = AI_PopInt();
	insert = AI_PopInt();
	id = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_SNDActiveInsertModifier_C(pst_GO, id, insert, active, reinit);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDGetStatusModifier(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	id = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);

	AI_PushInt(AI_EvalFunc_SNDGetStatusModifier_C(pst_GO, id));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SNDGetStatusModifier_C(OBJ_tdst_GameObject *_pst_GAO, int _i_id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier		*pst_MdF;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	unsigned int			tmp, tmp2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_id == -10) return 0;
	if(!SND_gst_Params.l_Available) return 0;
	if(_pst_GAO == NULL) return 0;
	if(_pst_GAO->pst_Extended == NULL) return 0;
	if(_pst_GAO->pst_Extended->pst_Modifiers == NULL) return 0;

	M_StartRaster();
	pst_MdF = _pst_GAO->pst_Extended->pst_Modifiers;
	tmp2 = tmp = 0;

	if(_i_id == -1)
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				if(MDF_M_SndGetState(pst_SndMdF) == MDF_Cte_SndPlaying) return MDF_Cte_SndPlaying;
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}
	else
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				M_StopRaster();
				if(pst_SndMdF->ui_Id == (unsigned int) _i_id) return MDF_M_SndGetState(pst_SndMdF);
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}

	M_StopRaster();
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDIsTrackPlaying(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int id;
	/*~~~*/

	id = AI_PopInt();
	AI_PushInt(SND_i_IsTrackPlaying(id));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDTrackSeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~*/
	int id, track;
	int value;
	/*~~~~~~~~~~*/

	value = AI_PopInt();
	id = AI_PopInt();
	track = AI_PopInt();
	AI_PushInt(SND_i_TrackSeti(track, id, value));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDTrackGeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~*/
	int id, track;
	/*~~~~~~~~~~*/

	id = AI_PopInt();
	track = AI_PopInt();
	AI_PushInt(SND_i_TrackGeti(track, id));

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDMicroPosSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	AI_EvalFunc_SNDMicroPosSet_C(pst_GO);
	return ++_pst_Node;
}

extern OBJ_tdst_GameObject	*SND_gpst_RqMicroPosFromAI;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDMicroPosSet_C(OBJ_tdst_GameObject *pst_GO)
{
	if(pst_GO->pst_GlobalMatrix)
	{
		SND_gpst_RqMicroPosFromAI = pst_GO;
	}
}

/*$4
 ***********************************************************************************************************************
    MERGE BANK
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_MergeBankOver(OBJ_tdst_GameObject *_pst_GOSrc, OBJ_tdst_GameObject *_pst_GODst)
{
	SND_MergeBank(_pst_GODst, _pst_GOSrc, 0, -1, 1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_MergeBankNoOver(OBJ_tdst_GameObject *_pst_GOSrc, OBJ_tdst_GameObject *_pst_GODst)
{
	SND_MergeBank(_pst_GODst, _pst_GOSrc, 0, -1, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDBankMerge(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GODest, *pst_GOSrc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GOSrc);
	pst_GODest = AI_PopGameObject();
	SND_MergeBankNoOver(pst_GOSrc, pst_GODest);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDBankMergeOver(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GODest, *pst_GOSrc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GOSrc);
	pst_GODest = AI_PopGameObject();
	SND_MergeBankOver(pst_GOSrc, pst_GODest);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDBankRestore(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GODest;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GODest);
	SND_RestoreGaoBank(pst_GODest);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDInstPosVSet_C(int _l_Index, MATH_tdst_Vector *pVec)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_SoundInstance	*pst_SI;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInstanceOrReturn(_l_Index, pst_SI, ;);
	if(!pVec) return;

	MATH_CopyVector(&pst_SI->st_UpdatedPos, pVec);

	pst_SI->pst_GlobalPos = &pst_SI->st_UpdatedPos;
	pst_SI->pst_GlobalMatrix = NULL;
	pst_SI->p_GameObject = NULL;

	ERR_X_Warning
	(
		(pst_SI->ul_Flags & (SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis)) == 0,
		"[SND][AIfunction.c] you cannot set the position of an instance if it uses one x,y,z axis",
		NULL
	);

	pst_SI->ul_Flags &= ~(SND_Cul_SF_DynVolXaxis | SND_Cul_SF_DynVolYaxis | SND_Cul_SF_DynVolZaxis);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDInstPosVSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	MATH_tdst_Vector	*v;
	/*~~~~~~~~~~~~~~~~~~~*/

	v = AI_PopVectorPtr();
	AI_EvalFunc_SNDInstPosVSet_C(AI_PopInt(), v);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDPrefetchModifier(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	id = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_SNDPrefetchModifier_C(pst_GO, id);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDPrefetchModifier_C(OBJ_tdst_GameObject *_pst_GAO, int _i_id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier		*pst_MdF;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_id == -10) return;
	if(!SND_gst_Params.l_Available) return;
	if(_pst_GAO->pst_Extended == NULL) return;
	if(_pst_GAO->pst_Extended->pst_Modifiers == NULL) return;

	M_StartRaster();
	pst_MdF = _pst_GAO->pst_Extended->pst_Modifiers;

	if(_i_id == -1)
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				SND_PrefetchMdF(pst_SndMdF);
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}
	else
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				if(pst_SndMdF->ui_Id == (unsigned int) _i_id) SND_PrefetchMdF(pst_SndMdF);
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}

	M_StopRaster();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDInstancePrefetch(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int id;
	/*~~~*/

	id = AI_PopInt();
	SND_StreamPrefetch(id);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDPrefetch_C(OBJ_tdst_GameObject *pGao, int iIdx, float fTime)
{
    SND_tdst_Bank			*pBank;
    int id;

	if(OBJ_b_TestIdentityFlag(pGao, OBJ_C_IdentityFlag_Sound)) 
    {
		if(!SND_M_GoodPointer(pGao->pst_Extended)) return ;
		if(!SND_M_GoodPointer(pGao->pst_Extended->pst_Sound)) return ;
		
        pBank = SND_p_MainGetBank(pGao->pst_Extended->pst_Sound);
		if(!SND_M_GoodPointer(pBank)) return ;
		if(!SND_M_GoodPointer(pBank->pi_Bank)) return ;
		if((iIdx < 0) || (iIdx >= pBank->i_SoundNb)) return ;
		iIdx = pBank->pi_Bank[iIdx];
    }

    if(iIdx == -1) return ;
	if((iIdx < 0) || (iIdx >= SND_gst_Params.l_SoundNumber)) return ;

    id = SND_StreamPrefetchOneSound(iIdx , SND_Cte_StreamPrefetchAsync);
    SND_StreamPrefetchSetLife(id, fTime);
}

AI_tdst_Node *AI_EvalFunc_SNDPrefetch(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int     iIdx;
    float  fTime;
    OBJ_tdst_GameObject *pGao;
	/*~~~*/

	fTime = AI_PopFloat();
	iIdx = AI_PopInt();
    AI_M_GetCurrentObject(pGao);

	AI_EvalFunc_SNDPrefetch_C(pGao, iIdx, fTime);
	return ++_pst_Node;
}


/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDPrefetchDialog_C(TEXT_tdst_Eval *_pst_Text, float _f_TimeOut)
{
	int				i_SndIdx, id;
	
	i_SndIdx = AI_EvalFunc_TEXTToSound_C(_pst_Text);
	if((i_SndIdx < 0) || (i_SndIdx >= SND_gst_Params.l_SoundNumber)) return ;

    id = SND_StreamPrefetchOneSound(i_SndIdx , SND_Cte_StreamPrefetchAsync);
    SND_StreamPrefetchSetLife(id, _f_TimeOut);
}

AI_tdst_Node *AI_EvalFunc_SNDPrefetchDialog(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
    float			fTimeOut;
	TEXT_tdst_Eval	st_Text;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	fTimeOut = AI_PopFloat();
	st_Text.i_Id = AI_PopInt();
	st_Text.i_FileKey= AI_PopInt();

	AI_EvalFunc_SNDPrefetchDialog_C(&st_Text, fTimeOut);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDFlushModifier(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					id;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	id = AI_PopInt();
	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_SNDFlushModifier_C(pst_GO, id);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_SNDFlushModifier_C(OBJ_tdst_GameObject *_pst_GAO, int _i_id)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	MDF_tdst_Modifier		*pst_MdF;
	GEN_tdst_ModifierSound	*pst_SndMdF;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_i_id == -10) return;
	if(!SND_gst_Params.l_Available) return;
	if(_pst_GAO->pst_Extended == NULL) return;
	if(_pst_GAO->pst_Extended->pst_Modifiers == NULL) return;

	M_StartRaster();
	pst_MdF = _pst_GAO->pst_Extended->pst_Modifiers;

	if(_i_id == -1)
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				SND_FlushMdF(pst_SndMdF);
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}
	else
	{
		while(pst_MdF)
		{
			if(pst_MdF->i->ul_Type == MDF_C_Modifier_Sound)
			{
				pst_SndMdF = (GEN_tdst_ModifierSound *) pst_MdF->p_Data;
				if(pst_SndMdF->ui_Id == (unsigned int) _i_id) SND_FlushMdF(pst_SndMdF);
			}

			pst_MdF = pst_MdF->pst_Next;
		}
	}

	M_StopRaster();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDFlush(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int id;
	/*~~~*/

	id = AI_PopInt();
	SND_StreamFlush(id);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    instance seti/geti setf/getf
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDInstanceSeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~*/
	int rq, val, index;
	/*~~~~~~~~~~~~~~~*/

	val = AI_PopInt();
	rq = AI_PopInt();
	index = AI_PopInt();
	AI_PushInt(SND_i_InstanceSeti(index, rq, val));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDInstanceGeti(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~*/
	int rq, index;
	/*~~~~~~~~~~*/

	rq = AI_PopInt();
	index = AI_PopInt();
	AI_PushInt(SND_i_InstanceGeti(index, rq));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDInstanceSetf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~*/
	int		rq, index;
	float	val;
	/*~~~~~~~~~~~~~~*/

	val = AI_PopFloat();
	rq = AI_PopInt();
	index = AI_PopInt();
	AI_PushInt(SND_i_InstanceSetf(index, rq, val));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDInstanceGetf(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~*/
	int rq, index;
	/*~~~~~~~~~~*/

	rq = AI_PopInt();
	index = AI_PopInt();
	AI_PushFloat(SND_f_InstanceGetf(index, rq));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SND_SetObject(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);

	SND_SetObject(pst_GO, AI_PopInt());
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
AI_tdst_Node *AI_EvalFunc_SND_SoundDurationGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	index = AI_PopInt();
	AI_PushFloat(SND_SoundDurationGet(pst_GO, index));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_SoundDurationGet(OBJ_tdst_GameObject *_pst_GO, int idx)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_OneSound	*pst_Sound;
	SND_tdst_Bank		*pBank;
	float				fOut;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!SND_gst_Params.l_Available) return 0.0f;

	if(OBJ_b_TestIdentityFlag(_pst_GO, OBJ_C_IdentityFlag_Sound))
	{
		if(!MEM_b_CheckPointer(_pst_GO->pst_Extended)) return 0.0f;
		if(!MEM_b_CheckPointer(_pst_GO->pst_Extended->pst_Sound)) return 0.0f;

		pBank = NULL;
		if(_pst_GO->pst_Extended->pst_Sound->st_Bank.ul_Flags & SND_Cte_Bank)
		{
			pBank = &_pst_GO->pst_Extended->pst_Sound->st_Bank;
		}
		else if(_pst_GO->pst_Extended->pst_Sound->st_Metabank.ul_Flags & SND_Cte_Metabank)
		{
			pBank = _pst_GO->pst_Extended->pst_Sound->st_Metabank.pst_Curr;
		}

		if(pBank)
		{
			if(!MEM_b_CheckPointer(pBank->pi_Bank)) return 0.0f;
			if((idx < 0) || (idx >= pBank->i_SoundNb)) return 0.0f;
			idx = pBank->pi_Bank[idx];
		}
	}

	if(idx >= SND_gst_Params.l_SoundNumber) return 0.0f;

	pst_Sound = SND_gst_Params.dst_Sound + idx;
	if(pst_Sound->pst_SModifier)
	{
		if((LONG) pst_Sound->pst_DSB >= SND_gst_Params.l_SoundNumber) return 0.0f;
		pst_Sound = SND_gst_Params.dst_Sound + (int) pst_Sound->pst_DSB;
	}

	if(!(pst_Sound->ul_Flags & SND_Cul_DSF_Used)) return 0.0f;
	if(!pst_Sound->pst_Wave) return 0.0f;
	if(!pst_Sound->pst_Wave->dwSamplesPerSec) return 0.0f;

	fOut = (float) SND_ui_SizeToSample
		(
			pst_Sound->pst_Wave->wFormatTag,
			pst_Sound->pst_Wave->wChannels,
			pst_Sound->pst_Wave->ul_DataSize
		);
	fOut /= (float) pst_Sound->pst_Wave->dwSamplesPerSec;
	return fOut;
}

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    GROUP
 -----------------------------------------------------------------------------------------------------------------------
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SND_GroupVolumeSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	SND_tden_GroupId	Id;
	float				vol;
	/*~~~~~~~~~~~~~~~~~~~~*/

	vol = AI_PopFloat();
	Id = (SND_tden_GroupId)AI_PopInt();
	SND_GroupVolumeSet(Id, vol);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SND_GroupVolumeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	SND_tden_GroupId	Id;
	float				vol;
	/*~~~~~~~~~~~~~~~~~~~~*/

	Id = (SND_tden_GroupId)AI_PopInt();
	vol = SND_f_GroupVolumeGet(Id);
	AI_PushFloat(vol);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SND_GroupFxVolumeSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	SND_tden_GroupId	Id;
	float				vol;
	/*~~~~~~~~~~~~~~~~~~~~*/

	vol = AI_PopFloat();
	Id = (SND_tden_GroupId)AI_PopInt();
	SND_GroupFxVolumeSet(Id, vol);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SND_GroupFxVolumeGet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	SND_tden_GroupId	Id;
	float				vol;
	/*~~~~~~~~~~~~~~~~~~~~*/

	Id = (SND_tden_GroupId)AI_PopInt();
	vol = SND_f_GroupFxVolumeGet(Id);
	AI_PushFloat(vol);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SND_InsertActive(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int index, id, active, reinit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	reinit = AI_PopInt();
	active = AI_PopInt();
	id = AI_PopInt();
	index = AI_PopInt();
	AI_PushInt(SND_l_ActiveInstanceInsert(index, id, active, reinit));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDInsertVarSet(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~*/
	int		id;
	int		instance;
	float	val;
	/*~~~~~~~~~~~~~*/

	val = AI_PopFloat();
	id = AI_PopInt();
	instance = AI_PopInt();
	SND_InsertVarSet(instance, id, val);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SNDTrackChain(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				iLoop, iOldInstance, iNewInstance;
	unsigned int	uiMode, uiStartRegionTag;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	iLoop = AI_PopInt();
	uiStartRegionTag = AI_PopInt();
	uiMode = AI_PopInt();
	iNewInstance = AI_PopInt();
	iOldInstance = AI_PopInt();

	AI_PushFloat(SND_f_Track_ChainPlayer(iOldInstance, iNewInstance, uiMode, uiStartRegionTag, iLoop));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SND_SignalIsActive(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~*/
	char	*s1;
	/*~~~~~~~~*/

	s1 = (char *) AI_PopStringPtr();
	AI_PushInt(SND_i_SignalIsActive(s1));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SND_RenderAvailableModeGet_C(void)
{
	return SND_i_ChangeRenderMode(SND_Cte_RenderCapacity);
}
/**/
AI_tdst_Node *AI_EvalFunc_SND_RenderAvailableModeGet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_SND_RenderAvailableModeGet_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SND_RenderModeSet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(SND_i_ChangeRenderMode(AI_PopInt()));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_SND_RenderModeGet_C(void)
{
	return SND_i_ChangeRenderMode(SND_Cte_RenderCurrent);
}
/**/
AI_tdst_Node *AI_EvalFunc_SND_RenderModeGet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(AI_EvalFunc_SND_RenderModeGet_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_SND_GroupVolRqGet(AI_tdst_Node *_pst_Node)
{
	AI_PushInt(SND_i_GroupGetRequestId(AI_PopInt()));
	return ++_pst_Node;
}

AI_tdst_Node *AI_EvalFunc_SND_GroupVolRqDestroy(AI_tdst_Node *_pst_Node)
{
	/*~~~*/
	int Id;
	int i;
	/*~~~*/

	i = AI_PopInt();
	Id = AI_PopInt();
	SND_GroupFreeRequestId(Id, i);
	return ++_pst_Node;
}

AI_tdst_Node *AI_EvalFunc_SND_GroupVolRqSend(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~*/
	int		Id;
	int		RqId;
	float	f_Volume;
	/*~~~~~~~~~~~~~*/

	f_Volume = AI_PopFloat();
	RqId = AI_PopInt();
	Id = AI_PopInt();

	SND_GroupSendRequest(Id, RqId, f_Volume);
	return ++_pst_Node;
}

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
