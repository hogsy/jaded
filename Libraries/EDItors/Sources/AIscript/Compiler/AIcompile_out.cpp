/*$T AIcompile_out.cpp GC! 1.100 03/20/01 16:08:00 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLImem.h"
#include "../AIframe.h"
#include "../AIerrid.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIstruct.h"
#include "AIinterp/Sources/AIBench.h"
#include "LINKs/LINKmsg.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGkey.h"
#include "EDIpaths.h"
#include "BASe/MEMory/MEM.h"
#include "../EditView/AIleftview.h"

extern void		AI2C_FuncToC(BIG_INDEX, AI_tdst_Function *);
BOOL			AI2C_gb_Activated = FALSE;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Compiler::b_IsBreakPointAtLine(int _i_Line)
{
	/*~~~~~~~~~~~~~~~~~~*/
	POSITION	pos, pos1;
	int			i_Line;
	BIG_INDEX	ul_Index;
	/*~~~~~~~~~~~~~~~~~~*/

	if(mb_GlobalVarMode) return FALSE;
	pos = mpo_Frame->mpo_LeftView->mo_ListBreak.GetHeadPosition();
	pos1 = mpo_Frame->mpo_LeftView->mo_ListBreakFile.GetHeadPosition();
	while(pos)
	{
		i_Line = mpo_Frame->mpo_LeftView->mo_ListBreak.GetNext(pos);
		ul_Index = BIG_ul_SearchKeyToFat(mpo_Frame->mpo_LeftView->mo_ListBreakFile.GetNext(pos1));
		if((i_Line == _i_Line) && (ul_Index == mpo_Frame->mul_CurrentEditFile)) return TRUE;
	}

	return FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::OutputStr(char *pz)
{
	/*~~~~~~~~*/
	int		len;
	ULONG	ul, ul1;
	/*~~~~~~~~*/

	len = (strlen(pz) / 8) + 1;
	Output(0, len, 0);	/* Nombre de paquets */

	/* On doit découper le nom en paquets de ULONG */
	len = (strlen(pz) / 4) + 1;
	while(len)
	{
		if(len == 1)
		{
			ul = 0;
			if(pz[2] && pz[1] && pz[0])
			{
				ul <<= 8;
				ul += pz[2];
			}

			if(pz[1] && pz[0])
			{
				ul <<= 8;
				ul += pz[1];
			}

			ul <<= 8;
			ul += *pz;
		}
		else
		{
			ul = *(ULONG *) pz;
		}

		len--;
		pz += 4;

		if(len == 1)
		{
			ul1 = 0;
			if(pz[2] && pz[1] && pz[0])
			{
				ul1 <<= 8;
				ul1 += pz[2];
			}

			if(pz[1] && pz[0])
			{
				ul1 <<= 8;
				ul1 += pz[1];
			}

			ul1 <<= 8;
			ul1 += *pz;
		}
		else if(len)
		{
			ul1 = *(ULONG *) pz;
		}

		Output((ul1 >> 24) & 0xFF, ul, ul1 & 0xFFFF /*((ul1 & 255) << 8) | ((ul1 & 0xFF00) >> 8)*/, (ul1 >> 16) & 0xFF);
		if(len) len--;
		pz += 4;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::Output(int _c_Type, int _l_Param, int _w_Param, int _c_Flags)
{
	/*~~~~~~~~~~~~~~*/
	char	*psz_Temp;
	/*~~~~~~~~~~~~~~*/

	if(!mb_CanOutput) return;

	/* If we want to set a breakpoint for that node */
	if(mb_NeedBreak) _c_Flags |= AI_Cflag_BreakPoint;

	/* Allocate/Reallocate output buffer */
	if(e.i_CurrentOutput == e.i_MaxOutput)
	{
		e.i_MaxOutput += 100;
		if(e.i_MaxOutput == 100)
		{
			e.pst_Output = (AI_tdst_Node *) MEM_p_Alloc(e.i_MaxOutput * sizeof(AI_tdst_Node));
			e.pst_OtherOutput = (AI_tdst_Node *) MEM_p_Alloc(e.i_MaxOutput * sizeof(AI_tdst_Node));
		}
		else
		{
			e.pst_Output = (AI_tdst_Node *) MEM_p_Realloc(e.pst_Output, e.i_MaxOutput * sizeof(AI_tdst_Node));
			e.pst_OtherOutput = (AI_tdst_Node *) MEM_p_Realloc(e.pst_OtherOutput, e.i_MaxOutput * sizeof(AI_tdst_Node));
		}
	}

	/* Init node */
	e.pst_Output[e.i_CurrentOutput].l_Param = (LONG) _l_Param;
	e.pst_Output[e.i_CurrentOutput].w_Param = (short) _w_Param;
	e.pst_Output[e.i_CurrentOutput].c_Type = (char) _c_Type;
	e.pst_Output[e.i_CurrentOutput].c_Flags = (char) _c_Flags;

	/* Init dummy node for debug */
	if(e.b_TopNode)
	{
		e.pst_OtherOutput[e.i_CurrentOutput].l_Param = e.i_NumLineBreak;

		/* To debug with an include .fct */
		psz_Temp = L_strrchr(e.asz_File, '.');
		if(psz_Temp && !L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFct))
		{
			*((LONG *) &e.pst_OtherOutput[e.i_CurrentOutput].w_Param) = BIG_FileKey(BIG_ul_SearchFileExt(e.asz_Path, e.asz_File));
		}
		else
			*((LONG *) &e.pst_OtherOutput[e.i_CurrentOutput].w_Param) = BIG_FileKey(mul_CurrentFile);
	}
	else
	{
		e.pst_OtherOutput[e.i_CurrentOutput].l_Param = 0xFFFFFFFF;
		*((LONG *) &e.pst_OtherOutput[e.i_CurrentOutput].w_Param) = 0xFFFFFFFF;
	}

	e.i_CurrentOutput++;

	/* Reset breakmode */
	mb_NeedBreak = FALSE;
	e.b_TopNode = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GenerateOutput(AI_tdst_Function *_pst_Func)
{
	if(!mb_CanOutput) return;

	/* Generation : To log */
	if(mb_CompileToLog) GenerateOutputLog(_pst_Func);

	/* Generation : Nodes (except for global vars) */
	if(!mb_GlobalVarMode) GenerateOutputFile(_pst_Func);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BIG_KEY EAI_cl_Compiler::ul_GetEngineFileForForFunction(BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*psz_Temp;
	char		asz_Name[BIG_C_MaxLenPath];
	char		asz_Temp[BIG_C_MaxLenPath];
	BIG_INDEX	ul_File;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_File = BIG_ul_SearchKeyToFat(_ul_Key);
	ERR_X_Assert(ul_File != BIG_C_InvalidIndex);
	L_strcpy(asz_Name, BIG_NameFile(ul_File));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtAIEngineFct);
	BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Temp);
	ul_File = BIG_ul_SearchFileExt(asz_Temp, asz_Name);
	if(ul_File == BIG_C_InvalidIndex) return BIG_C_InvalidIndex;
	return BIG_FileKey(ul_File);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BIG_KEY EAI_cl_Compiler::ul_GetEngineFileForForProcList(BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char		*psz_Temp;
	char		asz_Name[BIG_C_MaxLenPath];
	char		asz_Temp[BIG_C_MaxLenPath];
	BIG_INDEX	ul_File;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_File = BIG_ul_SearchKeyToFat(_ul_Key);
	ERR_X_Assert(ul_File != BIG_C_InvalidIndex);
	L_strcpy(asz_Name, BIG_NameFile(ul_File));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtAIEngineFctLib);
	BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Temp);
	ul_File = BIG_ul_SearchFileExt(asz_Temp, asz_Name);
	if(ul_File == BIG_C_InvalidIndex) return BIG_C_InvalidIndex;
	return BIG_FileKey(ul_File);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG EAI_cl_Compiler::ul_GetEngineModelIndex(BIG_INDEX _ul_EditorModel, char *_psz_Path, char *_psz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	*psz_Temp;
	char	asz_Name[BIG_C_MaxLenPath];
	char	asz_Dir[BIG_C_MaxLenPath];
	char	asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_strcpy(asz_Name, BIG_NameFile(_ul_EditorModel));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcpy(asz_Dir, asz_Name);
	L_strcat(asz_Name, EDI_Csz_ExtAIEngineModel);
	BIG_ComputeFullName(BIG_ParentFile(_ul_EditorModel), asz_Path);
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, asz_Dir);
	if(_psz_Path) L_strcpy(_psz_Path, asz_Path);
	if(_psz_Name) L_strcpy(_psz_Name, asz_Name);
	return BIG_ul_SearchFileExt(asz_Path, asz_Name);
}

