/*$T AIcompile_vars.cpp GC! 1.100 03/08/01 12:24:12 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/ERRors/ERRasser.h"
#include "../AIerrid.h"
#include "../AIframe.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIstack.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BASe/CLIbrary/CLImem.h"
#include "EDIpaths.h"

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoPush(void)
{
	/*~~~~~~~*/
	int i_Type;
	/*~~~~~~~*/

	Next();
	MatchString("(");
	i_Type = BoolExpression();

	switch(i_Type)
	{
	case TYPE_INT:
	case TYPE_FLOAT:
	case TYPE_GAMEOBJECT:
	case TYPE_VECTOR:
	case TYPE_MESSAGEID:
		break;
	default:
		ERR_X_ForceErrorThrow("Invalid type for push", NULL);
		break;
	}

	MatchString(")");
	Output(CATEG_KEYWORD, 0, KEYWORD_PUSH);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::DoField(int _i_Type)
{
	if(e.c_Token == TOKEN_FIELD)
	{
		CompatibleTypes(_i_Type, EAI_gast_Fields[e.i_IndexValue].i_Type);
		_i_Type = EAI_gast_Fields[e.i_IndexValue].i_ReturnType;
		Output(CATEG_FIELD, 0, e.i_Value);
		Next();
	}

	return _i_Type;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::DoVariable(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	EAI_cl_Variable *po_Var;
	int				i_Type, i_Pos;
	char			c_Mem;
	int				i_Mem;
	int				i_Dim, i_Index;
	int				iRes;
	int				iPosUltra, iNumUltra;
	BOOL			bDirectModel;
	BOOL			bUltra;
	int				iMemOut;
	BOOL			bMem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* An ultra var */
	bUltra = e.b_Ultra;
	iPosUltra = e.i_PosUltra;
	iNumUltra = e.i_NumUltra;
	if(e.b_Ultra)
	{
		iMemOut = e.i_CurrentOutput;
		iRes = DoUltra(e.i_PosUltra, e.i_NumUltra, FALSE);
		i_Index = -1;

		/* No model cast */
		if(iRes == BIG_C_InvalidIndex)
		{
			i_Index = e.pst_Output[e.i_CurrentOutput - 2].l_Param;
			bDirectModel = FALSE;
		}

		/* Model cast */
		else
		{
			bDirectModel = TRUE;
			i_Index = iRes;
		}

		e.i_CurrentOutput = iMemOut;

		/* Get variable for specified model */
		bMem = mb_ProcListParent;
		mb_ProcListParent = FALSE;
		i_Index = mpo_Frame->i_GetVarOfModel(i_Index, (char *) (LPCSTR) e.o_Value, bDirectModel);
		mb_ProcListParent = bMem;
		ERR_X_Error(i_Index != -1, ERR_COMPILER_Csz_UnkownID, (char *) (LPCSTR) e.o_Value);
		e.c_Token = TOKEN_GLOBALVAR;
		e.i_Value = i_Index;
	}

	/* Test if variable is here */
	ERR_X_Error(e.i_Value != -1, ERR_COMPILER_Csz_UnkownID, (char *) (LPCSTR) e.o_Value);

	/* Save current token, and go to next one */
	i_Type = TYPE_EVERY;
	c_Mem = e.c_Token;
	i_Mem = e.i_Value;
	po_Var = (EAI_cl_Variable *) e.i_Value;
	Next();

	/* Be sure that this is for a real variable declaration, and not a push/pop keyword */
	if(c_Mem != TOKEN_KEYWORD)
	{
		i_Type = po_Var->mi_Type;
		i_Pos = po_Var->mi_Pos;
		po_Var->mb_HasBeenUsed = TRUE;

		/* Is it for an array ? */
		i_Dim = 0;
		while(e.c_Token == '[')
		{
			Next();
			BoolExpression();
			MatchString("]");
			switch(i_Dim++)
			{
			case 0:
				if(c_Mem == TOKEN_LOCALVAR)
					c_Mem = TOKEN_LOCALVARARRAY;
				else
					c_Mem = TOKEN_GLOBALVARARRAY;
				break;
			case 1:
				if(c_Mem == TOKEN_LOCALVARARRAY)
					c_Mem = TOKEN_LOCALVARARRAY2;
				else
					c_Mem = TOKEN_GLOBALVARARRAY2;
				break;
			case 2:
				if(c_Mem == TOKEN_LOCALVARARRAY2)
					c_Mem = TOKEN_LOCALVARARRAY3;
				else
					c_Mem = TOKEN_GLOBALVARARRAY3;
				break;
			default:
				ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Dim, NULL);
				break;
			}
		}

		ERR_X_Error(po_Var->byrefarr || po_Var->mi_SizeArray == i_Dim, ERR_COMPILER_Csz_BadDim, NULL);
	}

	if(bUltra)
	{
		bUltra = e.b_Ultra;
		e.b_Ultra = TRUE;
		DoUltra(iPosUltra, iNumUltra, TRUE);
		e.b_Ultra = bUltra;
	}

	/* Normal process */
	switch(c_Mem)
	{
	case TOKEN_LOCALVAR:
		Output((po_Var->byref || po_Var->byrefarr) ? CATEG_LOCALVARREF : CATEG_LOCALVAR, i_Pos, i_Type);
		break;

	case TOKEN_LOCALVARARRAY:
		Output((po_Var->byref || po_Var->byrefarr) ? CATEG_LOCALVARARRAYREF : CATEG_LOCALVARARRAY, i_Pos, i_Type);
		break;
	case TOKEN_LOCALVARARRAY2:
		Output((po_Var->byref || po_Var->byrefarr) ? CATEG_LOCALVARARRAY2REF : CATEG_LOCALVARARRAY2, i_Pos, i_Type);
		break;
	case TOKEN_LOCALVARARRAY3:
		Output((po_Var->byref || po_Var->byrefarr) ? CATEG_LOCALVARARRAY3REF : CATEG_LOCALVARARRAY3, i_Pos, i_Type);
		break;

	case TOKEN_GLOBALVAR:
		Output(CATEG_GLOBALVAR, i_Pos, i_Type);
		break;

	case TOKEN_GLOBALVARARRAY:
		Output(CATEG_GLOBALVARARRAY, i_Pos, i_Type);
		break;
	case TOKEN_GLOBALVARARRAY2:
		Output(CATEG_GLOBALVARARRAY2, i_Pos, i_Type);
		break;
	case TOKEN_GLOBALVARARRAY3:
		Output(CATEG_GLOBALVARARRAY3, i_Pos, i_Type);
		break;

	case TOKEN_KEYWORD:
		if(i_Mem == KEYWORD_POP)
		{
			Output(CATEG_KEYWORD, 0, i_Mem);
			break;
		}

		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
		break;

	default:
		ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_UnkownID, (char *) (LPCSTR) e.o_Value);
		break;
	}

	/* Check for a field */
	i_Type = DoField(i_Type);

	return i_Type;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::DoVarInit(EAI_cl_Variable *_po_Var)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i_Type, i_Num;
	int					i_MemoOutput;
	AI_tdst_Node		*pst_CurrentNode;
	AI_tdst_UnionVar	Val;
	AI_tdst_PushVar		st_Var;
	BOOL				b_MemoCan;
	int					iOffset, iNum, iMax, i, iSize;
	int					iinit;
	BOOL				b_Neg;
	BOOL				b_enum, b_array;
	int					i_SizeEnum;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	i_MemoOutput = e.i_CurrentOutput;

	i_SizeEnum = 0;
	b_MemoCan = mb_CanOutput;
	mb_CanOutput = TRUE;	
	b_enum = FALSE;
	b_array = FALSE;

	/* For an array, attempt to find a ( */
	iMax = 1;
	if(_po_Var->mi_SizeArray)
	{
		b_array = TRUE;
		iMax = _po_Var->mai_SizeElem[0];
		for(i = 1; i < _po_Var->mi_SizeArray; i++) iMax *= _po_Var->mai_SizeElem[i];
		if(e.c_Token == TOKEN_TYPE && e.i_Value == TYPE_ENUM) goto l_enum;
		ERR_X_Error(e.c_Token == '(', ERR_COMPILER_Csz_Syntax, NULL);
		Next();
	}

	/* Enum of constants */
	else if(e.c_Token == TOKEN_TYPE && e.i_Value == TYPE_ENUM)
	{
l_enum:
		b_enum = TRUE;
		Next();
		ERR_X_Error(e.c_Token == '(', ERR_COMPILER_Csz_Syntax, NULL);
		mi_RequestedType = TYPE_STRING;
		Next();
		i_Num = 0;
		while(e.c_Token != ')')
		{
			ERR_X_Error(e.c_Token == TOKEN_STRING, ERR_COMPILER_Csz_Syntax, NULL);
			_po_Var->mo_String1 += e.o_Value + "\n";
			Next();
			b_Neg = FALSE;
			if(e.c_Token == '-')
			{
				b_Neg = TRUE;
				Next();
			}

			ERR_X_Error(e.c_Token == TOKEN_NUMBER, ERR_COMPILER_Csz_Syntax, NULL);
			if(b_Neg)
				_po_Var->mo_String1 += "-" + e.o_Value + "\n";
			else
				_po_Var->mo_String1 += e.o_Value + "\n";
			_po_Var->mi_SpecialVar |= VAR_ENUM | VAR_STRING1;
			mi_RequestedType = TYPE_STRING;

			if(!i_Num)
			{
				if(!b_array)
				{
					iSize = EAI_gast_Types[AI_gaw_EnumLink[_po_Var->mi_Type]].i_Size;
					_po_Var->mpac_InitValue = (char *) L_malloc(iSize);
					L_memset(_po_Var->mpac_InitValue, 0, iSize);
					i_Num = L_atol(e.o_Value);
					if(b_Neg) i_Num *= -1;
					L_memcpy(_po_Var->mpac_InitValue, &i_Num, iSize);
				}
				else
				{
					iinit = L_atol(e.o_Value);
					if(b_Neg) iinit *= -1;
					i_SizeEnum = EAI_gast_Types[AI_gaw_EnumLink[_po_Var->mi_Type]].i_Size;
				}

				i_Num = 1;
			}

			Next();
		}

		Next();
		if(!b_array) return; /* Continue for array */
	}

	/* Allocate buffer to receive init value */
	iOffset = 0;
	iSize = iMax * EAI_gast_Types[AI_gaw_EnumLink[_po_Var->mi_Type]].i_Size;
	if(mb_GlobalVarMode && _po_Var->mi_SizeArray)
	{
		iSize += _po_Var->mi_SizeArray * sizeof(int);
		iOffset = _po_Var->mi_SizeArray * sizeof(int);
	}

	_po_Var->mpac_InitValue = (char *) L_malloc(iSize);
	L_memset(_po_Var->mpac_InitValue, 0, iSize);

	if(b_enum)
	{
		L_memcpy(_po_Var->mpac_InitValue + iOffset, &iinit, i_SizeEnum);
	}

	iNum = 0;
	while(!b_enum)
	{
		i_Type = BoolExpression();
		CompatibleTypes(i_Type, _po_Var->mi_Type);

		/* Resolve references of affection */
		Output(CATEG_ENDTREE, 0, 0);
		pst_CurrentNode = e.pst_Output + i_MemoOutput;
		AI_ResolveFctRefSimple(pst_CurrentNode);

		/* Interpret nodes. We will get the result of the interpretation on the stack */
		AI_gb_InterpForVar = TRUE;
_Try_
		while(pst_CurrentNode->c_Type != CATEG_ENDTREE)
			pst_CurrentNode = AI_gast_Categs[pst_CurrentNode->c_Type].pfn_Func(pst_CurrentNode);

		/* Get the result on the stack */
		AI_PopVar(&Val, &st_Var);

		/* Static cast */
		if((_po_Var->mi_Type == TYPE_FLOAT) && (i_Type == TYPE_INT))
			Val.f = fLongToFloat(Val.i);
		else if((_po_Var->mi_Type == TYPE_INT) && (i_Type == TYPE_FLOAT))
			Val.i = lFloatToLong(Val.f);

		/* Copy initial value in buffer */
		L_memcpy(_po_Var->mpac_InitValue + iOffset, &Val, EAI_gast_Types[AI_gaw_EnumLink[i_Type]].i_Size);
		iOffset += EAI_gast_Types[AI_gaw_EnumLink[i_Type]].i_Size;

		iNum++;

		_po_Var->mb_HasBeenInit = TRUE;
_Catch_
_End_
		AI_gb_InterpForVar = FALSE;
		ERR_X_Error(i_Type != TYPE_STRING, "Can't initialize a string here", NULL);

		/* Restore output to previous state cause we doesn't want nodes to be present... */
		e.i_CurrentOutput = i_MemoOutput;

		/* End if its not an array */
		if(!_po_Var->mi_SizeArray) break;

		/* Else attempt to find a comma or a close paren */
		switch(e.c_Token)
		{
		case ',':	Next(); break;
		case ')':	Next(); goto lEnd;
		default:	ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, NULL);
		}

		/* Check that we have not initialised to much values */
		ERR_X_Error(iNum < iMax, ERR_COMPILER_Csz_InitArray, NULL);
	}

