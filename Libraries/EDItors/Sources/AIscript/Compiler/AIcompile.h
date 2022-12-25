/*$T AIcompile.h GC 1.138 12/03/03 10:21:46 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#pragma once
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIwin.h"
#include "AIinterp/Sources/AIstruct.h"
#include "BIGfiles/BIGdefs.h"
#include "BIGfiles/BIGgroup.h"
#include "AIinterp/Sources/AIengine.h"

#define TOKEN_NAME				1
#define TOKEN_NUMBER			2
#define TOKEN_TYPE				3
#define TOKEN_KEYWORD			4
#define TOKEN_FUNCTION			5
#define TOKEN_FIELD				6
#define TOKEN_ULTRA				7
#define TOKEN_LOCALVAR			10
#define TOKEN_LOCALVARARRAY		11
#define TOKEN_LOCALVARARRAY2	12
#define TOKEN_LOCALVARARRAY3	13

#define TOKEN_PP				20
#define TOKEN_STRING			21
#define TOKEN_AIFUNCTION		22
#define TOKEN_GAMEOBJECT		23
#define TOKEN_MODEL				24
#define TOKEN_NETWORK			25
#define TOKEN_TEXT				26
#define TOKEN_LABEL				27

#define TOKEN_GLOBALVAR			30
#define TOKEN_GLOBALVARARRAY	31
#define TOKEN_GLOBALVARARRAY2	32
#define TOKEN_GLOBALVARARRAY3	33

#define TOKEN_CALLPROC			34


/*
 -----------------------------------------------------------------------------------------------------------------------
    Definition of a switch
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	LyN_sc_Switch_
{
	CList<UINT, UINT>	o_PosCase;				/* Position du noeud d'un case */
	CList<int, int>		o_ValCase;				/* Valeur du case */
	ULONG				u32_PosDefault;			/* Position du default dans laliste de case */
	CList<UINT, UINT>	o_PosBreak;				/* Position du noeud d'un break */
} LyN_sc_Switch;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Definition of a macro for PP
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	tdst_PPMacro_
{
	CList<CString, CString> o_Params;	/* List of parameters names */
	CString					o_Content;	/* The content of the macro */
} tdst_PPMacro;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Main structure use to compile.
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	tdst_Parse_
{
	int				i_Error;
	int				i_NumLine;
	int				i_NumLineBreak;
	char			*psz_Buf;
	char			*psz_Before;
	char			*psz_Text;
	BOOL			b_TopNode;

	AI_tdst_Node	*pst_Output;
	AI_tdst_Node	*pst_OtherOutput;
	AI_tdst_Local	*pst_LocalVars;
	int				i_CurrentOutput;
	int				i_MaxOutput;

	char			*pc_StringBuf;
	int				i_NumStringBuf;
	int				i_MaxStringBuf;

	char			c_Look;
	int				i_InLoop[100];
	int				i_InLoopContinue[100];
	int				i_CurLoop;
	BOOL			b_Ultra;
	int				i_NumUltra;
	int				i_PosUltra;

	char			c_Token;
	CString			o_Value;
	CString			o_Value1;
	CString			o_LastComment;
	int				i_Value;
	int				i_Value2;
	int				i_IndexValue;
	int				StmtLevel;

	BIG_INDEX		ul_Model, ul_File;
	char			asz_OrgPath[BIG_C_MaxLenPath];	/* Original compiled file */
	char			asz_OrgFile[BIG_C_MaxLenName];
	char			asz_Path[BIG_C_MaxLenPath];		/* File after parsing (for errors) */
	char			asz_File[BIG_C_MaxLenName];
} tdst_Parse;

/*$4
 ***********************************************************************************************************************
    Description for variables
 ***********************************************************************************************************************
 */

/* Special variable (for field mi_SpecialVar) */
#define VAR_NONE		0x00
#define VAR_TRACK		0x01	/* The variable is track<nnn> */
#define VAR_PRIVATE		0x02	/* The variable is private */
#define VAR_SEP			0x04	/* A dummy separator for edition */
#define VAR_ENUM		0x08	/* Enumeration */
#define VAR_STRING1		0x10	/* Mo_String1 is valid */
#define VAR_STRING2		0x20	/* Mo_String2 is valid */
#define VAR_SAVE		0x40	/* Variable need to be save */
#define VAR_OPTIM		0x80
#define VAR_REINIT		0x100	/* Univ variable must be reinit */
#define VAR_SAVEAL		0x200	/* Second mode save */

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    One variable description
 -----------------------------------------------------------------------------------------------------------------------
 */