/*
 =======================================================================================================================
    Aim:    Save file for global variables of model.
 =======================================================================================================================
 */
void EAI_cl_Compiler::GenerateOutputVars(BIG_INDEX _ul_Model)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EAI_cl_Variable *po_Var;
	CString			o_Name;
	char			*psz_Temp;
	char			asz_Name[BIG_C_MaxLenPath];
	char			asz_Temp[BIG_C_MaxLenPath];
	int				i, j, k, l, i_Size, i_SizeInit, i_Size1, i_Num, i_Max, i_Offset;
	int				i_Size2, i_Size3, i_SizeR;
	BOOL			bCanInc;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	L_memset(&mst_VarDes, 0, sizeof(AI_tdst_VarDes));

	L_strcpy(asz_Name, BIG_NameFile(_ul_Model));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;

	/* Force init func to 0 */
	L_memset(maul_InitFct, 0, sizeof(LONG) * AI_C_MaxTracks);
	L_memset(maul_Callbacks, 0, sizeof(LONG) * AI_C_MaxCallbacks);
	mul_MaxCallbacks = -1;
	L_memset(&mst_VarDes, 0, sizeof(AI_tdst_VarDes));

	/* Compute model name */
	BIG_ComputeFullName(BIG_ParentFile(_ul_Model), asz_Temp);
	L_strcat(asz_Temp, "/");
	L_strcat(asz_Temp, asz_Name);

	/* Compute output file name */
	L_strcat(asz_Name, EDI_Csz_ExtAIEngineVars);
	SAV_Begin(asz_Temp, asz_Name);

	/* Compute elements that will appeared in variable list (extract special vars) */
	pos = mst_GlobalVars.o_Vars.GetStartPosition();
	i = 0;
	i_SizeInit = 0;
	i_Size2 = i_Size3 = 0;
	while(pos)
	{
		mst_GlobalVars.o_Vars.GetNextAssoc(pos, o_Name, (void * &) po_Var);
		if
		(
			(!(po_Var->mi_SpecialVar & VAR_TRACK))
		&&	(!(po_Var->mi_SpecialVar & VAR_SEP))
		)
		{
			i++;
			if(po_Var->mi_SizeArray)
			{
				i_Max = po_Var->mai_SizeElem[0];
				for(j = 1; j < po_Var->mi_SizeArray; j++) i_Max *= po_Var->mai_SizeElem[j];
				i_SizeInit += (i_Max * EAI_gast_Types[AI_gaw_EnumLink[po_Var->mi_Type]].i_Size);
				i_SizeInit += po_Var->mi_SizeArray * sizeof(int);
			}
			else
			{
				i_SizeInit += EAI_gast_Types[AI_gaw_EnumLink[po_Var->mi_Type]].i_Size;
			}
		}

		/* Extended strings ? */
		if((po_Var->mi_SpecialVar & VAR_STRING1) || (po_Var->mi_SpecialVar & VAR_STRING2))
		{
			i_Size2 += sizeof(AI_tdst_EditorVarInfo2);
			if(po_Var->mi_SpecialVar & VAR_STRING1) i_Size3 += L_strlen((char *) (LPCSTR) po_Var->mo_String1) + 1;
			if(po_Var->mi_SpecialVar & VAR_STRING2) i_Size3 += L_strlen((char *) (LPCSTR) po_Var->mo_String2) + 1;
		}
	}

	/* Allocate var description */
	mst_VarDes.ul_NbVars = (ULONG) i;
	i_Size = i * sizeof(AI_tdst_VarInfo);
	i_SizeR = i * 12;
	if(i_Size)
	{
		mst_VarDes.pst_VarsInfos = (AI_tdst_VarInfo *) MEM_p_Alloc(i_Size);
		L_memset(mst_VarDes.pst_VarsInfos, 0, i_Size);
	}

	i_Size1 = i * sizeof(AI_tdst_EditorVarInfo);
	if(i_Size1)
	{
		mst_VarDes.pst_EditorVarsInfos = (AI_tdst_EditorVarInfo *) MEM_p_Alloc(i_Size1);
		L_memset(mst_VarDes.pst_EditorVarsInfos, 0, i_Size1);
	}

	mst_VarDes.ul_NbVarsInfos2 = 0;
	if(i_Size2)
	{
		mst_VarDes.pst_EditorVarsInfos2 = (AI_tdst_EditorVarInfo2 *) MEM_p_Alloc(i_Size2);
		L_memset(mst_VarDes.pst_EditorVarsInfos2, 0, i_Size2);
	}

	/* Init all vars descriptions */
	pos = mst_GlobalVars.o_Vars.GetStartPosition();
	i = 0;
	j = 0;
