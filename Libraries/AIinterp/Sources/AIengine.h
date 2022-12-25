/*$T AIengine.h GC! 1.080 03/21/00 10:41:47 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __AIENGINE_H__
#define __AIENGINE_H__
#include "BASe/ERRors/ERRasser.h"
#include "BASe/BASsys.h"
#include "BASe/BAStypes.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AImsg.h"
#include "AIinterp/Sources/AIdebug.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "ENGine/Sources/WORld/WORvars.h"
#include "ENGine/Sources/OBJects/OBJstruct.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/TEXT/TEXTstruct.h"
#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Global function pointer for evaluation
 -----------------------------------------------------------------------------------------------------------------------
 */

typedef AI_tdst_Node * (*AI_td_EvalFunc) (AI_tdst_Node *);
typedef struct
{
	short			w_ID;
	AI_td_EvalFunc	pfn_Func;
#ifdef ACTIVE_EDITORS
	BOOL			b_SysBreak;
#endif
#ifdef AI_FULL_RASTERS
	PRO_tdst_TrameRaster st_Raster;
	int					 aaa;
#endif
} AI_tdst_Eval;
typedef void (*AI_td_VarFunc) (void *, void *);
typedef struct
{
	short			w_ID;
	short			w_Size;
	AI_td_VarFunc	pfn_FuncGet;
} AI_tdst_EvalVar;
typedef struct
{
	short	w_Type;
	short	w_1;
	void	*pv_Addr;
} AI_tdst_PushVar;
typedef union
{
	int					i;
	float				f;
	MATH_tdst_Vector	v;
	SCR_tt_Trigger		m;
	TEXT_tdst_Eval		t;
} AI_tdst_UnionVar;

/*$4
 ***********************************************************************************************************************
    Protos
 ***********************************************************************************************************************
 */

#define AI_C_NotUltra	0	/* Function is ultraable */
#define AI_C_Ultra		1	/* Function is not ultraable */
/*$off*/
extern AI_tdst_Node *AI_dum(AI_tdst_Node *);

#undef DEFINE_CATEG
#define DEFINE_CATEG(a, b, c) extern AI_tdst_Node *c(AI_tdst_Node *);
#include "Categs/AIdefcateg.h"
#undef DEFINE_CATEG

#undef DEFINE_TYPE
#define DEFINE_CONSTANT(a, b, c)
#define DEFINE_TYPE(a, b, c, d, e) extern void e(void *, void *);
#include "Types/AIdeftyp.h"
#undef DEFINE_TYPE

#undef DEFINE_KEYWORD
#define DEFINE_KEYWORD(a, b, c, d) extern AI_tdst_Node *d(AI_tdst_Node *);
#include "Keywords/AIdefkey.h"
#undef DEFINE_KEYWORD

#undef DEFINE_FUNCTION
#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j) extern AI_tdst_Node *f(AI_tdst_Node *);
#include "Functions/AIdeffct.h"
#undef DEFINE_FUNCTION

#undef DEFINE_FIELD
#define DEFINE_FIELD(a, b, c, d, e, f) extern AI_tdst_Node *f(AI_tdst_Node *);
#include "Fields/AIdeffields.h"
#undef DEFINE_FIELD

/*
 * Definition of all enum for types, categs, functions etc...
 * !! Categs must by the first, cause ID is save on a char.
 * Other ids (functions, types, fields, keywords) are save on a short.
 */
#define DEFINE_CATEG(a, b, c) b = a,
#define DEFINE_TYPE(a, b, c, d, e) b = a,
#define DEFINE_CONSTANT(a, b, c)
#define DEFINE_FIELD(a, b, c, d, e, f) b = a,
#define DEFINE_KEYWORD(a, b, c, d) b = a,
#define DEFINE_FUNCTION(a, b, c, d, e, f, g, h, i, j) b = a,
enum
{
#include "Categs/AIdefcateg.h"
#include "Types/AIdeftyp.h"
#include "Fields/AIdeffields.h"
#include "Keywords/AIdefkey.h"
#include "Functions/AIdeffct.h"
AI_ENUM_LAST
};
#undef DEFINE_CATEG
#undef DEFINE_TYPE
#undef DEFINE_CONSTANT
#undef DEFINE_KEYWORD
#undef DEFINE_FUNCTION
#undef DEFINE_FIELD

extern short	AI_gaw_EnumLink[AI_ENUM_LAST];
/*$on*/

/*$4
 ***********************************************************************************************************************
    Global vars
 ***********************************************************************************************************************
 */

extern AI_tdst_Eval			AI_gast_Categs[];
extern AI_tdst_EvalVar		AI_gast_Types[];
extern AI_tdst_Eval			AI_gast_Keywords[];
extern AI_tdst_Eval			AI_gast_Functions[];
extern AI_tdst_Eval			AI_gast_Fields[];

