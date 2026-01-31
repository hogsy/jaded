/*$T AIcompile_pp.cpp GC! 1.100 05/31/01 16:54:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/ERRors/ERRasser.h"
#include "BIGfiles/BIGread.h"
#include "../AIerrid.h"
#include "../AIframe.h"
#include "EDIpaths.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGdefs.h"

/* All the constants */
CMapStringToString						go_PPConstants;
CMapStringToPtr							go_PPConstantsFiles;
CMapStringToPtr							go_PPConstantsLines;
CMapStringToPtr							go_PPMacros;
CMapStringToPtr							go_PPMacrosLines;

/* All the functions */
CMapStringToPtr							go_PPFunctions;

/*=====================================================================================================================*/
CList<CString, CString>					go_DefineCst;
CMapStringToPtr							go_DefineCstNames;
CMapStringToString						go_DefineCstFiles;
CList<tdst_PPMacro *, tdst_PPMacro *>	go_DefineMacros;
CList<tdst_PPMacro *, tdst_PPMacro *>	go_DefineMacrosGlob;
CMapStringToPtr							go_DefineMacroNames;
CMapStringToPtr							go_DefineMacroNamesGlob;
CMapStringToString						go_DefineMacrosFiles;
CList<CString, CString>					go_DefineCstGlob;
CMapStringToPtr							go_DefineCstNamesGlob;
CMapStringToString						go_DefineCstFilesGlob;
CList<tdst_PPMacro *, tdst_PPMacro *>	go_DefineMacrosGlog;
CMapStringToString						go_DefineMacrosFilesGlob;
/*=====================================================================================================================*/

/* The list of open include files (to avoid cross references) */
CMapStringToPtr							go_FileNames;

/* The list of define names that can't be used (to avoid recurse replacement) */
CMapStringToPtr							go_DisableDefNames;

/* Global for output */
char					*gpsz_OutBuf, *gpsz_Out;
int						gi_SizeOut;

/* Constants for ifdef, ifndef */
CList<CString, CString> go_IfDefConsts;

/*
 =======================================================================================================================
    Aim:    Output a char to destination buffer.
 =======================================================================================================================
 */
#define OUTPP() \
	{ \
		if(gpsz_Out - gpsz_OutBuf == gi_SizeOut) \
		{ \
			if(!gpsz_OutBuf) \
				gpsz_OutBuf = (char *) L_malloc(gi_SizeOut + 10024L); \
			else \
				gpsz_OutBuf = (char *) L_realloc(gpsz_OutBuf, gi_SizeOut + 10024L); \
			ERR_X_Assert(gpsz_OutBuf != NULL); \
			gpsz_Out = gpsz_OutBuf + gi_SizeOut; \
			gi_SizeOut += 10024L; \
		} \
		*gpsz_Out++ = *psz_Scan++; \
	}

/*
 =======================================================================================================================
    Aim:    Output a complete string (without 0 mark) to output buffer
 =======================================================================================================================
 */
