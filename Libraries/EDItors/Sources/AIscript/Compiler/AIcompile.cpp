/*$T AIcompile.cpp GC 1.134 04/27/04 11:26:39 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "LINKs/LINKmsg.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BIGfiles/BIGread.h"

#include "../AIerrid.h"
#include "../AIframe.h"
#include "AIinterp/Sources/AIengine.h"
#include "EDImainframe.h"
#include "BASe/MEMory/MEM.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "EDIpaths.h"
#include "BIGfiles/SAVing/SAVdefs.h"

#include "AIinterp/Sources/AIBench.h"

extern CMapStringToPtr							go_PPFunctions;
extern CList<tdst_PPMacro *, tdst_PPMacro *>	go_DefineMacros;
extern CList<tdst_PPMacro *, tdst_PPMacro *>	go_DefineMacrosGlob;
extern CMapStringToPtr							go_DefineMacroNames;
extern CMapStringToPtr							go_DefineMacroNamesGlob;
extern CMapStringToPtr							go_DefineCstNames;
extern CMapStringToPtr							go_DefineCstNamesGlob;
extern CList<CString, CString>					go_DefineCst;
extern CList<CString, CString>					go_DefineCstGlob;
extern CMapStringToString						go_DefineCstFiles;
extern CMapStringToString						go_DefineCstFilesGlob;
extern CMapStringToString						go_DefineMacrosFiles;
extern CMapStringToString						go_DefineMacrosFilesGlob;

CMapPtrToPtr	go_PPAllPV;		/* Marquage des ';' */
CMapPtrToPtr	go_PPAllRef;	/* Marquage des références */
CMapPtrToPtr	go_PPAllPop;	/* Marquage des types de pop */
CMapPtrToPtr	go_PPReplace;
int				gi_PPAllPV;
int				gi_PPAllPVBeforeNext;
int				gi_PPAllPVCant = 0;
int				gi_FirstCode = -1;