/* For helping interpretation */
extern AI_tdst_Function		*AI_gpst_CurrentFunction;
extern AI_tdst_Instance		*AI_gpst_CurrentInstance;
extern AI_tdst_Instance		*AI_gpst_CurrentInstanceUltra;
extern AI_tdst_Node			**AI_gppst_CurrentJumpNode;
extern OBJ_tdst_GameObject	*AI_gpst_CurrentGameObject;
extern OBJ_tdst_GameObject	*AI_gpst_CurrentUltra;
extern BOOL					AI_gb_CanBreakSystem;
extern BOOL					AI_gb_CanBreak;
extern int					AI_gi_RunFctLevel;
extern ULONG				AI_gul_CurrentTrack;
extern BOOL					AI_gb_InterpForVar;
extern ULONG				AI_gul_SizeGlobalStack_SAVE[500];
extern ULONG				AI_gul_SizeGlobalVarStack_SAVE[500];

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define AI_CR_StopInstance		1	/* To stop interpretation of current instance */
#define AI_CR_StopReturn		2	/* To stop interpretation of current function by a return */
#define AI_CR_StopAI			3	/* To stop AI parser */
#define AI_CR_StopReturnTrack	4	/* To stop AI parser */
#define AI_CR_CurChangeNow		5
#define AI_CR_Destroy			6
#define AI_CR_Max				7	/* Max number of possible return values */

/*$4
 ***********************************************************************************************************************
    Main actors
 ***********************************************************************************************************************
 */

#define AI_C_MainActorsNum	2	/* Max number of main actors */
#define AI_C_MainActor0		0x00000001
#define AI_C_MainActor1		0x00000002
extern struct OBJ_tdst_GameObject_	**AI_gpst_MainActors;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ OBJ_tdst_GameObject *AI_Mpst_GetCurrentObject(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *obj;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	obj = AI_gpst_CurrentUltra;
	AI_gpst_CurrentUltra = AI_gpst_CurrentGameObject;
	AI_gpst_CurrentInstanceUltra = AI_gpst_CurrentInstance;
	AI_Check(OBJ_IsNullOrValidGAO(obj), "AI object is incorrect");
	return obj;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ AI_tdst_Instance *AI_Mpst_GetCurrentInstance(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Instance	*obj;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	obj = AI_gpst_CurrentInstanceUltra;

#ifdef _DEBUG
	if(!AI_gpst_CurrentInstanceUltra)
	{
		char	asz_Msg[256];
		sprintf(asz_Msg, "Game object doesn't have AI (%s)", AI_gpst_CurrentUltra->sz_Name);
		AI_Check(0, asz_Msg);
	}
#endif

	AI_gpst_CurrentInstanceUltra = AI_gpst_CurrentInstance;
	AI_gpst_CurrentUltra = AI_gpst_CurrentGameObject;

	return obj;
}

#define AI_M_GetCurrentObject(obj)		obj = AI_Mpst_GetCurrentObject();
#define AI_M_GetCurrentInstance(obj)	obj = AI_Mpst_GetCurrentInstance();

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define AI_MAX_WATCH	30
#define AI_MAXBUF_WATCH	512
typedef struct AI_Watch_
{
	char	*p_Data;
	int		Size;
} AI_Watch;

/*$4
 ***********************************************************************************************************************
    Functions
 ***********************************************************************************************************************
 */

extern void			AI_Reset(void);
extern void			AI_ResolveEnums(void);
extern void			AI_FreeVarDes(AI_tdst_VarDes *);
extern void			AI_FreeModel(AI_tdst_Model *);
extern void			AI_FreeInstance(AI_tdst_Instance *);
extern AI_tdst_Node *AI_pst_RunFunction(AI_tdst_Function *, AI_tdst_Node **, int);
extern void			AI_RunNode(AI_tdst_Node *);
extern BOOL			AI_RunInstance(AI_tdst_Instance *);
extern void			AI_MainCall(WOR_tdst_World *);
extern void			AI_ReinitInstance(AI_tdst_Instance *);
extern void			AI_ReinitUniverse(void);
extern void			AI_DuplicateInstance(OBJ_tdst_GameObject *, OBJ_tdst_GameObject *, BOOL);
extern void			AI_ExecCallbackAll(WOR_tdst_World *, int);
extern void			AI_RunContext(WOR_tdst_World *, int);
extern void			AI_DeleteFromRunContext( OBJ_tdst_GameObject * );
extern void			AI_ExecCallback(OBJ_tdst_GameObject *, int);
extern BOOL         AI_bIsWatched(void *);
extern void         AI_RemoveWatch(void *);
extern void         AI_RemoveWatchOnAddressRange(void *,void *);
extern void         AI_AddWatch(void *, int );


extern OBJ_tdst_GameObject *AI_gapst_Differed[AI_CXT_Max][1000];
extern int			AI_gai_Differed[AI_CXT_Max];
extern BOOL			AI_gb_FullRasters;

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __AIENGINE_H__ */