lzap:
	while(pos && mst_VarDes.pst_VarsInfos)
	{
		mst_GlobalVars.o_Vars.GetNextAssoc(pos, o_Name, (void * &) po_Var);

		/* Flags */
		if(mst_VarDes.pst_VarsInfos)
		{
			if((po_Var->mi_SpecialVar & VAR_SEP) && !pos) i--;
			if((po_Var->mi_SpecialVar & VAR_TRACK) && !pos) i--;
			if((ULONG) i >= mst_VarDes.ul_NbVars) i = mst_VarDes.ul_NbVars - 1;

			if
			(
				!(po_Var->mi_SpecialVar & VAR_TRACK)
			&&	!(po_Var->mi_SpecialVar & VAR_SEP)
			) mst_VarDes.pst_VarsInfos[i].w_Flags = 0;

			if(!(po_Var->mi_SpecialVar & VAR_TRACK))
			{
				if(po_Var->mi_SpecialVar & VAR_PRIVATE) mst_VarDes.pst_VarsInfos[i].w_Flags |= AI_CVF_Private;
				if(po_Var->mi_SpecialVar & VAR_SAVE) mst_VarDes.pst_VarsInfos[i].w_Flags |= AI_CVF_Save;
				if(po_Var->mi_SpecialVar & VAR_ENUM) mst_VarDes.pst_VarsInfos[i].w_Flags |= AI_CVF_Enum;
				if(po_Var->mi_SpecialVar & VAR_SEP) mst_VarDes.pst_VarsInfos[i].w_Flags |= AI_CVF_Sep;
				if(po_Var->mi_SpecialVar & VAR_STRING2) mst_VarDes.pst_VarsInfos[i].w_Flags |= AI_CVF_Help;
				if(po_Var->mi_SpecialVar & VAR_REINIT) mst_VarDes.pst_VarsInfos[i].w_Flags |= AI_CVF_Reinit;
				if(po_Var->mi_SpecialVar & VAR_SAVEAL) mst_VarDes.pst_VarsInfos[i].w_Flags |= AI_CVF_SaveAl;
				if(po_Var->mi_SpecialVar & VAR_OPTIM) mst_VarDes.pst_VarsInfos[i].w_Flags |= AI_CVF_Pointer;
			}
		}

		/* Infos 2 */
		if(po_Var->mi_SpecialVar & VAR_STRING1)
		{
			if(po_Var->mi_SpecialVar & VAR_ENUM)
			{
				for(l = 0; l < j; l++)
				{
					if
					(
						(mst_VarDes.pst_EditorVarsInfos2[l].i_Offset == po_Var->mi_Pos)
					&&	(mst_VarDes.pst_EditorVarsInfos2[l].uw_Flags & AI_CVF_Sep)
					&&	(mst_VarDes.pst_EditorVarsInfos2[l].psz_StringCst)
					)
					{
						psz_Temp = (char *) MEM_p_Alloc(L_strlen(mst_VarDes.pst_EditorVarsInfos2[l].psz_StringCst) + L_strlen((char *) (LPCSTR) po_Var->mo_String1) + 10);
						L_strcpy(psz_Temp, mst_VarDes.pst_EditorVarsInfos2[l].psz_StringCst);
						L_strcat(psz_Temp, "¤");
						L_strcat(psz_Temp, (char *) (LPCSTR) po_Var->mo_String1);
						if(l == j) MEM_Free(mst_VarDes.pst_EditorVarsInfos2[l].psz_StringCst);
						goto com;
					}
				}
			}
			else
			{
				/* Separator with the same offset ? */
				for(l = 0; l < j; l++)
				{
					if
					(
						(mst_VarDes.pst_EditorVarsInfos2[l].i_Offset == po_Var->mi_Pos)
					&&	(mst_VarDes.pst_EditorVarsInfos2[l].uw_Flags & AI_CVF_Sep)
					&&	(mst_VarDes.pst_EditorVarsInfos2[l].psz_StringCst)
					)
					{
						goto lzap;
					}
				}
			}

			psz_Temp = (char *) MEM_p_Alloc(L_strlen((char *) (LPCSTR) po_Var->mo_String1) + 1);
			L_strcpy(psz_Temp, (char *) (LPCSTR) po_Var->mo_String1);
com:
			mst_VarDes.pst_EditorVarsInfos2[j].uw_Flags = mst_VarDes.pst_VarsInfos[i].w_Flags;
			mst_VarDes.pst_EditorVarsInfos2[j].i_Offset = po_Var->mi_Pos;
			mst_VarDes.pst_EditorVarsInfos2[j].psz_StringCst = psz_Temp;
		}

		if(po_Var->mi_SpecialVar & VAR_STRING2)
		{
			mst_VarDes.pst_EditorVarsInfos2[j].uw_Flags = mst_VarDes.pst_VarsInfos[i].w_Flags;
			psz_Temp = (char *) MEM_p_Alloc(L_strlen((char *) (LPCSTR) po_Var->mo_String2) + 1);
			L_strcpy(psz_Temp, (char *) (LPCSTR) po_Var->mo_String2);
			mst_VarDes.pst_EditorVarsInfos2[j].i_Offset = po_Var->mi_Pos;
			mst_VarDes.pst_EditorVarsInfos2[j].psz_StringHelp = psz_Temp;
		}

		if((po_Var->mi_SpecialVar & VAR_STRING1) || (po_Var->mi_SpecialVar & VAR_STRING2)) j++;
		if((po_Var->mi_SpecialVar & VAR_TRACK) || (po_Var->mi_SpecialVar & VAR_SEP)) continue;

		/* Number of elements of the variable */
		mst_VarDes.pst_VarsInfos[i].i_NumElem = 1;
		if(po_Var->mi_SizeArray)
		{
			i_Max = po_Var->mai_SizeElem[0];
			for(k = 1; k < po_Var->mi_SizeArray; k++)
				i_Max *= po_Var->mai_SizeElem[k];

			/* Number of elements if number of values */
			mst_VarDes.pst_VarsInfos[i].i_NumElem = i_Max;

			/* Save the number of dimensions in high bits */
			mst_VarDes.pst_VarsInfos[i].i_NumElem += (po_Var->mi_SizeArray << 30);
		}

		mst_VarDes.pst_VarsInfos[i].i_Offset = po_Var->mi_Pos;
		mst_VarDes.pst_VarsInfos[i].w_Type = po_Var->mi_Type;
		if(o_Name.GetLength() >= AI_C_MaxLenVar)
		{
			L_memcpy(mst_VarDes.pst_EditorVarsInfos[i].asz_Name, (char *) (LPCSTR) o_Name, AI_C_MaxLenVar - 1);
			mst_VarDes.pst_EditorVarsInfos[i].asz_Name[AI_C_MaxLenVar - 1] = 0;
		}
		else
			L_strcpy(mst_VarDes.pst_EditorVarsInfos[i].asz_Name, (char *) (LPCSTR) o_Name);
		i++;
	}

	/* Save variable description (engine) */
	i_SizeR = mst_VarDes.ul_NbVars * 12;
	SAV_Buffer(&i_SizeR, sizeof(int));
	if(i_SizeR) 
	{
		int kk;
		for(kk = 0; kk < mst_VarDes.ul_NbVars; kk++)
		{
			SAV_Buffer(&mst_VarDes.pst_VarsInfos[kk].i_Offset, 4);
			SAV_Buffer(&mst_VarDes.pst_VarsInfos[kk].i_NumElem, 4);
			SAV_Buffer(&mst_VarDes.pst_VarsInfos[kk].w_Type, 2);
			SAV_Buffer(&mst_VarDes.pst_VarsInfos[kk].w_Flags, 2);
		}

//		SAV_Buffer(mst_VarDes.pst_VarsInfos, i_Size);
	}

	/* Save variable description (editor) */
	SAV_Buffer(&i_Size1, sizeof(int));
	if(i_Size1) SAV_Buffer(mst_VarDes.pst_EditorVarsInfos, i_Size1);

	/* Save variable description2 (editor) */
	SAV_Buffer(&i_Size2, sizeof(int));	/* Size fo infos 2 */
	SAV_Buffer(&i_Size3, sizeof(int));	/* Size of string buf after it */
	mst_VarDes.ul_NbVarsInfos2 = i_Size2 / sizeof(AI_tdst_EditorVarInfo2);
	if(i_Size2)
	{
		SAV_Buffer(mst_VarDes.pst_EditorVarsInfos2, i_Size2);
		for(i = 0; (ULONG) i < i_Size2 / sizeof(AI_tdst_EditorVarInfo2); i++)
		{
			if(mst_VarDes.pst_EditorVarsInfos2[i].psz_StringCst)
			{
				SAV_Buffer
				(
					mst_VarDes.pst_EditorVarsInfos2[i].psz_StringCst,
					L_strlen(mst_VarDes.pst_EditorVarsInfos2[i].psz_StringCst) + 1
				);
			}

			if(mst_VarDes.pst_EditorVarsInfos2[i].psz_StringHelp)
			{
				SAV_Buffer
				(
					mst_VarDes.pst_EditorVarsInfos2[i].psz_StringHelp,
					L_strlen(mst_VarDes.pst_EditorVarsInfos2[i].psz_StringHelp) + 1
				);
			}
		}
	}

	/* Save all init */
	SAV_Buffer(&i_SizeInit, sizeof(int));
	if(i_SizeInit)
	{
		mst_VarDes.pc_BufferInit = (char *) MEM_p_Alloc(i_SizeInit);
		L_memset(mst_VarDes.pc_BufferInit, 0, i_SizeInit);
	}

	mst_VarDes.ul_SizeBufferInit = (ULONG) i_SizeInit;

	i = 0;
	pos = mst_GlobalVars.o_Vars.GetStartPosition();
	while(pos)
	{
		mst_GlobalVars.o_Vars.GetNextAssoc(pos, o_Name, (void * &) po_Var);
		if(po_Var->mi_SpecialVar & VAR_SEP) continue;

		/* Array */
		i_Max = 1;
		i_Offset = 0;
		if(po_Var->mi_SizeArray)
		{
			i_Max = po_Var->mai_SizeElem[0];
			for(j = 1; j < po_Var->mi_SizeArray; j++) i_Max *= po_Var->mai_SizeElem[j];
			i_Offset = po_Var->mi_SizeArray * sizeof(int);

			/* Copy size of each dim */
			if(po_Var->mpac_InitValue)
			{
				L_memcpy
				(
					mst_VarDes.pc_BufferInit + mst_VarDes.pst_VarsInfos[i].i_Offset,
					po_Var->mpac_InitValue,
					4 * po_Var->mi_SizeArray
				);
			}
			else
			{
				for(j = 0; j < po_Var->mi_SizeArray; j++)
				{
					((ULONG *) (mst_VarDes.pc_BufferInit + mst_VarDes.pst_VarsInfos[i].i_Offset))[j] = po_Var->mai_SizeElem[j];
				}
			}
		}

		/* Second pass to change some init values */
		bCanInc = TRUE;
		while(i_Max--)
		{
			if(po_Var->mpac_InitValue)
			{
				switch(po_Var->mi_Type)
				{
				/* Change editor reference to engine reference */
				case TYPE_FUNCTIONREF:
					if(*((BIG_KEY *) po_Var->mpac_InitValue + i_Offset))
					{
						*((BIG_KEY *) po_Var->mpac_InitValue + i_Offset) = ul_GetEngineFileForForFunction(*((BIG_KEY *) po_Var->mpac_InitValue + i_Offset));
					}
					break;
				}

				/* Save init buffer */
				if(po_Var->mi_SpecialVar & VAR_TRACK)
				{
					psz_Temp = (char *) (LPCSTR) o_Name;
					psz_Temp += L_strlen("track");
					i_Num = L_atol(psz_Temp);
					ERR_X_Error(i_Num < AI_C_MaxTracks, ERR_COMPILER_Csz_TooTrack, NULL);
					maul_InitFct[i_Num] = *((BIG_KEY *) po_Var->mpac_InitValue);
					bCanInc = FALSE;
				}
				else
				{
					L_memcpy
					(
						mst_VarDes.pc_BufferInit + mst_VarDes.pst_VarsInfos[i].i_Offset + i_Offset,
						po_Var->mpac_InitValue + i_Offset,
						EAI_gast_Types[AI_gaw_EnumLink[po_Var->mi_Type]].i_Size
					);
				}
			}

			i_Offset += EAI_gast_Types[AI_gaw_EnumLink[po_Var->mi_Type]].i_Size;
		}

		if(bCanInc) i++;
	}

	/* Save init buffer */
	if(i_SizeInit) SAV_Buffer(mst_VarDes.pc_BufferInit, i_SizeInit);

	/* Save initial functions */
	SAV_Buffer(maul_InitFct, sizeof(LONG) * AI_C_MaxTracks);

	/* End of save */
	SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GenerateOutputFile(AI_tdst_Function *_pst_Func)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char			asz_Path[BIG_C_MaxLenPath];
	char			asz_Name[BIG_C_MaxLenPath];
	char			*psz_Temp;
	int				i_Num, i, j;
	char			c_Mem;
	EAI_cl_Variable *po_Var;
	CString			o_Name;
	POSITION		pos;
	BIG_INDEX		ul_File;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Compute file name */
	BIG_ComputeFullName(BIG_ParentFile(e.ul_File), asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(e.ul_File));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtAIEngineFct);

