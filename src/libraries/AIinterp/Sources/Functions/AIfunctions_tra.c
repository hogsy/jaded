/*$T AIfunctions_tra.c GC 1.134 03/18/02 12:57:44 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "AIinterp/Sources/Events/EVEplay.h"
#include "AIinterp/Sources/Events/EVEinit.h"
#include "AIinterp/Sources/Events/EVEconst.h"
#include "AIinterp/Sources/Events/EVEstruct.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TRAPlayAll_C(OBJ_tdst_GameObject *pst_GO)
{
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	EVE_SetRunningTracks(pst_GO->pst_Extended->pst_Events, TRUE);
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAPlayAll(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_TRAPlayAll_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TRAStopAll_C(OBJ_tdst_GameObject *pst_GO)
{
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	EVE_SetRunningTracks(pst_GO->pst_Extended->pst_Events, FALSE);
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAStopAll(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_TRAStopAll_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TRAPauseAll_C(OBJ_tdst_GameObject *pst_GO)
{
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	EVE_SetPauseTracks(pst_GO->pst_Extended->pst_Events, TRUE);
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAPauseAll(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_TRAPauseAll_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TRAUnPauseAll_C(OBJ_tdst_GameObject *pst_GO)
{
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	EVE_SetPauseTracks(pst_GO->pst_Extended->pst_Events, FALSE);
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAUnPauseAll(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_TRAUnPauseAll_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TRAReinitAll_C(OBJ_tdst_GameObject *pst_GO)
{
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	EVE_ReinitData(pst_GO->pst_Extended->pst_Events);
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAReinitAll(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_EvalFunc_TRAReinitAll_C(pst_GO);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TRAPlay_C(OBJ_tdst_GameObject *pst_GO, int i_Num)
{
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	AI_Check(i_Num < pst_GO->pst_Extended->pst_Events->pst_ListTracks->uw_NumTracks, "Track num is incorrect");

	EVE_SetRunningTrack(&pst_GO->pst_Extended->pst_Events->pst_ListParam[i_Num], TRUE);
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAPlay(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Num;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Num = AI_PopInt();
	AI_EvalFunc_TRAPlay_C(pst_GO, i_Num);

	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TRAStop_C(OBJ_tdst_GameObject *pst_GO, int i_Num)
{
	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	AI_Check(i_Num < pst_GO->pst_Extended->pst_Events->pst_ListTracks->uw_NumTracks, "Track num is incorrect");
	EVE_SetRunningTrack(&pst_GO->pst_Extended->pst_Events->pst_ListParam[i_Num], FALSE);
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAStop(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Num;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Num = AI_PopInt();
	AI_EvalFunc_TRAStop_C(pst_GO, i_Num);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_TRAPause(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Num;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Num = AI_PopInt();

	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	AI_Check(i_Num < pst_GO->pst_Extended->pst_Events->pst_ListTracks->uw_NumTracks, "Track num is incorrect");

	EVE_SetPauseTrack(&pst_GO->pst_Extended->pst_Events->pst_ListParam[i_Num], TRUE);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_TRAUnPause(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Num;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Num = AI_PopInt();

	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	AI_Check(i_Num < pst_GO->pst_Extended->pst_Events->pst_ListTracks->uw_NumTracks, "Track num is incorrect");

	EVE_SetPauseTrack(&pst_GO->pst_Extended->pst_Events->pst_ListParam[i_Num], FALSE);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
AI_tdst_Node *AI_EvalFunc_TRAReinit(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Num;
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Num = AI_PopInt();

	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	AI_Check(i_Num < pst_GO->pst_Extended->pst_Events->pst_ListTracks->uw_NumTracks, "Track num is incorrect");

	EVE_ReinitTrack(pst_GO->pst_Extended->pst_Events, i_Num);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TRAIsStop_C(OBJ_tdst_GameObject *pst_GO, int i_Num)
{
	EVE_tdst_Data		*pst_List;

	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	pst_List = pst_GO->pst_Extended->pst_Events;
	AI_Check(i_Num < pst_List->pst_ListTracks->uw_NumTracks, "Track num is incorrect");
	return pst_List->pst_ListParam[i_Num].uw_Flags & EVE_C_Track_Running ? 0 : 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAIsStop(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Num = AI_PopInt();
	AI_PushInt(AI_EvalFunc_TRAIsStop_C(pst_GO, i_Num));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TRAIsStopAll_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_Num;
	EVE_tdst_Data	*pst_List;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	pst_List = pst_GO->pst_Extended->pst_Events;
	if(!pst_List || !pst_List->pst_ListTracks || !pst_List->pst_ListTracks->pst_AllTracks || ((ULONG) pst_List->pst_ListTracks->pst_AllTracks == 0xFFFFFFFF)) return 1;
	for(i_Num = 0; i_Num < pst_List->pst_ListTracks->uw_NumTracks; i_Num++)
	{
		if
		(
			pst_List->pst_ListTracks->pst_AllTracks[i_Num].pst_AllEvents
		&&	pst_List->pst_ListParam[i_Num].uw_Flags & EVE_C_Track_Running
		)
		{
			return 0;
		}
	}

	return 1;
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAIsStopAll(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushInt(AI_EvalFunc_TRAIsStopAll_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TRASetEnd_C(OBJ_tdst_GameObject *pst_GO, int i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*pst_List;
	int				i_Evt;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	pst_List = pst_GO->pst_Extended->pst_Events;
	if(pst_List->pst_ListTracks->pst_AllTracks[i_Num].uw_NumEvents)
	{
		i_Evt = pst_List->pst_ListTracks->pst_AllTracks[i_Num].uw_NumEvents - 1;
		pst_List->pst_ListParam[i_Num].uw_CurrentEvent = i_Evt;
		pst_List->pst_ListParam[i_Num].f_Time = EVE_FrameToTime(pst_List->pst_ListTracks->pst_AllTracks[i_Num].pst_AllEvents[i_Evt].uw_NumFrames & 0x7FFF);
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_TRASetEnd(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					i_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	i_Num = AI_PopInt();
	AI_EvalFunc_TRASetEnd_C(pst_GO, i_Num);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TRASetTime_C(OBJ_tdst_GameObject *pst_GO, float f_Time)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*pst_Data;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	pst_Data = pst_GO->pst_Extended->pst_Events;
	EVE_SetTracksTime(pst_Data, f_Time);
}
/**/
AI_tdst_Node *AI_EvalFunc_TRASetTime(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	float				f_Time;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	f_Time = AI_PopFloat();
	AI_EvalFunc_TRASetTime_C(pst_GO, f_Time);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_TRAGetTotalTime_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*pst_Data;
	float			f_Time;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	pst_Data = pst_GO->pst_Extended->pst_Events;
	EVE_GetTracksTime(pst_Data, 0, NULL, &f_Time);
	return f_Time;
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAGetTotalTime(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_TRAGetTotalTime_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_TRAGetCurrentTime_C(OBJ_tdst_GameObject *pst_GO)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*pst_Data;
	float			f_Time;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(pst_GO->pst_Extended && pst_GO->pst_Extended->pst_Events, "Object doesn't have events");
	pst_Data = pst_GO->pst_Extended->pst_Events;
	EVE_GetTracksTime(pst_Data, 0, &f_Time, NULL);

	return f_Time;
}
/**/
AI_tdst_Node *AI_EvalFunc_TRAGetCurrentTime(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	AI_PushFloat(AI_EvalFunc_TRAGetCurrentTime_C(pst_GO));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TRASetTimeToLabel_C(OBJ_tdst_GameObject *_pt_GO, int track, int label)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EVE_tdst_Data	*pst_Data;
	float			f_TotalTime;
	int				i_Jump;
	extern int EVE_i_SearchLabel(EVE_tdst_Track *, int, float *);
	extern EVE_tdst_ListTracks	*EVE_gpst_CurrentListTracks;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_Check(_pt_GO->pst_Extended && _pt_GO->pst_Extended->pst_Events, "Object doesn't have events");
	pst_Data = _pt_GO->pst_Extended->pst_Events;
	AI_Check(track >= 0 && track < pst_Data->pst_ListTracks->uw_NumTracks, "Track number is incorrect");

	i_Jump = EVE_i_SearchLabel(pst_Data->pst_ListTracks->pst_AllTracks + track, label << 12, &f_TotalTime);
	if(i_Jump != -1)
	{
		EVE_gpst_CurrentListTracks = pst_Data->pst_ListTracks;
		EVE_gpst_OwnerGAO = _pt_GO;
		EVE_SetAllTimesTo(f_TotalTime);
		EVE_gpst_CurrentListTracks = NULL;
		EVE_gpst_OwnerGAO = NULL;
	}
}
/**/
AI_tdst_Node *AI_EvalFunc_TRASetTimeToLabel(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	int					label, track;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentObject(pst_GO);
	label = AI_PopInt();
	track = AI_PopInt();
	AI_EvalFunc_TRASetTimeToLabel_C(pst_GO, track, label);
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