lEnd:
	/* Init the beginning of the buffer for a global array */
	if(mb_GlobalVarMode && _po_Var->mi_SizeArray)
	{
		for(i = 0; i < _po_Var->mi_SizeArray; i++)
			*(LONG *) (_po_Var->mpac_InitValue + (i * sizeof(int))) = _po_Var->mai_SizeElem[i];
	}

	mb_CanOutput = b_MemoCan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
EAI_cl_Variable *EAI_cl_Compiler::AddVariable(int _i_Type)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	EAI_cl_Variable *po_Var;
	int				i_SizeArray;
	int				iDim, i;
	int				iSpecialVar;
	BIG_INDEX		ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Special variables */
	iSpecialVar = VAR_NONE;
recom:
	switch(_i_Type)
	{
	case TYPE_REINIT:
		/* Only in universe */
		if((mul_CurrentInstance == BIG_C_InvalidIndex) || (BIG_FileKey(mul_CurrentInstance) != BIG_UniverseKey()))
		{
			ul_Index = -1;
			if(WOR_gst_Universe.pst_AI)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) WOR_gst_Universe.pst_AI->pst_Model);
				if(ul_Index != ul_GetEngineModelIndex(mul_CurrentModel, NULL, NULL)) ul_Index = -1;
			}

			ERR_X_Error(ul_Index != -1, "\"save\" keyword must be defined for universe only (is it loaded ?)", NULL);
		}

		Next(TRUE);
		ERR_X_Error(e.c_Token == TOKEN_TYPE, ERR_COMPILER_Csz_Syntax, NULL);
		_i_Type = e.i_Value;
		iSpecialVar |= VAR_REINIT;
		goto recom;

	case TYPE_SAVE:
		/* Only in universe */
		if((mul_CurrentInstance == BIG_C_InvalidIndex) || (BIG_FileKey(mul_CurrentInstance) != BIG_UniverseKey()))
		{
			ul_Index = -1;
			if(WOR_gst_Universe.pst_AI)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) WOR_gst_Universe.pst_AI->pst_Model);
				if(ul_Index != ul_GetEngineModelIndex(mul_CurrentModel, NULL, NULL)) ul_Index = -1;
			}

			ERR_X_Error(ul_Index != -1, "\"save\" keyword must be defined for universe only (is it loaded ?)", NULL);
		}

		Next(TRUE);
		ERR_X_Error(e.c_Token == TOKEN_TYPE, ERR_COMPILER_Csz_Syntax, NULL);
		_i_Type = e.i_Value;
		iSpecialVar |= VAR_SAVE;
		break;

	case TYPE_OPTIM:
		Next(TRUE);
		ERR_X_Error(e.c_Token == TOKEN_TYPE, ERR_COMPILER_Csz_Syntax, NULL);
		_i_Type = e.i_Value;
		iSpecialVar |= VAR_OPTIM;
		goto recom;

	case TYPE_SAVEAL:
		/* Only in universe */
		if((mul_CurrentInstance == BIG_C_InvalidIndex) || (BIG_FileKey(mul_CurrentInstance) != BIG_UniverseKey()))
		{
			ul_Index = -1;
			if(WOR_gst_Universe.pst_AI)
			{
				ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) WOR_gst_Universe.pst_AI->pst_Model);
				if(ul_Index != ul_GetEngineModelIndex(mul_CurrentModel, NULL, NULL)) ul_Index = -1;
			}

			ERR_X_Error(ul_Index != -1, "\"save\" keyword must be defined for universe only (is it loaded ?)", NULL);
		}

		Next(TRUE);
		ERR_X_Error(e.c_Token == TOKEN_TYPE, ERR_COMPILER_Csz_Syntax, NULL);
		_i_Type = e.i_Value;
		iSpecialVar |= VAR_SAVE;
		iSpecialVar |= VAR_SAVEAL;
		break;

	case TYPE_PRIVATE:
		ERR_X_Error(mb_GlobalVarMode, ERR_COMPILER_Csz_CantPrivateLocal, NULL);
		Next(TRUE);
		ERR_X_Error(e.c_Token == TOKEN_TYPE, ERR_COMPILER_Csz_Syntax, NULL);
		_i_Type = e.i_Value;
		iSpecialVar |= VAR_PRIVATE;
		break;

	case TYPE_SEPARATOR:
		mi_RequestedType = TYPE_STRING;
		Next(FALSE);
		ERR_X_Error(e.c_Token == TOKEN_STRING, ERR_COMPILER_Csz_Syntax, 0);
		po_Var = new EAI_cl_Variable(0, mst_LocalVars.i_LastPosVar);
		mst_LocalVars.o_Vars.SetAt((LPCSTR) e.o_Value, po_Var);
		po_Var->mi_SpecialVar = VAR_SEP | VAR_STRING1;
		po_Var->mo_String1 = e.o_Value;
		Next();
		return po_Var;
	}

	/* Get name of variable */
	Next(FALSE);
	ERR_X_Error(e.c_Token == TOKEN_NAME, ERR_COMPILER_Csz_VarName, 0);

	/* Add local variable to cmap */
	po_Var = new EAI_cl_Variable(_i_Type, mst_LocalVars.i_LastPosVar);

	/* Check if name is already present in list of variables */
	ERR_X_Error
	(
		mst_LocalVars.o_Vars.Lookup((LPCSTR) e.o_Value, (void * &) po_Var) == FALSE,
		ERR_COMPILER_Csz_DupSymbol,
		(char *) (LPCSTR) e.o_Value
	);

	ERR_X_Error
	(
		mst_GlobalVars.o_Vars.Lookup((LPCSTR) e.o_Value, (void * &) po_Var) == FALSE,
		ERR_COMPILER_Csz_DupSymbol,
		(char *) (LPCSTR) e.o_Value
	);

	/* Add name to list */
	po_Var->StmtLevel = e.StmtLevel;
	mst_LocalVars.o_Vars.SetAt((LPCSTR) e.o_Value, po_Var);

	/* Is it a special name ? */
	if(!L_strnicmp("track", (char *) (LPCSTR) e.o_Value, 5))
	{
		ERR_X_Error(!iSpecialVar, ERR_COMPILER_Csz_Syntax, NULL);
		ERR_X_Error(mb_GlobalVarMode, ERR_COMPILER_Csz_Syntax, NULL);
		iSpecialVar = VAR_TRACK;
		ERR_X_Error(_i_Type == TYPE_FUNCTIONREF, ERR_COMPILER_Csz_BadTypes, (char *) (LPCSTR) e.o_Value);
	}

	po_Var->mi_SpecialVar = iSpecialVar;

	/* Is it an array ? */
	i_SizeArray = 1;
	L_memset(po_Var->mai_SizeElem, 0, sizeof(po_Var->mai_SizeElem));
	iDim = 0;
	Next();
	while(e.c_Token == '[')
	{
		ERR_X_Error(iSpecialVar != VAR_TRACK, ERR_COMPILER_Csz_Syntax, NULL);
		ERR_X_Error(iDim != 3, ERR_COMPILER_Csz_Dim, NULL);

		Next();
		ERR_X_Error(e.c_Token == TOKEN_NUMBER, ERR_COMPILER_Csz_SizeArray, 0);

		po_Var->mai_SizeElem[iDim] = L_atoi(e.o_Value);
		ERR_X_Error(po_Var->mai_SizeElem[iDim] > 0, ERR_COMPILER_Csz_SizeArray, 0);
		i_SizeArray *= po_Var->mai_SizeElem[iDim];

		Next();
		MatchString("]");

		iDim++;
		po_Var->mi_SizeArray = iDim;
	}

	/* Initialisation of the variable */
	if(e.c_Token == '=')
	{
		ERR_X_Error(mb_GlobalVarMode, ERR_COMPILER_Csz_CantInitLocal, NULL);
		mi_RequestedType = _i_Type;
		Next();
		DoVarInit(po_Var);
	}

	/*
	 * Special case for an array. We must generate a node that will serve to keep
	 * trace of max size of each dimension. £
	 * We push size of each dimension, and number of dimensions.
	 */
	if(iDim > 0)
	{
		/* Init the beginning of the buffer for a global array */
		if(mb_GlobalVarMode && iDim && po_Var->mpac_InitValue)
		{
			for(i = 0; i < iDim; i++)
				*(LONG *) (po_Var->mpac_InitValue + (i * sizeof(int))) = po_Var->mai_SizeElem[i];
		}

		/* Node that will serve to initialise infos about the array */
		if(!mb_GlobalVarMode)
		{
			/* Size of each dimension */
			for(i = 0; i < iDim; i++) Output(CATEG_TYPE, po_Var->mai_SizeElem[i], TYPE_INT);

			/* Number of dimensions */
			Output(CATEG_TYPE, iDim, TYPE_INT);

			/* Function that will serve to init the local buffer */
			Output(CATEG_INITLOCALVARARRAY, mst_LocalVars.i_LastPosVar, _i_Type);
		}

		/* Keep memory to store dim and size of each dim for engine */
		mst_LocalVars.i_LastPosVar += sizeof(int) * iDim;
	}

	/* Is there a comment after ? */
	if(!e.o_LastComment.IsEmpty())
	{
		po_Var->mi_SpecialVar |= VAR_STRING2;
		po_Var->mo_String2 = e.o_LastComment;
	}

	/* Increment pos by var size (except for a special var) */
	if(!(iSpecialVar & VAR_TRACK))
	{
		po_Var->TotalSize = (EAI_gast_Types[AI_gaw_EnumLink[_i_Type]].i_Size * i_SizeArray);
		mst_LocalVars.i_LastPosVar += po_Var->TotalSize;
	}

	return po_Var;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::RemoveVarStmt(int _i_Level)
{
	/*~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EAI_cl_Variable *po_Var;
	CString			o_Str;
	/*~~~~~~~~~~~~~~~~~~~~*/

	pos = mst_LocalVars.o_Vars.GetStartPosition();
	while(pos)
	{
		mst_LocalVars.o_Vars.GetNextAssoc(pos, o_Str, (void * &) po_Var);
		if(po_Var->StmtLevel == _i_Level)
		{
			mst_LocalVars.o_Vars.RemoveKey(o_Str);
//			mst_LocalVars.i_LastPosVar -= po_Var->TotalSize;
			delete po_Var;
		}
	}
}

#endif /* ACTIVE_EDITORS */