#ifdef BENCH_IA_TEST
	AIBnch_AddLastFunctionIfo(e.i_CurrentOutput , ul_GetEngineFileForForFunction(BIG_FileKey(e.ul_File)));
#endif

	/* Special link */
	for(i_Num = 0; i_Num < e.i_CurrentOutput; i_Num++)
	{
		if(e.pst_Output[i_Num].c_Type == CATEG_KEYWORD && e.pst_Output[i_Num].w_Param == KEYWORD_CALLPROC) 
			i_Num += 2 + e.pst_Output[i_Num + 2].l_Param;
		else if(e.pst_Output[i_Num].c_Type == CATEG_TYPE)
		{
			switch(e.pst_Output[i_Num].w_Param)
			{
			/* Change editor reference to engine reference */
			case TYPE_FUNCTIONREF:
				e.pst_Output[i_Num].l_Param = ul_GetEngineFileForForFunction(e.pst_Output[i_Num].l_Param);
				break;
			}
		}
	}

_Try_
	SAV_Begin(asz_Path, asz_Name);

	/* Write size of local stack */
	SAV_Buffer(&mst_LocalVars.i_LastPosVar, 4);

	/* Write all nodes engine (without flags) */
	i_Num = e.i_CurrentOutput * sizeof(AI_tdst_Node);
	SAV_Buffer(&i_Num, 4);
	for(i = 0; i < i_Num / (int) sizeof(AI_tdst_Node); i++)
	{
		c_Mem = e.pst_Output[i].c_Flags;
		e.pst_Output[i].c_Flags = 0;
		SAV_Buffer(&e.pst_Output[i], sizeof(AI_tdst_Node));
		e.pst_Output[i].c_Flags = c_Mem;
		if(e.pst_Output[i].c_Type == CATEG_KEYWORD && e.pst_Output[i].w_Param == KEYWORD_CALLPROC) 
		{
			for(j = 1; j < 3 + e.pst_Output[i + 2].l_Param; j++)
			{
				SAV_Buffer(&e.pst_Output[i + j], sizeof(AI_tdst_Node));
			}

			i += 2 + e.pst_Output[i + 2].l_Param;
		}
	}

	/* Write all nodes editors (debug) */
	SAV_Buffer(&i_Num, 4);
	SAV_Buffer(e.pst_OtherOutput, i_Num);

	/* Write string buffer len */
	SAV_Buffer(&e.i_NumStringBuf, 4);
	SAV_Buffer(e.pc_StringBuf, e.i_NumStringBuf);

	/* Local vars information */
	i_Num = mst_LocalVars.o_Vars.GetCount() * sizeof(AI_tdst_Local);
	SAV_Buffer(&i_Num, 4);
	if(i_Num)
	{
		e.pst_LocalVars = (AI_tdst_Local *) MEM_p_Alloc(i_Num);
		pos = mst_LocalVars.o_Vars.GetStartPosition();
		i = 0;
		while(pos)
		{
			mst_LocalVars.o_Vars.GetNextAssoc(pos, o_Name, (void * &) po_Var);
			L_strcpy(e.pst_LocalVars[i].asz_Name, (char *) (LPCSTR) o_Name);
			e.pst_LocalVars[i].i_Offset = po_Var->mi_Pos;
			e.pst_LocalVars[i].i_Type = po_Var->mi_Type;
			i++;
		}

		SAV_Buffer(e.pst_LocalVars, i_Num);
	}

	ul_File = SAV_ul_End();
