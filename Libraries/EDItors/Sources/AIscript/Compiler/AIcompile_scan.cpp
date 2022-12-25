/*$T AIcompile_scan.cpp GC!1.71 01/31/00 10:21:04 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/ERRors/ERRasser.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "../AIerrid.h"
#include "../AIframe.h"
#include "AIinterp/Sources/AIengine.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGfat.h"

extern int gi_PPAllPVBeforeNext;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GetField(void)
{
    /*~~~~~~~~~~~~~~~~*/
    int     i_Index;
    char    astr[2];
    /*~~~~~~~~~~~~~~~~*/

    GetChar();  /* Zap "." character */

    e.o_Value = ".";
    e.c_Token = TOKEN_FIELD;
    ERR_X_Error(L_isalpha(e.c_Look), ERR_COMPILER_Csz_Identifier, NULL);

    astr[1] = 0;
    do
    {
        astr[0] = e.c_Look;
        e.o_Value += astr;
        GetChar();
    } while(L_isalnum(e.c_Look));

    /* Is it really a field ? */
    if(mpo_Frame->mo_FieldList.Lookup((LPCSTR) e.o_Value, (void * &) i_Index))
    {
        e.i_Value = EAI_gast_Fields[i_Index].w_ID;
        e.i_IndexValue = i_Index;
        return;
    }

    ERR_X_ForceError(ERR_COMPILER_Csz_Field, (char *) (LPCSTR) e.o_Value);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GetName(BOOL _b_Scan)
{
    /*~~~~~~~~~~~~~~~~*/
	char	asz[512];
	int		i;
    /*~~~~~~~~~~~~~~~~*/

    ERR_X_Error(L_isalpha(e.c_Look), ERR_COMPILER_Csz_Identifier, NULL);

    e.o_Value.Empty();
    e.c_Token = TOKEN_NAME;
	i = 0;
    do
    {
        asz[i++] = e.c_Look;
        GetChar();
    } while(L_isalnum(e.c_Look));

	if(e.c_Look == ':') 
	{
		asz[i++] = ':';
        GetChar();
	}

	asz[i] = 0;
	e.o_Value = asz;
    if(_b_Scan) Scan();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GetPP(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    char    *psz_Temp, *psz_Temp1;
	int		mempv;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    GetChar();
    GetName(FALSE);

    if(!L_stricmp(e.o_Value, "breakoff"))
    {
        e.i_NumLine -= 2;
        if(mi_BreakLineOn == 0) e.i_NumLineBreak -= 2;
        mi_BreakLineOn--;
        return;
    }

    if(!L_stricmp(e.o_Value, "breakon"))
    {
        mi_BreakLineOn++;
        e.i_NumLine -= 2;
        if(mi_BreakLineOn == 0) e.i_NumLineBreak -= 2;
        return;
    }

    if(!L_stricmp(e.o_Value, "line"))
    {
		mempv = gi_PPAllPVBeforeNext;

        /* Get line num */
        Next();
        ERR_X_Error(e.c_Token == TOKEN_NUMBER, ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
        e.i_NumLine = L_atol((char *) (LPCSTR) e.o_Value);
        if(mi_BreakLineOn == 0) e.i_NumLineBreak = e.i_NumLine;

        /* Get path/file */
        mi_RequestedType = TYPE_STRING;
        Next();
        ERR_X_Error(e.c_Token == TOKEN_STRING, ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);

        psz_Temp = (char *) (LPCSTR) e.o_Value;
        psz_Temp1 = L_strrchr(psz_Temp, '/');
        ERR_X_Error(psz_Temp1, ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
        *psz_Temp1 = 0;

        L_strcpy(e.asz_Path, psz_Temp);
        L_strcpy(e.asz_File, psz_Temp1 + 1);

        /*
         * Attempt to find an '\n'. Decrease line number cause don't count the '\n' juste
         * after the #line directive
         */
        while(e.c_Look && (e.c_Look == ' ' || e.c_Look == '\t'))
            GetChar();
        ERR_X_Error(e.c_Look == '\n', ERR_COMPILER_Csz_Syntax, NULL);
        e.i_NumLine--;
        if(mi_BreakLineOn == 0) e.i_NumLineBreak = e.i_NumLine;
        e.c_Token = TOKEN_PP;

		gi_PPAllPVBeforeNext = mempv;
        return;
    }

    ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, (char *) (LPCSTR) e.o_Value);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GetString(void)
{
    /*~~~~~~~~~~~~~~~~~*/
    char    asz[512];
    int     i;
    /*~~~~~~~~~~~~~~~~~*/

    GetChar();
    e.o_Value.Empty();
    e.c_Token = TOKEN_STRING;
    i = 0;
    while(e.c_Look && e.c_Look != '"')
    {
		if(i == 512)
		{
			ERR_X_Error(0, ERR_COMPILER_Csz_MissString, NULL);
		}
        asz[i++] = e.c_Look;
        GetChar();
    }

	asz[i] = 0;
    e.o_Value = asz;
    ERR_X_Error(e.c_Look == '"', ERR_COMPILER_Csz_MissString, NULL);
    GetChar();

    /* Treat references */
    GetReference();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Compiler::GetUltra(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
    int             i_MemoOut, i_MemoOut1;
    EAI_cl_Variable *po_Var;
    int             i_Type;
	BOOL			bMem;
    /*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	bMem = mb_ProcListParent;
	mb_ProcListParent = FALSE;

    e.i_PosUltra = mi_UltraMode;
    i_MemoOut = e.i_CurrentOutput;

	ep_copy[mi_UltraMode].b_TopNode = e.b_TopNode;
	ep_copy[mi_UltraMode].i_NumLine = e.i_NumLine;

    GetChar();
    mi_RequestedType = TYPE_GAMEOBJECT;
    Next();

    /* A model ? */
    if(e.c_Token == TOKEN_MODEL)
    {
        Output(CATEG_TYPE, e.i_Value, TYPE_MODEL);
        Next();
    }

    po_Var = (EAI_cl_Variable *) e.i_Value;
    if
    (
        (e.c_Token == TOKEN_LOCALVAR) ||
        (e.c_Token == TOKEN_LOCALVARARRAY) ||
        (e.c_Token == TOKEN_GLOBALVAR) ||
        (e.c_Token == TOKEN_GLOBALVARARRAY)
    )
    {
        ERR_X_Error((int) po_Var != -1, ERR_COMPILER_Csz_UnkownID, NULL);
    }
    ERR_X_Error
    (
        (e.c_Token == TOKEN_GAMEOBJECT) ||
        ((e.c_Token == TOKEN_LOCALVAR) && (po_Var->mi_Type == TYPE_GAMEOBJECT)) ||
        ((e.c_Token == TOKEN_LOCALVARARRAY) && (po_Var->mi_Type == TYPE_GAMEOBJECT)) ||
        ((e.c_Token == TOKEN_GLOBALVAR) && (po_Var->mi_Type == TYPE_GAMEOBJECT)) ||
        ((e.c_Token == TOKEN_GLOBALVARARRAY) && (po_Var->mi_Type == TYPE_GAMEOBJECT)),
        ERR_COMPILER_Csz_UltraGO,
        NULL
    );

    /* Attempt to find a variable */
    switch(e.c_Token)
    {
    case TOKEN_LOCALVAR:
    case TOKEN_GLOBALVAR:
        i_Type = DoVariable();
        break;
    case TOKEN_GAMEOBJECT:
        Output(CATEG_TYPE, e.i_Value, TYPE_GAMEOBJECT);
        Next();
        i_Type = TYPE_GAMEOBJECT;
        break;
    default:
        ERR_X_ForceErrorThrow(ERR_COMPILER_Csz_Syntax, NULL);
        break;
    }

    /* Copy all output nodes */
    e.b_Ultra = TRUE;
    e.i_NumUltra = e.i_CurrentOutput - i_MemoOut;
    i_MemoOut1 = i_MemoOut;
    while(i_MemoOut1 != e.i_CurrentOutput)
    {
        L_memcpy(&e_copy[mi_UltraMode], &e.pst_Output[i_MemoOut1], sizeof(AI_tdst_Node));
        mi_UltraMode++;
        i_MemoOut1++;
    }

    e.i_CurrentOutput = i_MemoOut;

	mb_ProcListParent = bMem;
    return i_Type;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GetNum(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~*/
    BOOL    mb_FirstPoint;
	char	tmp[1000];
	int		cur;
    /*~~~~~~~~~~~~~~~~~~~~~~*/

	cur = 0;
    mb_FirstPoint = FALSE;

    ERR_X_Error(L_isdigit(e.c_Look), ERR_COMPILER_Csz_Number, NULL);

    e.c_Token = TOKEN_NUMBER;
    e.i_Value = TYPE_INT;

    /* Is it a special representation of a number ? */
    if(e.c_Look == '0')
    {
        GetChar();

        /* Hexadecimal */
        if(e.c_Look == 'x')
        {
            GetChar();
            ERR_X_Error(L_isxdigit(e.c_Look), ERR_COMPILER_Csz_Number, NULL);
            e.i_Value = TYPE_HEXA;
            do
            {
				tmp[cur++] = L_toupper(e.c_Look);
                GetChar();
            } while(L_isxdigit(e.c_Look));

			tmp[cur] = 0;
			e.o_Value = tmp;
            return;
        }

        /* Binary */
        else if(e.c_Look == 'b')
        {
            GetChar();
            ERR_X_Error(L_isbinary(e.c_Look), ERR_COMPILER_Csz_Number, NULL);
            e.i_Value = TYPE_BINARY;
            do
            {
				tmp[cur++] = L_toupper(e.c_Look);
                GetChar();
            } while(L_isbinary(e.c_Look));

			tmp[cur] = 0;
			e.o_Value = tmp;
            return;
        }

        /* Normal number that began with a 0 */
        e.o_Value += "0";
    }

    do
    {
        if(L_isdigit(e.c_Look))
        {
			tmp[cur++] = e.c_Look;
            GetChar();
        }

        if(e.c_Look == '.')
        {
            ERR_X_Error(mb_FirstPoint == FALSE, ERR_COMPILER_Csz_Number, NULL);
            mb_FirstPoint = TRUE;
            e.i_Value = TYPE_FLOAT;
			tmp[cur++] = e.c_Look;
            GetChar();
        }
    } while(L_isdigit(e.c_Look));

	tmp[cur] = 0;
	e.o_Value = tmp;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GetOp(void)
{
    /*~~~~~~~~~~~~~~~~*/
    char    astr[2];
    /*~~~~~~~~~~~~~~~~*/

    e.o_Value.Empty();
    e.c_Token = e.c_Look;
    astr[1] = 0;
    astr[0] = e.c_Look;
    e.o_Value = astr;
    GetChar();

    /* Special opers */
    if((e.c_Token == '<') || (e.c_Token == '>'))
    {
        if(e.c_Look == e.c_Token)
        {
            astr[1] = 0;
            astr[0] = e.c_Look;
            e.o_Value = astr;
            e.o_Value += astr;
            GetChar();
        }
    }

    if((e.c_Token == '&') || (e.c_Token == '|'))
    {
        if(e.c_Look == e.c_Token)
        {
            astr[1] = 0;
            astr[0] = e.c_Look;
            e.o_Value = astr;
            e.o_Value += astr;
            GetChar();
        }
    }
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::Scan(void)
{
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/
    int             i_Index;
    EAI_cl_Variable *po_Var;
    /*~~~~~~~~~~~~~~~~~~~~~~~~*/

    if(e.c_Token == TOKEN_NAME)
    {
        /* Is it a globalvar ? */
        if(mst_GlobalVars.o_Vars.Lookup((LPCSTR) e.o_Value, (void * &) po_Var))
        {
			ERR_X_Error(!mb_ProcListParent || mb_ProcListLocal, "You can't use global vars in procedures", "");
            e.c_Token = TOKEN_GLOBALVAR;
            e.i_Value = (int) po_Var;
            return;
        }

        /* Is it a function ? */
        if(mpo_Frame->mo_FunctionList.Lookup((LPCSTR) e.o_Value, (void * &) i_Index))
        {
            e.c_Token = TOKEN_FUNCTION;
            e.i_Value = EAI_gast_Functions[i_Index].w_ID;
            e.i_IndexValue = i_Index;

            /*$F to check use of a function
            {
                static int i_Fct2Find = 8386;
                if ( e.i_Value == i_Fct2Find ) 
                {
                    char sz_Text[256];
                    sprintf( sz_Text, "%d used by %08X ( %s )", e.i_Value, BIG_FileKey(e.ul_File), BIG_NameFile( e.ul_File ) );
                    LINK_PrintStatusMsg( sz_Text );
                }
            }
            */
            return;
        }

        /* Is it a localvar ? */
        if(mst_LocalVars.o_Vars.Lookup((LPCSTR) e.o_Value, (void * &) po_Var))
        {
            e.c_Token = TOKEN_LOCALVAR;
            e.i_Value = (int) po_Var;
            return;
        }

        /* Is it a keyword ? */
        if(mpo_Frame->mo_KeywordList.Lookup((LPCSTR) e.o_Value, (void * &) i_Index))
        {
            e.c_Token = TOKEN_KEYWORD;
            e.i_Value = EAI_gast_Keywords[i_Index].w_ID;
            e.i_IndexValue = i_Index;
            return;
        }

        /* Is it a type ? */
        if(mpo_Frame->mo_TypeList.Lookup((LPCSTR) e.o_Value, (void * &) i_Index))
        {
            e.c_Token = TOKEN_TYPE;
            e.i_Value = EAI_gast_Types[i_Index].w_ID;
            e.i_IndexValue = i_Index;
            return;
        }

        /* Is it a constant ? */
        if(mpo_Frame->mo_ConstantList.Lookup((LPCSTR) e.o_Value, (void * &) i_Index))
        {
            e.o_Value = EAI_gast_Constants[i_Index].psz_Value;
            e.c_Token = TOKEN_NUMBER;
            e.i_Value = EAI_gast_Constants[i_Index].i_Type;
            return;
        }

		if(mo_ProcList.Lookup((LPCSTR) e.o_Value, (void * &) i_Index))
		{
            e.c_Token = TOKEN_CALLPROC;
			e.i_Value = i_Index;
            return;
		}

		/* Label ? */
		if(e.o_Value[e.o_Value.GetLength() - 1] == ':')
		{
            e.c_Token = TOKEN_LABEL;
            return;
		}

        /* Dummy value for the moment */
        e.c_Token = TOKEN_GLOBALVAR;
        e.i_Value = -1;
    }
}

#endif /* ACTIVE_EDITORS */
