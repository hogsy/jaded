/*$T AIfunctions_time.c GC! 1.100 03/16/01 18:03:07 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIstack.h"
#include "AIinterp/Sources/AItools.h"
#include "TIMer/TIMdefs.h"

#include "Sound/Sources/SNDstruct.h"
#include "Sound/Sources/SNDtrack.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif
extern float	TIM_gf_SpeedFactor;

float			TIM_gf_LockBegTime = 0.0f;
float			TIM_gf_LockTime = 0.0f;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void TIME_Wait_C(float f_Time)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*pst_Instance;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_M_GetCurrentInstance(pst_Instance);
	pst_Instance->af_WaitTime[AI_gul_CurrentTrack] = f_Time;
}
/**/
AI_tdst_Node *AI_EvalFunc_TIMEWait(AI_tdst_Node *_pst_Node)
{
	TIME_Wait_C(AI_PopFloat());
	*AI_gppst_CurrentJumpNode = _pst_Node + 1;
	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_TIMEGet_C(void)
{
	return TIM_gf_MainClock - TIM_gf_LockTime;
}
/**/
AI_tdst_Node *AI_EvalFunc_TIMEGet(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_TIMEGet_C());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float AI_EvalFunc_TIMETTGet_C(void)
{
#ifdef ACTIVE_EDITORS
	return ((float) TIM_ul_GetLowPartTimerInternalCounter() / (float) TIM_gul_TicksPerSecond) * TIM_gf_SynchroFrequency;
#endif
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_TIMETTGet(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(AI_EvalFunc_TIMETTGet_C());
	return ++_pst_Node;
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int AI_EvalFunc_TIMEElapsed_C(float f_Time, float f_Test)
{
	if(fSupEq(fSub(TIM_gf_MainClock - TIM_gf_LockTime, f_Time), f_Test)) return 1;
	return 0;
}
/**/
AI_tdst_Node *AI_EvalFunc_TIMEElapsed(AI_tdst_Node *_pst_Node)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	float	f_Time, f_Test;
	/*~~~~~~~~~~~~~~~~~~~*/

	f_Test = AI_PopFloat();
	f_Time = AI_PopFloat();
	AI_PushInt(AI_EvalFunc_TIMEElapsed_C(f_Time, f_Test));
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Return the dt (time in seconds between two frames)
 =======================================================================================================================
 */
float AI_EvalFunc_TIMEGetDt_C(void)
{
	return TIM_gf_dt;
}
/**/
AI_tdst_Node *AI_EvalFunc_TIMEGetDt(AI_tdst_Node *_pst_Node)
{
	AI_PushFloat(TIM_gf_dt);
	return ++_pst_Node;
}

/*
 =======================================================================================================================
    Aim:    Sets the Engine Time Factor
 =======================================================================================================================
 */
void AI_EvalFunc_TIMEFactorSet_C(float f)
{	
#ifdef ACTIVE_EDITORS
    if(TIM_gf_SpeedFactor != f)
    {
        SND_TrackModulateFrequency(f);
    }
#endif
    TIM_gf_SpeedFactor = f;
}
/**/
AI_tdst_Node *AI_EvalFunc_TIMEFactorSet(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_TIMEFactorSet_C(AI_PopFloat());
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TIMELock_C(void)
{
	TIM_gf_LockBegTime = TIM_gf_MainClock;
}
/**/
AI_tdst_Node *AI_EvalFunc_TIMELock(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_TIMELock_C();
	return ++_pst_Node;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void AI_EvalFunc_TIMEUnlock_C(void)
{
	TIM_gf_LockTime += TIM_gf_MainClock - TIM_gf_LockBegTime;
}
/**/
AI_tdst_Node *AI_EvalFunc_TIMEUnlock(AI_tdst_Node *_pst_Node)
{
	AI_EvalFunc_TIMEUnlock_C();
	return ++_pst_Node;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