_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::GenerateOutputLog(AI_tdst_Function *_pst_Func)
{
	/*~~~~~~~~~~~~~~~~*/
	int		i, j, iSize;
	char	asz[512];
	/*~~~~~~~~~~~~~~~~*/

	iSize = 0;
	for(i = 0; i < e.i_CurrentOutput; i++)
	{
		iSize += sizeof(AI_tdst_Node);
		sprintf
		(
			asz,
			"%-08x(C)%-06x(L)%-012x(W)%-010x",
			i,
			_pst_Func->pst_RootNode[i].c_Type,
			_pst_Func->pst_RootNode[i].l_Param,
			_pst_Func->pst_RootNode[i].w_Param
		);

		switch(e.pst_Output[i].c_Type)
		{
		case CATEG_TYPE:
			strcat(asz, "TYPE ");
			switch(_pst_Func->pst_RootNode[i].w_Param)
			{
			case TYPE_BOOL:			strcat(asz, "bool"); break;
			case TYPE_INT:			strcat(asz, "int"); break;
			case TYPE_FLOAT:		strcat(asz, "float"); break;
			case TYPE_VOID:			strcat(asz, "void"); break;
			case TYPE_HEXA:			strcat(asz, "hexa"); break;
			case TYPE_BINARY:		strcat(asz, "binary"); break;
			case TYPE_STRING:		strcat(asz, "string"); break;
			case TYPE_FUNCTIONREF:	strcat(asz, "function"); break;
			}
			break;

		case CATEG_KEYWORD:
			strcat(asz, "KEYWORD ");
			for(j = 0;; j++)
			{
				if(EAI_gast_Keywords[j].w_ID == _pst_Func->pst_RootNode[i].w_Param)
				{
					strcat(asz, EAI_gast_Keywords[j].psz_Name);
					break;
				}
			}
			break;

		case CATEG_FUNCTION:
			strcat(asz, "FUNCTION ");
			for(j = 0;; j++)
			{
				if(EAI_gast_Functions[j].w_ID == _pst_Func->pst_RootNode[i].w_Param)
				{
					strcat(asz, EAI_gast_Functions[j].psz_Name);
					break;
				}
			}
			break;

		case CATEG_FIELD:
			strcat(asz, "FIELD ");
			for(j = 0;; j++)
			{
				if(EAI_gast_Fields[j].w_ID == _pst_Func->pst_RootNode[i].w_Param)
				{
					strcat(asz, EAI_gast_Fields[j].psz_Name);
					break;
				}
			}
			break;

		case CATEG_LOCALVAR:
			strcat(asz, "CATEG_LOCALVAR");
			break;

		case CATEG_INITLOCALVARARRAY:
			strcat(asz, "CATEG_INITLOCALVARARRAY");
			break;
		case CATEG_LOCALVARARRAY:
			strcat(asz, "CATEG_LOCALVARARRAY");
			break;
		case CATEG_LOCALVARARRAY2:
			strcat(asz, "CATEG_LOCALVARARRAY2");
			break;
		case CATEG_LOCALVARARRAY3:
			strcat(asz, "CATEG_LOCALVARARRAY3");
			break;

		case CATEG_GLOBALVAR:
			strcat(asz, "CATEG_GLOBALVAR");
			break;

		case CATEG_GLOBALVARARRAY:
			strcat(asz, "CATEG_GLOBALVARARRAY");
			break;
		case CATEG_GLOBALVARARRAY2:
			strcat(asz, "CATEG_GLOBALVARARRAY2");
			break;
		case CATEG_GLOBALVARARRAY3:
			strcat(asz, "CATEG_GLOBALVARARRAY3");
			break;
		}

		LINK_PrintStatusMsg(asz);
	}

	sprintf(asz, "\nTotal Size : %d", iSize);
	LINK_PrintStatusMsg(asz);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Compiler::OutputProcList(BIG_INDEX _h_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION			pos;
	SCR_sc_Procedure	*pt_Proc;
	int					count;
	CString				dum;
	int					len;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenPath];
	char				*psz_Temp;
	int					idum;
	ULONG				i_Num;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Compute file name */
	BIG_ComputeFullName(BIG_ParentFile(e.ul_File), asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(e.ul_File));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtAIEngineFctLib);
	SAV_Begin(asz_Path, asz_Name);

	/* Comptage */
	count = 0;
	pos = mo_ProcList.GetStartPosition();
	while(pos)
	{
		mo_ProcList.GetNextAssoc(pos, dum, (void * &) pt_Proc);
		if(e.ul_File == pt_Proc->h_File && pt_Proc->pu32_Nodes) count++;
	}

	SAV_Buffer(&count, 2);

	pos = mo_ProcList.GetStartPosition();
	while(pos)
	{
		mo_ProcList.GetNextAssoc(pos, dum, (void * &) pt_Proc);
		if(e.ul_File != pt_Proc->h_File) continue;
		if(!pt_Proc->pu32_Nodes) continue;

		/* Nom */
		len = L_strlen(pt_Proc->az_Name);
		SAV_Buffer(&len, 4);
		SAV_Buffer(pt_Proc->az_Name, len);

		/* Pile */
		idum = 0xAACCBBDD;
		SAV_Buffer(&idum, 4);
		SAV_Buffer(&pt_Proc->u16_SizeLocal, 2);

		/* Special link */
		for(i_Num = 0; i_Num < pt_Proc->u32_NumNodes; i_Num++)
		{
			if(pt_Proc->pu32_Nodes[i_Num].c_Type == CATEG_KEYWORD && pt_Proc->pu32_Nodes[i_Num].w_Param == KEYWORD_CALLPROC) 
				i_Num += 2 + pt_Proc->pu32_Nodes[i_Num + 2].l_Param;
			else if(pt_Proc->pu32_Nodes[i_Num].c_Type == CATEG_TYPE)
			{
				switch(pt_Proc->pu32_Nodes[i_Num].w_Param)
				{
				/* Change editor reference to engine reference */
				case TYPE_FUNCTIONREF:
					pt_Proc->pu32_Nodes[i_Num].l_Param = ul_GetEngineFileForForFunction(pt_Proc->pu32_Nodes[i_Num].l_Param);
					break;
				}
			}
		}

		/* Noeuds */
		SAV_Buffer(&pt_Proc->u32_NumNodes, 4);
		SAV_Buffer(pt_Proc->pu32_Nodes, pt_Proc->u32_NumNodes * sizeof(AI_tdst_Node));
		SAV_Buffer(pt_Proc->pu32_DbgNodes, pt_Proc->u32_NumNodes * sizeof(AI_tdst_Node));

		idum = 0x66666666;
		SAV_Buffer(&idum, 4);
		SAV_Buffer(&pt_Proc->i_NumLocalVar, 4);
		SAV_Buffer(pt_Proc->az_Comment, 1024);
		SAV_Buffer(pt_Proc->pst_LocalVar, pt_Proc->i_NumLocalVar * sizeof(AI_tdst_Local));
	}

	/* Write string buffer len */
	idum = 0xC0DE6660;
	SAV_Buffer(&idum, 4);
	SAV_Buffer(&e.i_NumStringBuf, 4);
	SAV_Buffer(e.pc_StringBuf, e.i_NumStringBuf);

	SAV_Buffer(&BIG_FileKey(e.ul_File), 4);
	SAV_ul_End();
}


#endif /* ACTIVE_EDITORS */
