/*$T AIdebug.h GC 1.138 12/10/03 16:34:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __AIDEBUG_H__
#define __AIDEBUG_H__

#ifdef ACTIVE_EDITORS
#include "BASe/BAStypes.h"
#include "BASe/BASarray.h"
#include "BASe/CLIbrary/CLIxxx.h"
#include "BASe/MEMory/MEM.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*
 -----------------------------------------------------------------------------------------------------------------------
    Definition of a breakpoint
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_BreakPoint_
{
	int					i_Line;			/* Line where breakpoint occured */
	ULONG				ul_File;		/* The file for the function */
	ULONG				ul_EngFile;		/* Function concerned */
	AI_tdst_Model		*pst_Model;		/* Model concerned */
	AI_tdst_Instance	*pst_Instance;	/* Instance concerned */
	AI_tdst_Function	*pst_Function;
	AI_tdst_Node		*pt_Node;		/* Node concerned */
} AI_tdst_BreakPoint;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Pile d'appel des execution de fonction
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_CallStack_
{
	AI_tdst_Function	*pt_Function;	/* Fonction appelante */
	AI_tdst_Node		*pt_Node;		/* Noeud d'appel */
} AI_tdst_CallStack;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern L_jmp_buf			AI_gst_ContextIn;
extern L_jmp_buf			AI_gst_ContextOut;
extern L_jmp_buf			AI_gst_ContextCheck;
extern char					*AI_gpc_StackIn;
extern int					AI_gi_SizeStackIn;
extern int					AI_gi_NumBreak;
extern BOOL					AI_gb_ExitByBreak;
extern BOOL					AI_gb_ErrorWhenBreak;
extern int					AI_gi_OneStepMode;
extern int					AI_gi_FctLevelWhenBreak;
extern AI_tdst_BreakPoint	AI_gst_BreakInfo;
extern AI_tdst_CallStack	AI_gst_CallStack[1000];

#define AI_C_MaxBreakPoint	100
extern AI_tdst_BreakPoint	AI_gast_BreakList[AI_C_MaxBreakPoint];

/*$2
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */

extern void					AI_InitBreakList(void);
extern void					AI_ReinitBreakMode(void);
extern void					AI_DelAllBreakPoints(void);
extern void					AI_DelBreakListForModel(AI_tdst_Model *);
extern void					AI_AddBreakPoint(ULONG, ULONG, AI_tdst_Model *, AI_tdst_Instance *, int);
extern BOOL					AI_SearchBreakPoint(AI_tdst_Function *, AI_tdst_Model *, AI_tdst_Instance *, int);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*$off*/

/*$2
 ===================================================================================================
 ===================================================================================================
 */
#define AI_M_DebugInStack(a)\
	_asm mov eax, dword ptr[AI_gst_ContextOut + 0x10] \
	_asm add eax, 12 \
	_asm mov ebx, esp \
	_asm sub eax, ebx \
	_asm mov AI_gi_SizeStackIn, eax \
	_asm mov eax, esp \
	_asm mov ebx, AI_gpc_StackIn \
	_asm mov ecx, AI_gi_SizeStackIn \
	_asm label1##a: \
	_asm cmp ecx, 0 \
	_asm je label2##a \
	_asm mov dl, byte ptr[eax] \
	_asm mov byte ptr[ebx], dl \
	_asm inc eax \
	_asm inc ebx \
	_asm dec ecx \
	_asm jmp label1##a \
	_asm label2##a:

/*$2
 ===================================================================================================
 ===================================================================================================
 */
#define AI_M_DebugOutStack(a)\
	_asm mov eax, AI_gpc_StackIn \
	_asm mov ebx, dword ptr[AI_gst_ContextIn + 0x10] \
	_asm add ebx, 12 \
	_asm mov ecx, AI_gi_SizeStackIn \
	_asm label11##a: \
	_asm cmp ecx, 0 \
	_asm je label22##a \
	_asm mov dl, byte ptr[eax] \
	_asm mov byte ptr[ebx], dl \
	_asm inc eax \
	_asm inc ebx \
	_asm dec ecx \
	_asm jmp label11##a \
	_asm label22##a:

/*$on*/
#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Macro to check an expression. If expression is false, engine will break
 -----------------------------------------------------------------------------------------------------------------------
 */

#ifdef ACTIVE_EDITORS
#		define AI_CheckPointer( __Pointer )                          \
			{                                                         \
				if ( IsBadReadPtr( __Pointer, 1 ) )                   \
				{                                                     \
					ERR_X_ForceError( "Bad Pointer Detected", NULL ); \
					L_longjmp( AI_gst_ContextCheck, 1 );              \
				}                                                     \
			}

#		if 1
#			define AI_Check( __Expr, __Str )                                                   \
				{                                                                               \
					if ( !( __Expr ) )                                                          \
					{                                                                           \
						if ( ERR_ScriptAssertFailed( BAS_FILENAME, __LINE__, #__Expr, __Str ) ) \
						{                                                                       \
							L_longjmp( AI_gst_ContextCheck, 1 );                                \
						}                                                                       \
					}                                                                           \
				}
#		else
#			define AI_Check( __Expr, __Str )
#		endif

/*$2------------------------------------------------------------------------------------------------------------------*/

#elif defined(_DEBUG)
#ifdef PSX2_TARGET
#define IsBadReadPtr(_p, _s)	!((int) _p > 0x200000 && (int) _p < 0x8000000)	/* s */
#endif
#define AI_CheckPointer(__Pointer)	{ if(IsBadReadPtr(__Pointer, 1)) { ERR_X_ForceError("Bad Pointer Detected", NULL); } }
#define AI_Check(__Expr, __Str)		{ if(!(__Expr)) { ERR_X_ForceError(__Str, NULL); } }

/*$2------------------------------------------------------------------------------------------------------------------*/

#else
#define AI_CheckPointer(a)
#define AI_Check(a, b)
#endif
#endif /* __AIDEBUG_H__ */