class EAI_cl_Variable :
	public CObject
{
/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	EAI_cl_Variable(int _i_Type, int _i_Pos)
	{
		mi_SpecialVar = 0;
		mi_Type = _i_Type;
		mi_Pos = _i_Pos;
		mi_SizeArray = 0;
		mb_HasBeenInit = FALSE;
		mpac_InitValue = NULL;
		mb_HasBeenUsed = FALSE;
		byref = FALSE;
		byrefarr = FALSE;
	}

	~EAI_cl_Variable(void)
	{
		mo_String1.Empty();
		mo_String2.Empty();
	};

/*
 -----------------------------------------------------------------------------------------------------------------------
 -----------------------------------------------------------------------------------------------------------------------
 */
public:
	int		mi_SpecialVar;
	int		mi_Type;
	int		mi_Pos;
	int		mi_SizeArray;
	int		mai_SizeElem[10];
	BOOL	mb_HasBeenInit;
	char	*mpac_InitValue;
	CString mo_String1;
	CString mo_String2;
	int		StmtLevel;
	int		TotalSize;
	BOOL	mb_HasBeenUsed;
	BOOL	byref;
	BOOL	byrefarr;
};

/*
 -----------------------------------------------------------------------------------------------------------------------
    A serie of variables
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	EAI_tdst_Vars_
{
	CMapStringToPtr o_Vars;
	int				i_LastPosVar;
} EAI_tdst_Vars;

/*
 -----------------------------------------------------------------------------------------------------------------------
    A label
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	EAI_tdst_Label_
{
	CString o_Name;
	int		i_Pos;
} EAI_tdst_Label;

/*
 -----------------------------------------------------------------------------------------------------------------------
    Procédures
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SCR_sc_ProcedureParam_
{
	char	az_Name[BIG_C_MaxLenName];			/* Nom */
	UINT	u32_Type;							/* Type */
	UINT	u32_Size;							/* Size (bytes) contenu */
	UINT	u32_Pos;							/* Position sur la pile */
	int		byref;								/* 1 byref, 2 byrefarr */
} SCR_sc_ProcedureParam;

typedef struct	SCR_sc_Procedure_
{
	BIG_INDEX				h_File;				/* Fichier editeur */
	char					az_Name[512];
	UINT					u32_NumParameters;	/* Nombre de parametres */
	SCR_sc_ProcedureParam	*pt_Parameters;		/* Tous les parametres */
	AI_tdst_Node			*pu32_Nodes;		/* Noeuds */
	AI_tdst_Node			*pu32_DbgNodes;		/* Noeuds debug */
	UINT					u32_NumNodes;		/* Nbre de noeuds */
	UINT					u32_ReturnType;		/* Type de retour */
	USHORT					u16_SizeLocal;		/* Taille pile locale */
	AI_tdst_Local			*pst_LocalVar;
	int						i_NumLocalVar;
	int						i_Line;				/* Ligne de définition */
	char					az_Comment[1024];
	char					ultra;
} SCR_sc_Procedure;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

class	EAI_cl_Frame;
class	EAI_cl_Compiler
{

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    CONSTRUCT.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EAI_cl_Compiler(void);
	~EAI_cl_Compiler(void);

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    ATTRIBUTES.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	EAI_cl_Frame		*mpo_Frame;
	char				*mpsz_Text;

	tdst_Parse			e;

	CMapStringToPtr		mo_ReferenceFiles;
	CMapStringToPtr		mo_DependFiles;
	CList<BOOL, BOOL>	mo_forWhile;

	EAI_tdst_Vars		mst_LocalVars;
	EAI_tdst_Vars		mst_GlobalVars;
	BOOL				mb_GlobalVarMode;					/* TRUE when compiling a var file */

	BIG_tdst_GroupElem	*mpst_CurrentModel;					/* Content of the group file current model */
	int					mi_SizeModel;						/* Number of file in model group */
	BIG_INDEX			mul_CurrentModel;					/* The current model */
	BIG_INDEX			mul_CurrentInstance;				/* The current instance */
	BIG_INDEX			mul_CurrentFile;					/* Current compiled file */
	int					mi_RequestedType;					/* When we know the type before parsing */
	int					mi_SpecReturn;

	BOOL				mb_CanOutput;						/* TRUE to generate an output */
	BOOL				mb_CompileToLog;					/* TRUE to compile to log and not to file */

	BOOL				mb_NeedBreak;
	int					mi_BreakLineOn;
	ULONG				mul_NbNodes;
	ULONG				mul_NbDelNodes;

	int					mi_UltraMode;						/* For ultra mode */
	AI_tdst_Node		e_copy[2000];
	struct
	{
		BOOL	b_TopNode;
		int		i_NumLine;
	} ep_copy[2000];
	BOOL									mb_Optim;

