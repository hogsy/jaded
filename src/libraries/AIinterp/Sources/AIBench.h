/*$T AIsave.h GC!1.41 08/20/99 09:49:41 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __AIBENCH_H__
#define __AIBENCH_H__

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
#include "BASe/BAStypes.h"
#include "AIstruct.h"
#include "ENGine/Sources/WORld/WORstruct.h"



#ifdef BENCH_IA_TEST


#ifdef ACTIVE_EDITORS
void AIBnch_AddFunction(BIG_KEY	MdlKey,BIG_KEY	FuncKey);
void AIBnch_AddLastFunctionIfo(ULONG ulNumberOfNode,BIG_KEY	OFCKey);
void AIBnch_BeforeCompileAll();
void AIBnch_AfterCompileAll();
#endif

void AIBnch_BeginWorldLoad(BIG_KEY	WorldKey);
void AIBnch_CreateFunctionBench(AI_tdst_Function	*p_Func , BIG_KEY	FunctionKey);

void AIBnch_BeforeRunFunc(AI_tdst_Function	*p_Func);
void AIBnch_AfterRunFunc(AI_tdst_Function	*p_Func);
void AIBnch_BeginFrameCall();
void AIBnch_EndFrameCall();
void AIBnch_Close();


#endif

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __AIBENCH_H__ */