#define OUTS(b) { char	  *p; char	  *p1; p = b; p1 = psz_Scan; psz_Scan = p; while(*psz_Scan) OUTPP(); psz_Scan = p1; }

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *EAI_cl_Compiler::PPLine(char *psz_Mem, char *psz_Scan)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*psz_Beg, *psz_Last;
	char	c_Mem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Analyse the directive : Num line */
	while(*psz_Scan && L_isspace(*psz_Scan)) psz_Scan++;
	ERR_X_Error(*psz_Scan, ERR_COMPILER_Csz_Syntax, NULL);
	psz_Beg = psz_Scan;
	while(*psz_Scan && L_isdigit(*psz_Scan)) psz_Scan++;
	ERR_X_Error(*psz_Scan, ERR_COMPILER_Csz_Syntax, NULL);
	c_Mem = *psz_Scan;
	*psz_Scan = 0;
	e.i_NumLine = L_atol(psz_Beg) - 1;
	*psz_Scan = c_Mem;

	/* File name */
	if(*psz_Scan != '\n' && *psz_Scan != '\a')
	{
		while(*psz_Scan && L_isspace(*psz_Scan)) psz_Scan++;
		ERR_X_Error(*psz_Scan == '"', ERR_COMPILER_Csz_Syntax, NULL);
		psz_Scan++;
		psz_Beg = psz_Scan;
		while(*psz_Scan && *psz_Scan != '"') psz_Scan++;
		c_Mem = *psz_Scan;
		*psz_Scan = 0;

		psz_Last = L_strrchr(psz_Beg, '/');
		if(!psz_Last) psz_Last = L_strrchr(psz_Beg, '\\');
		ERR_X_Error(psz_Last, ERR_COMPILER_Csz_Syntax, NULL);
		*psz_Last = 0;

		L_strcpy(e.asz_Path, psz_Beg);
		L_strcpy(e.asz_File, psz_Last + 1);
		*psz_Last = '/';

		/* Copy the complete line (this is a PP directive that is known by the compiler) */
		*psz_Scan = c_Mem;
		psz_Scan++;
		while(*psz_Scan && (*psz_Scan == ' ' || *psz_Scan == '\t')) psz_Scan++;
		ERR_X_Error(*psz_Scan == '\n', ERR_COMPILER_Csz_Syntax, NULL);

		psz_Scan = psz_Mem - 1;
		while(*psz_Scan && *psz_Scan != '\n') OUTPP();
	}

	return psz_Scan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *EAI_cl_Compiler::PPEnable(char *psz_Scan)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*psz_Mem;
	char		c_Mem;
	POSITION	posdef;
	CString		o_DefName, o_Tmp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Error(*psz_Scan == ' ', ERR_COMPILER_Csz_Syntax, NULL);
	while(L_isspace(*psz_Scan)) psz_Scan++;
	ERR_X_Error(L_isalpha(*psz_Scan), ERR_COMPILER_Csz_Syntax, NULL);

	/* Define name */
	psz_Mem = psz_Scan;
	while(L_isalnum(*psz_Scan)) psz_Scan++;
	c_Mem = *psz_Scan;
	*psz_Scan = 0;

	ERR_X_Error(go_DisableDefNames.Lookup(psz_Mem, (void * &) posdef), ERR_PP_Csz_UnknownConstant, psz_Mem);

	go_DisableDefNames.RemoveKey(psz_Mem);
	*psz_Scan = c_Mem;

	return psz_Scan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *EAI_cl_Compiler::PPUndef(char *psz_Scan)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*psz_Mem;
	char		c_Mem;
	POSITION	posdef;
	CString		o_DefName, o_Tmp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Error(*psz_Scan == ' ', ERR_COMPILER_Csz_Syntax, NULL);
	while(L_isspace(*psz_Scan)) psz_Scan++;
	ERR_X_Error(L_isalpha(*psz_Scan), ERR_COMPILER_Csz_Syntax, NULL);

	/* Define constant */
	psz_Mem = psz_Scan;
	while(L_isalnum(*psz_Scan)) psz_Scan++;
	c_Mem = *psz_Scan;
	*psz_Scan = 0;

	if(go_DefineCstNames.Lookup(psz_Mem, (void * &) posdef))
	{
		go_DefineCstNames.RemoveKey(psz_Mem);
	}
	else if(go_DefineCstNamesGlob.Lookup(psz_Mem, (void * &) posdef))
	{
		go_DefineCstNamesGlob.RemoveKey(psz_Mem);
	}
	else
	{
		ERR_X_Error(0, ERR_PP_Csz_UnknownConstant, psz_Mem);
	}

	*psz_Scan = c_Mem;

	return psz_Scan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *EAI_cl_Compiler::PPInclude(char *psz_Scan)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*psz_Mem, *psz_Beg, *psz_Last;
	POSITION	posdef;
	CString		o_DefName, o_Tmp;
	char		asz_Temp[512];
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Path1[BIG_C_MaxLenPath];
	char		asz_File[BIG_C_MaxLenPath];
	char		asz_File1[BIG_C_MaxLenPath];
	BIG_INDEX	ul_File;
	ULONG		ul_Size;
	char		*pc_BufInclude;
	int			iSizeInit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	iSizeInit = L_strlen(e.psz_Buf);

	/* Get path/file name */
	while(*psz_Scan && *psz_Scan != '"') psz_Scan++;
	ERR_X_Error(*psz_Scan == '"', ERR_COMPILER_Csz_Syntax, NULL);
	psz_Scan++;
	psz_Mem = psz_Scan;
	while(*psz_Scan && *psz_Scan != '"') psz_Scan++;
	ERR_X_Error(*psz_Scan == '"', ERR_COMPILER_Csz_Syntax, NULL);
	*psz_Scan = 0;

	/* Check open file name. posdef is unused */
	ERR_X_Error(!go_FileNames.Lookup(psz_Mem, (void * &) posdef), ERR_PP_Csz_IncludeTwice, psz_Mem);
	go_FileNames.SetAt(psz_Mem, NULL);

	/* Get file name */
	psz_Beg = psz_Mem;
	psz_Last = NULL;
	while(*psz_Beg)
	{
		if((*psz_Beg == '\\') || (*psz_Beg == '/'))
		{
			*psz_Beg = '/';
			psz_Last = psz_Beg;
		}

		psz_Beg++;
	}

	/* If no path, take path of current model */
	if(!psz_Last && mul_CurrentModel != -1)
	{
		BIG_ComputeFullName(BIG_ParentFile(mul_CurrentModel), asz_Path1);
		L_strcpy(asz_File1, BIG_NameFile(mul_CurrentModel));
		*L_strrchr(asz_File1, '.') = 0;
		L_strcat(asz_Path1, "/");
		L_strcat(asz_Path1, asz_File1);
		psz_Last = psz_Mem;
		psz_Mem = asz_Path1;
	}
	else
	{
		*psz_Last = 0;
		psz_Last++;
	}

	ERR_X_Error(psz_Last, ERR_PP_Csz_InvalidPath, NULL);

	/* Open and read include file */
	ul_File = BIG_ul_SearchFileExt(psz_Mem, psz_Last);
	ERR_X_Error(ul_File != BIG_C_InvalidIndex, ERR_PP_Csz_UnkownFile, NULL);
	pc_BufInclude = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_File), &ul_Size);
	if(!pc_BufInclude) 
	{
		*psz_Scan = '\"';
		return psz_Scan + 1;
	}

	/* Remember path && file */
	L_strcpy(asz_Path, psz_Mem);
	L_strcpy(asz_File, psz_Last);

	/*
	 * Mark for compiler. Tell that now it's another file with another line number.
	 * Mark is added before generate include file.
	 */
	if(!mb_GenPP && !mb_ForceGenPP)
	{
		sprintf(asz_Temp, "\n#line 0 \"%s/%s\"\n", psz_Mem, psz_Last);
		OUTS(asz_Temp);
	}

	/* Allocate buffer for source */
	psz_Beg = (char *) L_malloc(2 * iSizeInit + ul_Size + 512 + 2048);

	/* Insert file in source */
	L_strcpy(psz_Beg, pc_BufInclude);

	/*
	 * Insert a mark for compiler. Tell that this is now the correct file (mark is
	 * added a the end of the include file)
	 */
	if(!mb_GenPP && !mb_ForceGenPP)
	{
		sprintf(asz_Temp, "\n#line %d \"%s/%s\"\n", e.i_NumLine, e.asz_Path, e.asz_File);
		L_strcat(psz_Beg, asz_Temp);
	}

	/* The end of the source buffer */
	L_strcat(psz_Beg, e.psz_Buf + (psz_Scan - e.psz_Buf) + 1);

	/* Current line and file is now changed */
	e.i_NumLine = 0;
	L_strcpy(e.asz_Path, asz_Path);
	L_strcpy(e.asz_File, asz_File);

	/* Replace source buffer by new one */
	L_free(e.psz_Buf);
	e.psz_Text = e.psz_Buf = psz_Beg;
	psz_Scan = e.psz_Buf;

	L_free(pc_BufInclude);

	return psz_Scan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *EAI_cl_Compiler::PPDefine(char *psz_Scan)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			*psz_Mem;
	char			*psz_Tmp1;
	char			c_Mem;
	POSITION		posdef;
	CString			o_DefName, o_Tmp;
	int				i_Line, i_MemLine;
	char			asz_Temp[512];
	BOOL			bForMacro;
	tdst_PPMacro	*pmacro;
	int				iNumParen;
	char			cMem;
	char			*pz;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Error(L_isspace(*psz_Scan), ERR_COMPILER_Csz_Syntax, NULL);
	while(L_isspace(*psz_Scan)) psz_Scan++;
	ERR_X_Error(L_isalpha(*psz_Scan), ERR_COMPILER_Csz_Syntax, NULL);

	i_MemLine = e.i_NumLine;

	/* Define constant name (or macro name) */
	psz_Mem = psz_Scan;
	while(L_isalnum(*psz_Scan)) psz_Scan++;
	c_Mem = *psz_Scan;
	*psz_Scan = 0;

	if
	(
		(go_DefineCstNames.Lookup(psz_Mem, (void * &) posdef))
	||	(go_DefineCstNamesGlob.Lookup(psz_Mem, (void * &) posdef))
	)
	{
#ifdef JADEFUSION
		while (*psz_Scan != '\n')
			psz_Scan++;
		return psz_Scan;
#else
		ERR_X_Error(0, ERR_PP_Csz_DefineTwice, psz_Mem);
#endif
	}

	o_DefName = psz_Mem;
	*psz_Scan = c_Mem;

	/* Is it a macro ? (there's an open parenthese just after the define name) */
	bForMacro = FALSE;
	if(c_Mem == '(')
	{
		bForMacro = TRUE;
		psz_Scan++;
		pmacro = new tdst_PPMacro;

		/* Parse all parameters */
		iNumParen = 1;
		while(iNumParen && *psz_Scan)
		{
			while(L_isspace(*psz_Scan)) psz_Scan++;
			psz_Mem = psz_Scan;
			while(*psz_Scan && (*psz_Scan != ','))
			{
				if(*psz_Scan == '(') iNumParen++;
				if(*psz_Scan == ')') iNumParen--;
				if(!iNumParen) break;
				psz_Scan++;
			}

			/* Add the parameter to the list */
			ERR_X_Error(*psz_Scan, ERR_COMPILER_Csz_Syntax, NULL);

			psz_Tmp1 = psz_Scan - 1;
			while(L_isspace(*psz_Tmp1)) psz_Tmp1--;
			if(psz_Tmp1 >= psz_Mem)
			{
				psz_Tmp1++;
				cMem = *psz_Tmp1;
				*psz_Tmp1 = 0;
				pmacro->o_Params.AddTail(psz_Mem);
				*psz_Tmp1 = cMem;
			}

			psz_Scan++;
		}

		ERR_X_Error(*psz_Scan, ERR_COMPILER_Csz_Syntax, NULL);
	}

	/* Define value */
	psz_Mem = psz_Scan;
	i_Line = e.i_NumLine;
	while(*psz_Scan && *psz_Scan != '\n')
	{
		if(*psz_Scan == '\"')
		{
			psz_Scan++;
			while(*psz_Scan && *psz_Scan != '\n' && *psz_Scan != '\"') psz_Scan++;
			if(*psz_Scan == '\"') psz_Scan++;
			continue;
		}

		if(*psz_Scan == '\\')
		{
			*psz_Scan = '\n';
			OUTPP();
			psz_Scan--;
			*psz_Scan = ' ';
			psz_Scan += 2;
			e.i_NumLine++;
		}

		psz_Scan++;
	}

	if(*psz_Scan == '\n')
	{
		OUTPP();
		e.i_NumLine++;
	}

	c_Mem = *psz_Scan;
	*psz_Scan = 0;

	/* Add a #line directive to retreive macro def in case of error inside define */
	if(!mb_GenPP && !mb_ForceGenPP)
	{
//		sprintf(asz_Temp, "\n#breakoff\n#line %d \"%s/%s\"\n", i_Line, e.asz_Path, e.asz_File);
		sprintf(asz_Temp, "\n#breakoff\n");
		o_Tmp = asz_Temp;
		o_Tmp += psz_Mem;
	}
	else
	{
		o_Tmp = psz_Mem;

		/* Kill comments */
		pz = (char *) (LPCSTR) o_Tmp;
		while(*pz)
		{
			if(pz[0] == '/' && pz[1] == '/')
			{
				while(*pz && *pz != '\n') *pz++ = ' ';
                if (!(*pz)) break; // If o_Tmp does not end by '\n'
			}
#ifdef JADEFUSION
			if (*pz == 0)
				break;
#endif
			pz++;
		}
	}

	/* Add the content of the constant or the macro to the corresponding list. */
	if(bForMacro)
	{
		if(!L_strcmpi(e.asz_Path, EDI_Csz_Path_AILib "/Common"))
		{
			pmacro->o_Content = o_Tmp;
			posdef = go_DefineMacrosGlog.AddTail(pmacro);
			go_DefineMacroNamesGlob.SetAt(o_DefName, posdef);
			go_PPMacros.SetAt(o_DefName, (void *) BIG_ul_SearchFileExt(e.asz_Path, e.asz_File));
			go_PPMacrosLines.SetAt(o_DefName, (void *) i_MemLine);
			go_DefineMacrosFilesGlob.SetAt(o_DefName, e.asz_Path);
		}
		else
		{
			pmacro->o_Content = o_Tmp;
			posdef = go_DefineMacros.AddTail(pmacro);
			go_DefineMacroNames.SetAt(o_DefName, posdef);
			go_PPMacros.SetAt(o_DefName, (void *) BIG_ul_SearchFileExt(e.asz_Path, e.asz_File));
			go_PPMacrosLines.SetAt(o_DefName, (void *) i_MemLine);
			go_DefineMacrosFiles.SetAt(o_DefName, e.asz_Path);
		}
	}
	else
	{
		if(!L_strcmpi(e.asz_Path, EDI_Csz_Path_AILib "/Common"))
		{
			posdef = go_DefineCstGlob.AddTail(o_Tmp);
			go_DefineCstNamesGlob.SetAt(o_DefName, posdef);
			if(mb_GenPP || mb_ForceGenPP)
				go_PPConstants.SetAt(o_DefName, o_Tmp);
			else
				go_PPConstants.SetAt(o_DefName, psz_Mem);
			go_PPConstantsFiles.SetAt(o_DefName, (void *) BIG_ul_SearchFileExt(e.asz_Path, e.asz_File));
			go_PPConstantsLines.SetAt(o_DefName, (void *) i_MemLine);
			go_DefineCstFilesGlob.SetAt(o_DefName, e.asz_Path);
		}
		else
		{
			posdef = go_DefineCst.AddTail(o_Tmp);
			go_DefineCstNames.SetAt(o_DefName, posdef);
			if(mb_GenPP || mb_ForceGenPP)
				go_PPConstants.SetAt(o_DefName, o_Tmp);
			else
				go_PPConstants.SetAt(o_DefName, psz_Mem);
			go_PPConstantsFiles.SetAt(o_DefName, (void *) BIG_ul_SearchFileExt(e.asz_Path, e.asz_File));
			go_PPConstantsLines.SetAt(o_DefName, (void *) i_MemLine);
			go_DefineCstFiles.SetAt(o_DefName, e.asz_Path);
		}
	}

	*psz_Scan = c_Mem;

	return psz_Scan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *EAI_cl_Compiler::PPDef(char *psz_Scan, BOOL _b_Def)
{
	/*~~~~~~~~~~~~~*/
	char	*psz_Beg;
	char	*psz_Mem;
	BOOL	b_Find;
	char	c_Mem;
	POSITION	posdef;
	/*~~~~~~~~~~~~~*/

	while(*psz_Scan && L_isspace(*psz_Scan)) psz_Scan++;
	psz_Beg = psz_Scan;
	while(*psz_Scan && L_isalnum(*psz_Scan)) psz_Scan++;
	c_Mem = *psz_Scan;
	*psz_Scan = 0;
	b_Find = go_IfDefConsts.Find(psz_Beg) ? TRUE : FALSE;
	if(!b_Find) b_Find = go_DefineCstNames.Lookup(psz_Beg, (void * &) posdef) ? TRUE : FALSE;
	*psz_Scan = c_Mem;

	if(_b_Def != b_Find)
	{
		while(1)
		{
			if(psz_Scan[0] == '/' && psz_Scan[1] == '/')
				while(*psz_Scan && *psz_Scan != '\n') psz_Scan++;
			if(*psz_Scan == '\n')
			{
				e.i_NumLine++;
				OUTPP();
			}

			if(!(*psz_Scan)) return psz_Scan;

			psz_Beg = psz_Scan;
			if(*psz_Scan == '#')
			{
				psz_Scan++;
				psz_Mem = psz_Scan;
				while(L_isalpha(*psz_Scan)) psz_Scan++;

				c_Mem = *psz_Scan;
				*psz_Scan = 0;
				if(!L_strcmp(psz_Mem, "endif"))
				{
					*psz_Scan = c_Mem;
					return psz_Scan;
				}

				*psz_Scan = c_Mem;
			}

			psz_Scan++;
		}
	}

	return psz_Scan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
char *EAI_cl_Compiler::PPReplaceConstant(char *psz_Scan)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString					o_DefName, o_Tmp, o_Res;
	char					*psz_Beg, *psz_Mem, *psz_Id;
	char					c_Mem;
	POSITION				posdef, posparam, posmacro;
	char					asz_Temp[2048];
	char					asz_Temp1[2048];
	BOOL					bRes;
	BOOL					bForMacro;
	CList<CString, CString> o_MacroParams;
	int						iNumParen;
	tdst_PPMacro			*pmacro;
	char					*psz_1, *psz_2, *psz_3;
	int						sizesize;
	int						len0, len1, len2, len3;
	char					memmem;
	BOOL					glob = FALSE;
	char					*pz;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	psz_Beg = psz_Scan;
	while(L_isalnum(*psz_Scan)) psz_Scan++;
	c_Mem = *psz_Scan;
	*psz_Scan = 0;
	psz_Id = psz_Scan;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Search if it's a constant, a macro or nothing at all...
	 -------------------------------------------------------------------------------------------------------------------
	 */

	bForMacro = FALSE;
	glob = FALSE;
	bRes = go_DefineCstNames.Lookup(psz_Beg, (void * &) posdef);
	if(!bRes) 
	{
		bRes = go_DefineCstNamesGlob.Lookup(psz_Beg, (void * &) posdef);
		glob = TRUE;
	}
	if(!bRes)
	{
		glob = FALSE;
		bRes = go_DefineMacroNames.Lookup(psz_Beg, (void * &) posdef);
		if(!bRes) 
		{
			bRes = go_DefineMacroNamesGlob.Lookup(psz_Beg, (void * &) posdef);
			glob = TRUE;
		}
		if(bRes)
		{
			if(glob)
				pmacro = go_DefineMacrosGlob.GetAt(posdef);
			else
				pmacro = go_DefineMacros.GetAt(posdef);

			/* For a macro, check for an open parenthese, and save all the parameters */
			*psz_Scan = c_Mem;
			while(L_isspace(*psz_Scan)) 
			{
				if(*psz_Scan == '\n') e.i_NumLine++;
				psz_Scan++;
			}
			ERR_X_Error(*psz_Scan == '(', ERR_COMPILER_Csz_Syntax, NULL);
			psz_Scan++;
			*psz_Id = 0;

			/* Parse all parameters */
			iNumParen = 1;
			while(iNumParen && *psz_Scan)
			{
				while(L_isspace(*psz_Scan)) 
				{
					if(*psz_Scan == '\n') e.i_NumLine++;
					psz_Scan++;
				}

				psz_Mem = psz_Scan;
				while(*psz_Scan && ((*psz_Scan != ',') || (iNumParen > 1)))
				{
					if(*psz_Scan == '(') iNumParen++;
					if(*psz_Scan == ')') iNumParen--;
					if(!iNumParen) break;
					if(*psz_Scan == '\n') e.i_NumLine++;
					psz_Scan++;
				}

				/* Add the parameter to the list */
				ERR_X_Error(*psz_Scan, ERR_COMPILER_Csz_Syntax, NULL);
				c_Mem = *psz_Scan;
				*psz_Scan = 0;
				o_MacroParams.AddTail(psz_Mem);
				*psz_Scan = c_Mem;
				psz_Scan++;
			}

			ERR_X_Error(o_MacroParams.GetCount() == pmacro->o_Params.GetCount(), ERR_PP_Csz_BadNumParamMacro, psz_Beg);

			/* Now we must replace all macro parameters with caller parameters */
			o_DefName = pmacro->o_Content;

			psz_1 = (char *) (LPCSTR) o_DefName;
			while(*psz_1)
			{
recom:
				/* Zap comments */
				if(psz_1[0] == '/' && psz_1[1] == '/')
				{
					while(*psz_1 && *psz_1 != '\n') psz_1++;
					continue;
				}

				/* Zap pp directives */
				if(psz_1[0] == '#')
				{
					psz_2 = psz_1;
					while(*psz_1 && *psz_1 != '\n') psz_1++;
					memmem = *psz_1;
					*psz_1 = 0;
					o_Res += psz_2;
					*psz_1 = memmem;
					continue;
				}

				if(psz_1[0] == '\n')
				{
					o_Res += '\n';
					psz_1++;
					continue;
				}

				if(!L_isalnum(*psz_1))
				{
					psz_2 = psz_1;
					while(*psz_1 && !L_isalnum(*psz_1)) psz_1++;
					memmem = *psz_1;
					*psz_1 = 0;
					o_Res += psz_2;
					*psz_1 = memmem;
					continue;
				}

				posparam = pmacro->o_Params.GetHeadPosition();
				posmacro = o_MacroParams.GetHeadPosition();
				while(posparam)
				{
					psz_2 = (char *) (LPCSTR) pmacro->o_Params.GetNext(posparam);
					psz_3 = (char *) (LPCSTR) o_MacroParams.GetNext(posmacro);
					len0 = L_strlen(psz_2);
					if(!L_strncmp(psz_1, psz_2, len0))
					{
						psz_2 = psz_1 + len0;
						if(!L_isalnum(*psz_2))
						{
							o_Res += psz_3;
							psz_1 = psz_2;
							goto recom;
						}
					}
				}

				/* Add the alnum character that is not a macro parameter */
				psz_2 = psz_1;
				while(L_isalnum(*psz_1)) psz_1++;
				while(*psz_1 && !L_isalnum(*psz_1)) psz_1++;
				memmem = *psz_1;
				*psz_1 = 0;
				o_Res += psz_2;
				*psz_1 = memmem;
			}

			o_DefName = o_Res;
		}
	}
	else
	{
		/* A single constant. Retreive content of define */
		if(glob)
			o_DefName = go_DefineCstGlob.GetAt(posdef);
		else
			o_DefName = go_DefineCst.GetAt(posdef);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Make the replacement (constant or macro). o_DefName must be the string to insert.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(bRes)
	{
		/* Check if name is forbidden (to avoid recurse replacement) */
		ERR_X_Error(!go_DisableDefNames.Lookup(psz_Beg, (void * &) posdef), ERR_PP_Csz_RecurseReplace, NULL);

		/* This define name is disabled until the end of the macro replacement */
		go_DisableDefNames.SetAt(psz_Beg, (void * &) posdef);

		/* Compute the enable mark */
		len0 = o_DefName.GetLength();
		sprintf(asz_Temp, "#enable %s", psz_Beg);
		len1 = L_strlen(asz_Temp);
		*psz_Id = c_Mem;
		sprintf(asz_Temp1, "\n#breakon\n#line %d \"%s/%s\"\n", e.i_NumLine, e.asz_Path, e.asz_File);
		len2 = L_strlen(asz_Temp1);

		if(mb_GenPP || mb_ForceGenPP)
		{
			pz = (char *) (LPCSTR) o_DefName;
			if(pz[len0 - 1] == '\n') pz[len0 - 1] = ' ';
			len2 = 0;
			*asz_Temp1 = 0;
		}

		sizesize = psz_Scan - e.psz_Buf;
		if(sizesize > len0 + len1 + len2)
		{
			psz_Beg = psz_Scan - (len0 + len1 + len2);
			L_memcpy(psz_Beg, (char *) (LPCSTR) o_DefName, len0);
			L_memcpy(psz_Beg + len0, asz_Temp, len1);
			L_memcpy(psz_Beg + len0 + len1, asz_Temp1, len2);
			psz_Scan = psz_Beg;
		}
		else
		{
			/* Insert constant in source */
			len3 = L_strlen(e.psz_Buf);
			psz_Beg = (char *) L_malloc(len3 + len0 + len1 + len2 + 1024L);

			L_memcpy(psz_Beg, (char *) (LPCSTR) o_DefName, len0);
			L_memcpy(psz_Beg + len0, asz_Temp, len1);
			L_memcpy(psz_Beg + len0 + len1, asz_Temp1, len2);
			L_memcpy(psz_Beg + len0 + len1 + len2, e.psz_Buf + (psz_Scan - e.psz_Buf), len3 - sizesize + 1);
			L_free(e.psz_Buf);

			/*
			 * We continue to parse at the beginning of the inserted constant. This is quit
			 * different of the C preprocessor. It permit to add preprocessor directives in
			 * macros, but it can be recursive if a macro contains a call to itselve.
			 */
			e.psz_Text = e.psz_Buf = psz_Beg;
			psz_Scan = e.psz_Buf;
		}
		return psz_Scan;
	}

	/* Copy name to output */
	*psz_Scan = c_Mem;
	psz_Mem = psz_Scan;
	psz_Scan = psz_Beg;
	while(psz_Scan != psz_Mem) OUTPP();

	return psz_Scan;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::ResetSystemVar(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void Eliminate(void)
{
	go_DefineCst.RemoveAll();
	go_DefineCstFiles.RemoveAll();
	go_DefineCstNames.RemoveAll();
	go_DefineMacros.RemoveAll();
	go_DefineMacrosFiles.RemoveAll();
	go_DefineMacroNames.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::PP(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*psz_Scan, *psz_Beg, *psz_Mem;
	char		c_Mem;
	char		asz_Temp[512];
	CString		o_Str, o_Str1, o_Str2;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	Eliminate();
	go_FileNames.RemoveAll();
	go_DisableDefNames.RemoveAll();

	/* Add the current file to the list of included one (to avoid an auto reference) */
	L_strcpy(asz_Temp, e.asz_Path);
	L_strcat(asz_Temp, "/");
	L_strcat(asz_Temp, e.asz_File);
	go_FileNames.SetAt(asz_Temp, NULL);

	/* Default constant include file */
	if(go_DefineCstNamesGlob.IsEmpty())
	{
		if(BIG_ul_SearchFileExt(EDI_Csz_Path_AILib "/Common", "System.var") != BIG_C_InvalidIndex)
		{
			if(mb_GenPP || mb_ForceGenPP)
			{
				sprintf
				(
					asz_Temp,
					"#include \"%s/System.var\"\n",
					EDI_Csz_Path_AILib "/Common"
				);
			}
			else
			{
				sprintf
				(
					asz_Temp,
					"#include \"%s/System.var\"\n#line 0 \"%s/%s\"\n",
					EDI_Csz_Path_AILib "/Common",
					e.asz_Path,
					e.asz_File
				);
			}
		}
		else if(BIG_ul_SearchFileExt(EDI_Csz_Path_AILib, "System.var") != BIG_C_InvalidIndex)
		{
			if(mb_GenPP || mb_ForceGenPP)
			{
				sprintf
				(
					asz_Temp,
					"#include \"%s/System.var\"\n",
					EDI_Csz_Path_AILib
				);
			}
			else
			{
				sprintf
				(
					asz_Temp,
					"#include \"%s/System.var\"\n#line 0 \"%s/%s\"\n",
					EDI_Csz_Path_AILib,
					e.asz_Path,
					e.asz_File
				);
			}
		}
		else
			goto r;
		e.psz_Buf = (char *) L_malloc(L_strlen(e.psz_Text) + L_strlen(asz_Temp) + 512);
		L_strcpy(e.psz_Buf, asz_Temp);
		L_strcat(e.psz_Buf, e.psz_Text);
		e.psz_Text = e.psz_Buf;
	}

	/* Duplicate source buffer, and set psz_Scan to beginning */
	else
	{
r:
		e.psz_Buf = e.psz_Text = L_strdup(e.psz_Text);
	}

	psz_Scan = e.psz_Buf;

	/* Allocate output PP buffer */
	gpsz_OutBuf = gpsz_Out = L_strdup(e.psz_Buf);
	gi_SizeOut = L_strlen(e.psz_Buf);

	e.i_NumLine = 0;
	while(*psz_Scan)
	{
		if(psz_Scan[0] == '/' && psz_Scan[1] == '/')
			while(*psz_Scan && *psz_Scan != '\n') OUTPP();
		if(*psz_Scan == '\n') e.i_NumLine++;
		if(!(*psz_Scan))
			break;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Search a PP mark
		 ---------------------------------------------------------------------------------------------------------------
		 */

		psz_Beg = psz_Scan;
		if(*psz_Scan == '#')
		{
			/* Search the word after the PP mark */
			psz_Scan++;
			psz_Mem = psz_Scan;
			while(L_isalpha(*psz_Scan)) psz_Scan++;

			c_Mem = *psz_Scan;
			*psz_Scan = 0;

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(!L_strcmp(psz_Mem, "ifndef"))
			{
				*psz_Scan = c_Mem;
				psz_Scan = PPDef(psz_Scan, FALSE);
				continue;
			}

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(!L_strcmp(psz_Mem, "ifdef"))
			{
				*psz_Scan = c_Mem;
				psz_Scan = PPDef(psz_Scan, TRUE);
				continue;
			}

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(!L_strcmp(psz_Mem, "define"))
			{
				*psz_Scan = c_Mem;
				psz_Scan = PPDefine(psz_Scan);
				continue;
			}

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(!L_strcmp(psz_Mem, "include"))
			{
				*psz_Scan = c_Mem;
				psz_Scan = PPInclude(psz_Scan);
				continue;
			}

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(!L_strcmp(psz_Mem, "line"))
			{
				*psz_Scan = c_Mem;
				psz_Scan = PPLine(psz_Mem, psz_Scan);
				continue;
			}

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(!L_strcmp(psz_Mem, "enable"))
			{
				*psz_Scan = c_Mem;
				psz_Scan = PPEnable(psz_Scan);
				continue;
			}

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(!L_strcmp(psz_Mem, "endif"))
			{
				*psz_Scan = c_Mem;
				continue;
			}

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if(!L_strcmp(psz_Mem, "undef"))
			{
				*psz_Scan = c_Mem;
				psz_Scan = PPUndef(psz_Scan);
				continue;
			}

			/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

			if((!L_strcmp(psz_Mem, "breakoff")) || (!L_strcmp(psz_Mem, "breakon")))
			{
				OUTS("#");
				OUTS(psz_Mem);
				*psz_Scan = c_Mem;
				continue;
			}

			ERR_X_ForceErrorThrow(ERR_PP_Csz_InvalidDirective, NULL);
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Zap string
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(*psz_Scan == '"')
		{
			OUTPP();
			while(*psz_Scan && *psz_Scan != '"') OUTPP();
			if(!*psz_Scan) break;
			OUTPP();
			continue;
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Search a word to replace by a define constant
		 ---------------------------------------------------------------------------------------------------------------
		 */

		if(L_isalpha(*psz_Scan))
		{
			psz_Scan = PPReplaceConstant(psz_Scan);
			continue;
		}

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    Normal: Simply copy to out
		 ---------------------------------------------------------------------------------------------------------------
		 */

		OUTPP();
	}

	/* Set new buf */
	c_Mem = 0;
	psz_Scan = &c_Mem;
	OUTPP();
	L_free(e.psz_Buf);
	e.psz_Buf = e.psz_Text = gpsz_OutBuf;
	e.i_NumLine = 0;
}

#endif /* ACTIVE_EDITORS */