	/* When generate var file */
	BIG_INDEX								maul_InitFct[AI_C_MaxTracks];
	ULONG									mul_MaxCallbacks;
	BIG_INDEX								maul_Callbacks[AI_C_MaxCallbacks];
	AI_tdst_VarDes							mst_VarDes;

	CList<EAI_tdst_Label, EAI_tdst_Label>	mo_AllLabels;	/* Tous les labels */
	CList<EAI_tdst_Label, EAI_tdst_Label>	mo_ResLabels;	/* Tous les labels à resoudre */
	CList<LyN_sc_Switch *, LyN_sc_Switch *>	mo_Switch;		/* Tous les switch */

	BOOL									mb_ProcList;
	BOOL									mb_ProcListParent;
	BOOL									mb_ProcListLocal;
	CMapStringToPtr							mo_ProcList;
	BOOL									mb_GenPP;
	BOOL									mb_ForceGenPP;
	BOOL									mb_HasReturn;
	int										mi_NumProc;

/*$2
 -----------------------------------------------------------------------------------------------------------------------
    FUNCTIONS.
 -----------------------------------------------------------------------------------------------------------------------
 */

public:
	char			*PPLine(char *, char *);
	char			*PPEnable(char *);
	char			*PPUndef(char *);
	char			*PPInclude(char *);
	char			*PPDefine(char *);
	char			*PPReplaceConstant(char *);
	char			*PPDef(char *, BOOL);
	void			PP(void);

	void			Optim(void);

	void			FreeCompilerOut(void);
	void			FreeCompilerVarDes(void);
	void			ResetVars(EAI_tdst_Vars *);
	void			ResetNoVars(void);
	void			Reset(BOOL = TRUE);
	void			ResetSystemVar(void);

	void			GetChar(void);
	void			UnGetChar(void);
	void			SkipComment(void);
	void			SkipWhite(void);
	void			MatchString(char *);

	void			GetField(void);
	void			GetName(BOOL = TRUE);
	void			GetPP(void);
	void			GetString(void);
	void			GetNum(void);
	void			GetOp(void);
	int				GetUltra(void);

	void			GetReference(void);
	BOOL			b_GetAIModel(void);
	BOOL			b_GetEngineObject(void);
	BOOL			b_GetAIFunction(char *);
	BOOL			b_GetNetwork(void);

	BOOL			VarOfModel(char *);
	void			Scan(void);
	void			Next(BOOL = TRUE);

	BOOL			b_IsBreakPointAtLine(int);
	void			Output(int, int, int, int = 0);
	void			OutputStr(char *);

	BIG_KEY			ul_GetEngineFileForForProcList(BIG_KEY);
	BIG_KEY			ul_GetEngineFileForForFunction(BIG_KEY);
	ULONG			ul_GetEngineModelIndex(BIG_INDEX, char *, char *);
	void			GenerateOutput(AI_tdst_Function *);
	void			GenerateOutputVars(BIG_INDEX);
	void			GenerateOutputLog(AI_tdst_Function *);
	void			GenerateOutputFile(AI_tdst_Function *);
	void			OutputProcList(BIG_INDEX);

	POSITION		x_SearchLabel(CString &);
	void			ResolveLabels(void);

	int				CompatibleTypes(int, int);
	void			AssertIntType(int);
	void			AssertScalarType(int);
	void			ParamList(void);
	int				DoUltra(int, int, BOOL);
	int				DoString(void);
	int				DoNumber(void);
	int				DoCall(void);
	void			DoProcParamList(int);
	int				DoProcCall(void);
	int				DoTriggerCall(void);
	void			DoProcedure(int, int, int);
	void			DoIf(void);
	void			DoSwitch(void);
	void			DoCase(void);
	void			DoDefault(void);
	void			DoWhile(void);
	void			DoFor(void);
	void			DoMeta(void);
	void			DoGoto(void);
	void			DoBreak(void);
	void			DoContinue(void);
	void			DoPush(void);
	void			DoAffect(void);
	void			DoAffectCom(int);

	int				DoField(int);
	void			DoVarInit(EAI_cl_Variable *);
	int				DoVariable(void);
	EAI_cl_Variable *AddVariable(int);
	void			RemoveVarStmt(int);

	int				Factor(void);
	int				Expression(void);
	int				Relation(void);
	int				BoolExpression(void);
	void			TopDecls(void);
	void			Statement(BOOL = TRUE);
	void			BeginBlock(BOOL = TRUE);

	int				i_Compile(BIG_INDEX, BIG_INDEX, char *);
};

extern void AI2C_Reinit(void);

#endif /* ACTIVE_EDITORS */
