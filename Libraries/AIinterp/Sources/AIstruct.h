/*$T AIstruct.h GC! 1.100 03/12/01 14:56:19 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif

#ifndef __AISTRUCT_H__
#define __AISTRUCT_H__
#include "BASe/BAStypes.h"


#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif
//#define BENCH_IA_TEST

void AI_vAddKeyDependency(ULONG h_Used,ULONG h_User,char *_s_CallbackName);
extern void AI_ResolveTrigger(struct SCR_tt_Trigger_ *, BOOL);
extern BOOL			AI_gb_Optim2;
extern BOOL			AI_gb_GenOptim2;

#include "BIGfiles/BIGdefs.h"
#include "AIinterp/Sources/AImsg.h"

/*$4
 ***********************************************************************************************************************
    Constants
 ***********************************************************************************************************************
 */

#define AI_C_MaxTracks		5	/* Max number of AI tracks. There can be one executed function per track in the same trame */
#define AI_C_MaxLenVar		30	/* Max length of a variable name */
#define AI_MaxLenTrigger	64	/* Max length of a trigger name */

/*$4
 ***********************************************************************************************************************
    Callbacks
 ***********************************************************************************************************************
 */

#define AI_C_MaxCallbacks			6	/* Max number of callbacks */
#define AI_C_Callback_BeforeDisplay 0
#define AI_C_Callback_AfterDisplay	1
#define AI_C_Callback_AfterRec		2
#define AI_C_Callback_AfterBlend	3
#define AI_C_Callback_WhenDestroy	4
#define AI_C_Callback_ShortWay		5
#define AI_C_Callback_SectoActOn	6
#define AI_C_Callback_SectoActOff	7
#define AI_C_Callback_SectoVisOn	8
#define AI_C_Callback_SectoVisOff	9

/*$4
 ***********************************************************************************************************************
    Structures
 ***********************************************************************************************************************
 */

#define AI_Cflag_BreakPoint			0x01	/* BreakPoint */
#define AI_Cflag_ForceBreakPoint	0x02
#define AI_Cflag_FctTransform		0x04
#define AI_Cflag_JumpDest			0x80

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Type for C function
 -----------------------------------------------------------------------------------------------------------------------
 */

#define AI2C
#ifdef AI2C

/******************************************************************/
typedef int (*AI2C_tdfn_Func) (void);
typedef struct	AI2C_fctdef_
{
	ULONG			ul_Key;
#ifdef GAMECUBE_USE_AI2C_DLL
	AI2C_tdfn_Func	*ppst_Func;
#else // GAMECUBE_USE_AI2C_DLL
	AI2C_tdfn_Func	pst_Func;
#endif // GAMECUBE_USE_AI2C_DLL
	char			*pz_name;
} AI2C_fctdef;
extern AI2C_fctdef AI2C_gat_fctdefs[];

#endif /* AI2C */

/*
 -----------------------------------------------------------------------------------------------------------------------
    Structure to define a node.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_Node_
{
	LONG	l_Param;
	short	w_Param;
	char	c_Flags;
	char	c_Type;
} AI_tdst_Node;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Description of a local variable. Only for editor
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_Local_
{
	int		i_Offset;					/* Offset in local buffer */
	int		i_Type;						/* Type of variable */
	char	asz_Name[AI_C_MaxLenVar];	/* Name of the variable for display it */
} AI_tdst_Local;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Description of IA function.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_Function_
{
	AI_tdst_Node	*pst_RootNode;		/* First node of function tree */
	int				i_SizeLocalStack;	/* Size in bytes of local stack */
	char			*pc_Strings;		/* Address of string buffer */
	USHORT			uw_NbRefs;			/* Number of references to that function */
	char			*pz_name;			/* Name in AI to C */
#ifdef ACTIVE_EDITORS
	AI_tdst_Node	*pst_OtherRootNode; /* First node of function tree */
	ULONG			ul_NbLocals;		/* Number of variables */
	AI_tdst_Local	*pst_LocalVars;		/* Infos about local vars */
	char			az_Name[128];		/* Nom de la fonction */
	ULONG			h_File;
#endif
	AI2C_tdfn_Func	pfn_CFunc;			/* C executed function */
#ifdef BENCH_IA_TEST
	void			*p_BenchDataForFunction;
#endif
#ifdef AI_FULL_RASTERS
	PRO_tdst_TrameRaster	st_Raster;
	int						aaa;
#endif
} AI_tdst_Function;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    Flags for w_Flags field of a var description
 -----------------------------------------------------------------------------------------------------------------------
 */