#define MGP() \
	if(mb_GenPP && !gi_PPAllPVCant) go_PPAllPV.SetAt((void *) gi_PPAllPVBeforeNext, (void *) gi_PPAllPVBeforeNext);

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_Compiler::EAI_cl_Compiler(void)
{
	e.pst_Output = NULL;
	e.pst_OtherOutput = NULL;
	e.pst_LocalVars = NULL;
	e.pc_StringBuf = NULL;
	e.psz_Buf = NULL;
	mb_CompileToLog = FALSE;
	mb_GlobalVarMode = FALSE;
	mb_CanOutput = TRUE;
	mb_Optim = FALSE;
	mb_ProcList = FALSE;
	mb_ProcListLocal = FALSE;
	mb_ProcListParent = FALSE;
	mb_GenPP = FALSE;
	mb_ForceGenPP = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_Compiler::~EAI_cl_Compiler(void)
{
	Reset(FALSE);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::ResetVars(EAI_tdst_Vars *_pst_Vars)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		x_Pos;
	CString			o_String;
	EAI_cl_Variable *po_Variable;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Delete local variables */
	if(!_pst_Vars->o_Vars.IsEmpty())
	{
		x_Pos = _pst_Vars->o_Vars.GetStartPosition();
		while(x_Pos)
		{
			_pst_Vars->o_Vars.GetNextAssoc(x_Pos, o_String, (void * &) po_Variable);
			if(po_Variable->mpac_InitValue) L_free(po_Variable->mpac_InitValue);
			delete po_Variable;
		}

		_pst_Vars->o_Vars.RemoveAll();
	}

	_pst_Vars->i_LastPosVar = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::ResetNoVars(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		x_Pos;
	CString			o_String;
	EAI_cl_Compiler *po_Obj;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	mi_SpecReturn = 0;

	/* Delete reference files */
	if(!mo_ReferenceFiles.IsEmpty())
	{
		x_Pos = mo_ReferenceFiles.GetStartPosition();
		while(x_Pos)
		{
			mo_ReferenceFiles.GetNextAssoc(x_Pos, o_String, (void * &) po_Obj);
			delete po_Obj;
		}

		mo_ReferenceFiles.RemoveAll();
	}

	/* Free output buffer */
	if(e.pst_Output)
	{
		e.pst_Output = NULL;
		e.i_CurrentOutput = e.i_MaxOutput = 0;
	}

	/* Free other output buffer (debug) */
	if(e.pst_OtherOutput) e.pst_OtherOutput = NULL;
	if(e.pst_LocalVars) e.pst_LocalVars = NULL;

	/* Free string buffer */
	if(e.pc_StringBuf)
	{
		e.pc_StringBuf = NULL;
		e.i_NumStringBuf = e.i_MaxStringBuf = 0;
	}

	e.b_Ultra = FALSE;
	e.b_TopNode = FALSE;
	mi_UltraMode = 0;
	mb_NeedBreak = FALSE;
	mi_BreakLineOn = 0;
	mi_RequestedType = TYPE_GAMEOBJECT;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::Reset(BOOL _b_All)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos;
	CString				name;
	SCR_sc_Procedure	*pt_ProcEd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ResetVars(&mst_LocalVars);
	ResetVars(&mst_GlobalVars);
	mo_DependFiles.RemoveAll();
	ResetNoVars();
	mpst_CurrentModel = NULL;
	mi_SizeModel = 0;
	mul_CurrentModel = BIG_C_InvalidIndex;
	mul_CurrentInstance = BIG_C_InvalidIndex;
	mul_CurrentFile = BIG_C_InvalidIndex;
	L_memset(&mst_VarDes, 0, sizeof(AI_tdst_VarDes));
	mi_SpecReturn = 0;

	pos = mo_ProcList.GetStartPosition();
	while(pos)
	{
		mo_ProcList.GetNextAssoc(pos, name, (void * &) pt_ProcEd);
		if(pt_ProcEd->pt_Parameters) MEM_Free(pt_ProcEd->pt_Parameters);
		delete pt_ProcEd;
	}

	mo_ProcList.RemoveAll();
	while(mo_Switch.GetCount()) delete mo_Switch.RemoveTail();

	/* Constants */
	if(_b_All)
	{
		go_DefineMacroNames.RemoveAll();
		pos = go_DefineMacros.GetHeadPosition();
		while(pos) delete go_DefineMacros.GetNext(pos);
		go_DefineMacros.RemoveAll();
		go_DefineMacrosFiles.RemoveAll();

		go_DefineCstFiles.RemoveAll();
		go_DefineCstNames.RemoveAll();
		go_DefineCst.RemoveAll();

		go_DefineMacroNamesGlob.RemoveAll();
		pos = go_DefineMacrosGlob.GetHeadPosition();
		while(pos) delete go_DefineMacrosGlob.GetNext(pos);
		go_DefineMacrosGlob.RemoveAll();
		go_DefineMacrosFilesGlob.RemoveAll();

		go_DefineCstFilesGlob.RemoveAll();
		go_DefineCstNamesGlob.RemoveAll();
		go_DefineCstGlob.RemoveAll();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::FreeCompilerOut(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
//	POSITION			pos;
//	CString				name;
//	SCR_sc_Procedure	*pt_ProcEd;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*	pos = mo_ProcList.GetStartPosition();
	while(pos)
	{
		mo_ProcList.GetNextAssoc(pos, name, (void * &) pt_ProcEd);
		MEM_Free(pt_ProcEd->pu32_Nodes);
		MEM_Free(pt_ProcEd->pu32_DbgNodes);
		MEM_Free(pt_ProcEd->pt_Parameters);
		MEM_Free(pt_ProcEd->pst_LocalVar);
		delete pt_ProcEd;
	}

	mo_ProcList.RemoveAll();*/

	if(e.pst_Output)
	{
		MEM_Free(e.pst_Output);
		e.pst_Output = NULL;
	}

	if(e.pst_OtherOutput)
	{
		MEM_Free(e.pst_OtherOutput);
		e.pst_OtherOutput = NULL;
	}

	if(e.pst_LocalVars)
	{
		MEM_Free(e.pst_LocalVars);
		e.pst_LocalVars = NULL;
	}

	if(e.pc_StringBuf)
	{
		MEM_Free(e.pc_StringBuf);
		e.pc_StringBuf = NULL;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::FreeCompilerVarDes(void)
{
	/*~~~~~~*/
	ULONG	j;
	/*~~~~~~*/

	if(mst_VarDes.pc_BufferInit)
	{
		MEM_Free(mst_VarDes.pc_BufferInit);
		mst_VarDes.pc_BufferInit = NULL;
	}

	if(mst_VarDes.pst_VarsInfos)
	{
		MEM_Free(mst_VarDes.pst_VarsInfos);
		mst_VarDes.pst_VarsInfos = NULL;
	}

	if(mst_VarDes.pst_EditorVarsInfos)
	{
		MEM_Free(mst_VarDes.pst_EditorVarsInfos);
		mst_VarDes.pst_EditorVarsInfos = NULL;
	}

	if(mst_VarDes.pst_EditorVarsInfos2)
	{
		for(j = 0; j < mst_VarDes.ul_NbVarsInfos2; j++)
		{
			if(mst_VarDes.pst_EditorVarsInfos2[j].psz_StringCst)
				MEM_Free(mst_VarDes.pst_EditorVarsInfos2[j].psz_StringCst);
			if(mst_VarDes.pst_EditorVarsInfos2[j].psz_StringHelp)
				MEM_Free(mst_VarDes.pst_EditorVarsInfos2[j].psz_StringHelp);
		}

		MEM_Free(mst_VarDes.pst_EditorVarsInfos2);
		mst_VarDes.pst_EditorVarsInfos2 = NULL;
	}
}

/*$4
 ***********************************************************************************************************************
    Basic operation on parsing
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GetChar(void)
{
	e.c_Look = *(e.psz_Text++);
	if(mb_GenPP) gi_PPAllPV++;
	if(e.c_Look == '\n')
	{
		e.i_NumLine++;
		if(mi_BreakLineOn == 0) e.i_NumLineBreak++;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::UnGetChar(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::SkipComment(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL	b_CanAdd, b_First;
	char	asz[1024];
	int		i;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	b_CanAdd = FALSE;
	b_First = TRUE;
	if(e.o_LastComment.IsEmpty()) b_CanAdd = TRUE;

	i = 0;
	while(e.c_Look != '\n' && e.c_Look)
	{
		GetChar();
		if(b_CanAdd)
		{
			if(!L_isspace(e.c_Look) || !b_First)
			{
				if(!L_isspace(e.c_Look)) b_First = FALSE;
				asz[i++] = e.c_Look;
			}
		}
	}

	if(b_CanAdd)
	{
		asz[i] = 0;
		e.o_LastComment = asz;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::SkipWhite(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	char	c_Look;
	char	*psz_Text;
	BOOL	b_First, b_Del;
	int		i_NumLine;
	int		i_mempv;
	int		i_mempvnext;
	/*~~~~~~~~~~~~~~~~~~~*/

	e.o_LastComment.Empty();
	b_First = TRUE;
	b_Del = FALSE;
	i_NumLine = e.i_NumLine;
	while(1)
	{
		while(L_isspace(e.c_Look)) GetChar();
		if((i_NumLine != e.i_NumLine) && b_First) b_Del = TRUE;
		i_mempv = gi_PPAllPV;
		i_mempvnext = gi_PPAllPVBeforeNext;
		b_First = FALSE;

		if(e.c_Look == '/')
		{
			c_Look = e.c_Look;
			psz_Text = e.psz_Text;

			GetChar();
			if(e.c_Look == '/')
			{
				SkipComment();
				continue;
			}

			gi_PPAllPV = i_mempv;
			gi_PPAllPVBeforeNext = i_mempvnext;
			e.c_Look = c_Look;
			e.psz_Text = psz_Text;
		}
		break;
	}

	if(b_Del) e.o_LastComment.Empty();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::MatchString(char *string)
{
	ERR_X_Error(!L_strcmp((char *) (LPCSTR) e.o_Value, string), ERR_COMPILER_Csz_ExpString, string);
	Next();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::Next(BOOL _b_Scan)
{
	gi_PPAllPVBeforeNext = gi_PPAllPV;
	e.b_Ultra = FALSE;
	e.psz_Before = e.psz_Text;
	while(1)
	{
		SkipWhite();
		e.o_Value.Empty();
		if(L_isalpha(e.c_Look))
		{
			GetName(_b_Scan);
			break;
		}
		else if(L_isdigit(e.c_Look))
		{
			GetNum();
			break;
		}
		else if(e.c_Look == '#')
		{
			GetPP();
			continue;
		}
		else if(e.c_Look == '.')
		{
			GetField();
			break;
		}
		else if(e.c_Look == '"')
		{
			GetString();
			break;
		}

		/* Ultra */
		else if(e.c_Look == '@')
		{
			e.c_Token = TOKEN_ULTRA;
			e.o_Value = "@";
			break;
		}
		else
		{
			GetOp();
			break;
		}
	}
}

/*$4
 ***********************************************************************************************************************
    Grammar
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::CompatibleTypes(int _i_Type1, int _i_Type2)
{
	if(_i_Type1 == _i_Type2) return _i_Type1;

	/*
	 * The dummy TYPE_EVERY represents every possibles types, and can be combine with
	 * anythings
	 */
	if(_i_Type1 == TYPE_EVERY) return _i_Type1;
	if(_i_Type2 == TYPE_EVERY) return _i_Type2;
	if((_i_Type1 == TYPE_BOOL) && (_i_Type2 == TYPE_BOOL)) return _i_Type1;

	if(((_i_Type1 == TYPE_KEY) || (_i_Type1 == TYPE_INT)) && ((_i_Type2 == TYPE_KEY) || (_i_Type2 == TYPE_INT)))
		return _i_Type1;

	if(((_i_Type1 == TYPE_COLOR) || (_i_Type1 == TYPE_INT)) && ((_i_Type2 == TYPE_COLOR) || (_i_Type2 == TYPE_INT)))
		return _i_Type1;

	if(((_i_Type1 == TYPE_FLOAT) || (_i_Type1 == TYPE_INT)) && ((_i_Type2 == TYPE_FLOAT) || (_i_Type2 == TYPE_INT)))
		return _i_Type1;

	if(((_i_Type1 == TYPE_STRING) || (_i_Type1 == TYPE_TEXT)) && ((_i_Type2 == TYPE_TEXT) || (_i_Type2 == TYPE_STRING)))
		return TYPE_TEXT;

	if((_i_Type1 == TYPE_TEXT) && (_i_Type2 == TYPE_INT)) return TYPE_TEXT;

	/* Error, incompatibles types */
	ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_BadTypes, NULL);
	return 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::AssertIntType(int _i_Type1)
{
	if
	(
		(_i_Type1 == TYPE_FLOAT)
	||	(_i_Type1 == TYPE_INT)
	||	(_i_Type1 == TYPE_EVERY)
	||	(_i_Type1 == TYPE_KEY)
	||	(_i_Type1 == TYPE_COLOR)
	) return;
	ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_BadTypes, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::AssertScalarType(int _i_Type1)
{
	if((_i_Type1 == TYPE_VECTOR) || (_i_Type1 == TYPE_MESSAGE))
		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_BadTypes, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::DoString(void)
{
	/*~~~~~~*/
	int i_Mem;
	/*~~~~~~*/

//	ERR_X_Error(!mb_ProcListParent, "You can't use strings in procedures", "");

	i_Mem = e.i_NumStringBuf;
	e.i_NumStringBuf += e.o_Value.GetLength() + 1;

	/* Need to realloc ? */
	if(e.i_NumStringBuf > e.i_MaxStringBuf)
	{
		e.i_MaxStringBuf += e.o_Value.GetLength() + 1;
		if(e.pc_StringBuf == NULL)
			e.pc_StringBuf = (char *) MEM_p_Alloc(e.i_MaxStringBuf);
		else
			e.pc_StringBuf = (char *) MEM_p_Realloc(e.pc_StringBuf, e.i_MaxStringBuf);
	}

	/* Copy string */
	L_strcpy(e.pc_StringBuf + i_Mem, (char *) (LPCSTR) e.o_Value);

	/* Output Node */
	Output(CATEG_TYPE, i_Mem, TYPE_STRING);
	e.i_Value = TYPE_STRING;

	Next();
	return TYPE_STRING;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::DoNumber(void)
{
	/*~~~~~~~~~~~~~~~~*/
	int		i_Param;
	char	*psz_String;
	int		i_Type;
	LONG	l_Value;
	/*~~~~~~~~~~~~~~~~*/

	i_Param = 0;

	switch(e.i_Value)
	{
	/* For some refs, save an int */
	case TYPE_GAMEOBJECT:
	case TYPE_FUNCTIONREF:
	case TYPE_NETWORK:
		Output(CATEG_TYPE, L_atoi(e.o_Value), TYPE_INT);
		break;

	/* Convert an int format */
	case TYPE_INT:
		Output(CATEG_TYPE, L_atoi(e.o_Value), e.i_Value);
		break;

	/* Convert an hexa ascii format to an int */
	case TYPE_HEXA:
		psz_String = (char *) (LPCSTR) e.o_Value;
		do
		{
			if(L_isdigit(*psz_String))
				i_Param += *psz_String - '0';
			else
				i_Param += *psz_String - 'A' + 10;
			psz_String++;
			if(*psz_String) i_Param <<= 4;
		} while(*psz_String);

		e.i_Value = TYPE_INT;
		Output(CATEG_TYPE, i_Param, e.i_Value);
		break;

	/* Convert a binary ascii format to an int */
	case TYPE_BINARY:
		psz_String = (char *) (LPCSTR) e.o_Value;
		do
		{
			if(*psz_String++ == '1') i_Param++;
			if(*psz_String) i_Param <<= 1;
		} while(*psz_String);

		e.i_Value = TYPE_INT;
		Output(CATEG_TYPE, i_Param, e.i_Value);
		break;

	/* Convert a real ascii format */
	case TYPE_FLOAT:
		e.i_Value = TYPE_FLOAT;
		*((float *) &(l_Value)) = (float) L_atof(e.o_Value);
		Output(CATEG_TYPE, l_Value, e.i_Value);
		break;

	default:
		ERR_X_ForceAssert();
	}

	i_Type = e.i_Value;
	Next();
	return i_Type;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::ParamList(void)
{
	/*~~~~~~~~~~~~~*/
	int		i_Num;
	CString o_Name;
	int		i_SavFct;
	int		i_Type;
	/*~~~~~~~~~~~~~*/

	i_Num = 0;
	o_Name = e.o_Value;
	i_SavFct = e.i_IndexValue;

	Next();

	if(e.c_Token == '[')
	{
		MatchString("[");
		BoolExpression();
		while(e.c_Token == ',')
		{
			Next();
			BoolExpression();
		}

		MatchString("]");
	}

	/* Save attempt type */
	mi_RequestedType = EAI_gast_Functions[i_SavFct].ai_TypePars[i_Num];

	/* Be sure we have an open parent, and scan just after */
	MatchString("(");

	if(e.b_Ultra || e.c_Token != ')')
	{
		i_Num++;
		ERR_X_Error
		(
			i_Num <= EAI_gast_Functions[i_SavFct].i_NumPars,
			ERR_COMPILER_Csz_BadNumPars,
			(char *) (LPCSTR) o_Name
		);

		/* Save attempt type */
		mi_RequestedType = EAI_gast_Functions[i_SavFct].ai_TypePars[i_Num - 1];

		/* Address */
		if(EAI_gast_Functions[i_SavFct].ai_PTypePars[i_Num - 1])
		{
			mi_RequestedType = EAI_gast_Functions[i_SavFct].ai_TypePars[i_Num - 1];

			if(mb_GenPP)
			{
				switch(mi_RequestedType)
				{
				case TYPE_VECTOR:
                    // Replace before "&".
					go_PPReplace.SetAt((void *) (e.psz_Before - e.psz_Buf - 1), "(MATH_tdst_Vector *)");
					break;
				}
			}

			ERR_X_Error(e.c_Token == '&', "You need to specify an address for parameter", NULL);
			Next();
		}

		/* Parse first parameter */
		i_Type = BoolExpression();
		mi_RequestedType = TYPE_GAMEOBJECT;

		/* Check if the type is correct */
		CompatibleTypes(i_Type, EAI_gast_Functions[i_SavFct].ai_TypePars[i_Num - 1]);

		while(e.c_Token == ',')
		{
			ERR_X_Error
			(
				i_Num <= EAI_gast_Functions[i_SavFct].i_NumPars,
				ERR_COMPILER_Csz_BadNumPars,
				(char *) (LPCSTR) o_Name
			);
			i_Num++;

			/* Save attempt type */
			mi_RequestedType = EAI_gast_Functions[i_SavFct].ai_TypePars[i_Num - 1];
			Next();

			/* Address */
			if(EAI_gast_Functions[i_SavFct].ai_PTypePars[i_Num - 1])
			{
				ERR_X_Error(e.c_Token == '&', "You need to specify an address for parameter", NULL);
				Next();
				mi_RequestedType = EAI_gast_Functions[i_SavFct].ai_TypePars[i_Num - 1];
			}

			/* Parse parameter */
			i_Type = BoolExpression();
			mi_RequestedType = TYPE_GAMEOBJECT;

			/* Check if the type is correct */
			CompatibleTypes(i_Type, EAI_gast_Functions[i_SavFct].ai_TypePars[i_Num - 1]);
		}
	}

	MatchString(")");

	ERR_X_Error(i_Num == EAI_gast_Functions[i_SavFct].i_NumPars, ERR_COMPILER_Csz_BadNumPars, (char *) (LPCSTR) o_Name);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::DoUltra(int _i_PosUltra, int _i_NumUltra, BOOL output)
{
	/*~~~~~~~~~*/
	int i;
	int iRes;
	int iNumLine;
	int i_MemPos;
	/*~~~~~~~~~*/

	if(!mi_UltraMode) return FALSE;
	if(!e.b_Ultra) return FALSE;

	/* Model cast ? */
	iRes = BIG_C_InvalidIndex;
	i_MemPos = _i_PosUltra;
	if((e_copy[_i_PosUltra].c_Type == CATEG_TYPE) && (e_copy[_i_PosUltra].w_Param == TYPE_MODEL))
	{
		iRes = e_copy[_i_PosUltra].l_Param;
		_i_PosUltra++;
		_i_NumUltra--;
	}

	/* Output all copied nodes */
	iNumLine = e.i_NumLine;
	e.b_TopNode = ep_copy[i_MemPos].b_TopNode;
	e.i_NumLine = ep_copy[i_MemPos].i_NumLine;
	for(i = 0; i < _i_NumUltra; i++)
	{
		Output(e_copy[_i_PosUltra].c_Type, e_copy[_i_PosUltra].l_Param, e_copy[_i_PosUltra].w_Param);
		_i_PosUltra++;
	}

	if(output)
	{
		if(_i_PosUltra == mi_UltraMode - 1) mi_UltraMode -= _i_NumUltra;
	}

	Output(CATEG_KEYWORD, 0, KEYWORD_ULTRA);

	if(output)
	{
		e.i_NumLine = iNumLine;
	}

	return iRes;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::DoCall(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int		i_SavFct, i_SavIndex, i_PosUltra;
	int		i_Type, i_NumUltra;
	int		i_LineBreak, i_LineBreak1;
	BOOL	b_Ultra, b_Ultra1;
	CString o_Val;
	BOOL	b_TopNode;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_SavFct = e.i_Value;
	i_SavIndex = e.i_IndexValue;
	o_Val = e.o_Value;
	b_Ultra = e.b_Ultra;
	i_NumUltra = e.i_NumUltra;
	i_PosUltra = e.i_PosUltra;
	i_LineBreak = e.i_NumLineBreak;

	/* Treat all parameters */
	b_TopNode = e.b_TopNode;
	e.b_TopNode = FALSE;
	ParamList();

	/* Treat ultra operator */
	b_Ultra1 = e.b_Ultra;
	e.b_Ultra = b_Ultra;
	i_LineBreak1 = e.i_NumLineBreak;
	e.i_NumLineBreak = i_LineBreak;
	if(e.b_Ultra)
	{
		ERR_X_Error(EAI_gast_Functions[i_SavIndex].c_IsUltra, ERR_COMPILER_Csz_NotUltra, (char *) (LPCSTR) o_Val);
	}

	if(EAI_gast_Functions[i_SavIndex].w_ID == FUNCTION_AI_TRACKCURCHANGENOW)
	{
		ERR_X_Error(!mb_ProcListParent, "You can't use that function in a procedure", NULL);
	}

	/* Output ultra nodes */
	DoUltra(i_PosUltra, i_NumUltra, TRUE);

	e.b_TopNode = b_TopNode;
	Output(CATEG_FUNCTION, 0, i_SavFct);
	i_Type = EAI_gast_Functions[i_SavIndex].i_ReturnType;
	e.b_Ultra = b_Ultra1;
	e.i_NumLineBreak = i_LineBreak1;

	/* Check for a field */
	return DoField(i_Type);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoProcParamList(int trigger)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SCR_sc_Procedure	*pt_Proc;
	ULONG				u32_Num;
	ULONG				u32_Type;
	char				az[1024];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pt_Proc = (SCR_sc_Procedure *) e.i_Value;

	if(trigger)
		mi_RequestedType = TYPE_INT;
	else
	{
		Next();
		mi_RequestedType = pt_Proc->pt_Parameters ? pt_Proc->pt_Parameters[0].u32_Type : 0;
	}

	MatchString("(");
	u32_Num = 0;

	while(e.c_Token != ')')
	{
		u32_Num++;
		if(trigger)
		{
			ERR_X_Error(u32_Num <= 1, "Bas number of parameters", "");
		}
		else
		{
			ERR_X_Error(u32_Num <= pt_Proc->u32_NumParameters, "Bas number of parameters", "");
			if(pt_Proc->pt_Parameters[u32_Num - 1].byref == 1)
			{
				ERR_X_Error
				(
					e.c_Token == TOKEN_LOCALVAR || e.c_Token == TOKEN_GLOBALVAR,
					"A byref must be called with a variable",
					NULL
				);
			}

			/* Address */
			if(pt_Proc->pt_Parameters[u32_Num - 1].byref == 2)
			{
				mi_RequestedType = pt_Proc->pt_Parameters[u32_Num - 1].u32_Type;
				ERR_X_Error(e.c_Token == '&', "You need to specify an address for parameter", NULL);
				Next();
			}
		}

		u32_Type = BoolExpression();

		/* Verification du type */
		sprintf(az, "Invalid type for parameter %d", u32_Num - 1);
		if(trigger)
		{
			ERR_X_Error(u32_Type == TYPE_MESSAGE, az, "");
		}
		else
		{
			ERR_X_Error(u32_Type == pt_Proc->pt_Parameters[u32_Num - 1].u32_Type, az, "");

			/* Autre paramètre */
			if(u32_Num < pt_Proc->u32_NumParameters)
			{
				mi_RequestedType = pt_Proc->pt_Parameters[u32_Num].u32_Type;
				MatchString(",");
			}
		}
	}

	MatchString(")");
	if(trigger)
	{
		ERR_X_Error(u32_Num == 1, "Bad number of parameters", "");
	}
	else
	{
		ERR_X_Error(u32_Num == pt_Proc->u32_NumParameters, "Bad number of parameters", "");
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::DoProcCall(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SCR_sc_Procedure	*pt_Proc;
	int		i_PosUltra;
	int		i_NumUltra;
	BOOL	b_Ultra, b_Ultra1;
	int		i_LineBreak, i_LineBreak1;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pt_Proc = (SCR_sc_Procedure *) e.i_Value;
	b_Ultra = e.b_Ultra;
	ERR_X_Error(pt_Proc->ultra || !b_Ultra, "Ultra must be called on procedure_ultra only", NULL);
//	ERR_X_Error(!pt_Proc->ultra || b_Ultra, "procedure_ultra must ALWAYS be called with an ultra operator", NULL);
	i_NumUltra = e.i_NumUltra;
	i_PosUltra = e.i_PosUltra;
	i_LineBreak = e.i_NumLineBreak;

	/* Sauve état pile */
	Output(CATEG_KEYWORD, 0, KEYWORD_INPROCSTACK);

	/* Liste des paramètres */
	DoProcParamList(0);

	/* Treat ultra operator */
	b_Ultra1 = e.b_Ultra;
	e.b_Ultra = b_Ultra;
	i_LineBreak1 = e.i_NumLineBreak;
	e.i_NumLineBreak = i_LineBreak;

	/* Output ultra nodes */
	DoUltra(i_PosUltra, i_NumUltra, TRUE);

	/* Call */
	Output(CATEG_KEYWORD, -1, KEYWORD_CALLPROC);
	Output(0, 0, 0);				/* Fichier */
	OutputStr(pt_Proc->az_Name);	/* Nom */

	e.b_Ultra = b_Ultra1;
	e.i_NumLineBreak = i_LineBreak1;

	return pt_Proc->u32_ReturnType;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::DoTriggerCall(void)
{
	/*~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Node	t_Copy;
	int				i_Type;
	/*~~~~~~~~~~~~~~~~~~~*/

	/* Sauve état pile */
	Output(CATEG_KEYWORD, 0, KEYWORD_INPROCSTACK);

	/* Variable de type trigger */
	Next();
	MatchString("(");
	ERR_X_Error(e.c_Token == TOKEN_LOCALVAR || e.c_Token == TOKEN_GLOBALVAR, "Need a variable trigger", NULL);

	i_Type = DoVariable();
	ERR_X_Error(i_Type == TYPE_TRIGGER, "Need a variable trigger", NULL);
	L_memcpy(&t_Copy, &e.pst_Output[--e.i_CurrentOutput], sizeof(AI_tdst_Node));

	MatchString(")");

	/* Call */
	Output(t_Copy.c_Type, t_Copy.l_Param, t_Copy.w_Param);
	Output(CATEG_KEYWORD, -1, KEYWORD_CALLTRIGGER);
	return TYPE_INT;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoProcedure(int trigger, int local, int ultra = 0)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int						RetType;
	SCR_sc_Procedure		*pt_Proc;
	EAI_cl_Variable			*po_Var;
	SCR_sc_ProcedureParam	*pt_Param;
	BOOL					already;
	int						i;
	int						save_param;
	SCR_sc_ProcedureParam	*p_save_param;
	BOOL					byref;
	BOOL					byrefarr;
	CString					str;
	int						line;
	CString					o_Cmt;
	BOOL					alreadydef;
	CString					oalready;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mb_ProcListLocal = local;
	line = e.i_NumLine;
	o_Cmt = e.o_LastComment;
	alreadydef = FALSE;

	/* Type retour */
	Next();
	ERR_X_Error(e.c_Token == TOKEN_TYPE, "Need return type", NULL);
	RetType = e.i_Value;
	ERR_X_Error(!trigger || RetType == TYPE_INT, "Bad return type for trigger", NULL);

	/* Nom */
	Next(FALSE);
	ERR_X_Error(e.c_Token == TOKEN_NAME, "Need procedure name", NULL);
	ERR_X_Error
	(
		!trigger || L_strlen((char *) (LPCSTR) e.o_Value) < AI_MaxLenTrigger,
		"Name too long for trigger",
		NULL
	);
	if(trigger)
	{
		str.Format("@%d%s", mi_NumProc++, (char *) (LPCSTR) e.o_Value);
		e.o_Value = str;
	}

	/* Le nom existe deja ? */
	already = FALSE;
	if(mo_ProcList.Lookup(e.o_Value, (void * &) pt_Proc))
	{
		if(pt_Proc->pu32_Nodes)
		{
			oalready = e.o_Value;
			alreadydef = TRUE;
		}

		ERR_X_Error((int) pt_Proc->u32_ReturnType == RetType, "Return type different from prototype", "");

		already = TRUE;
		save_param = pt_Proc->u32_NumParameters;
		p_save_param = pt_Proc->pt_Parameters;
		pt_Proc->pt_Parameters = NULL;
		pt_Proc->u32_NumParameters = 0;
	}
	else
	{
		ERR_X_Error
		(
			!mo_ProcList.Lookup(e.o_Value, (void * &) pt_Proc),
			"Procedure already defined",
			(char *) (LPCSTR) e.o_Value
		);
	}

	/* Une proc en plus */
	if(!already)
	{
		pt_Proc = new SCR_sc_Procedure;
		pt_Proc->u32_NumParameters = 0;
		pt_Proc->pt_Parameters = NULL;
		mo_ProcList.SetAt(e.o_Value, pt_Proc);
		pt_Proc->pu32_Nodes = NULL;
		pt_Proc->pu32_DbgNodes = NULL;
		pt_Proc->pt_Parameters = NULL;
		L_strcpy(pt_Proc->az_Name, (char *) (LPCSTR) e.o_Value);
		pt_Proc->u32_ReturnType = RetType;
		pt_Proc->pst_LocalVar = NULL;
		pt_Proc->i_NumLocalVar = 0;
		pt_Proc->h_File = mul_CurrentFile;
		pt_Proc->i_Line = line;
		pt_Proc->ultra = ultra;
	}

	ResetVars(&mst_LocalVars);

	/* Parametres */
	Next();
	MatchString("(");
	while(e.c_Token != ')')
	{
		/* Type */
		byref = byrefarr = FALSE;
		ERR_X_Error(e.c_Token == TOKEN_TYPE, "Need a parameter type", "");
		if(e.i_Value == TYPE_BYREF)
		{
			byref = TRUE;
			Next();
			ERR_X_Error(e.c_Token == TOKEN_TYPE, "Need a parameter type", "");
		}
		else if(e.i_Value == TYPE_BYREFARR)
		{
			byrefarr = TRUE;
			Next();
			ERR_X_Error(e.c_Token == TOKEN_TYPE, "Need a parameter type", "");
		}

		pt_Proc->u32_NumParameters++;
		if(!pt_Proc->pt_Parameters)
			pt_Proc->pt_Parameters = (SCR_sc_ProcedureParam *) MEM_p_Alloc(sizeof(SCR_sc_ProcedureParam));
		else
		{
			pt_Proc->pt_Parameters = (SCR_sc_ProcedureParam *) MEM_p_Realloc
				(
					pt_Proc->pt_Parameters,
					pt_Proc->u32_NumParameters * sizeof(SCR_sc_ProcedureParam)
				);
		}

		pt_Param = pt_Proc->pt_Parameters + pt_Proc->u32_NumParameters - 1;
		pt_Param->u32_Type = e.i_Value;
		pt_Param->byref = byref ? 1 : byrefarr ? 2 : 0;

		/* Parametre */
		Next(FALSE);
		ERR_X_Error(e.c_Token == TOKEN_NAME, "Need a parameter name", "");
		L_strcpy(pt_Param->az_Name, (char *) (LPCSTR) e.o_Value);

		/* Variable déjà présente ? */
		ERR_X_Error
		(
			!mst_LocalVars.o_Vars.Lookup((char *) (LPCSTR) e.o_Value, (void * &) po_Var),
			"Variable already defined",
			(char *) (LPCSTR) e.o_Value
		);

		/* Nouvelle variable locale */
		po_Var = new EAI_cl_Variable(pt_Param->u32_Type, mst_LocalVars.i_LastPosVar);
		mst_LocalVars.o_Vars.SetAt((LPCSTR) e.o_Value, po_Var);
		po_Var->byref = byref;
		po_Var->byrefarr = byrefarr;
		if(byref || byrefarr)
			po_Var->TotalSize = 4;
		else
			po_Var->TotalSize = (EAI_gast_Types[AI_gaw_EnumLink[pt_Param->u32_Type]].i_Size);
		pt_Param->u32_Pos = mst_LocalVars.i_LastPosVar;
		pt_Param->u32_Size = po_Var->TotalSize;
		if(byref || byrefarr) pt_Param->u32_Size |= 0x80000000;
		mst_LocalVars.i_LastPosVar += po_Var->TotalSize;

		ERR_X_Error(!trigger || byref, "Trigger need a byref parameter", NULL);

		if(already)
		{
			ERR_X_Error
			(
				pt_Param->u32_Type == p_save_param[pt_Proc->u32_NumParameters - 1].u32_Type,
				"Function different from declaration",
				""
			);
			ERR_X_Error
			(
				pt_Param->u32_Size == p_save_param[pt_Proc->u32_NumParameters - 1].u32_Size,
				"Function different from declaration",
				""
			);
			ERR_X_Error
			(
				pt_Param->u32_Pos == p_save_param[pt_Proc->u32_NumParameters - 1].u32_Pos,
				"Function different from declaration",
				""
			);
		}

		/* Suite */
		Next();
		if(e.c_Token == ')') break;
		ERR_X_Error(e.c_Token == ',', "Need a comma", "");
		Next();
	}

	ERR_X_Error(!trigger || pt_Proc->u32_NumParameters == 1, "Bad number of parameters for trigger", NULL);
	ERR_X_Error
	(
		!trigger || pt_Proc->pt_Parameters[0].u32_Type == TYPE_MESSAGE,
		"Bad type for parameter of trigger",
		NULL
	);

	if(already)
	{
		ERR_X_Error((int) pt_Proc->u32_NumParameters == save_param, "Function different from declaration", "");
		MEM_Free(p_save_param);
	}

	/* Contenu */
	Next();

	/* Commentaire fin de ligne */
	L_strcpy(pt_Proc->az_Comment, (char *) (LPCSTR) e.o_LastComment);

	if(e.c_Token == ';')
	{
		ERR_X_Error(mb_ProcList || pt_Proc->ultra, "Can't define a procedure here", NULL);
		Next();
		ResetVars(&mst_LocalVars);
		return; /* Proto */
	}

	ERR_X_Error
	(
		!alreadydef,
		"Procedure already defined",
		(char *) (LPCSTR) oalready
	);

	ERR_X_Error(mb_ProcList, "Can't define a procedure here", NULL);
	pt_Proc->h_File = mul_CurrentFile;

	/* Depile les variables */
	e.i_CurrentOutput = 0;
	for(i = (int) pt_Proc->u32_NumParameters - 1; i >= 0; i--)
	{
		if(pt_Proc->pt_Parameters[i].u32_Size & 0x80000000)
		{
			pt_Proc->pt_Parameters[i].u32_Size &= ~0x80000000;
			Output(CATEG_POPPROCREF, pt_Proc->pt_Parameters[i].u32_Pos, pt_Proc->pt_Parameters[i].u32_Size);
		}
		else
		{
			Output(CATEG_POPPROC, pt_Proc->pt_Parameters[i].u32_Pos, pt_Proc->pt_Parameters[i].u32_Size);
		}
	}

	ERR_X_Error(e.c_Token == '{', "Need a begin statement", "");

	mb_ProcList = FALSE;

	mb_HasReturn = FALSE;
	mi_SpecReturn = pt_Proc->u32_ReturnType;
	Statement(FALSE);
	//if(!mb_HasReturn) 
	Output(CATEG_KEYWORD, 0, KEYWORD_OUTPROCSTACK);
	Output(CATEG_ENDTREE, 0, 0, 0);

	ERR_X_Error(mb_HasReturn || mi_SpecReturn == TYPE_VOID, "Need a return", "");

	mi_SpecReturn = 0;
	mb_ProcList = TRUE;

	/* Noeuds */
	pt_Proc->u16_SizeLocal = mst_LocalVars.i_LastPosVar;
	pt_Proc->u32_NumNodes = e.i_CurrentOutput;

	pt_Proc->pu32_Nodes = (AI_tdst_Node *) MEM_p_Alloc(e.i_CurrentOutput * sizeof(AI_tdst_Node));
	L_memcpy(pt_Proc->pu32_Nodes, e.pst_Output, e.i_CurrentOutput * sizeof(AI_tdst_Node));
	pt_Proc->pu32_DbgNodes = (AI_tdst_Node *) MEM_p_Alloc(e.i_CurrentOutput * sizeof(AI_tdst_Node));
	L_memcpy(pt_Proc->pu32_DbgNodes, e.pst_OtherOutput, e.i_CurrentOutput * sizeof(AI_tdst_Node));

	/* Local vars information */
	{
		/*~~~~~~~~~~~~~~~~~*/
		int			i, i_Num;
		POSITION	pos;
		CString		o_Name;
		/*~~~~~~~~~~~~~~~~~*/

		pt_Proc->i_NumLocalVar = mst_LocalVars.o_Vars.GetCount();
		i_Num = mst_LocalVars.o_Vars.GetCount() * sizeof(AI_tdst_Local);
		if(i_Num)
		{
			pt_Proc->pst_LocalVar = (AI_tdst_Local *) MEM_p_Alloc(i_Num);
			pos = mst_LocalVars.o_Vars.GetStartPosition();
			i = 0;
			while(pos)
			{
				mst_LocalVars.o_Vars.GetNextAssoc(pos, o_Name, (void * &) po_Var);
				if(po_Var->byref || po_Var->byrefarr) o_Name = "@" + o_Name;
				L_strcpy(pt_Proc->pst_LocalVar[i].asz_Name, (char *) (LPCSTR) o_Name);
				pt_Proc->pst_LocalVar[i].i_Offset = po_Var->mi_Pos;
				pt_Proc->pst_LocalVar[i].i_Type = po_Var->mi_Type;
				i++;
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoSwitch(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				i_Pos, i_Pos1, i_Type, i_First;
	int				i_Count;
	BOOL			mem, mem1;
	LyN_sc_Switch	*tt;
	POSITION		pos, pos1;
	int				poscase, valcase;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	mem = e.b_TopNode;
	e.b_TopNode = FALSE;
	mem1 = e.i_NumLineBreak;

	Next();
	MatchString("(");
	i_Type = BoolExpression();
	ERR_X_Error(i_Type != TYPE_VECTOR, "Incompatible types (vector not allowed)", NULL);
	ERR_X_Error(i_Type != TYPE_FLOAT, "Incompatible types (float not allowed)", NULL);
	MatchString(")");

	i_Pos = e.i_CurrentOutput;
	Output(CATEG_KEYWORD, 0, KEYWORD_JUMP);

	tt = new LyN_sc_Switch;
	tt->u32_PosDefault = -1;	/* Pas de default trouvé */
	mo_forWhile.AddTail(FALSE);
	mo_Switch.AddTail(tt);
	BeginBlock();

	/* Dernier jump pour sortir du switch */
	i_Pos1 = e.i_CurrentOutput;
	Output(CATEG_KEYWORD, 0, KEYWORD_JUMP);

	/* Jump du début jusqu'ici */
	e.pst_Output[i_Pos].l_Param = e.i_CurrentOutput - i_Pos;

	tt = mo_Switch.GetTail();
	if(tt->u32_PosDefault == -1)
	{
		/* Nbre de case + default */
		Output(CATEG_KEYWORD, tt->o_PosCase.GetCount() + 1, KEYWORD_SWITCH);
	}
	else
	{
		/* Nbre de case (default inclu) */
		Output(CATEG_KEYWORD, tt->o_PosCase.GetCount(), KEYWORD_SWITCH);
	}

	i_First = e.i_CurrentOutput;

	/* Default toujours en premier */
	if(tt->u32_PosDefault != -1)
	{
		poscase = tt->o_PosCase.GetAt(tt->o_PosCase.FindIndex(tt->u32_PosDefault));
		Output(0, 0, 0);
		Output(0, poscase - i_First, 0);
	}
	else
	{
		Output(0, 0, 0);
		Output(0, 0, 0);
	}

	/* Liste des valeurs des cases rencontrées (doublon valeur/deplacement) */
	pos = tt->o_ValCase.GetHeadPosition();
	pos1 = tt->o_PosCase.GetHeadPosition();
	i_Count = 0;
	while(pos)
	{
		valcase = tt->o_ValCase.GetNext(pos);
		poscase = tt->o_PosCase.GetNext(pos1);
		if(i_Count != tt->u32_PosDefault)
		{
			Output(0, valcase, 0);
			Output(0, poscase - i_First, 0);
		}

		i_Count++;
	}

	/* Fin du switch */
	e.pst_Output[i_Pos1].l_Param = e.i_CurrentOutput - i_Pos1;

	/* On init tous les break */
	pos = tt->o_PosBreak.GetHeadPosition();
	while(pos)
	{
		poscase = tt->o_PosBreak.GetNext(pos);
		e.pst_Output[poscase].l_Param = e.i_CurrentOutput - poscase;
	}

	delete tt;
	mo_Switch.RemoveTail();
	mo_forWhile.RemoveTail();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoContinue(void)
{
	ERR_X_Error(e.i_CurLoop, ERR_COMPILER_Csz_BreakWhile, NULL);
	Output(CATEG_KEYWORD, e.i_InLoopContinue[e.i_CurLoop - 1] - e.i_CurrentOutput, KEYWORD_JUMP);
	Next();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoBreak(void)
{
	/* Break dans un case */
	if(mo_Switch.GetCount() && !mo_forWhile.GetTail())
	{
		mo_Switch.GetTail()->o_PosBreak.AddTail(e.i_CurrentOutput);
		Output(CATEG_KEYWORD, 0, KEYWORD_JUMP);
		Next();
	}
	else
	{
		ERR_X_Error(e.i_CurLoop, ERR_COMPILER_Csz_BreakWhile, NULL);
		Output(CATEG_KEYWORD, e.i_InLoop[e.i_CurLoop - 1] - e.i_CurrentOutput, KEYWORD_JUMPFF);
		Next();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoCase(void)
{
	/*~~~~~~*/
	int i_Val;
    char *psz_String ;    
	/*~~~~~~*/

	ERR_X_Error(mo_Switch.GetCount(), "No switch defined", NULL);
	Next();
	ERR_X_Error(e.c_Token == TOKEN_NUMBER, "Attempt to find a number", NULL);
    
    if(e.i_Value == TYPE_BINARY) 
    {
        i_Val = 0;
		psz_String = (char *) (LPCSTR) e.o_Value;
		do
		{
			if(*psz_String++ == '1') i_Val ++;
			if(*psz_String) i_Val <<= 1;
		} while(*psz_String);
    }
    else
    {
	    ERR_X_Error(e.i_Value == TYPE_INT, "Attempt to find an integer value", NULL);
	    i_Val = L_atoi(e.o_Value);
    }
	ERR_X_Error(!mo_Switch.GetTail()->o_ValCase.Find(i_Val), "Label already defined", NULL);
	mo_Switch.GetTail()->o_PosCase.AddTail(e.i_CurrentOutput);
	mo_Switch.GetTail()->o_ValCase.AddTail(i_Val);
	Next();
	MatchString(":");
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoDefault(void)
{
	/*~~~~~~~~~~~~~~~~*/
	LyN_sc_Switch	*tt;
	/*~~~~~~~~~~~~~~~~*/

	ERR_X_Error(mo_Switch.GetCount(), "No switch defined", NULL);
	tt = mo_Switch.GetTail();
	ERR_X_Error(tt->u32_PosDefault == -1, "Default defined twice", NULL);
	tt->u32_PosDefault = tt->o_PosCase.GetCount();
	mo_Switch.GetTail()->o_PosCase.AddTail(e.i_CurrentOutput);
	mo_Switch.GetTail()->o_ValCase.AddTail(0);
	Next();
}

/*
 =======================================================================================================================
    Aim: Compile an IF. £
    <Expression to test> £
    JUMPFALSESP Label1 £
    <Content of IF> £
    JUMP Label2 £
    Optionnal (if else) £
    Label1: £
    <Else content> Optionnal (if else) £
    Label2: Optionnal (if else)
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoIf(void)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	int		i_Pos;
	int		i_Type;
	BOOL	mem, mem1, mem2;
	/*~~~~~~~~~~~~~~~~~~~~*/

	mem = e.b_TopNode;
	e.b_TopNode = FALSE;
	mem1 = e.i_NumLineBreak;

	Next();
	MatchString("(");
	i_Type = BoolExpression();
	ERR_X_Error(i_Type != TYPE_VECTOR, "Incompatible types", NULL);
	MatchString(")");

	e.b_TopNode = mem;
	mem2 = e.i_NumLineBreak;
	e.i_NumLineBreak = mem1;
	i_Pos = e.i_CurrentOutput;
	Output(CATEG_KEYWORD, 0, KEYWORD_JUMPFALSESP);

	e.i_NumLineBreak = mem2;

	BeginBlock();
	e.pst_Output[i_Pos].l_Param = e.i_CurrentOutput - i_Pos;

	/* Treat an eventual else */
	if((e.c_Token == TOKEN_KEYWORD) && (e.i_Value == KEYWORD_ELSE))
	{
		e.pst_Output[i_Pos].l_Param++;	/* Cause of jump */
		Output(CATEG_KEYWORD, 0, KEYWORD_JUMP);

		i_Pos = e.i_CurrentOutput - 1;
		Next();
		BeginBlock();
		e.pst_Output[i_Pos].l_Param = e.i_CurrentOutput - i_Pos;
	}
}

/*
 =======================================================================================================================
    Aim: Compile a while. £
    Label1: £
    <Expression to test> £
    JUMPFALSESP Label2 £
    <Content of WHILE> £
    JUMP Label1 £
    Label2:
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoWhile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int i_PosBefore, i_PosJump;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Save pos for label1 (before expression to test) */
	i_PosBefore = e.i_CurrentOutput;
	e.i_InLoopContinue[e.i_CurLoop] = i_PosBefore;

	/* Parse expression */
	Next();
	MatchString("(");
	BoolExpression();
	MatchString(")");

	i_PosJump = e.i_CurrentOutput;
	Output(CATEG_KEYWORD, 0, KEYWORD_JUMPFALSESP);

	/* Content of while */
	mo_forWhile.AddTail(TRUE);
	e.i_InLoop[e.i_CurLoop++] = i_PosJump;
	BeginBlock();
	Output(CATEG_KEYWORD, i_PosBefore - e.i_CurrentOutput, KEYWORD_JUMP);
	e.pst_Output[i_PosJump].l_Param = e.i_CurrentOutput - i_PosJump;
	e.i_CurLoop--;
	mo_forWhile.RemoveTail();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoFor(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int i_Pos1, i_Pos2, i_Pos3, i_Pos4;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Parse expression */
	Next();
	MatchString("(");
	Statement();
	MatchString(";");

	i_Pos1 = e.i_CurrentOutput;
	BoolExpression();
	MatchString(";");

	i_Pos2 = e.i_CurrentOutput;
	Output(CATEG_KEYWORD, 0, KEYWORD_JUMPFALSESP);

	i_Pos3 = e.i_CurrentOutput;
	Output(CATEG_KEYWORD, 0, KEYWORD_JUMP);

	i_Pos4 = e.i_CurrentOutput;
	e.i_InLoopContinue[e.i_CurLoop] = i_Pos4;

	gi_PPAllPVCant = 1; /* To avoid ';' mark and the for statement */
	Statement();
	gi_PPAllPVCant = 0;

	MatchString(")");
	Output(CATEG_KEYWORD, i_Pos1 - e.i_CurrentOutput, KEYWORD_JUMP);

	/* Content of while */
	mo_forWhile.AddTail(TRUE);
	e.i_InLoop[e.i_CurLoop++] = i_Pos2;
	e.pst_Output[i_Pos3].l_Param = e.i_CurrentOutput - i_Pos3;
	BeginBlock();
	Output(CATEG_KEYWORD, i_Pos4 - e.i_CurrentOutput, KEYWORD_JUMP);

	e.pst_Output[i_Pos2].l_Param = e.i_CurrentOutput - i_Pos2;
	e.i_CurLoop--;
	mo_forWhile.RemoveTail();
}

/*
 =======================================================================================================================
    Aim: Compile a metaaction. £
    Label1: £
    <Expression to test> £
    JUMPFALSESP Label2 £
    <Content of WHILE> £
    META Label1 £
    Label2:
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoMeta(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~*/
	int i_PosBefore, i_PosJump;
	/*~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Save pos for label1 (before expression to test) */
	i_PosBefore = e.i_CurrentOutput;
	e.i_InLoopContinue[e.i_CurLoop] = i_PosBefore;

	/* Parse expression */
	Next();
	MatchString("(");
	BoolExpression();
	MatchString(")");
	ERR_X_Error(e.c_Token == '{', "You must specify a begin block statement { for the meta keyword", 0);

	i_PosJump = e.i_CurrentOutput;
	Output(CATEG_KEYWORD, 0, KEYWORD_JUMPFALSESP);

	/* Content of meta */
	mo_forWhile.AddTail(TRUE);
	e.i_InLoop[e.i_CurLoop++] = i_PosJump;
	BeginBlock();
	Output(CATEG_KEYWORD, i_PosBefore - e.i_CurrentOutput, KEYWORD_META);
	e.pst_Output[i_PosJump].l_Param = e.i_CurrentOutput - i_PosJump;
	e.i_CurLoop--;
	mo_forWhile.RemoveTail();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::ResolveLabels(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	EAI_tdst_Label	t, t1;
	POSITION		pos, pos1;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mo_ResLabels.GetHeadPosition();
	while(pos)
	{
		t = mo_ResLabels.GetNext(pos);
		pos1 = x_SearchLabel(t.o_Name);
		ERR_X_Error(pos1, "Unknown label", 0);
		t1 = mo_AllLabels.GetAt(pos1);

		e.pst_Output[t.i_Pos].l_Param = t1.i_Pos - t.i_Pos;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoGoto(void)
{
	/*~~~~~~~~~~~~~~*/
	EAI_tdst_Label	t;
	/*~~~~~~~~~~~~~~*/

	Next(FALSE);
	ERR_X_Error(e.c_Token == TOKEN_NAME, "Invalid goto", 0);
	t.o_Name = e.o_Value + ":";
	t.i_Pos = e.i_CurrentOutput;
	mo_ResLabels.AddTail(t);
	Output(CATEG_KEYWORD, 0, KEYWORD_JUMP);
	Next();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoAffect(void)
{
	/*~~~~~~~~*/
	int i_Type1;
	/*~~~~~~~~*/

	mi_RequestedType = i_Type1 = DoVariable();
	DoAffectCom(i_Type1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoAffectCom(int i_Type1)
{
	/*~~~~~~~~~~~~*/
	int		i_Type2;
	char	c_Mem;
	char	c_Next;
	CString o_Mem;
	/*~~~~~~~~~~~~*/

	c_Mem = e.c_Token;
	o_Mem = e.o_Value;
	c_Next = e.c_Look;
	Next();

	switch(c_Mem)
	{

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case '=':
		i_Type2 = BoolExpression();
		CompatibleTypes(i_Type1, i_Type2);

		if(i_Type1 == TYPE_TEXT && i_Type2 == TYPE_INT)
		{
			ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_BadTypes, NULL);
			return;
		}

		if((i_Type1 == TYPE_VECTOR) || (i_Type2 == TYPE_VECTOR))		/* Vector oper */
		{
			Output(CATEG_KEYWORD, 0, KEYWORD_VECAFFECT);
			break;
		}

		if((i_Type1 == TYPE_MESSAGE) || (i_Type2 == TYPE_MESSAGE))		/* Message oper */
		{
			Output(CATEG_KEYWORD, 0, KEYWORD_MSGAFFECT);
			break;
		}

		if((i_Type1 == TYPE_TEXT) || (i_Type2 == TYPE_TEXT))			/* Text oper */
		{
			Output(CATEG_KEYWORD, 0, KEYWORD_TEXTAFFECT);
			break;
		}

		if((i_Type1 == TYPE_MESSAGEID) || (i_Type2 == TYPE_MESSAGEID))	/* MsgId oper */
		{
			Output(CATEG_KEYWORD, 0, KEYWORD_MSGIDAFFECT);
			break;
		}

		if((i_Type1 == TYPE_TRIGGER) || (i_Type2 == TYPE_TRIGGER))		/* Trigger oper */
		{
			Output(CATEG_KEYWORD, 0, KEYWORD_TRIGGERAFFECT);
			break;
		}

		Output(CATEG_KEYWORD, 0, KEYWORD_AFFECT);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case '<':
		if(o_Mem.GetLength() == 2)
		{
			if(e.c_Token == '=')
			{
				if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
				Next();
				i_Type2 = BoolExpression();
				CompatibleTypes(i_Type1, i_Type2);
				AssertIntType(i_Type2);
				Output(CATEG_KEYWORD, 0, KEYWORD_SLEFTAFFECT);
				break;
			}
		}

		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case '>':
		if(o_Mem.GetLength() == 2)
		{
			if(e.c_Token == '=')
			{
				if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
				Next();
				i_Type2 = BoolExpression();
				CompatibleTypes(i_Type1, i_Type2);
				AssertIntType(i_Type2);
				Output(CATEG_KEYWORD, 0, KEYWORD_SRIGHTAFFECT);
				break;
			}
		}

		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case '|':
		if(e.c_Token == '=')
		{
			if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			Next();
			i_Type2 = BoolExpression();
			CompatibleTypes(i_Type1, i_Type2);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_ORAFFECT);
			break;
		}

		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case '&':
		if(e.c_Token == '=')
		{
			if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			Next();
			i_Type2 = BoolExpression();
			CompatibleTypes(i_Type1, i_Type2);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_ANDAFFECT);
			break;
		}

		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case '^':
		if(e.c_Token == '=')
		{
			if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			Next();
			i_Type2 = BoolExpression();
			CompatibleTypes(i_Type1, i_Type2);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_XORAFFECT);
			break;
		}

		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case '+':
		if(e.c_Token == '=')
		{
			if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			Next();
			i_Type2 = BoolExpression();
			CompatibleTypes(i_Type1, i_Type2);

			if((i_Type1 == TYPE_VECTOR) || (i_Type2 == TYPE_VECTOR))	/* Vector oper */
			{
				Output(CATEG_KEYWORD, 0, KEYWORD_VECPLUSAFFECT);
				break;
			}

			Output(CATEG_KEYWORD, 0, KEYWORD_PLUSAFFECT);
			break;
		}
		else if(e.c_Token == '+')
		{
			if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			AssertIntType(i_Type1);
			Output(CATEG_KEYWORD, 0, KEYWORD_INCAFFECT);
			i_Type2 = TYPE_INT;
			Next();
			break;
		}

		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case '-':
		if(e.c_Token == '=')
		{
			if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			Next();
			i_Type2 = BoolExpression();
			CompatibleTypes(i_Type1, i_Type2);

			if((i_Type1 == TYPE_VECTOR) || (i_Type2 == TYPE_VECTOR))	/* Vector oper */
			{
				Output(CATEG_KEYWORD, 0, KEYWORD_VECMINUSAFFECT);
				break;
			}

			Output(CATEG_KEYWORD, 0, KEYWORD_MINUSAFFECT);
			break;
		}
		else if(e.c_Token == '-')
		{
			if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			AssertIntType(i_Type1);
			Output(CATEG_KEYWORD, 0, KEYWORD_DECAFFECT);
			i_Type2 = TYPE_INT;
			Next();
			break;
		}

		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case '*':
		if(e.c_Token == '=')
		{
			if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			Next();
			i_Type2 = BoolExpression();

			if((i_Type1 == TYPE_VECTOR) || (i_Type2 == TYPE_VECTOR))	/* Vector oper */
			{
				AssertIntType(i_Type2);
				Output(CATEG_KEYWORD, 0, KEYWORD_VECMULAFFECT);
				return;
			}

			CompatibleTypes(i_Type1, i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_MULAFFECT);
			break;
		}

		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	case '/':
		if(e.c_Token == '=')
		{
			if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			Next();
			i_Type2 = BoolExpression();

			if((i_Type1 == TYPE_VECTOR) || (i_Type2 == TYPE_VECTOR))	/* Vector oper */
			{
				AssertIntType(i_Type2);
				Output(CATEG_KEYWORD, 0, KEYWORD_VECDIVAFFECT);
				return;
			}

			CompatibleTypes(i_Type1, i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_DIVAFFECT);
			break;
		}

		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	default:
		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;
	}

	CompatibleTypes(i_Type1, i_Type2);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::Factor(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	int			i_Type;
	BIG_INDEX	ul_Model;
	/*~~~~~~~~~~~~~~~~~*/

	if(e.c_Token == '!')
	{
		Next();
		i_Type = Factor();
		if(i_Type == TYPE_VECTOR)
		{
			ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_BadTypes, NULL);
		}
		else if(i_Type == TYPE_GAMEOBJECT)
		{
			Output(CATEG_KEYWORD, 0, KEYWORD_NOTOBJ);
			i_Type = TYPE_INT;
		}
		else
		{
			Output(CATEG_KEYWORD, 0, KEYWORD_NOT);
		}
	}
	else if(e.c_Token == '-')
	{
		Next();
		i_Type = Factor();
		if(i_Type == TYPE_VECTOR)
			Output(CATEG_KEYWORD, 0, KEYWORD_VECNEG);
		else
			Output(CATEG_KEYWORD, 0, KEYWORD_NEG);
	}
	else if(e.c_Token == '~')
	{
		Next();
		i_Type = Factor();
		AssertIntType(i_Type);
		Output(CATEG_KEYWORD, 0, KEYWORD_INVERT);
	}
	else if(e.c_Token == '(')
	{
		Next();
		i_Type = BoolExpression();
		MatchString(")");
	}
	else
	{
		switch(e.c_Token)
		{
		case TOKEN_ULTRA:
			GetUltra();
			return Factor();
			break;

		case TOKEN_FUNCTION:
			i_Type = DoCall();
			break;

		case TOKEN_NUMBER:
			i_Type = DoNumber();
			break;

		case TOKEN_STRING:
			i_Type = DoString();
			break;

		case TOKEN_LOCALVAR:
		case TOKEN_GLOBALVAR:
			i_Type = DoVariable();
			break;

		case TOKEN_KEYWORD:
			if(e.i_Value == KEYWORD_POP)
			{
				switch(mi_RequestedType)
				{
				case TYPE_INT:
				case TYPE_FLOAT:
				case TYPE_GAMEOBJECT:
				case TYPE_VECTOR:
				case TYPE_MESSAGEID:
					break;
				default:
					ERR_X_ForceErrorThrow("Invalid type for pop", NULL);
					break;
				}

				if(mb_GenPP) go_PPAllPop.SetAt((void *) gi_PPAllPVBeforeNext, (void *) mi_RequestedType);
				i_Type = DoVariable();
				break;
			}

			if(e.i_Value == KEYWORD_CALLTRIGGER)
			{
				i_Type = DoTriggerCall();
				break;
			}

			ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, NULL);
			break;

		case TOKEN_AIFUNCTION:
			Output(CATEG_TYPE, e.i_Value, TYPE_FUNCTIONREF);
			Next();
			return TYPE_FUNCTIONREF;

		case TOKEN_GAMEOBJECT:
			Output(CATEG_TYPE, e.i_Value, TYPE_GAMEOBJECT);
			Next();
			return TYPE_GAMEOBJECT;

		case TOKEN_NETWORK:
			Output(CATEG_TYPE, e.i_Value, TYPE_NETWORK);
			Next();
			return TYPE_NETWORK;

		case TOKEN_MODEL:
			ul_Model = ul_GetEngineModelIndex(e.i_Value, NULL, NULL);
			ERR_X_Assert(ul_Model != BIG_C_InvalidIndex);
			Output(CATEG_TYPE, BIG_FileKey(ul_Model), TYPE_MODEL);
			Next();
			return TYPE_MODEL;

		case TOKEN_NAME:
			ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_UnkownID, (char *) (LPCSTR) e.o_Value);
			break;

		case TOKEN_CALLPROC:
			i_Type = DoProcCall();
			break;

		default:
			ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, NULL);
			break;
		}
	}

	return i_Type;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::Expression(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	char	c_Mem;
	int		i_Type1, i_Type2;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	i_Type1 = Factor();

	while
	(
		(e.c_Token == '-')
	||	(e.c_Token == '+')
	||	(e.c_Token == '*')
	||	(e.c_Token == '/')
	||	((e.c_Token == '|') && (e.o_Value.GetLength() == 1))
	||	((e.c_Token == '&') && (e.o_Value.GetLength() == 1))
	||	(e.c_Token == '^')
	||	((e.c_Token == '>') && (e.o_Value.GetLength() == 2))
	||	((e.c_Token == '<') && (e.o_Value.GetLength() == 2))
	)
	{
		c_Mem = e.c_Token;
		Next();
		i_Type2 = Factor();

		switch(c_Mem)
		{

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case '<':
			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_SLEFT);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case '>':
			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_SRIGHT);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case '|':
			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_OROP);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case '&':
			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_ANDOP);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case '^':
			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_XOROP);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case '-':
			CompatibleTypes(i_Type1, i_Type2);
			if((i_Type1 == TYPE_VECTOR) || (i_Type2 == TYPE_VECTOR))	/* Vector oper */
			{
				Output(CATEG_KEYWORD, 0, KEYWORD_VECMINUS);
				i_Type1 = i_Type2 = TYPE_VECTOR;
				break;
			}

			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_MINUS);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case '+':
			CompatibleTypes(i_Type1, i_Type2);
			if((i_Type1 == TYPE_VECTOR) || (i_Type2 == TYPE_VECTOR))	/* Vector oper */
			{
				Output(CATEG_KEYWORD, 0, KEYWORD_VECPLUS);
				i_Type1 = i_Type2 = TYPE_VECTOR;
				break;
			}

			if(i_Type1 == TYPE_TEXT)	/* text index decalage */
			{
				Output(CATEG_KEYWORD, 0, KEYWORD_TEXTPLUS);
				i_Type1 = i_Type2 = TYPE_TEXT;
				break;
			}

			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_PLUS);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case '*':
			if((i_Type1 == TYPE_VECTOR) || (i_Type2 == TYPE_VECTOR))	/* Vector oper */
			{
				if(i_Type1 == TYPE_VECTOR)
				{
					AssertIntType(i_Type2);
					Output(CATEG_KEYWORD, 0, KEYWORD_VECMUL);
				}
				else
				{
					AssertIntType(i_Type1);
					Output(CATEG_KEYWORD, 0, KEYWORD_VECMUL2);
				}

				i_Type1 = i_Type2 = TYPE_VECTOR;
				break;
			}

			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_MUL);
			break;

		/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		case '/':
			if(i_Type1 == TYPE_VECTOR)
			{
				AssertIntType(i_Type2);
				Output(CATEG_KEYWORD, 0, KEYWORD_VECDIV);
				i_Type1 = i_Type2 = TYPE_VECTOR;
				break;
			}

			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			Output(CATEG_KEYWORD, 0, KEYWORD_DIV);
			break;
		}

		/* Check if the 2 types are compatible */
		i_Type1 = CompatibleTypes(i_Type1, i_Type2);
	}

	return i_Type1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::Relation(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	char	c_Mem, c_Next;
	int		i_Type1, i_Type2;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	i_Type1 = Expression();
	while((e.c_Token == '=') || (e.c_Token == '>') || (e.c_Token == '<') || (e.c_Token == '!'))
	{
		c_Mem = e.c_Token;
		c_Next = e.c_Look;
		Next();

		switch(c_Mem)
		{
		case '=':
			if(e.c_Token == '=')
			{
				if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
				c_Mem = '=';
				Next();
			}
			else
			{
				ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			}

			i_Type2 = Expression();
			CompatibleTypes(i_Type1, i_Type2);
			if((i_Type1 == TYPE_VECTOR) || (i_Type2 == TYPE_VECTOR))	/* Vector oper */
				Output(CATEG_KEYWORD, 0, KEYWORD_VECEQUAL);
			else if(i_Type1 == TYPE_GAMEOBJECT || i_Type2 == TYPE_GAMEOBJECT)
			{
				i_Type1 = TYPE_INT;
				Output(CATEG_KEYWORD, 0, KEYWORD_OBJEQUAL);
			}
			else
				Output(CATEG_KEYWORD, 0, KEYWORD_EQUAL);
			break;

		case '!':
			if(e.c_Token == '=')
			{
				if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
				c_Mem = '=';
				Next();
			}
			else
			{
				ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			}

			i_Type2 = Expression();
			CompatibleTypes(i_Type1, i_Type2);
			if((i_Type1 == TYPE_VECTOR) || (i_Type2 == TYPE_VECTOR))	/* Vector oper */
				Output(CATEG_KEYWORD, 0, KEYWORD_VECDIFFERENT);
			else if(i_Type1 == TYPE_GAMEOBJECT || i_Type2 == TYPE_GAMEOBJECT)
			{
				i_Type1 = TYPE_INT;
				Output(CATEG_KEYWORD, 0, KEYWORD_OBJDIFFERENT);
			}
			else
				Output(CATEG_KEYWORD, 0, KEYWORD_DIFFERENT);
			break;

		case '>':
			if(e.c_Token == '=')
			{
				if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
				c_Mem = '=';
				Next();
			}

			i_Type2 = Expression();
			CompatibleTypes(i_Type1, i_Type2);
			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			if(c_Mem == '=')
			{
				Output(CATEG_KEYWORD, 0, KEYWORD_GREATEQ);
			}
			else
			{
				Output(CATEG_KEYWORD, 0, KEYWORD_GREAT);
			}
			break;

		case '<':
			if(e.c_Token == '=')
			{
				if(c_Next != e.c_Token) ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
				c_Mem = '=';
				Next();
			}

			i_Type2 = Expression();
			CompatibleTypes(i_Type1, i_Type2);
			AssertIntType(i_Type1);
			AssertIntType(i_Type2);
			if(c_Mem == '=')
				Output(CATEG_KEYWORD, 0, KEYWORD_LESSEQ);
			else
				Output(CATEG_KEYWORD, 0, KEYWORD_LESS);

			break;
		}

		return TYPE_BOOL;
	}

	return i_Type1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::BoolExpression(void)
{
	/*~~~~~~~~~~~*/
	char	c_Mem;
	int		i_Pos;
	int		i_Type;
	/*~~~~~~~~~~~*/

	i_Type = Relation();
	while((e.c_Token == '&') || (e.c_Token == '|'))
	{
		c_Mem = e.c_Token;

		switch(c_Mem)
		{
		case '&':
			ERR_X_Error(e.o_Value.GetLength() == 2, ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			Next();

			/* MatchString("&"); */
			i_Pos = e.i_CurrentOutput;
			Output(CATEG_KEYWORD, 0, KEYWORD_JUMPFALSE);

			Relation();

			Output(CATEG_KEYWORD, 0, KEYWORD_AND);
			e.pst_Output[i_Pos].l_Param = e.i_CurrentOutput - i_Pos;

			break;

		case '|':
			ERR_X_Error(e.o_Value.GetLength() == 2, ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			Next();

			/* MatchString("|"); */
			i_Pos = e.i_CurrentOutput;
			Output(CATEG_KEYWORD, 0, KEYWORD_JUMPTRUE);

			Relation();

			Output(CATEG_KEYWORD, 0, KEYWORD_OR);
			e.pst_Output[i_Pos].l_Param = e.i_CurrentOutput - i_Pos;
			break;
		}

		i_Type = TYPE_BOOL;
	}

	return i_Type;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
POSITION EAI_cl_Compiler::x_SearchLabel(CString &name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos, ppos;
	EAI_tdst_Label	t;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	pos = mo_AllLabels.GetHeadPosition();
	while(pos)
	{
		ppos = pos;
		t = mo_AllLabels.GetNext(pos);
		if(!L_strcmpi((char *) (LPCSTR) t.o_Name, (char *) (LPCSTR) name)) return ppos;
	}

	return NULL;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::Statement(BOOL remstmt)
{
	/*~~~~~~~*/
	int				i_Type;
	EAI_tdst_Vars	cpy;
	CString			o_Name;
	POSITION		pos;
	EAI_cl_Variable *po_Var;
	/*~~~~~~~*/

	if(!e.c_Token) return;

	/* Can't define code in a var file (but we can in a lib one) */
	if(e.c_Token != TOKEN_KEYWORD || e.i_Value != KEYWORD_PROCEDUREULTRA)
		ERR_X_Error(!mb_GlobalVarMode, ERR_COMPILER_Csz_VarModeDecl, NULL);

	/* Is there a breakpoint at that line ? */
	if(b_IsBreakPointAtLine(e.i_NumLineBreak)) mb_NeedBreak = TRUE;
	e.b_TopNode = TRUE;

	switch(e.c_Token)
	{
	case TOKEN_ULTRA:
		i_Type = GetUltra();
		Statement();
		break;

	case TOKEN_KEYWORD:
		switch(e.i_Value)
		{
		case KEYWORD_PROCEDUREULTRA:
			if(mb_GlobalVarMode && mst_LocalVars.o_Vars.GetStartPosition())
			{
				cpy.i_LastPosVar = mst_LocalVars.i_LastPosVar;
				pos = mst_LocalVars.o_Vars.GetStartPosition();
				while(pos)
				{
					mst_LocalVars.o_Vars.GetNextAssoc(pos, o_Name, (void * &) po_Var);
					cpy.o_Vars.SetAt(o_Name, po_Var);
				}

				mst_LocalVars.i_LastPosVar = 0;
				mst_LocalVars.o_Vars.RemoveAll();
			}

			DoProcedure(0, 1, 1);

			if(mb_GlobalVarMode && cpy.o_Vars.GetStartPosition())
			{
				mst_LocalVars.i_LastPosVar = cpy.i_LastPosVar;
				pos = cpy.o_Vars.GetStartPosition();
				while(pos)
				{
					cpy.o_Vars.GetNextAssoc(pos, o_Name, (void * &) po_Var);
					mst_LocalVars.o_Vars.SetAt(o_Name, po_Var);
				}
			}
			break;
		case KEYWORD_PROCEDURELOCAL:
			DoProcedure(0, 1);
			break;
		case KEYWORD_PROCEDURE:
			DoProcedure(0, 0);
			break;
		case KEYWORD_PROCTRIGGER:
			ERR_X_Error(mb_ProcList, "Can't define a trigger here", NULL);
			DoProcedure(1, 0);
			break;
		case KEYWORD_SWITCH:
			DoSwitch();
			break;
		case KEYWORD_CASE:
			DoCase();
			break;
		case KEYWORD_DEFAULT:
			DoDefault();
			break;
		case KEYWORD_IF:
			DoIf();
			break;
		case KEYWORD_WHILE:
			DoWhile();
			break;
		case KEYWORD_FOR:
			DoFor();
			break;
		case KEYWORD_META:
			DoMeta();
			break;
		case KEYWORD_PUSH:
			DoPush();
			break;
		case KEYWORD_RETURN:
			Next();
			if(mi_SpecReturn)
			{
				mb_HasReturn = TRUE;
				mi_RequestedType = mi_SpecReturn;
				Output(CATEG_KEYWORD, 0, KEYWORD_OUTPROCSTACK);
				if(mi_SpecReturn != TYPE_VOID) 
				{
					i_Type = BoolExpression();
					CompatibleTypes(i_Type, mi_SpecReturn);
				}
			}

			Output(CATEG_KEYWORD, 0, KEYWORD_RETURN);
			break;
		case KEYWORD_RETURNTRACK:
			Output(CATEG_KEYWORD, 0, KEYWORD_RETURNTRACK);
			Next();
			break;
		case KEYWORD_STOP:
			Output(CATEG_KEYWORD, 0, KEYWORD_STOP);
			Next();
			break;
		case KEYWORD_BREAK:
			DoBreak();
			break;
		case KEYWORD_CONTINUE:
			DoContinue();
			break;
		case KEYWORD_GOTO:
			DoGoto();
			break;
		case KEYWORD_CALLTRIGGER:
			DoTriggerCall();
			break;
		default:
			ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
			break;
		}
		break;

	case TOKEN_CALLPROC:
		DoProcCall();
		break;

	case TOKEN_FUNCTION:
		i_Type = DoCall();
		if(e.c_Token == '=') DoAffectCom(i_Type);
		break;

	case TOKEN_LOCALVAR:
	case TOKEN_GLOBALVAR:
		DoAffect();
		break;

	case TOKEN_NAME:
		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_UnkownID, (char *) (LPCSTR) e.o_Value);
		break;

	case TOKEN_PP:
		Next();
		break;

	case '{':
		BeginBlock(remstmt);
		break;

	case TOKEN_LABEL:
		{
			/*~~~~~~~~~~~~~~*/
			EAI_tdst_Label	t;
			/*~~~~~~~~~~~~~~*/

			ERR_X_Error(!x_SearchLabel(e.o_Value), "Label already defined", NULL);
			t.o_Name = e.o_Value;
			t.i_Pos = e.i_CurrentOutput;
			mo_AllLabels.AddTail(t);
			Next();
		}
		break;

	default:
		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;
	}

	MGP();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::BeginBlock(BOOL remstmt)
{
	/*~~~~~~~~~~~*/
	int i_MemoType;
	/*~~~~~~~~~~~*/

	if(e.c_Token == '{')
	{
		e.StmtLevel++;

		MatchString("{");

		/* Detect a type declaration */
		while(e.c_Token == TOKEN_TYPE)
		{
			i_MemoType = e.i_Value;
			AddVariable(i_MemoType);
			while(e.c_Token == ',') AddVariable(i_MemoType);
			MGP();
		}

		while(e.c_Token != '}' && e.c_Token)
		{
			Statement();
		}

		MatchString("}");

		if(remstmt) RemoveVarStmt(e.StmtLevel);
		e.StmtLevel--;
	}
	else if(e.c_Token)
	{
		Statement();
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::TopDecls(void)
{
	/*~~~~~~~~~~~*/
	int i_MemoType;
	/*~~~~~~~~~~~*/

	while(e.c_Token)
	{
		/* Detect a type declaration */
		while(e.c_Token == TOKEN_TYPE)
		{
			i_MemoType = e.i_Value;
			AddVariable(i_MemoType);
			while(e.c_Token == ',') AddVariable(i_MemoType);
			MGP();
		}

		if(mb_GenPP && gi_FirstCode == -1) 
		{
			gi_FirstCode = e.psz_Before - e.psz_Buf;
			if(!L_isspace(*e.psz_Before)) gi_FirstCode--;
		}

		Statement();
	}
}

/*$4
 ***********************************************************************************************************************
    Main function
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
    Aim: Main function to compile a text. In: _ul_Model File index of model (can be BIG_C_InvalidIndex). _ul_File File
    index of file to compile. _psz_Text Address of the buffer to compile.
 =======================================================================================================================
 */
int EAI_cl_Compiler::i_Compile(BIG_INDEX _ul_Model, BIG_INDEX _ul_File, char *_psz_Text)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	AI_tdst_Function	st_Func;
	char				asz_Name[BIG_C_MaxLenPath];
	ULONG				ul_Num, ul_Num1;
	EAI_cl_Variable		*po_Var;
	CString				str, str1;
	POSITION			pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifdef BENCH_IA_TEST
	if(_ul_Model != BIG_C_InvalidIndex)
	{
		AIBnch_AddFunction(BIG_FileKey(_ul_Model), BIG_FileKey(_ul_File));
	}
#endif

	/* Name of function for completion */
	L_strcpy(asz_Name, BIG_NameFile(_ul_File));
	*L_strrchr(asz_Name, '.') = 0;
	go_PPFunctions.SetAt(asz_Name, 0);

	ResetNoVars();

	/* Load file */
	mpsz_Text = _psz_Text;

	/* Init parse struct */
	e.i_CurLoop = 0;
	mo_forWhile.RemoveAll();
	e.i_Error = -1;
	e.i_NumLine = 0;
	e.i_NumLineBreak = 0;
	e.psz_Buf = e.psz_Text = e.psz_Before = _psz_Text;
	e.pst_Output = NULL;
	e.pst_OtherOutput = NULL;
	e.pst_LocalVars = NULL;
	e.i_CurrentOutput = e.i_MaxOutput = 0;
	e.i_NumStringBuf = e.i_MaxStringBuf = 0;
	e.StmtLevel = 0;
	mo_AllLabels.RemoveAll();
	mo_ResLabels.RemoveAll();

	/* Compute name of current file */
	e.ul_Model = _ul_Model;
	e.ul_File = _ul_File;
	BIG_ComputeFullName(BIG_ParentFile(_ul_File), e.asz_OrgPath);
	L_strcpy(e.asz_OrgFile, BIG_NameFile(_ul_File));
	L_strcpy(e.asz_Path, e.asz_OrgPath);
	L_strcpy(e.asz_File, e.asz_OrgFile);

	_Try_	
	PP();

	/* Generate PP file */
	if(mb_GenPP || mb_ForceGenPP)
	{
		/*~~~~~~~~~~~~~~*/
		char	name[512];
		char	path[512];
		/*~~~~~~~~~~~~~~*/

		L_strcpy(name, BIG_NameFile(_ul_File));
		L_strcat(name, ".pp");
		BIG_ComputeFullName(BIG_ParentFile(_ul_File), path);
		SAV_Begin(path, name);
		SAV_Buffer(e.psz_Buf, strlen(e.psz_Buf));
		SAV_ul_End();
	}

	if(mb_GenPP)
	{
		go_PPAllPV.RemoveAll();
		go_PPAllRef.RemoveAll();
		go_PPAllPop.RemoveAll();
		go_PPReplace.RemoveAll();
		gi_PPAllPV = 0;
		gi_PPAllPVCant = 0;
		gi_PPAllPVBeforeNext = 0;
		gi_FirstCode = -1;
	}

	GetChar();
	Next();
	TopDecls();

	/*
	 * Mark end of tree. This node is useful for interpreter to detect the end of the
	 * evaluation
	 */
	if(!mb_GlobalVarMode) Output(CATEG_ENDTREE, 0, 0);

	/* Optimisation pass */
	ul_Num1 = -1;
	do
	{
		ul_Num = mul_NbDelNodes;
		Optim();
		if(ul_Num1 == -1) ul_Num1 = mul_NbNodes;
	} while(ul_Num != mul_NbDelNodes);
	mul_NbNodes = ul_Num1;

	if(mb_ProcList)
	{
		ResolveLabels();
		OutputProcList(0);

		if(e.pst_Output)
		{
			MEM_Free(e.pst_Output);
			e.pst_Output = NULL;
		}

		if(e.pst_OtherOutput)
		{
			MEM_Free(e.pst_OtherOutput);
			e.pst_OtherOutput = NULL;
		}
	}
	else
	{
		/* Save infos in function structure */
		st_Func.i_SizeLocalStack = mst_LocalVars.i_LastPosVar;
		st_Func.pst_RootNode = e.pst_Output;
		st_Func.pst_OtherRootNode = e.pst_OtherOutput;
		st_Func.pc_Strings = e.pc_StringBuf;

		/* Generate output */
		ResolveLabels();
		GenerateOutput(&st_Func);

		/* Infos about local vars */
		st_Func.pst_LocalVars = e.pst_LocalVars;
		st_Func.ul_NbLocals = mst_LocalVars.o_Vars.GetCount();

		/* Warning local not used */
		if(!mb_GlobalVarMode)
		{
			if(mpo_Frame->mst_Ini.b_WarningHigh)
			{
				pos = mst_LocalVars.o_Vars.GetStartPosition();
				while(pos)
				{
					mst_LocalVars.o_Vars.GetNextAssoc(pos, str, (void * &) po_Var);
					if(!po_Var->mb_HasBeenUsed)
					{
						str1.Format("Function %s : Local variable %s is not used", BIG_NameFile(_ul_File), str);
						ERR_X_Warning(0, (char *) (LPCSTR) str1, 0);
					}
				}
			}
		}
	}

	_Catch_ 
	FreeCompilerVarDes();
	FreeCompilerOut();
	e.i_Error = e.i_NumLine;
	Reset();
	_End_

	/* Free text file buffer */
	if(e.psz_Buf)
	{
		L_free(e.psz_Buf);
		e.psz_Buf = NULL;
	}

	if(mb_GenPP || mb_ForceGenPP)
	{
		/*~~~~~~~~~~~~~~~~~~~~~*/
		char		name[512];
		BIG_INDEX	h_PP;
		char		*pc_Buf, *pz;
		ULONG		ul_Size;
		char		path[512];
		int			i, p;
		/*~~~~~~~~~~~~~~~~~~~~~*/

		L_strcpy(name, BIG_NameFile(_ul_File));
		L_strcat(name, ".pp");
		BIG_ComputeFullName(BIG_ParentFile(_ul_File), path);
		h_PP = BIG_ul_SearchFile(BIG_ParentFile(_ul_File), name);
		pz = pc_Buf = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(h_PP), &ul_Size);
		if(pz)
		{
			/* Vire cmt */
			while(*pz && ((ULONG)(pz - pc_Buf) <= ul_Size))
			{
				if(pz[0] == '/' && pz[1] == '/')
				{
					while(*pz && *pz != '\n') *pz++ = ' ';
				}

				pz++;
			}

			pz = pc_Buf;
			i = 0;
			SAV_Begin(path, name);
			while(*pz && ((ULONG)(pz - pc_Buf) <= ul_Size))
			{
				if(!mb_ForceGenPP)
				{
					/* Code begin */
					if(i == gi_FirstCode)
					{
						sprintf(name, "/*FIRST_CODE*/", p);
						SAV_Buffer(name, strlen(name));
					}
				}

				i++;
				SAV_Buffer(pz, 1);

				if(!mb_ForceGenPP)
				{
					/* Put all ';' in PP file */
					if(go_PPAllPV.Lookup((void *) i, (void * &) p)) SAV_Buffer(";", 1);

					/* Put all refs marks */
					if(go_PPAllRef.Lookup((void *) i, (void * &) p))
					{
						sprintf(name, "/*0x%x*/", p);
						SAV_Buffer(name, strlen(name));
					}

					/* String */
					if(go_PPReplace.Lookup((void *) i, (void * &) p))
					{
						SAV_Buffer((char *) p, strlen((char *) p));
					}

					/* Pop type */
					if(go_PPAllPop.Lookup((void *) i, (void * &) p))
					{
						switch(p)
						{
						case TYPE_INT:			sprintf(name, " int", p); break;
						case TYPE_FLOAT:		sprintf(name, " float", p); break;
						case TYPE_VECTOR:		sprintf(name, " vec", p); break;
						case TYPE_GAMEOBJECT:	sprintf(name, " obj", p); break;
						case TYPE_MESSAGEID:	sprintf(name, " msgid", p); break;
						}

						SAV_Buffer(name, strlen(name));
					}
				}

				pz++;
			}

			SAV_ul_End();
			free(pc_Buf);
		}
	}

	return e.i_Error;
}
#endif /* ACTIVE_EDITORS */
