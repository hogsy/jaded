/*$T AIframe_comp.cpp GC! 1.098 12/06/00 14:53:45 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#ifdef ACTIVE_EDITORS
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLIerrid.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGmdfy_file.h"
#include "BIGfiles/BIGmdfy_dir.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/BIGspecial.h"

#include "AIscript/AIframe_act.h"
#include "AIframe.h"
#include "AIerrid.h"
#include "EditView/AIview.h"
#include "EditView/AIleftview.h"
#include "EDImainframe.h"
#include "EDIapp.h"
#include "EDIpaths.h"
#include "AIstrings.h"
#include "DIAlogs/DIAname_dlg.h"
#include "LINKs/LINKmsg.h"
#include "LINKs/LINKtoed.h"
#include "EDIstrings.h"
#include "EDIerrid.h"
#include "AIinterp/Sources/AIload.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "AIleftframe.h"

#include "BASe/MEMory/MEM.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIsave.h"
#include "AIinterp/Sources/AIBench.h"
#include "MATHs/MATH.h"
#include "Res/Res.h"
#include "EDImsg.h"
#include "LINKs/LINKstruct.h"
#include "LINKs/LINKstruct_reg.h"
#include "ENGine/Sources/WORld/WORvars.h"
#include "ENGine/Sources/WORld/WORuniverse.h"
#include "ENGine/Sources/WORld/WORaccess.h"
#include "AIinterp/Sources/AIdebug.h"
#include "DIAlogs/DIAmsglink_dlg.h"
#include "DIAlogs/DIA_UPDATE_dlg.h"

int aaa = 0;
extern CMapStringToString		go_PPConstants;
extern CMapStringToPtr			go_PPConstantsFiles;
extern CMapStringToPtr			go_PPConstantsLines;
extern CMapStringToPtr			go_PPMacros;
extern CMapStringToPtr			go_PPMacrosLines;
extern EDIA_cl_MsgLinkDialog	EDI_go_MsgGetLatest;
extern EDIA_cl_MsgLinkDialog	EDI_go_MsgUndoCheck;
extern EDIA_cl_MsgLinkDialog	EDI_go_MsgCheckInNo;
extern CMapStringToPtr			go_DefineCstNames;
extern CMapStringToPtr			go_DefineCstNamesGlob;
extern CList<CString, CString>	go_DefineCst;
extern CList<CString, CString>	go_DefineCstGlob;
extern CMapStringToPtr			go_DefineMacroNames;
extern CMapStringToPtr			go_DefineMacroNamesGlob;
#ifdef JADEFUSION
extern BOOL					LOA_gb_SlashY;
#else
extern "C" BOOL					LOA_gb_SlashY;
#endif
extern BOOL						AI2C_gb_Activated;
extern void						AI2C_ModelToC(EAI_cl_Compiler *, BIG_INDEX);
extern void						AI2C_GenerateVars(BIG_INDEX, BIG_INDEX);
extern BOOL						EDI_can_lock;
BOOL							AI_gb_Compile1 = FALSE;
CMapPtrToPtr					AI_go_CommonList;

#ifdef JADEFUSION
extern USHORT				gu16_ProcLink;
#else
extern "C" USHORT				gu16_ProcLink;
#endif

extern BAS_tdst_barray          AI2C_gst_FctList;
extern BOOL                     AI2C_gb_FctListInit;
#ifdef JADEFUSION
extern OBJ_tdst_GameObject		*AI_gp_ResolveGO;
#else
extern "C" OBJ_tdst_GameObject		*AI_gp_ResolveGO;
#endif
/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::SetCompile1(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	h_File, h_Dir;
	ULONG		date;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	AI_gb_Compile1 = FALSE;
	if(!mst_Ini.b_LookDate) return;
	AI_gb_Compile1 = TRUE;
	h_Dir = BIG_ul_SearchDir(EDI_Csz_Path_AILib "/Common");
	if(h_Dir == BIG_C_InvalidIndex) return;
	h_File = BIG_FirstFile(h_Dir);
	while(h_File != BIG_C_InvalidIndex)
	{
		if(!AI_go_CommonList.Lookup((void *) BIG_FileKey(h_File), (void * &) date))
		{
			AI_gb_Compile1 = FALSE;
			AI_go_CommonList.SetAt((void *) BIG_FileKey(h_File), (void *) BIG_TimeFile(h_File));
		}
		else if(BIG_TimeFile(h_File) != (int) date)
		{
			AI_gb_Compile1 = FALSE;
			AI_go_CommonList.SetAt((void *) BIG_FileKey(h_File), (void *) BIG_TimeFile(h_File));
		}
		h_File = BIG_NextFile(h_File);
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::UpdateEngineNodes(EAI_cl_Compiler *_po_Compiler, BIG_INDEX _ul_File)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int					i;
	ULONG				jj, ul_Index, ul_Index1;
	BIG_tdst_GroupElem	*pst_Buf;
	char				*psz_Temp;
	BIG_INDEX			ul_File, ul_Load;
	AI_tdst_Function	*pst_Func;
	SCR_tt_ProcedureList *pst_ProcList;
	SCR_sc_Procedure	*pt_ProcEd;
	CString				name;
	POSITION			pos;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* No engine model */
	if(!mpst_Model)
	{
		if(_po_Compiler->e.pst_Output)
		{
			MEM_Free(_po_Compiler->e.pst_Output);
			_po_Compiler->e.pst_Output = NULL;
		}

		if(_po_Compiler->e.pst_OtherOutput)
		{
			MEM_Free(_po_Compiler->e.pst_OtherOutput);
			_po_Compiler->e.pst_OtherOutput = NULL;
		}

		if(_po_Compiler->e.pst_LocalVars)
		{
			MEM_Free(_po_Compiler->e.pst_LocalVars);
			_po_Compiler->e.pst_LocalVars = NULL;
		}

		if(_po_Compiler->e.pc_StringBuf)
		{
			MEM_Free(_po_Compiler->e.pc_StringBuf);
			_po_Compiler->e.pc_StringBuf = NULL;
		}

		return;
	}

	LINK_UpdatePointer(mpst_Model);

	/* Get indice of function in model */
	pst_Buf = _po_Compiler->mpst_CurrentModel;
	ul_Index = 0;
	ul_Index1 = 0;
	for(i = 0; i < _po_Compiler->mi_SizeModel; i++)
	{
		if(!pst_Buf[i].ul_Key) continue;
		ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
		if(ul_File == BIG_C_InvalidIndex) continue;
		psz_Temp = L_strrchr(BIG_NameFile(ul_File), '.');
		if(!psz_Temp) continue;

		/* Get all proclist of model until reach the compiled one */
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFctLib))
		{
			if(ul_File == _ul_File)
			{
				/* Retreive fat index of corresponding engine file */
				ul_File = BIG_ul_SearchKeyToFat(_po_Compiler->ul_GetEngineFileForForProcList(BIG_FileKey(_ul_File)));

				ul_Index1 = -1;
				ul_Load = LOA_ul_SearchAddress(BIG_PosFile(ul_File));
				for(jj = 0; jj < mpst_Model->u32_NbProcList; jj++)
				{
					if((ULONG) mpst_Model->pp_ProcList[jj] == ul_Load)
					{
						ul_Index1 = jj;
						break;
					}
				}

				if(ul_Index1 == -1)
				{
					ul_Index1 = mpst_Model->u32_NbProcList;
					if(ul_Index1 == 0)
					{
						mpst_Model->pp_ProcList = (SCR_tt_ProcedureList **) MEM_p_Alloc((ul_Index1 + 1) * sizeof(void *));
					}
					else
					{
						mpst_Model->pp_ProcList = (SCR_tt_ProcedureList **) MEM_p_Realloc
							(
								mpst_Model->pp_ProcList,
								(ul_Index1 + 1) * sizeof(void *)
							);
					}

					/* Create a new function and register it */
					ul_Load = LOA_ul_SearchAddress(BIG_PosFile(ul_File));
					if(ul_Load == BIG_C_InvalidIndex)
					{
						pst_ProcList = (SCR_tt_ProcedureList *) MEM_p_Alloc(sizeof(SCR_tt_ProcedureList));
						L_memset(pst_ProcList, 0, sizeof(SCR_tt_ProcedureList));
						LOA_AddAddress(ul_File, pst_ProcList);
						pst_ProcList->uw_NbRefs = 1;
					}
					else
					{
						pst_ProcList = (SCR_tt_ProcedureList *) ul_Load;
						pst_ProcList->uw_NbRefs++;
					}

					mpst_Model->u32_NbProcList++;
					mpst_Model->pp_ProcList[ul_Index1] = pst_ProcList;
				}

				pst_ProcList = mpst_Model->pp_ProcList[ul_Index1];
				ERR_X_Error(pst_ProcList, "A proclist does not exists in model", BIG_NameFile(mul_CurrentEditModel));

				for(i = 0; i < pst_ProcList->u16_Num; i++)
				{
					if(pst_ProcList->pt_All[i].pu32_Nodes) MEM_Free(pst_ProcList->pt_All[i].pu32_Nodes);
					if(pst_ProcList->pt_All[i].pu32_DbgNodes) MEM_Free(pst_ProcList->pt_All[i].pu32_DbgNodes);
					if(pst_ProcList->pt_All[i].pz_Name) MEM_Free(pst_ProcList->pt_All[i].pz_Name);
					if(pst_ProcList->pt_All[i].pst_LocalVars) MEM_Free(pst_ProcList->pt_All[i].pst_LocalVars);
				}

				if(pst_ProcList->pt_All) MEM_Free(pst_ProcList->pt_All);

				/* New string buf */
				if(pst_ProcList->pc_Strings) MEM_Free(pst_ProcList->pc_Strings);
				pst_ProcList->pc_Strings = _po_Compiler->e.pc_StringBuf;

				/* Comptage des procedures appartenant à ce fichier */
				i = 0;
				pos = _po_Compiler->mo_ProcList.GetStartPosition();
				while(pos)
				{
					_po_Compiler->mo_ProcList.GetNextAssoc(pos, name, (void * &) pt_ProcEd);
					if(pt_ProcEd->h_File == _ul_File && pt_ProcEd->pu32_Nodes) i++;
				}
				pst_ProcList->u16_Num = i;
				if(i)
				{
					pst_ProcList->pt_All = (SCR_tt_Procedure *) MEM_p_Alloc(i * sizeof(SCR_tt_Procedure));
					L_memset(pst_ProcList->pt_All, 0, i * sizeof(SCR_tt_Procedure));
				}
				else
					pst_ProcList->pt_All = NULL;

				i = 0;
				pos = _po_Compiler->mo_ProcList.GetStartPosition();
				while(pos)
				{
					_po_Compiler->mo_ProcList.GetNextAssoc(pos, name, (void * &) pt_ProcEd);
					if(pt_ProcEd->h_File == _ul_File && pt_ProcEd->pu32_Nodes)
					{
						pst_ProcList->pt_All[i].pt_ProcList = pst_ProcList;
						pst_ProcList->pt_All[i].pu32_DbgNodes = pt_ProcEd->pu32_DbgNodes;
						pst_ProcList->pt_All[i].pu32_Nodes = pt_ProcEd->pu32_Nodes;
						pst_ProcList->pt_All[i].pz_Name = (char *) MEM_p_Alloc(L_strlen(pt_ProcEd->az_Name) + 1);
						L_strcpy(pst_ProcList->pt_All[i].pz_Name, pt_ProcEd->az_Name);
						pst_ProcList->pt_All[i].u16_SizeLocal = pt_ProcEd->u16_SizeLocal;
						pst_ProcList->pt_All[i].pst_LocalVars = pt_ProcEd->pst_LocalVar;
						pst_ProcList->pt_All[i].ul_NbLocals = pt_ProcEd->i_NumLocalVar;
						L_strcpy(pst_ProcList->pt_All[i].asz_Comment, pt_ProcEd->az_Comment);
						i++;
					}
				}

				return;
			}

			ul_Index1++;
		}

		/* Get all functions of model until reach the compiled one */
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFct))
		{
			if(ul_File == _ul_File)
			{
				/* Retreive fat index of corresponding engine file */
				ul_File = BIG_ul_SearchKeyToFat(_po_Compiler->ul_GetEngineFileForForFunction(BIG_FileKey(_ul_File)));

				ul_Index = -1;
				ul_Load = LOA_ul_SearchAddress(BIG_PosFile(ul_File));
				for(jj = 0; jj < mpst_Model->ul_NbFunctions; jj++)
				{
					if((ULONG) mpst_Model->ppst_Functions[jj] == ul_Load)
					{
						ul_Index = jj;
						break;
					}
				}

				if(ul_Index == -1)
				{
					ul_Index = mpst_Model->ul_NbFunctions;
					if(ul_Index == 0)
					{
						mpst_Model->ppst_Functions = (AI_tdst_Function **) MEM_p_Alloc((ul_Index + 1) * sizeof(void *));
					}
					else
					{
						mpst_Model->ppst_Functions = (AI_tdst_Function **) MEM_p_Realloc
							(
								mpst_Model->ppst_Functions,
								(ul_Index + 1) * sizeof(void *)
							);
					}

					/* Create a new function and register it */
					ul_Load = LOA_ul_SearchAddress(BIG_PosFile(ul_File));
					if(ul_Load == BIG_C_InvalidIndex)
					{
						pst_Func = (AI_tdst_Function *) MEM_p_Alloc(sizeof(AI_tdst_Function));
						L_memset(pst_Func, 0, sizeof(AI_tdst_Function));
						LOA_AddAddress(ul_File, pst_Func);
						pst_Func->uw_NbRefs = 1;
					}
					else
					{
						pst_Func = (AI_tdst_Function *) ul_Load;
						pst_Func->uw_NbRefs++;
					}

					mpst_Model->ul_NbFunctions++;
					mpst_Model->ppst_Functions[ul_Index] = pst_Func;
				}

				pst_Func = mpst_Model->ppst_Functions[ul_Index];
				ERR_X_Error(pst_Func, "A function does not exists in model", BIG_NameFile(mul_CurrentEditModel));

				/* Free actual node list of function */
				if(pst_Func->pst_RootNode) MEM_Free(pst_Func->pst_RootNode);

				/* Free actual node list of function */
				if(pst_Func->pst_OtherRootNode) MEM_Free(pst_Func->pst_OtherRootNode);
				if(pst_Func->pst_LocalVars) MEM_Free(pst_Func->pst_LocalVars);

				/* New string buf */
				if(pst_Func->pc_Strings) MEM_Free(pst_Func->pc_Strings);

				/* Change function */
				pst_Func->pst_RootNode = _po_Compiler->e.pst_Output;
				pst_Func->pst_RootNode->c_Flags &= ~AI_Cflag_FctTransform;
				pst_Func->pst_OtherRootNode = _po_Compiler->e.pst_OtherOutput;
				pst_Func->pst_LocalVars = _po_Compiler->e.pst_LocalVars;
				pst_Func->ul_NbLocals = _po_Compiler->mst_LocalVars.o_Vars.GetCount();
				pst_Func->i_SizeLocalStack = _po_Compiler->mst_LocalVars.i_LastPosVar;
				pst_Func->pc_Strings = _po_Compiler->e.pc_StringBuf;
				return;
			}

			ul_Index++;
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::SaveAllInstances(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElemW;
	TAB_tdst_PFelem		*pst_EndElemW;
	WOR_tdst_World		*pst_World;
	AI_tdst_Instance	*pst_Instance;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
	{
		pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
		if(!TAB_b_IsAHole(pst_World))
		{
			pst_AIEOT = &(pst_World->st_AllWorldObjects);
			pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
			pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
			for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
				pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
				if(TAB_b_IsAHole(pst_GAO)) continue;

				if((pst_GAO->pst_Extended) && (pst_GAO->pst_Extended->pst_Ai))
				{
					pst_Instance = (AI_tdst_Instance *) pst_GAO->pst_Extended->pst_Ai;
					if(pst_Instance->pst_Model == mpst_Model)
					{
_Try_
						AI_SaveInstance(pst_Instance);
_Catch_
_End_
					}
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::Resolve(EAI_cl_Compiler *_po_Compiler)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem		*pst_CurrentElemW;
	TAB_tdst_PFelem		*pst_EndElemW;
	WOR_tdst_World		*pst_World;
	AI_tdst_Instance	*pst_Instance;
	TAB_tdst_PFtable	*pst_AIEOT;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GAO;
	ULONG				i, j;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!mpst_Model && !mpst_Instance)
		return;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    New instance description.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mpst_Instance)
	{
		if(mpst_Instance->pc_VarsBufferInit)
		{
			MEM_Free(mpst_Instance->pc_VarsBufferInit);
			mpst_Instance->pc_VarsBufferInit = NULL;
		}

		if(mpst_Instance->pc_VarsBuffer)
		{
			MEM_Free(mpst_Instance->pc_VarsBuffer);
			mpst_Instance->pc_VarsBuffer = NULL;
		}

		mpst_Instance->pst_Model->uw_NbInstances--;
		AI_ResolveInstanceRef(mpst_Instance);
		AI_ReinitInstance(mpst_Instance);
		LINK_UpdatePointer(mpst_Instance);
	}
	else if(WOR_gst_Universe.pst_AI && mpst_Model == WOR_gst_Universe.pst_AI->pst_Model)
	{
		if(WOR_gst_Universe.pst_AI->pc_VarsBufferInit)
		{
			MEM_Free(WOR_gst_Universe.pst_AI->pc_VarsBufferInit);
			WOR_gst_Universe.pst_AI->pc_VarsBufferInit = NULL;
		}

		if(WOR_gst_Universe.pst_AI->pc_VarsBuffer)
		{
			MEM_Free(WOR_gst_Universe.pst_AI->pc_VarsBuffer);
			WOR_gst_Universe.pst_AI->pc_VarsBuffer = NULL;
		}

		WOR_gst_Universe.pst_AI->pst_Model->uw_NbInstances--;
		AI_ResolveInstanceRef(WOR_gst_Universe.pst_AI);
		if(WOR_gst_Universe.pst_AI->pc_VarsBuffer && WOR_gst_Universe.pst_AI->pc_VarsBufferInit && WOR_gst_Universe.pst_AI->pst_Model->pst_VarDes)
			L_memcpy(WOR_gst_Universe.pst_AI->pc_VarsBuffer, WOR_gst_Universe.pst_AI->pc_VarsBufferInit, WOR_gst_Universe.pst_AI->pst_Model->pst_VarDes->ul_SizeBufferInit);
		AI_ReinitInstance(WOR_gst_Universe.pst_AI);
		LINK_UpdatePointer(WOR_gst_Universe.pst_AI);
	}
	else
	{
		AI_ResolveModelRef(mpst_Model);
		LINK_UpdatePointer(mpst_Model);
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Universe
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(WOR_gst_Universe.pst_AI)
	{
		if(mpst_Model == WOR_gst_Universe.pst_AI->pst_Model)
		{
			AI_AllocateSaveBuffer(WOR_gst_Universe.pst_AI);
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Update all instances of the same model
	 -------------------------------------------------------------------------------------------------------------------
	 */

	pst_CurrentElemW = TAB_pst_PFtable_GetFirstElem(&WOR_gst_Universe.st_WorldsTable);
	pst_EndElemW = TAB_pst_PFtable_GetLastElem(&WOR_gst_Universe.st_WorldsTable);
	for(pst_CurrentElemW; pst_CurrentElemW <= pst_EndElemW; pst_CurrentElemW++)
	{
		pst_World = (WOR_tdst_World *) pst_CurrentElemW->p_Pointer;
		if(!TAB_b_IsAHole(pst_World))
		{
			pst_AIEOT = &(pst_World->st_AllWorldObjects);
			pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(pst_AIEOT);
			pst_EndElem = TAB_pst_PFtable_GetLastElem(pst_AIEOT);
			for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
			{
				pst_GAO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
				if(TAB_b_IsAHole(pst_GAO)) continue;
				AI_gp_ResolveGO = pst_GAO;

				if((pst_GAO->pst_Extended) && (pst_GAO->pst_Extended->pst_Ai))
				{
					pst_Instance = (AI_tdst_Instance *) pst_GAO->pst_Extended->pst_Ai;

					/* Update all instances of the same model */
					if((pst_Instance->pst_Model == mpst_Model) && (pst_Instance != mpst_Instance))
					{
						LINK_UpdatePointer(pst_Instance);
						if(pst_Instance->pc_VarsBufferInit)
						{
							MEM_Free(pst_Instance->pc_VarsBufferInit);
							pst_Instance->pc_VarsBufferInit = NULL;
						}

						if(pst_Instance->pc_VarsBuffer)
						{
							MEM_Free(pst_Instance->pc_VarsBuffer);
							pst_Instance->pc_VarsBuffer = NULL;
						}

						pst_Instance->pst_Model->uw_NbInstances--;
						AI_ResolveInstanceRef(pst_Instance);
						AI_ReinitInstance(pst_Instance);
					}

					/* Search if the instance has a shared function currently executed */
					else if((pst_Instance != mpst_Instance) && (pst_Instance->pst_Model))
					{
						for(i = 0; i < pst_Instance->pst_Model->ul_NbFunctions; i++)
						{
							for(j = 0; j < mpst_Model->ul_NbFunctions; j++)
							{
								if(pst_Instance->apst_CurrentFct[i] == mpst_Model->ppst_Functions[j])
								{
_Try_
									AI_SaveInstance(pst_Instance);
_Catch_
_End_
									AI_ReinitInstance(pst_Instance);
									goto zap;
								}
							}
						}

zap: ;
					}
				}
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::UpdateEngineVars(EAI_cl_Compiler *_po_Compiler)
{
	/*~~~~~~~~~~~~~~~*/
	int		i;
	ULONG	j;
	ULONG	ul_Address;
	/*~~~~~~~~~~~~~~~*/

	/* If no model and instance, need to free compiler output */
	if(!mpst_Model && !mpst_Instance)
	{
		_po_Compiler->FreeCompilerVarDes();
		_po_Compiler->FreeCompilerOut();
		return;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    New model description.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(mpst_Model && (mpst_Model->pst_VarDes))
	{
		if(mpst_Model->pst_VarDes->pc_BufferInit)
		{
			MEM_Free(mpst_Model->pst_VarDes->pc_BufferInit);
			mpst_Model->pst_VarDes->pc_BufferInit = NULL;
		}

		if(mpst_Model->pst_VarDes->pst_VarsInfos)
		{
			MEM_Free(mpst_Model->pst_VarDes->pst_VarsInfos);
			mpst_Model->pst_VarDes->pst_VarsInfos = NULL;
		}

		if(mpst_Model->pst_VarDes->pst_EditorVarsInfos)
		{
			MEM_Free(mpst_Model->pst_VarDes->pst_EditorVarsInfos);
			mpst_Model->pst_VarDes->pst_EditorVarsInfos = NULL;
		}

		if(mpst_Model->pst_VarDes->pst_EditorVarsInfos2)
		{
			for(j = 0; j < mpst_Model->pst_VarDes->ul_NbVarsInfos2; j++)
			{
				if(mpst_Model->pst_VarDes->pst_EditorVarsInfos2[j].psz_StringCst)
					MEM_Free(mpst_Model->pst_VarDes->pst_EditorVarsInfos2[j].psz_StringCst);
				if(mpst_Model->pst_VarDes->pst_EditorVarsInfos2[j].psz_StringHelp)
					MEM_Free(mpst_Model->pst_VarDes->pst_EditorVarsInfos2[j].psz_StringHelp);
			}

			MEM_Free(mpst_Model->pst_VarDes->pst_EditorVarsInfos2);
			mpst_Model->pst_VarDes->ul_NbVarsInfos2 = 0;
			mpst_Model->pst_VarDes->pst_EditorVarsInfos2 = NULL;
		}

		/* New vardes for model */
		L_memcpy(mpst_Model->pst_VarDes, &_po_Compiler->mst_VarDes, sizeof(AI_tdst_VarDes));

		/* Initial functions */
		L_memcpy(mpst_Model->pst_VarDes->apst_InitFct, _po_Compiler->maul_InitFct, AI_C_MaxTracks * sizeof(void *));

		/* Check if all functions have been loaded (we can have add a new one) */
		for(i = 0; i < AI_C_MaxTracks; i++)
		{
			if(mpst_Model->pst_VarDes->apst_InitFct[i])
			{
				ul_Address = BIG_ul_SearchKeyToFat((BIG_KEY) mpst_Model->pst_VarDes->apst_InitFct[i]);
				ul_Address = LOA_ul_SearchAddress(BIG_PosFile(ul_Address));
				if(ul_Address == BIG_C_InvalidIndex)
				{
					LOA_MakeFileRef
					(
						(BIG_KEY) mpst_Model->pst_VarDes->apst_InitFct[i],
						NULL,
						AI_ul_CallbackLoadFunction,
						LOA_C_MustExists
					);
					LOA_Resolve();
				}
			}
		}

		mpst_Model->c_SecondPassDone = 0;
	}

	Resolve(_po_Compiler);
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::OnCompileFile
(
	EAI_cl_Compiler *_po_Compiler,
	BIG_INDEX		_ul_Model,
	BIG_INDEX		_ul_File,
	char			*_psz_Text
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString		o_Text;
	BIG_INDEX	ul_Index;
	int			i_Error, i_Line;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

_Try_
	_po_Compiler->mul_CurrentFile = _ul_File;
	i_Error = _po_Compiler->i_Compile(_ul_Model, _ul_File, _psz_Text);
_Catch_
_End_
	/*
	 * To avoid recurse error when an error occured during a compile in a compile
	 * (external vars)
	 */
	if(mb_RecurseError)
		mb_RecurseError = FALSE;
	else
	{
		/*
		 * If error is not in current file (cause of a #include), we load the file in the
		 * editor to view the error
		 */
		if(i_Error != -1)
		{
			if
			(
				L_strcmpi(_po_Compiler->e.asz_OrgPath, _po_Compiler->e.asz_Path)
			||	L_strcmpi(_po_Compiler->e.asz_OrgFile, _po_Compiler->e.asz_File)
			||	(_po_Compiler->e.ul_Model != mul_CurrentEditModel)
			||	(_po_Compiler->e.ul_File != mul_CurrentEditFile)
			)
			{
				ul_Index = BIG_ul_SearchFileExt(_po_Compiler->e.asz_Path, _po_Compiler->e.asz_File);
				mb_CompileAll = FALSE;	/* To force load */
				InitListAI();
				mb_LockHistory = TRUE;
				Load(ul_Index);
				mb_LockHistory = FALSE;
			}

			/* Put cursor at the beg of the error line */
			i_Line = mpo_Edit->LineIndex(i_Error);
//			mpo_Edit->LockWindowUpdate();
//			mpo_Edit->LineScroll(i_Error - ((mpo_Edit->l_LastVisibleLine() - mpo_Edit->l_FirstVisibleLine()) / 2));
//			mpo_Edit->UnlockWindowUpdate();
			mpo_Edit->SetSel(i_Line, i_Line);
		}

		/* Inform left view about error line */
		mpo_LeftView->mi_LineError = i_Error;
		mpo_LeftView->mi_LineBreak = -1;
		mpo_LeftView->Invalidate();

		/* We update nodes of current model for compiled function */
		if((i_Error == -1) && (!mo_Compiler.mb_CompileToLog))
		{
			UpdateEngineNodes(_po_Compiler, _ul_File);
		}
	}

	return(i_Error == -1);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnCompileCurrentFile(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	CString				o_Text;
	BOOL				b_Ok;
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_SizeModel;
	char				*psz_Temp;
	CString				o_String;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ResetRefCompilers();
	mo_Compiler.Reset();

	/* Read model in a temporary buffer */
	mo_Compiler.mpst_CurrentModel = NULL;
	mo_Compiler.mi_SizeModel = 0;
	mo_Compiler.mul_CurrentModel = mul_CurrentEditModel;
	mo_Compiler.mul_CurrentInstance = mul_CurrentEditInstance;
	pst_Buf = NULL;
	if(mul_CurrentEditModel != BIG_C_InvalidIndex)
	{
		pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(mul_CurrentEditModel), &ul_SizeModel);
		mo_Compiler.mpst_CurrentModel = pst_Buf;
		mo_Compiler.mi_SizeModel = ul_SizeModel / sizeof(BIG_tdst_GroupElem);
	}

	/* Compile vars of model */
	b_Ok = TRUE;
	if(mul_CurrentEditModel != BIG_C_InvalidIndex)
	{
		mo_Compiler.mb_CanOutput = FALSE;
		b_Ok = OnCompileVarsOfModel(&mo_Compiler, mul_CurrentEditModel);
		mo_Compiler.mb_CanOutput = TRUE;
	}
	else
		b_Ok = FALSE;

	/* Compile file */
	psz_Temp = L_strrchr(BIG_NameFile(mul_CurrentEditFile), '.');
	if(!psz_Temp || L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorVars))
	{
		if(b_Ok)
		{
			mpo_Edit->GetWindowText(o_Text);

			o_String = EAI_STR_Csz_CompileFunc;
			o_String += BIG_NameFile(mul_CurrentEditFile);
			if(!mo_Compiler.mb_CompileToLog) o_String += "\n";
			LINK_PrintStatusMsg((char *) (LPCSTR) o_String);
			b_Ok = OnCompileFile(&mo_Compiler, mul_CurrentEditModel, mul_CurrentEditFile, (char *) (LPCSTR) o_Text);
			if(!mo_Compiler.mb_CompileToLog)
			{
				if(b_Ok)
					LINK_PrintStatusMsg(EAI_STR_Csz_OK);
				else
					LINK_PrintStatusMsg(EAI_STR_Csz_Error);
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Error during compile.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(!b_Ok)
	{
		if(mpst_Model) mpst_Model->c_ModelInvalid = 1;
	}
	else
	{
		if(mpst_Model) mpst_Model->c_ModelInvalid = 0;
	}

	mo_Compiler.FreeCompilerOut();
	mo_Compiler.FreeCompilerVarDes();

	LINK_UpdatePointers();
	M_MF()->FatHasChanged();
	if(pst_Buf) L_free(pst_Buf);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::ResetRefCompilers(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/
	POSITION		pos;
	EAI_cl_Compiler *po_Compiler;
	ULONG			ul_Index;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Compilers for references */
	pos = mo_RefCompilers.GetStartPosition();
	while(pos)
	{
		mo_RefCompilers.GetNextAssoc(pos, (void * &) ul_Index, (void * &) po_Compiler);
		po_Compiler->Reset(mb_CompileAll | mb_CompileDep | AI_gb_Compile1 ? FALSE : TRUE);
		if(po_Compiler != &mo_Compiler) delete po_Compiler;
	}

	mo_RefCompilers.RemoveAll();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
int EAI_cl_Frame::i_GetVarOfModel(BIG_KEY _ul_Model, char *_psz_NameVar, BOOL _b_DirectModel)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_SizeModel;
	EAI_cl_Compiler		*po;
	LINK_tdst_Pointer	*p2;
	OBJ_tdst_GameObject *pst_Obj;
	AI_tdst_Instance	*pst_Instance;
	BOOL				bOk;
	BIG_INDEX			ul_Instance;
	char				asz_Name[BIG_C_MaxLenName];
	char				asz_Path[BIG_C_MaxLenPath];
	char				*psz_Temp;
	EAI_cl_Variable		*po_Var;
	BOOL				b_FirstTime;
	BIG_INDEX			ul_File, ul_Org;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Instance = BIG_C_InvalidIndex;
	bOk = TRUE;
	if(!_b_DirectModel)
	{
		/* Get fat index */
		ul_Org = _ul_Model;
		_ul_Model = BIG_ul_SearchKeyToFat(_ul_Model);
		if(_ul_Model == BIG_C_InvalidIndex) return -1;

		/* Search the engine object to retreive the model */
		_ul_Model = LOA_ul_SearchAddress(BIG_PosFile(_ul_Model));

		/* Special case for universe */
		if((_ul_Model == BIG_C_InvalidIndex) && (ul_Org == BIG_gst.st_ToSave.ul_UniverseKey))
		{
			ERR_X_Error(0, "You must load universe to reference it", NULL);
		}

		ERR_X_Error(_ul_Model != BIG_C_InvalidIndex, "Object not loaded", NULL);
		p2 = (LINK_tdst_Pointer *) LINK_p_SearchPointer((void *) _ul_Model);
		ERR_X_Error(p2 != NULL, ERR_COMPILER_Csz_UnkownRef, NULL);

		pst_Instance = (AI_tdst_Instance *) p2->pv_Data;
		if(p2->i_Type != LINK_C_AI_Instance)
		{
			ERR_X_Error(p2->i_Type == LINK_C_ENG_GameObjectOriented, ERR_COMPILER_Csz_UnkownRef, NULL);
			pst_Obj = (OBJ_tdst_GameObject *) _ul_Model;
			ERR_X_Error(pst_Obj->pst_Extended, ERR_COMPILER_Csz_UnkownRef, NULL);
			ERR_X_Error(pst_Obj->pst_Extended->pst_Ai, ERR_COMPILER_Csz_UnkownRef, NULL);
			pst_Instance = (AI_tdst_Instance *) pst_Obj->pst_Extended->pst_Ai;
		}

		/* Get the instance */
		ul_Instance = LOA_ul_SearchKeyWithAddress((ULONG) pst_Instance);
		ERR_X_Error(ul_Instance != BIG_C_InvalidIndex, ERR_COMPILER_Csz_UnkownRef, NULL);
		ul_Instance = BIG_ul_SearchKeyToFat(ul_Instance);

		/* Get the engine model */
		_ul_Model = LOA_ul_SearchKeyWithAddress((ULONG) pst_Instance->pst_Model);
		ERR_X_Error(_ul_Model != BIG_C_InvalidIndex, ERR_COMPILER_Csz_UnkownRef, NULL);
		_ul_Model = BIG_ul_SearchKeyToFat(_ul_Model);

		/* Get the editor model */
		BIG_ComputeFullName(BIG_ParentFile(_ul_Model), asz_Path);
		psz_Temp = L_strrchr(asz_Path, '/');
		ERR_X_Assert(psz_Temp);
		*psz_Temp = 0;
		L_strcpy(asz_Name, BIG_NameFile(_ul_Model));
		psz_Temp = L_strrchr(asz_Name, '.');
		ERR_X_Assert(psz_Temp);
		*psz_Temp = 0;
		L_strcat(asz_Name, EDI_Csz_ExtAIEditorModel);
		_ul_Model = BIG_ul_SearchFileExt(asz_Path, asz_Name);
		ERR_X_Error(_ul_Model != BIG_C_InvalidIndex, "AI model not found", asz_Name);
	}
	else
	{
		BIG_ComputeFullName(BIG_ParentFile(_ul_Model), asz_Path);
		L_strcpy(asz_Name, BIG_NameFile(_ul_Model));
	}

	/* Have we already parsed the model ? */
	b_FirstTime = FALSE;
	if(!mo_RefCompilers.Lookup((void *) _ul_Model, (void * &) po))
	{
		b_FirstTime = TRUE;

		/* Add a new compiler */
		po = new EAI_cl_Compiler;
		mo_RefCompilers.SetAt((void *) _ul_Model, po);
		po->Reset(FALSE);

		/* Read editor model and compile variables */
		pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(_ul_Model), &ul_SizeModel);
		po->mpst_CurrentModel = pst_Buf;
		po->mi_SizeModel = ul_SizeModel / sizeof(BIG_tdst_GroupElem);
		po->mb_CanOutput = FALSE;
		po->mul_CurrentModel = _ul_Model;
		po->mul_CurrentInstance = ul_Instance;
		po->mpo_Frame = this;
		bOk = OnCompileVarsOfModel(po, _ul_Model);

		/* Free temporary buffers */
		po->FreeCompilerVarDes();
		po->FreeCompilerOut();

		L_free(pst_Buf);
	}

	if(!bOk)
	{
		mb_RecurseError = TRUE;
		ERR_X_ForceThrow();
	}

	/* Search the var in the list */
	if(po->mst_GlobalVars.o_Vars.Lookup(_psz_NameVar, (void * &) po_Var))
	{
		/* If ok, add the dependencie */
		if(b_FirstTime)
		{
			psz_Temp = L_strrchr(asz_Name, '.');
			*psz_Temp = 0;
			L_strcat(asz_Path, "/");
			L_strcat(asz_Path, asz_Name);
			L_strcat(asz_Name, EDI_Csz_ExtAIEditorDepend);

			ul_File = BIG_ul_SearchFileExt(asz_Path, asz_Name);
			if(ul_File == BIG_C_InvalidIndex) ul_File = BIG_ul_CreateFile(asz_Path, asz_Name);

			if(_ul_Model != mul_CurrentEditModel)
			{
				BIG_AddRefInGroup(ul_File, BIG_NameFile(mul_CurrentEditModel), BIG_FileKey(mul_CurrentEditModel));
			}
		}

		return (int) po_Var;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::OnCompileVarsOfModel(EAI_cl_Compiler *_po_Compiler, BIG_INDEX _ul_Model, BOOL lookdate)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_SizeModel, ul_Size, i;
	BIG_INDEX			ul_File;
	char				*psz_Temp, *psz_Temp1;
	BOOL				b_Ok;
	POSITION			pos;
	EAI_cl_Variable		*po_Var;
	CString				o_Name;
	ULONG				date;
	int					i_OneTime;
	BOOL				bPP;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Read model in a temporary buffer */
	pst_Buf = _po_Compiler->mpst_CurrentModel;
	ul_SizeModel = _po_Compiler->mi_SizeModel;

	/* Search all file with right extension */
	b_Ok = TRUE;
	i_OneTime = TRUE;
	for(i = 0; i < ul_SizeModel; i++)
	{
		if(!pst_Buf[i].ul_Key) continue;
		ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
		if(ul_File == BIG_C_InvalidIndex) continue;
		psz_Temp = L_strrchr(BIG_NameFile(ul_File), '.');
		if(!psz_Temp) continue;

		/* Is it a global var file ? */
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorVars))
		{
			if(lookdate)
			{
				if(mo_ModelDateList.Lookup((void *) pst_Buf[i].ul_Key, (void * &) date))
				{
					if(BIG_TimeFile(ul_File) != (int) date) mo_ModelDateList.RemoveAll();
				}

				mo_ModelDateList.SetAt((void *) pst_Buf[i].ul_Key, (void *) BIG_TimeFile(ul_File));
			}

			psz_Temp1 = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_File), &ul_Size);
			if(psz_Temp1)
			{
				_po_Compiler->mb_ForceGenPP = AI2C_gb_Activated;
				bPP = _po_Compiler->mb_GenPP;
				_po_Compiler->mb_GenPP = FALSE;

				_po_Compiler->mb_GlobalVarMode = TRUE;
				b_Ok = OnCompileFile(_po_Compiler, _ul_Model, ul_File, psz_Temp1);
				_po_Compiler->FreeCompilerOut();
				_po_Compiler->mb_GlobalVarMode = FALSE;

				_po_Compiler->mb_ForceGenPP = FALSE;
				_po_Compiler->mb_GenPP = bPP;

				L_free(psz_Temp1);
			}

			/* Generation en C du fichier de variables */
			if(b_Ok && AI2C_gb_Activated && i_OneTime) 
			{
				i_OneTime = FALSE;
				AI2C_GenerateVars(_ul_Model, ul_File);
			}

			if(!b_Ok) 
			{
				mo_ModelDateList.RemoveAll();
				break;
			}
			continue;
		}
	}

	/* Copy local vars to global */
	if(b_Ok)
	{
		if(_po_Compiler->mst_LocalVars.o_Vars.GetStartPosition())
		{
			_po_Compiler->mst_GlobalVars.i_LastPosVar = _po_Compiler->mst_LocalVars.i_LastPosVar;
			pos = _po_Compiler->mst_LocalVars.o_Vars.GetStartPosition();
			while(pos)
			{
				_po_Compiler->mst_LocalVars.o_Vars.GetNextAssoc(pos, o_Name, (void * &) po_Var);
				_po_Compiler->mst_GlobalVars.o_Vars.SetAt(o_Name, po_Var);
			}

			_po_Compiler->mst_LocalVars.i_LastPosVar = 0;
			_po_Compiler->mst_LocalVars.o_Vars.RemoveAll();
		}
	}

	/* Remember buffer */
	if(b_Ok) _po_Compiler->GenerateOutputVars(_ul_Model);

	return b_Ok;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::OnCompileCurrentModel(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_SizeModel, ul_Size, i;
	BIG_INDEX			ul_File;
	char				*psz_Temp, *psz_Temp1;
	BOOL				b_Ok;
	CString				o_Name;
	char				asz_Name[BIG_C_MaxLenPath];
	char				asz_Path[BIG_C_MaxLenPath];
	CString				o_String, o_String1, str;
	int					i_ScrollV;
	int					i_ScrollH;
	CHARRANGE			cr;
	POSITION			pos;
	EAI_cl_Variable		*po_Var;
	CString				str1;
	int					date;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

    AI_gp_ResolveGO = NULL;
	mo_Compiler.mb_ProcList = FALSE;
	mo_Compiler.mb_ProcListLocal = FALSE;
	mo_Compiler.mi_NumProc = 0;
	mo_Compiler.mb_ProcListParent = FALSE;
	i_ScrollH = mpo_Edit->GetScrollPos(SB_HORZ);
	i_ScrollV = mpo_Edit->GetScrollPos(SB_VERT);
	mpo_Edit->GetSel(cr);

	SaveAllInstances();

	if(!mb_CompileAll && !mb_CompileDep && !AI_gb_Compile1) ResetRefCompilers();
	mo_Compiler.Reset(mb_CompileAll | mb_CompileDep | AI_gb_Compile1 ? FALSE : TRUE);

	if(!AI_gb_Compile1)
	{
		go_PPConstants.RemoveAll();
		go_PPConstantsFiles.RemoveAll();
		go_PPConstantsLines.RemoveAll();
		go_PPMacros.RemoveAll();
		go_PPMacrosLines.RemoveAll();
	}


	o_String = EAI_STR_Csz_ModelName;
	o_String += BIG_NameFile(mul_CurrentEditModel);

	if(EDI_gpo_EnterWnd)
	{
		EDI_gpo_EnterWnd->DisplayMessage((char *) (LPCSTR) o_String);
	}
	else
	{
		o_String += "\n";
		LINK_PrintStatusMsg((char *) (LPCSTR) o_String);
	}

	/* Read model in a temporary buffer */
	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(mul_CurrentEditModel), &ul_SizeModel);

	/* Save current model in compiler */
	mo_Compiler.mpst_CurrentModel = pst_Buf;
	mo_Compiler.mi_SizeModel = ul_SizeModel / sizeof(BIG_tdst_GroupElem);
	mo_Compiler.mul_CurrentModel = mul_CurrentEditModel;
	mo_Compiler.mul_CurrentInstance = mul_CurrentEditInstance;

	/* Generate result of PP */
	mo_Compiler.mb_GenPP = FALSE;
	mo_Compiler.mb_ForceGenPP = FALSE;
	if(AI2C_gb_Activated) mo_Compiler.mb_GenPP = TRUE;

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Create (or erase) all functions, cause compilation of variables can need to request the engine function file (its
	    key). So when compilation began, all engine files must already exist
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(i = 0; i < (ul_SizeModel / sizeof(BIG_tdst_GroupElem)); i++)
	{
		if(!pst_Buf[i].ul_Key) continue;
		ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
		if(ul_File == BIG_C_InvalidIndex) continue;
		L_strcpy(asz_Name, BIG_NameFile(ul_File));
		psz_Temp = L_strrchr(asz_Name, '.');
		if(!psz_Temp) continue;

		/* Is it a function file ? */
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFct))
		{
			*psz_Temp = 0;
			L_strcat(asz_Name, EDI_Csz_ExtAIEngineFct);
			BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Path);
			if(BIG_ul_SearchFileExt(asz_Path, asz_Name) == BIG_C_InvalidIndex)
			{
				SAV_Begin(asz_Path, asz_Name);
				SAV_ul_End();
			}
		}

		/* Is it a proclist file ? */
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFctLib))
		{
			*psz_Temp = 0;
			L_strcat(asz_Name, EDI_Csz_ExtAIEngineFctLib);
			BIG_ComputeFullName(BIG_ParentFile(ul_File), asz_Path);
			if(BIG_ul_SearchFileExt(asz_Path, asz_Name) == BIG_C_InvalidIndex)
			{
				SAV_Begin(asz_Path, asz_Name);
				SAV_ul_End();
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    First compile all global variables of the model
	 -------------------------------------------------------------------------------------------------------------------
	 */

	b_Ok = OnCompileVarsOfModel(&mo_Compiler, mul_CurrentEditModel, mst_Ini.b_LookDate);
	if(!b_Ok)
	{
		LINK_PrintStatusMsg(EAI_STR_Csz_Error);
		L_free(pst_Buf);
		if(mpst_Model) mpst_Model->c_ModelInvalid = 1;
		return FALSE;
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Then compile all proclist
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(i = 0; i < (ul_SizeModel / sizeof(BIG_tdst_GroupElem)); i++)
	{
		static int ii = 0;
		if(!pst_Buf[i].ul_Key) continue;
		ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
		if(ul_File == BIG_C_InvalidIndex) continue;
		psz_Temp = L_strrchr(BIG_NameFile(ul_File), '.');
		if(!psz_Temp) continue;

		/* Is it a function file ? */
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFctLib))
		{
			psz_Temp1 = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_File), &ul_Size);
			if(!psz_Temp1) psz_Temp1 = L_strdup(" ");
			mo_Compiler.ResetVars(&mo_Compiler.mst_LocalVars);
			mo_Compiler.mb_ProcList = TRUE;
			mo_Compiler.mi_NumProc = 0;
			mo_Compiler.mb_ProcListParent = TRUE;
			gu16_ProcLink++;
			ii++;
			if(ii == 8) aaa = 1;
			b_Ok = OnCompileFile(&mo_Compiler, mul_CurrentEditModel, ul_File, psz_Temp1);
			mo_Compiler.mb_ProcList = FALSE;
			mo_Compiler.mb_ProcListParent = FALSE;
			mo_Compiler.mb_ProcListLocal = FALSE;
			L_free(psz_Temp1);
			if(!b_Ok) break;
			continue;
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Then compile all functions
	 -------------------------------------------------------------------------------------------------------------------
	 */

	for(i = 0; b_Ok && (i < (ul_SizeModel / sizeof(BIG_tdst_GroupElem))); i++)
	{
		if(!pst_Buf[i].ul_Key) continue;
		ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
		if(ul_File == BIG_C_InvalidIndex) continue;
		psz_Temp = L_strrchr(BIG_NameFile(ul_File), '.');
		if(!psz_Temp) continue;

		/* Is it a function file ? */
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFct))
		{
			if(mst_Ini.b_LookDate)
			{
				if(mo_ModelDateList.Lookup((void *) pst_Buf[i].ul_Key, (void * &) date))
				{
					if(BIG_TimeFile(ul_File) == (int) date) continue;
				}

				mo_ModelDateList.SetAt((void *) pst_Buf[i].ul_Key, (void *) BIG_TimeFile(ul_File));
			}

			psz_Temp1 = BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_File), &ul_Size);
			if(!psz_Temp1) psz_Temp1 = L_strdup(" ");
			mo_Compiler.ResetVars(&mo_Compiler.mst_LocalVars);
			b_Ok = OnCompileFile(&mo_Compiler, mul_CurrentEditModel, ul_File, psz_Temp1);
			L_free(psz_Temp1);

			if(!b_Ok) 
			{
				mo_ModelDateList.RemoveKey((void *) pst_Buf[i].ul_Key);
				break;
			}
			continue;
		}
	}

	L_free(pst_Buf);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Generate engine model
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(b_Ok)
	{
		GenerateEngineModel(mul_CurrentEditModel);

		if(mst_Ini.b_WarningHigh)
		{
			pos = mo_Compiler.mst_GlobalVars.o_Vars.GetStartPosition();
			while(pos)
			{
				mo_Compiler.mst_GlobalVars.o_Vars.GetNextAssoc(pos, str, (void * &) po_Var);
				if(po_Var->mi_SpecialVar & (VAR_TRACK | VAR_SEP)) continue;
				if(!po_Var->mb_HasBeenUsed)
				{
					str1.Format("Model %s : Global variable %s is not used", BIG_NameFile(mul_CurrentEditModel), str);
					ERR_X_Warning(0, (char *) (LPCSTR) str1, 0);
				}
			}
		}
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    If engine model is present, resolve.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(b_Ok)
	{
		UpdateEngineVars(&mo_Compiler);
		if(mpst_Instance)
		{
			AI_ReinitInstance(mpst_Instance);
		}
		else if(mpst_Model && WOR_gst_Universe.pst_AI && WOR_gst_Universe.pst_AI->pst_Model == mpst_Model)
		{
			AI_ReinitInstance(WOR_gst_Universe.pst_AI);
			WOR_gst_Universe.pst_AI->uw_FirstTrack = AI_C_MaxTracks;
			WOR_gst_Universe.pst_AI->uw_LastTrack = 0;
			for(i = 0; i < AI_C_MaxTracks; i++)
			{
				if(WOR_gst_Universe.pst_AI->pst_Model->pst_VarDes->apst_InitFct[i] && i < WOR_gst_Universe.pst_AI->uw_FirstTrack) 
					WOR_gst_Universe.pst_AI->uw_FirstTrack = (unsigned short) i;
				if(WOR_gst_Universe.pst_AI->pst_Model->pst_VarDes->apst_InitFct[i] && i > WOR_gst_Universe.pst_AI->uw_LastTrack) 
					WOR_gst_Universe.pst_AI->uw_LastTrack = (unsigned short) i;
				if(WOR_gst_Universe.pst_AI->apst_CurrentFct[i])
					WOR_gst_Universe.pst_AI->apst_JumpNode[i] = WOR_gst_Universe.pst_AI->apst_CurrentFct[i]->pst_RootNode;
				else
					WOR_gst_Universe.pst_AI->apst_JumpNode[i] = NULL;

				/* Time to wait */
				WOR_gst_Universe.pst_AI->af_WaitTime[i] = 0.0f;
			}
		}

		AI_ReinitBreakMode();
		SetEngineBreakPoint();
		AI_Reset();
	}

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Error during compile.
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(!b_Ok)
	{
		if(mpst_Model) mpst_Model->c_ModelInvalid = 1;
	}
	else
	{
		if(mpst_Model) mpst_Model->c_ModelInvalid = 0;
		if(AI2C_gb_Activated) AI2C_ModelToC(&mo_Compiler, mul_CurrentEditModel);
	}

	Resolve(&mo_Compiler);

	if(!mb_CompileDep)
	{
		M_MF()->LockDisplay(mpo_Edit);
		mpo_ListAI->SelectFromFile();
		if(b_Ok)
		{
			if(i_ScrollH != mpo_Edit->GetScrollPos(SB_HORZ))
			{
				mpo_Edit->SetScrollPos(SB_HORZ, i_ScrollH);
				mpo_Edit->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (i_ScrollH << 16), 0);
			}

			if(i_ScrollV != mpo_Edit->GetScrollPos(SB_VERT))
			{
				mpo_Edit->SetScrollPos(SB_VERT, i_ScrollV);
				mpo_Edit->SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (i_ScrollV << 16), 0);
			}

			mpo_Edit->SetSel(cr);
		}

		M_MF()->UnlockDisplay(mpo_Edit);
	}

	if(!mb_CompileAll && !mb_CompileDep && b_Ok) LINK_UpdatePointers();
	if(!AI_gb_Compile1 && !mb_CompileAll && !mb_CompileDep) ResetRefCompilers();

	/* Save colors */
	if(!mb_CompileAll) SaveColors();

	LINK_PrintStatusMsg(" : OK");
	return b_Ok;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::LoadColors(void)
{
	BIG_INDEX	parent;
	char		path[512];
	char		name[512];
	CString		o_Name, o_Val;
	char		*pc_Tmp;
	ULONG		ul_Size;
	BIG_INDEX	ul_File;
	char		*pz1, *pz2;
	int					num, *pi2, mem;
	POSITION			pos;
	SCR_sc_Procedure	*pt_ProcEd;
	SCR_sc_Procedure	*pt_ProcEd1;

	return;
	parent = BIG_ParentFile(mul_CurrentEditModel);
	L_strcpy(name, BIG_NameFile(mul_CurrentEditModel));
	*L_strrchr(name, '.') = 0;
	BIG_ComputeFullName(parent, path);
	L_strcat(path, "/");
	L_strcat(path, name);
	L_strcat(name, ".mdc");
	ul_File = BIG_ul_SearchFileExt(path, name);
	if(ul_File == BIG_C_InvalidIndex) return;

	pc_Tmp = BIG_pc_ReadFileTmp(BIG_PosFile(ul_File), &ul_Size);

	go_PPConstants.RemoveAll();
	go_PPConstantsFiles.RemoveAll();
	go_PPConstantsLines.RemoveAll();
	pos = mo_Compiler.mo_ProcList.GetStartPosition();
	while(pos)
	{
		mo_Compiler.mo_ProcList.GetNextAssoc(pos, o_Name, (void * &) pt_ProcEd);
		if(pt_ProcEd->pt_Parameters) MEM_Free(pt_ProcEd->pt_Parameters);
		delete pt_ProcEd;
	}
	mo_Compiler.mo_ProcList.RemoveAll();

	while(*pc_Tmp)
	{
		num = *(int *) pc_Tmp;
		pc_Tmp += 4;
		while(num--)
		{
			pz1 = (char *) pc_Tmp;
			pz2 = (char *) pc_Tmp + strlen(pz1) + 1;
			go_PPConstants.SetAt(pz1, pz2);
			pc_Tmp = pz2 += strlen(pz2) + 1;
		}

		num = *(int *) pc_Tmp;
		pc_Tmp += 4;
		while(num--)
		{
			pz1 = (char *) pc_Tmp;
			pi2 = (int *) ((char *) pc_Tmp + strlen(pz1) + 1);
			mem = *pi2;
			go_PPConstantsFiles.SetAt(pz1, (void *) mem);
			pc_Tmp = (char *) (pi2 + 1);
		}

		num = *(int *) pc_Tmp;
		pc_Tmp += 4;
		while(num--)
		{
			pz1 = (char *) pc_Tmp;
			pi2 = (int *) ((char *) pc_Tmp + strlen(pz1) + 1);
			mem = *pi2;
			go_PPConstantsLines.SetAt(pz1, (void *) mem);
			pc_Tmp = (char *) (pi2 + 1);
		}

		num = *(int *) pc_Tmp;
		pc_Tmp += 4;
		while(num--)
		{
			pz1 = (char *) pc_Tmp;
			pi2 = (int *) ((char *) pc_Tmp + strlen(pz1) + 1);
			pt_ProcEd = (SCR_sc_Procedure *) pi2;
			pt_ProcEd1 = new SCR_sc_Procedure;
			L_memcpy(pt_ProcEd1, pt_ProcEd, sizeof(SCR_sc_Procedure));
			mo_Compiler.mo_ProcList.SetAt(pz1, pt_ProcEd1);
			if(pt_ProcEd->u32_NumParameters)
			{
				pt_ProcEd1->pt_Parameters = (SCR_sc_ProcedureParam *) MEM_p_Alloc(pt_ProcEd->u32_NumParameters * sizeof(SCR_sc_ProcedureParam));
				pt_ProcEd++;
				L_memcpy(pt_ProcEd1->pt_Parameters, pt_ProcEd, pt_ProcEd1->u32_NumParameters * sizeof(SCR_sc_ProcedureParam));
				pc_Tmp = (((char *) pt_ProcEd) + pt_ProcEd1->u32_NumParameters * sizeof(SCR_sc_ProcedureParam));
			}
			else
			{
				pc_Tmp = (char *) (pt_ProcEd + 1);
			}
		}
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::SaveColors(void)
{
	BIG_INDEX	parent;
	char		path[512];
	char		name[512];
	POSITION	pos;
	int			num;
	CString		o_Name, o_Val;
	ULONG		ul_Index;
	SCR_sc_Procedure	*pt_ProcEd;

	return;
	parent = BIG_ParentFile(mul_CurrentEditModel);
	L_strcpy(name, BIG_NameFile(mul_CurrentEditModel));
	*L_strrchr(name, '.') = 0;
	BIG_ComputeFullName(parent, path);
	L_strcat(path, "/");
	L_strcat(path, name);
	L_strcat(name, ".mdc");
	SAV_Begin(path, name);

	pos = go_PPConstants.GetStartPosition();
	num = go_PPConstants.GetCount();
	SAV_Buffer(&num, 4);
	while(pos)
	{
		go_PPConstants.GetNextAssoc(pos, o_Name, o_Val);
		SAV_Buffer((char *) (LPCSTR) o_Name, o_Name.GetLength() + 1);
		SAV_Buffer((char *) (LPCSTR) o_Val, o_Val.GetLength() + 1);
	}

	pos = go_PPConstantsFiles.GetStartPosition();
	num = go_PPConstantsFiles.GetCount();
	SAV_Buffer(&num, 4);
	while(pos)
	{
		go_PPConstantsFiles.GetNextAssoc(pos, o_Name, (void * &) ul_Index);
		SAV_Buffer((char *) (LPCSTR) o_Name, o_Name.GetLength() + 1);
		SAV_Buffer(&ul_Index, 4);
	}

	pos = go_PPConstantsLines.GetStartPosition();
	num = go_PPConstantsLines.GetCount();
	SAV_Buffer(&num, 4);
	while(pos)
	{
		go_PPConstantsLines.GetNextAssoc(pos, o_Name, (void * &) ul_Index);
		SAV_Buffer((char *) (LPCSTR) o_Name, o_Name.GetLength() + 1);
		SAV_Buffer(&ul_Index, 4);
	}

	num = mo_Compiler.mo_ProcList.GetCount();
	SAV_Buffer(&num, 4);
	pos = mo_Compiler.mo_ProcList.GetStartPosition();
	while(pos)
	{
		mo_Compiler.mo_ProcList.GetNextAssoc(pos, o_Name, (void * &) pt_ProcEd);
		SAV_Buffer((char *) (LPCSTR) o_Name, o_Name.GetLength() + 1);
		SAV_Buffer(pt_ProcEd, sizeof(SCR_sc_Procedure));
		if(pt_ProcEd->u32_NumParameters)
			SAV_Buffer(pt_ProcEd->pt_Parameters, pt_ProcEd->u32_NumParameters * sizeof(SCR_sc_ProcedureParam));
	}

	SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnCompileCurrentModelDep(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX			ul_Model, ul_Instance, ul_EditFile;
	char				asz_Path[BIG_C_MaxLenPath];
	char				asz_Name[BIG_C_MaxLenName];
	char				*psz_Temp;
	BIG_INDEX			ul_File;
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_Size, i, ul_Adr;
	BOOL				b_Ok;
	int					i_ScrollV;
	int					i_ScrollH;
	CHARRANGE			cr;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Don't compile dependencies if vars have not been touched */
	if(!mb_HasTouchedVars)
	{
		OnCompileCurrentModel();
		LINK_PrintStatusMsg("Finish");
		return;
	}

	mb_HasTouchedVars = FALSE;
	AfxGetApp()->DoWaitCursor(1);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    First compile all dependencies
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ul_Model = mul_CurrentEditModel;
	ul_Instance = mul_CurrentEditInstance;
	ul_EditFile = mul_CurrentEditFile;
	i_ScrollH = mpo_Edit->GetScrollPos(SB_HORZ);
	i_ScrollV = mpo_Edit->GetScrollPos(SB_VERT);
	mpo_Edit->GetSel(cr);
	b_Ok = TRUE;

	/* Get the dependencies file for that model */
	BIG_ComputeFullName(BIG_ParentFile(ul_Model), asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(ul_Model));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, asz_Name);
	L_strcat(asz_Name, EDI_Csz_ExtAIEditorDepend);

	ul_File = BIG_ul_SearchFileExt(asz_Path, asz_Name);
	if(ul_File != BIG_C_InvalidIndex)
	{
		RefreshMenu();

		/* Parse all models of dependencie list */
		mb_ListMode = TRUE;
		pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(ul_File), &ul_Size);
		for(i = 0; i < (ul_Size / sizeof(BIG_tdst_GroupElem)); i++)
		{
			if(!pst_Buf[i].ul_Key) continue;
			ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
			if(ul_File == BIG_C_InvalidIndex) continue;

			mb_CompileDep = TRUE;
			b_Ok = i_OnMessage(EDI_MESSAGE_SELFILE, 0, ul_File);
			mb_CompileDep = FALSE;
			if(!b_Ok) break;

			mb_CompileDep = TRUE;
			b_Ok = OnCompileCurrentModel();
			mb_CompileDep = FALSE;
			if(!b_Ok) break;
			if(LINK_gb_EscapeDetected) break;
		}

		L_free(pst_Buf);
		mb_ListMode = FALSE;
	}

	/* Be sure that AI list is correct */
	M_MF()->LockDisplay(mpo_Edit);
	if(b_Ok)
	{
		M_MF()->LockDisplay(this);
		OnClose(TRUE);
		if(ul_Instance != BIG_C_InvalidIndex)
		{
			ul_Adr = LOA_ul_SearchAddress(BIG_PosFile(ul_Instance));
			if(ul_Adr != -1)
				SelData(ul_Adr);
			else
				SelFile(ul_Instance);
		}
		else
		{
			ul_Adr = LOA_ul_SearchAddress(BIG_PosFile(ul_Model));
			if(ul_Adr != -1)
				SelData(ul_Adr);
			else
				SelFile(ul_Model);
		}

		mb_LockHistory = TRUE;
		Load(ul_EditFile);
		mb_LockHistory = FALSE;
		M_MF()->UnlockDisplay(this);
	}

	InitListAI();
	mpo_ListAI->SelectFromFile();

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    And then compile model
	 -------------------------------------------------------------------------------------------------------------------
	 */

	if(b_Ok)
	{
		mb_CompileDep = TRUE;
		b_Ok = OnCompileCurrentModel();
		mb_CompileDep = FALSE;

		if(i_ScrollH != mpo_Edit->GetScrollPos(SB_HORZ))
		{
			mpo_Edit->SetScrollPos(SB_HORZ, i_ScrollH);
			mpo_Edit->SendMessage(WM_HSCROLL, SB_THUMBPOSITION + (i_ScrollH << 16), 0);
		}

		if(i_ScrollV != mpo_Edit->GetScrollPos(SB_VERT))
		{
			mpo_Edit->SetScrollPos(SB_VERT, i_ScrollV);
			mpo_Edit->SendMessage(WM_VSCROLL, SB_THUMBPOSITION + (i_ScrollV << 16), 0);
		}

		mpo_Edit->SetSel(cr);
	}

	LINK_PrintStatusMsg("Finish");
	AfxGetApp()->DoWaitCursor(-1);

	M_MF()->UnlockDisplay(mpo_Edit);

	LINK_UpdatePointers();
	mb_CanReload = FALSE;
	M_MF()->FatHasChanged();
	mb_CanReload = TRUE;
}

static EDIA_cl_UPDATEDialog *po_Dial;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG SetState(float Position, char *Text)
{
	if(!po_Dial) return 0;
	return po_Dial->OnRefreshBarText(Position, Text);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnCompileAllModels(void)
{
	/*~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index;
	MSG			msg;
	char		az[512];
	/*~~~~~~~~~~~~~~~~~*/


	/* Generate C files */
	if(LOA_gb_SlashY) AI2C_gb_Activated = TRUE;

    if(AI2C_gb_FctListInit)
    {
        AI2C_gb_FctListInit = FALSE;
        BAS_bfree(&AI2C_gst_FctList);
    }

	if(!OnClose(FALSE, TRUE)) return;
    ERR_LogPrint("[AI compilation start]\n");
	while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) DispatchMessage(&msg);

	OnDeleteAllBkp();
	po_Dial = NULL;
	mo_Compiler.mul_NbNodes = 0;
	mo_Compiler.mul_NbDelNodes = 0;
	ResetRefCompilers();
	mo_Compiler.Reset();
	ul_Index = BIG_ul_SearchDir(EDI_Csz_Path_AIModels);
	ERR_X_Error(ul_Index != BIG_C_InvalidIndex, "No ai model path !!", NULL);
	mb_CompileAll = TRUE;
	AfxGetApp()->DoWaitCursor(1);
	mul_NumberAIModels = 0;
	mul_CurrentAIModel = 0;
	/* First, we count the number of AI Models */
_Try_
	if(OnCompileAllModels(ul_Index, FALSE, NULL)) OnClose(TRUE, TRUE);
_Catch_
_End_

	/* Then, we compile. */
	po_Dial = new EDIA_cl_UPDATEDialog((char *) "");
	po_Dial->DoModeless();

#ifdef BENCH_IA_TEST
	AIBnch_BeforeCompileAll();
#endif

_Try_
	if(OnCompileAllModels(ul_Index, TRUE, SetState)) 
    {
        OnClose(TRUE, TRUE);
        ERR_LogPrint("[AI compilation success]\n");
    }
    else
        ERR_LogPrint("[AI compilation failure]\n");

_Catch_
_End_

#ifdef BENCH_IA_TEST
	AIBnch_AfterCompileAll();
#endif

	if(po_Dial) delete(po_Dial);

	mb_CompileAll = FALSE;
	AfxGetApp()->DoWaitCursor(-1);

	sprintf(az, "Finish (%d nodes, %d del nodes)", mo_Compiler.mul_NbNodes, mo_Compiler.mul_NbDelNodes);
	LINK_PrintStatusMsg(az);

	LINK_UpdatePointers();
	M_MF()->FatHasChanged();

	mo_Compiler.Reset(TRUE);
	ResetRefCompilers();
}

extern CList<BIG_INDEX, BIG_INDEX>	o_SkipMdl;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::OnCompileAllModels(BIG_INDEX _ul_Dir, BOOL _b_Compile, ULONG (*SetBarrePos) (float F01, char *))
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index, ul_Temp;
	BOOL		b_Ok;
	char		asz_Path[BIG_C_MaxLenPath];
	char		asz_Name[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	b_Ok = TRUE;

	/* Delete dependencies file */
	ul_Index = _ul_Dir;
	ul_Index = BIG_FirstFile(ul_Index);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtAIEditorModel))
		{
			BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
			L_strcat(asz_Path, "/");
			L_strcat(asz_Path, BIG_NameFile(ul_Index));
			*L_strrchr(asz_Path, '.') = 0;
			L_strcpy(asz_Name, BIG_NameFile(ul_Index));
			*L_strrchr(asz_Name, '.') = 0;
			L_strcat(asz_Name, EDI_Csz_ExtAIEditorDepend);
			if(BIG_ul_SearchFileExt(asz_Path, asz_Name) != BIG_C_InvalidIndex)
			{
				SAV_Begin(asz_Path, asz_Name);
				ul_Temp = 0;
				SAV_Buffer(&ul_Temp, 4);
				SAV_Buffer(&ul_Temp, 4);
				SAV_ul_End();
			}
		}

		ul_Index = BIG_NextFile(ul_Index);
	}

	/* Compile */
	ul_Index = _ul_Dir;
	ERR_X_Error(ul_Index != BIG_C_InvalidIndex, "No ai model path !!", NULL);
	ul_Index = BIG_FirstFile(ul_Index);
	while(ul_Index != BIG_C_InvalidIndex)
	{
		if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtAIEditorModel))
		{
			if(o_SkipMdl.Find(ul_Index))
			{
				ul_Index = BIG_NextFile(ul_Index);
				continue;
			}

			if(_b_Compile)
			{
				extern BOOL EDIA_OnlyTotal;
				b_Ok = i_OnMessage(EDI_MESSAGE_SELFILE, 0, ul_Index);
				if(!b_Ok) break;

				L_strcpy(asz_Name, BIG_NameFile(ul_Index));
				*L_strrchr(asz_Name, '.') = 0;
				sprintf(asz_Path, "[%u/%u] Compiling %s", mul_CurrentAIModel, mul_NumberAIModels, asz_Name);
				EDIA_OnlyTotal = TRUE;
				SetBarrePos((float)mul_CurrentAIModel / (float)mul_NumberAIModels , asz_Path);
				if(po_Dial->bIsCanceled) return 0;
				b_Ok = OnCompileCurrentModel();
				mul_CurrentAIModel ++;
			}
			else
			{
				b_Ok = TRUE;
				mul_NumberAIModels ++;
			}
			if(!b_Ok) break;
			if(LINK_gb_EscapeDetected) break;
		}

		ul_Index = BIG_NextFile(ul_Index);
	}

	/* Recurse */
	if(b_Ok)
	{
		ul_Index = BIG_SubDir(_ul_Dir);
		while(ul_Index != BIG_C_InvalidIndex)
		{
			if(!OnCompileAllModels(ul_Index, _b_Compile, SetBarrePos)) return FALSE;
			ul_Index = BIG_NextDir(ul_Index);
		}
	}

	return b_Ok;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::GenerateEngineModel(BIG_INDEX _ul_Model)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char				*psz_Temp;
	char				asz_Name[BIG_C_MaxLenName];
	char				asz_Path[BIG_C_MaxLenPath];
	BIG_INDEX			ul_File, ul_Dir;
	ULONG				ul_Size;
	ULONG				i;
	BIG_tdst_GroupElem	*pst_Buf;
	BOOL				b_CanVar;
	BIG_KEY				ul_Key;
	BIG_INDEX			ul_EngModel;
	char				az[512];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Generate engine model file
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ul_EngModel = mo_Compiler.ul_GetEngineModelIndex(_ul_Model, asz_Path, asz_Name);
	ul_Dir = BIG_ul_CreateDir(asz_Path);

	/* Create file to write */
	SAV_Begin(asz_Path, asz_Name);

	b_CanVar = TRUE;

	/* Load editor model */
	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(_ul_Model), &ul_Size);
	for(i = 0; i < (ul_Size / sizeof(BIG_tdst_GroupElem)); i++)
	{
		if(!pst_Buf[i].ul_Key) continue;
		ul_File = BIG_ul_SearchKeyToFat(pst_Buf[i].ul_Key);
		sprintf(az, "Unknown key (%x)", pst_Buf[i].ul_Key);
		ERR_X_Error(ul_File != BIG_C_InvalidIndex, az, NULL);
		psz_Temp = L_strrchr(BIG_NameFile(ul_File), '.');
		if(!psz_Temp)
			continue;

		/*$2
		 ---------------------------------------------------------------------------------------------------------------
		    All extensions
		 ---------------------------------------------------------------------------------------------------------------
		 */

		/* AI vars */
		if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorVars) && b_CanVar)
		{
			L_strcpy(asz_Name, BIG_NameFile(_ul_Model));
			psz_Temp = L_strrchr(asz_Name, '.');
			if(psz_Temp) *psz_Temp = 0;
			L_strcat(asz_Name, EDI_Csz_ExtAIEngineVars);

			ul_File = BIG_ul_SearchFileExt(asz_Path, asz_Name);
			if(ul_File != BIG_C_InvalidIndex)
			{
				ul_Key = BIG_FileKey(ul_File);
				SAV_Buffer(&ul_Key, sizeof(BIG_KEY));
				SAV_Buffer(EDI_Csz_ExtAIEngineVars, L_strlen(EDI_Csz_ExtAIEngineVars));
			}
			else
			{
				ul_Key = 0;
				SAV_Buffer(&ul_Key, sizeof(BIG_KEY));
				SAV_Buffer(EDI_Csz_ExtAIEngineVars, L_strlen(EDI_Csz_ExtAIEngineVars));
			}

			b_CanVar = FALSE;
		}

		/* AI fct */
		else if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFct))
		{
			L_strcpy(asz_Name, BIG_NameFile(ul_File));
			psz_Temp = L_strrchr(asz_Name, '.');
			if(psz_Temp) *psz_Temp = 0;
			L_strcat(asz_Name, EDI_Csz_ExtAIEngineFct);
			ul_File = BIG_ul_SearchFile(BIG_ParentFile(ul_File), asz_Name);
			ul_Key = BIG_FileKey(ul_File);
			SAV_Buffer(&ul_Key, sizeof(BIG_KEY));
			SAV_Buffer(EDI_Csz_ExtAIEngineFct, L_strlen(EDI_Csz_ExtAIEngineFct));
		}

		/* ProcList */
		else if(!L_strcmpi(psz_Temp, EDI_Csz_ExtAIEditorFctLib))
		{
			L_strcpy(asz_Name, BIG_NameFile(ul_File));
			psz_Temp = L_strrchr(asz_Name, '.');
			if(psz_Temp) *psz_Temp = 0;
			L_strcat(asz_Name, EDI_Csz_ExtAIEngineFctLib);
			ul_File = BIG_ul_SearchFile(BIG_ParentFile(ul_File), asz_Name);
			ul_Key = BIG_FileKey(ul_File);
			SAV_Buffer(&ul_Key, sizeof(BIG_KEY));
			SAV_Buffer(EDI_Csz_ExtAIEngineFctLib, L_strlen(EDI_Csz_ExtAIEngineFctLib));
		}
	}

	L_free(pst_Buf);

	/* End of save */
	mul_CurrentEngineModel = SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnCreateInstance(void)
{
	/* First compile model */
	OnCompileCurrentModel();

	/* Create dummy instance */
	AI_ul_CreateInstance(NULL, BIG_C_InvalidIndex, mul_CurrentEditModel, NULL);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnSaveInstance(void)
{
_Try_
	if(mpst_Instance) AI_SaveInstance(mpst_Instance);
_Catch_
_End_
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::OnReinitInstance(void)
{
	AI_ReinitInstance(mpst_Instance);
}

/*$4
 ***********************************************************************************************************************
    Instance
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::SetCurrentInstance(BIG_INDEX _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG		*pul_Buf;
	ULONG		ul_Size;
	BIG_INDEX	ul_EngModel, ul_EdModel;
	char		asz_Name[BIG_C_MaxLenPath];
	char		asz_Path[BIG_C_MaxLenPath];
	char		*psz_Temp;
	BOOL		b_Ok;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Want to change with the same instance */
	if(mul_CurrentEditInstance == _ul_Index) return TRUE;

	/* Read reference to model, and load it */
	pul_Buf = (ULONG *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(_ul_Index), &ul_Size);
	ul_EngModel = BIG_ul_SearchKeyToFat(*pul_Buf);
	if(ul_EngModel == BIG_C_InvalidIndex)
	{
		L_free(pul_Buf);
		return FALSE;
	}

	/* Compute editor model */
	BIG_ComputeFullName(BIG_ParentFile(ul_EngModel), asz_Path);
	psz_Temp = L_strrchr(asz_Path, '/');
	if(psz_Temp) *psz_Temp = 0;

	L_strcpy(asz_Name, BIG_NameFile(ul_EngModel));
	psz_Temp = L_strrchr(asz_Name, '.');
	if(*psz_Temp) *psz_Temp = 0;
	L_strcat(asz_Name, EDI_Csz_ExtAIEditorModel);

	ul_EdModel = BIG_ul_SearchFileExt(asz_Path, asz_Name);
	if(ul_EdModel == BIG_C_InvalidIndex)
	{
		L_free(pul_Buf);
		return FALSE;
	}

	/* Save instance */
	mul_CurrentEditInstance = _ul_Index;

	/* Set model */
	b_Ok = SetCurrentModel(ul_EdModel);

	/* Free model buffer */
	L_free(pul_Buf);

	return b_Ok;
}

/*$4
 ***********************************************************************************************************************
    Models
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::UpdateOwner(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Path[BIG_C_MaxLenPath];
	char	asz_Path1[BIG_C_MaxLenPath];
	char	asz_Name[BIG_C_MaxLenPath];
	char	asz_Name1[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!M_MF()->mst_Ini.b_LinkControlON) return;
	if(!mb_UpdateOwner) return;
	AfxGetApp()->DoWaitCursor(1);
	BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Path);
	L_strcpy(asz_Path1, asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditModel));
	L_strcpy(asz_Name1, BIG_NameFile(mul_CurrentEditModel));
	*L_strrchr(asz_Name, '.') = 0;
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, asz_Name);
	M_MF()->UpdateOwnerPath(asz_Path, asz_Path1, asz_Name1);
	RefreshDialogBar();
	DisplayPaneNames();
	AfxGetApp()->DoWaitCursor(-1);
}

extern BOOL EDI_gb_NoUpdateVSS;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::UndoCheckOut(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Path[BIG_C_MaxLenPath];
	char	asz_Path1[BIG_C_MaxLenPath];
	char	asz_Name[BIG_C_MaxLenPath];
	char	asz_Name1[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;

	AfxGetApp()->DoWaitCursor(1);
	BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Path);
	L_strcpy(asz_Path1, asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditModel));
	L_strcpy(asz_Name1, BIG_NameFile(mul_CurrentEditModel));
	*L_strrchr(asz_Name, '.') = 0;
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, asz_Name);

	if(M_MF()->b_LockBigFile())
	{
		EDI_can_lock = FALSE;
		M_MF()->UndoCheckOutFile(asz_Path1, asz_Name1);
		EDI_gb_NoUpdateVSS = TRUE;
		M_MF()->UndoCheckOutDir(asz_Path);
		EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
	}

	/* M_MF()->FatHasChanged(); */
	RefreshDialogBar();
	DisplayPaneNames();
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gb_NoUpdateVSS = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::CheckOut(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Path[BIG_C_MaxLenPath];
	char	asz_Path1[BIG_C_MaxLenPath];
	char	asz_Name[BIG_C_MaxLenPath];
	char	asz_Name1[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;

	AfxGetApp()->DoWaitCursor(1);
	BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Path);
	L_strcpy(asz_Path1, asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditModel));
	L_strcpy(asz_Name1, BIG_NameFile(mul_CurrentEditModel));
	*L_strrchr(asz_Name, '.') = 0;
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, asz_Name);

	if(M_MF()->b_LockBigFile())
	{
		EDI_can_lock = FALSE;
		M_MF()->CheckOutFile(asz_Path1, asz_Name1);
		EDI_gb_NoUpdateVSS = TRUE;
		M_MF()->CheckOutDir(asz_Path);
		EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
	}

	RefreshDialogBar();
	DisplayPaneNames();
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gb_NoUpdateVSS = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::CheckIn(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Path[BIG_C_MaxLenPath];
	char	asz_Path1[BIG_C_MaxLenPath];
	char	asz_Name[BIG_C_MaxLenPath];
	char	asz_Name1[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;

	AfxGetApp()->DoWaitCursor(1);
	BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Path);
	L_strcpy(asz_Path1, asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditModel));
	L_strcpy(asz_Name1, BIG_NameFile(mul_CurrentEditModel));
	*L_strrchr(asz_Name, '.') = 0;
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, asz_Name);

	if(M_MF()->b_LockBigFile())
	{
		EDI_can_lock = FALSE;
		M_MF()->CheckInFile(asz_Path1, asz_Name1);
		EDI_gb_NoUpdateVSS = TRUE;
		M_MF()->CheckInDir(asz_Path);
		EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
	}

	RefreshDialogBar();
	DisplayPaneNames();
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gb_NoUpdateVSS = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::CheckInOut(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Path[BIG_C_MaxLenPath];
	char	asz_Path1[BIG_C_MaxLenPath];
	char	asz_Name[BIG_C_MaxLenPath];
	char	asz_Name1[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;

	AfxGetApp()->DoWaitCursor(1);
	BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Path);
	L_strcpy(asz_Path1, asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditModel));
	L_strcpy(asz_Name1, BIG_NameFile(mul_CurrentEditModel));
	*L_strrchr(asz_Name, '.') = 0;
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, asz_Name);

	if(M_MF()->b_LockBigFile())
	{
		EDI_can_lock = FALSE;
		M_MF()->CheckInFileNoOut(asz_Path1, asz_Name1);
		EDI_gb_NoUpdateVSS = TRUE;
		M_MF()->CheckInDirNoOut(asz_Path);
		EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
	}

	RefreshDialogBar();
	DisplayPaneNames();
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gb_NoUpdateVSS = FALSE;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void EAI_cl_Frame::GetLatest(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	char	asz_Path[BIG_C_MaxLenPath];
	char	asz_Path1[BIG_C_MaxLenPath];
	char	asz_Name[BIG_C_MaxLenPath];
	char	asz_Name1[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	EDI_go_MsgGetLatest.mb_ApplyToAll = FALSE;
	EDI_go_MsgUndoCheck.mb_ApplyToAll = FALSE;
	EDI_go_MsgCheckInNo.mb_ApplyToAll = FALSE;
	LINK_gb_EscapeDetected = FALSE;

	AfxGetApp()->DoWaitCursor(1);
	BIG_ComputeFullName(BIG_ParentFile(mul_CurrentEditModel), asz_Path);
	L_strcpy(asz_Path1, asz_Path);
	L_strcpy(asz_Name, BIG_NameFile(mul_CurrentEditModel));
	L_strcpy(asz_Name1, BIG_NameFile(mul_CurrentEditModel));
	*L_strrchr(asz_Name, '.') = 0;
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, asz_Name);

	if(M_MF()->b_LockBigFile())
	{
		EDI_can_lock = FALSE;
		M_MF()->GetLatestVersionFile(asz_Path1, asz_Name1);
		EDI_gb_NoUpdateVSS = TRUE;
		M_MF()->GetLatestVersionDir(asz_Path);
		EDI_can_lock = TRUE;
		M_MF()->UnLockBigFile();
	}

	RefreshDialogBar();
	DisplayPaneNames();
	AfxGetApp()->DoWaitCursor(-1);
	EDI_gb_NoUpdateVSS = FALSE;
}

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
BOOL EAI_cl_Frame::SetCurrentModel(BIG_INDEX _ul_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BOOL				b_Ok;
	BIG_tdst_GroupElem	*pst_Buf;
	ULONG				ul_SizeModel;
	ULONG				i;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* Now we can ask to checkout */
	mb_CanAskVss = TRUE;

	/* Want to change with the same model */
	if(mul_CurrentEditModel == _ul_Index)
		return TRUE;

	/* If there's already an edited file, check if it is in the model. If not, close it */
	if(mul_CurrentEditFile != BIG_C_InvalidIndex)
	{
		if(BIG_i_IsRefInGroup(_ul_Index, BIG_FileKey(mul_CurrentEditFile)) == -1)
		{
			if(!OnClose(FALSE)) return TRUE;
		}
	}

	/* Get engine model index */
	mul_CurrentEngineModel = mo_Compiler.ul_GetEngineModelIndex(_ul_Index, NULL, NULL);

	/* Set new model as current one */
	mul_CurrentEditModel = _ul_Index;
	if(!mb_CompileAll && !mb_CompileDep) UpdateOwner();
	mpo_DialogBar->GetDlgItem(IDC_INFOS)->SetWindowText(BIG_NameFile(mul_CurrentEditModel));

	/* Init AI list with model */
	InitListAI();

	/* Read model, and add each function/var in AI list */
	pst_Buf = (BIG_tdst_GroupElem *) BIG_pc_ReadFileTmpMustFree(BIG_PosFile(mul_CurrentEditModel), &ul_SizeModel);

	/* Compile variable to get description */
	if(!mb_CompileAll && !mb_CompileDep) ResetRefCompilers();
	mo_Compiler.Reset(mb_CompileAll | mb_CompileDep ? FALSE : TRUE);
	mo_Compiler.mpst_CurrentModel = pst_Buf;
	mo_Compiler.mi_SizeModel = ul_SizeModel / sizeof(BIG_tdst_GroupElem);
	mo_Compiler.mul_CurrentModel = mul_CurrentEditModel;
	mo_Compiler.mul_CurrentInstance = mul_CurrentEditInstance;
	if(mpst_Model)
		b_Ok = OnCompileVarsOfModel(&mo_Compiler, mul_CurrentEditModel);
	else
		b_Ok = TRUE;

	/* If there's no current edit file, get the first of the group */
	if(b_Ok && mul_CurrentEditFile == BIG_C_InvalidIndex)
	{
		mul_CurrentEditFile = BIG_C_InvalidIndex;
		if(!mb_ListMode)
		{
			i = 0;
			mul_CurrentEditFile = BIG_C_InvalidIndex;
			while((mul_CurrentEditFile == BIG_C_InvalidIndex) && (i < ul_SizeModel / sizeof(BIG_tdst_GroupElem)))
			{
				mul_CurrentEditFile = BIG_ul_GetFatGrpRef(mul_CurrentEditModel, i++);
			}

			if((mul_CurrentEditFile != BIG_C_InvalidIndex) && (!mb_CompileAll) && (!mb_CompileDep))
			{
				Load(mul_CurrentEditFile);
			}
		}
	}

	L_free(pst_Buf);

	LoadColors();
	return b_Ok;
}

#endif /* ACTIVE_EDITORS */