#define AI_CVF_Private	0x0001
#define AI_CVF_Enum		0x0002
#define AI_CVF_Sep		0x0004
#define AI_CVF_Help		0x0008
#define AI_CVF_Save		0x0010
#define AI_CVF_Reinit	0x0020
#define AI_CVF_SaveAl	0x0040
#define AI_CVF_ByRef	0x0080
#define AI_CVF_Pointer	0x0100

/*
 -----------------------------------------------------------------------------------------------------------------------
    Execution context
 -----------------------------------------------------------------------------------------------------------------------
 */
#define AI_CXT_Normal		0
#define AI_CXT_AfterRec		1
#define AI_CXT_AfterBlend	2
#define AI_CXT_Max			3

/*
 -----------------------------------------------------------------------------------------------------------------------
    Description of one variable
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_VarInfo_
{
	int		i_Offset;	/* Offset of the var in the buffer */
	int		i_Offset2;
	int		i_NumElem;	/* Number of elements (!= 1 for an array) */
	short	w_Type;		/* Type of the variable */
	short	w_Flags;	/* Flags for variable */
} AI_tdst_VarInfo;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Description of a variable. Only for editor
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_EditorVarInfo_
{
	char	asz_Name[AI_C_MaxLenVar];	/* Name of the variable for display it */
} AI_tdst_EditorVarInfo;

/*$1
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    An additionnal structure for complex edition
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

typedef struct	AI_tdst_EditorVarInfo2_
{
	int		i_Offset;			/* Offset of the variable in infos list */
	char	*psz_StringCst;		/* For enum edition */
	char	*psz_StringHelp;	/* An help string */
	USHORT	uw_Flags;
	USHORT	uw_p1;
	void	*p2;
} AI_tdst_EditorVarInfo2;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Description of all variables.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_VarDes_
{
	char					*pc_BufferInit; /* Default initial values for model */
	ULONG					ul_SizeBufferInit;				/* Size of buffer for variables */

	AI_tdst_VarInfo			*pst_VarsInfos;					/* Description of each variable */
	ULONG					ul_NbVars;						/* Number of variables */

	AI_tdst_EditorVarInfo	*pst_EditorVarsInfos;			/* Some descriptions for editors */
#ifdef ACTIVE_EDITORS
	ULONG					ul_NbVarsInfos2;				/* Number of vars in pst_EditorVarsInfos2 */
	AI_tdst_EditorVarInfo2	*pst_EditorVarsInfos2;			/* Another complex description */
#endif
	AI_tdst_Function		*apst_InitFct[AI_C_MaxTracks];	/* The initial function for each track */
} AI_tdst_VarDes;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Description of an AI model
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_Model_
{
	AI_tdst_VarDes		*pst_VarDes;		/* Description of vars */
	AI_tdst_Function	**ppst_Functions;	/* Array of functions description */
	ULONG				ul_NbFunctions;		/* Number of functions */
	USHORT				uw_NbInstances;		/* Number of instances for that model */
	UCHAR				c_SecondPassDone;	/* Second pass done on model */
	UCHAR				c_ModelInvalid;		/* Model is invalid, do not execute */
	struct SCR_tt_ProcedureList_	**pp_ProcList;				/* Liste des procecurelist */
	UINT							u32_NbProcList;				/* Nbre de proclist */
	BIG_KEY				ul_key;
#ifdef ACTIVE_EDITORS
	UCHAR				c_DisplayFilter;	/* Filter model */
#endif
	AI2C_tdfn_Func		pfn_CFunc;			/* Init executed function */
} AI_tdst_Model;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Callback
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_CB_
{
	AI_tdst_Function	*pst_Func;			/* Function */
	int					i_Type;				/* CB type */
	void				*pst_GameObject;	/* Source gameobject */
#ifdef AI_FULL_RASTERS
	PRO_tdst_TrameRaster	st_Raster;
	int						aaa;
#endif
} AI_tdst_CB;

#define AI_OPTIM

/*
 -----------------------------------------------------------------------------------------------------------------------
    Description of an AI instance
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	AI_tdst_Instance_
{
	AI_tdst_Model		*pst_Model;						/* Model for that instance */
	AI_tdst_Node		*apst_JumpNode[AI_C_MaxTracks]; /* The current node to execute in the function */
	AI_tdst_Function	*apst_CurrentFct[AI_C_MaxTracks];	/* The current function for each track */
	float				af_WaitTime[AI_C_MaxTracks];		/* Time to wait before execute track */

#if defined(ACTIVE_EDITORS) || !defined(AI_OPTIM)
	char				*pc_VarsBufferInit;					/* Initial values of vars for model */
#endif
	char				*pc_VarsBuffer;						/* Current values of vars for model */

	int					i_NumCB;			/* Number of callbacks */
	AI_tdst_CB			*pst_CB;			/* All callbacks */

	AI_tdst_VarDes		*pst_VarDes;		/* Description of vars (for cardec) */

	unsigned short		uw_FirstTrack;		/* First valid track */
	unsigned short		uw_LastTrack;		/* Last valid track */
	ULONG				ul_Flags;			/* Flags divers */

#ifdef ACTIVE_EDITORS
	void				*pst_GameObject;	/* The game object associated with the instance */
#endif
#ifdef AI_FULL_RASTERS
	PRO_tdst_TrameRaster	st_Raster;
	int						aaa;
#endif
} AI_tdst_Instance;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Une procédure
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SCR_tt_Procedure_
{
	AI_tdst_Node					*pu32_Nodes;	/* Noeuds à évaluer */
	USHORT							u16_SizeLocal;	/* Taille de la pile (variables locales) */
	char							*pz_Name;		/* Nom */
	struct SCR_tt_ProcedureList_	*pt_ProcList;	/* Liste à laquelle on appartient */
#ifdef ACTIVE_EDITORS
	AI_tdst_Node					*pu32_DbgNodes;		/* Noeuds spéciaux pour débugage */
	ULONG							ul_NbLocals;		/* Number of variables */
	AI_tdst_Local					*pst_LocalVars;		/* Infos about local vars */
	char							asz_Comment[1024];
#endif
#ifdef AI_FULL_RASTERS
	PRO_tdst_TrameRaster			st_Raster;
	int								aaa;
#endif
} SCR_tt_Procedure;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Liste de procedures
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SCR_tt_ProcedureList_
{
	USHORT				u16_Num;					/* Nombre de procedures */
	USHORT				u16_Flags;					/* Flags divers */
	SCR_tt_Procedure	*pt_All;					/* Toutes les procédures */
	BIG_KEY				h_SourceFile;				/* Clef du fichier (editeur) : Utile en moteur */
	USHORT				uw_NbRefs;
#ifdef ACTIVE_EDITORS
	ULONG				h_File;
#endif
	AI2C_tdfn_Func		pfn_CFunc;					/* Init executed function */
	char				*pc_Strings;				/* Address of string buffer */
} SCR_tt_ProcedureList;

/*
 -----------------------------------------------------------------------------------------------------------------------
   Triger
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct SCR_tt_Trigger_
{
	BIG_KEY				h_KeyFile;						/* Engine file where the trigger is defined */
	union
	{
		SCR_tt_Procedure		*pt_Proc;				/* Corresponding procedure */
		void					*pfn_CFunc;
	};
	char				az_Name[AI_MaxLenTrigger];		/* Name of the trigger */
	AI_tdst_Message		t_Msg;							/* Paramètres */
} SCR_tt_Trigger;

/*
 -----------------------------------------------------------------------------------------------------------------------
   Dynmaic special flag
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct SCR_tt_SFDyn_
{
	ULONG	ul_Key;	/* GAO key */
	USHORT	ul_Num;	/* Special flag number */
	USHORT	ul_Val;	/* special flag value */
} SCR_tt_SFDyn;
#define SFDYN_MAX	1512

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __AISTRUCT_H__ */
