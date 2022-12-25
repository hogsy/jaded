/*$T AIsave.c GC!1.52 10/26/99 12:22:19 */

/*
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */

#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/CLIbrary/CLImem.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/BIGkey.h"
#include "AIinterp/Sources/AIload.h"
#include "AIinterp/Sources/AIengine.h"
#include "AIinterp/Sources/AIBench.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "LINks/LINKtoed.h"
#include "ENGine/Sources/WORld/WORload.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "AIerrid.h"
#ifdef ACTIVE_EDITORS
#include "EDIpaths.h"
#include "ENGine/Sources/WORld/WORsave.h"
#endif

#ifdef BENCH_IA_TEST


#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C" {
#endif

typedef struct AIBnch_Function_
{
	BIG_KEY				MdlKey;
	BIG_KEY				FatherMdlKey;
	BIG_KEY				OFCKey;
	ULONG				PSX2CodeSize;
	ULONG				TotalTime;
	ULONG				TotalFrames;
	ULONG				ulNumberOfNodes;
	struct AIBnch_Function_	*pNextFunction;

	/* timer Bench */ 
	ULONG				StartTime;
	ULONG				AccTime;

	ULONG				ulNumberOfCall;

	ULONG				ulNumberOfLoads;

} AIBnch_Function;

typedef struct AIBnch_Dictionary_
{
	char							Value;
	AIBnch_Function				*FunctionIf0; // If Value == 0;
	struct	AIBnch_Dictionary_	*NextIfTrue;
	struct	AIBnch_Dictionary_		*NextIfFalse;
} AIBnch_Dictionary;

static AIBnch_Dictionary *p_RootMDL = NULL;
static AIBnch_Dictionary *p_RootFUNC = NULL;
static AIBnch_Dictionary *p_RootPSX2 = NULL;
static AIBnch_Function	 *p_TotalFunction;

AIBnch_Function *AIBnch_GetFunctionFromName(AIBnch_Dictionary *p_SearchNode, char *Name)
{
	while ((*Name) && p_SearchNode)
	{
		if ((*Name) == p_SearchNode->Value)
		{
			p_SearchNode = p_SearchNode->NextIfTrue;
			Name++;
		} else
		while (p_SearchNode && (p_SearchNode->Value != *Name))
			p_SearchNode = p_SearchNode->NextIfFalse;
	}

	if (p_SearchNode)
	{
		if (p_SearchNode->Value == 0)
		{
			if ((*Name) == 0)
			{
				return p_SearchNode->FunctionIf0;
			}
		}
	}
	return NULL;
}
static LONG AIBnch_NameNumber;
AIBnch_Function	*AIBnch_GetNameNum(AIBnch_Dictionary *p_SearchNode , char *Name )
{
	if (!p_SearchNode) return NULL;

	*Name = p_SearchNode->Value;

	if (p_SearchNode ->Value == 0)
	{
		AIBnch_NameNumber --;
		if (AIBnch_NameNumber == 0) 
			return p_SearchNode ->FunctionIf0;
		else
			return AIBnch_GetNameNum(p_SearchNode -> NextIfFalse , Name );
	} else
	{
		AIBnch_Function	*p_ReturnValue;
		p_ReturnValue = AIBnch_GetNameNum(p_SearchNode ->NextIfTrue , Name+1 );
		if (p_ReturnValue) 
			return p_ReturnValue;
		else		
			return AIBnch_GetNameNum(p_SearchNode ->NextIfFalse , Name );
	}
}
AIBnch_Function	*AIBnch_GetFunctionFromBK(AIBnch_Dictionary *p_SearchNode ,  BIG_KEY	MdlKey)
{
	if (!p_SearchNode) return NULL;
	if (p_SearchNode -> Value == 0)
	{
		if ((p_SearchNode -> FunctionIf0 -> MdlKey == MdlKey) ||(p_SearchNode -> FunctionIf0 -> OFCKey == MdlKey))
			return p_SearchNode ->FunctionIf0;
		else
			return AIBnch_GetFunctionFromBK(p_SearchNode -> NextIfFalse , MdlKey );
	} else
	{
		AIBnch_Function	*p_ReturnValue;
		p_ReturnValue = AIBnch_GetFunctionFromBK(p_SearchNode ->NextIfTrue , MdlKey );
		if (p_ReturnValue) 
			return p_ReturnValue;
		else		
			return AIBnch_GetFunctionFromBK(p_SearchNode ->NextIfFalse , MdlKey );
	}
}

ULONG AIBnch_GetNumberOfNames(AIBnch_Dictionary *p_SearchNode)
{
	if (p_SearchNode == NULL) return 0;
	if (p_SearchNode->Value == 0)
	{
		return 1 + AIBnch_GetNumberOfNames(p_SearchNode->NextIfFalse);
	} else
		return AIBnch_GetNumberOfNames(p_SearchNode->NextIfTrue) + AIBnch_GetNumberOfNames(p_SearchNode->NextIfFalse);
}

AIBnch_Function *AIBnch_AddFunctionName(AIBnch_Dictionary **p_Root, char *Name)
{
	AIBnch_Dictionary **p_SearchNode;
	p_SearchNode = p_Root;
	while ((*Name) && (*p_SearchNode))
	{
		if ((*Name) == (*p_SearchNode)->Value)
		{
			p_SearchNode = &(*p_SearchNode)->NextIfTrue;
			Name++;
		} else
		while ((*p_SearchNode) && ((*p_SearchNode)->Value != *Name))
			p_SearchNode = &(*p_SearchNode)->NextIfFalse;
	}

	if (*p_SearchNode)
	{
		if ((*p_SearchNode)->Value == 0)
		{
			if ((*Name) == 0)
			{
				return (*p_SearchNode)->FunctionIf0; // Already in
			}
		}
		*Name = *Name;
	} 
	// Add name from here
	while (*Name)
	{
		*p_SearchNode = (AIBnch_Dictionary *)MEM_p_Alloc(sizeof(AIBnch_Dictionary));
		L_memset(*p_SearchNode , 0 , sizeof(AIBnch_Dictionary));
		(*p_SearchNode)->Value = *(Name++);
		p_SearchNode = &(*p_SearchNode)->NextIfTrue;
	}
	*p_SearchNode = (AIBnch_Dictionary *)MEM_p_Alloc(sizeof(AIBnch_Dictionary));
	L_memset(*p_SearchNode , 0 , sizeof(AIBnch_Dictionary));
	(*p_SearchNode)->Value = 0;
	(*p_SearchNode)->FunctionIf0 = (AIBnch_Function *)MEM_p_Alloc(sizeof(AIBnch_Function));
	L_memset((*p_SearchNode)->FunctionIf0 , 0 , sizeof(AIBnch_Function));
	return (*p_SearchNode)->FunctionIf0;
}

AIBnch_Function *p_LastFuncNode = NULL;
AIBnch_Function *p_LastMdlNode = NULL;

void AIBnch_ImportData()
{
	FILE *File;
	File = fopen("X:/IABENCH_FUNC.TXT","rt");

	if (File)
	{
		char *RetValue;
		char NameX[4096];
		char NameX2[4096];
		/* Find "# .main"*/

		RetValue = fgets(NameX,4096,File ); // Skip First line
		RetValue = fgets(NameX,4096,File ); // Skip First line
		while (RetValue)
		{
			AIBnch_Function *p_Func;
			ULONG MDLKEY,FMDLKEY,ulNumberOfNodes, PSX2CodeSize,OFCKey;
			ULONG AccTime,NumberOfLoads,ulNumberOfCalls;
			sscanf(NameX,"%s 0x%08x 0x%08x 0x%08x     %8d    %8d    %8d     %8d      %8d",NameX2 , &MDLKEY,&FMDLKEY,&OFCKey,&ulNumberOfNodes, &PSX2CodeSize , &AccTime,&NumberOfLoads,&ulNumberOfCalls);
			p_Func = AIBnch_AddFunctionName(&p_RootFUNC, NameX2);
			if (p_Func)
			{
				p_Func->FatherMdlKey = FMDLKEY;
				p_Func->MdlKey = MDLKEY;
				p_Func->PSX2CodeSize = PSX2CodeSize;
				p_Func->ulNumberOfNodes = ulNumberOfNodes >> 3;
				p_Func->AccTime = AccTime;
				p_Func->ulNumberOfLoads = NumberOfLoads;
				p_Func->OFCKey = OFCKey;
				p_Func->ulNumberOfCall = ulNumberOfCalls;
			}
			RetValue = fgets(NameX,4096,File );
		}

		fclose(File);
	}
	if (!p_TotalFunction)
	{
		p_TotalFunction = AIBnch_AddFunctionName(&p_RootFUNC, "TOTAL");
	}

}

void AIBnch_ExportData()
{
	ULONG Counter;
	if (p_RootFUNC)
	{
		FILE *File;
		File = fopen("X:/IABENCH_FUNC.TXT","wb");
		if (File)
		{
			char NameX[1024];
			char NameX2[1024];
			char *CurrentN;
			sprintf(NameX,"FUNC NAME                                                           BIG_KEY    MDL_KEY    OFC_KEY   NODES SIZE   PSX2 SIZE  AverageTime NumberOfLoads NumberOfCalls");
			
			CurrentN = NameX;
			while (*CurrentN) CurrentN++;
			*(CurrentN++) = 0x0d;
			*(CurrentN++) = 0x0a;
			fwrite(NameX , CurrentN - NameX, 1 , File);
			Counter = AIBnch_GetNumberOfNames(p_RootFUNC);
			while (Counter--)
			{
				AIBnch_Function *pFunc;
				AIBnch_NameNumber = Counter + 1;
				pFunc = AIBnch_GetNameNum(p_RootFUNC, NameX2 );
				CurrentN = NameX2;
				while (*CurrentN) CurrentN++;
				while (CurrentN < &NameX2[64]) *(CurrentN++) = 32;
				*CurrentN = 0;
				*CurrentN = 0;
				sprintf(NameX,"%s 0x%08x 0x%08x 0x%08x     %8d    %8d     %8d      %8d      %8d",NameX2 , pFunc->MdlKey , pFunc->FatherMdlKey , pFunc->OFCKey , pFunc->ulNumberOfNodes * 8, pFunc->PSX2CodeSize , pFunc->AccTime , pFunc->ulNumberOfLoads , pFunc->ulNumberOfCall);
				CurrentN = NameX;
				while (*CurrentN)	CurrentN++;
				*(CurrentN++) = 0x0d;
				*(CurrentN++) = 0x0a;
				fwrite(NameX , CurrentN - NameX, 1 , File);
			}
			fclose(File);
		}
	}
}


#ifdef ACTIVE_EDITORS
void AIBnch_ReplaceSpaceByUnderscore(char *String)
{
	while (*String)
	{
		if (*String == 32) *String = '_';
		String++;
	}
}

void AIBnch_AddFunction(BIG_KEY	MdlKey, BIG_KEY	FuncKey)
{
	char NameX[4096];
	AIBnch_Function *p_PSX2;
	char			*CurrentN;
	char				asz_NameMdl[BIG_C_MaxLenPath];
	char				asz_NameFunc[BIG_C_MaxLenPath];

	L_strcpy(asz_NameFunc, BIG_NameFile(BIG_ul_SearchKeyToFat( FuncKey)));
	L_strcpy(asz_NameMdl, BIG_NameFile(BIG_ul_SearchKeyToFat( MdlKey)));
	*L_strrchr(asz_NameMdl, '.') = 0;
	p_LastFuncNode = p_LastMdlNode = NULL;
	CurrentN = asz_NameFunc;
	while (*CurrentN && (*CurrentN != '.')) CurrentN++;
	*(CurrentN++) = 0;
	if ((*CurrentN == 'v') || (*CurrentN == 'V'))
	if ((*(CurrentN + 1) == 'a') || (*(CurrentN + 1) == 'A'))
	if ((*(CurrentN + 2) == 'r') || (*(CurrentN + 2) == 'R'))
		return;//*/

	p_LastMdlNode = AIBnch_AddFunctionName(&p_RootMDL, asz_NameMdl );
	p_LastMdlNode -> MdlKey = MdlKey;
	sprintf(NameX , "%s:%s" , asz_NameMdl , asz_NameFunc);
	AIBnch_ReplaceSpaceByUnderscore(NameX );
	p_LastFuncNode = AIBnch_AddFunctionName(&p_RootFUNC, NameX );
	p_LastFuncNode -> MdlKey = FuncKey;
	p_LastFuncNode -> FatherMdlKey = MdlKey;
	sprintf(NameX , "%s_%s" , asz_NameMdl , asz_NameFunc);
	p_PSX2 = AIBnch_GetFunctionFromName(p_RootPSX2, NameX);
	if (p_PSX2)
	{
		p_LastMdlNode ->PSX2CodeSize += p_PSX2->PSX2CodeSize;
		p_LastFuncNode ->PSX2CodeSize = p_PSX2->PSX2CodeSize;//*/
	}
}

void AIBnch_AddLastFunctionIfo(ULONG ulNumberOfNode,BIG_KEY OFCKey)
{
	if (p_LastFuncNode) 
	{
		p_LastFuncNode->ulNumberOfNodes = ulNumberOfNode;
		p_LastFuncNode->OFCKey = OFCKey;
	}
}

void AIBnch_LoadPsx2_Info()
{
	FILE *File;
	File = fopen("X:/BENCH.xMAP","rt");
	if (File)
	{
		char *RetValue;
		char NameX[4096];
		char NameX2[4096];
		char NameX3[4096];
		/* Find "# .main"*/
		sprintf(NameX2 ,"# .main\n");
		do	RetValue = fgets(NameX,4096,File );
		while (RetValue && (strcmp(NameX2,NameX)));
		RetValue = fgets(NameX,4096,File );
		while (RetValue && (((ULONG*)NameX)[5] == 'xet.'))
		{
			AIBnch_Function *p_Func;
			ULONG Address , Size ;
			sscanf(NameX , " %8x %8x .text %s %s" , &Address , &Size , NameX2 , NameX3);
			p_Func = AIBnch_AddFunctionName(&p_RootPSX2, NameX2);
			if (p_Func)	p_Func->PSX2CodeSize = Size;
			RetValue = fgets(NameX,4096,File );
		}
		fclose(File);
	}
}

void AIBnch_BeforeCompileAll()
{
	p_LastFuncNode = NULL;
	p_LastMdlNode = NULL;
	AIBnch_LoadPsx2_Info();
	if (!p_TotalFunction)
	{
		p_TotalFunction = AIBnch_AddFunctionName(&p_RootFUNC, "TOTAL");
	}
}
void AIBnch_AfterCompileAll()
{
	AIBnch_ExportData();
}
#endif

void AIBnch_BeginWorldLoad(BIG_KEY	WorldKey)
// Called from Worload
{

	if (!p_RootFUNC) 
		AIBnch_ImportData();
	else
		AIBnch_ExportData();

	if (p_TotalFunction)
		p_TotalFunction->ulNumberOfLoads++;

}
void AIBnch_Close()
{
	AIBnch_ExportData();
}
void AIBnch_CreateFunctionBench(AI_tdst_Function	*p_Func , BIG_KEY	FunctionKey)
// Called from LoadFunction or SaveFunction
{
	AIBnch_Function	*p_FuncBench;
	/* Find function from BigKey */
	p_FuncBench = AIBnch_GetFunctionFromBK(p_RootFUNC ,  FunctionKey );
	if (p_FuncBench) 
	{
		p_Func->p_BenchDataForFunction = p_FuncBench;
		p_FuncBench->ulNumberOfLoads++;
	}
}
void AIBnch_BeginFrameCall()
{
	if (p_TotalFunction)
	{
		p_TotalFunction->ulNumberOfCall++;
		p_TotalFunction-> StartTime = TIM_ul_GetLowPartTimerInternalCounter();
	}
}

void AIBnch_EndFrameCall()
{
	if (p_TotalFunction)
	{
		if (p_TotalFunction->StartTime)
		{
			p_TotalFunction->AccTime = TIM_ul_GetLowPartTimerInternalCounter() - p_TotalFunction -> StartTime;
			p_TotalFunction->StartTime = 0;
		}
		p_TotalFunction->ulNumberOfCall++;
	}
}

void AIBnch_BeforeRunFunc(AI_tdst_Function	*p_Func)
// Called from Ai runfunction begin 
{
	AIBnch_Function	*p_FuncBench;
	p_FuncBench = (AIBnch_Function	*)p_Func->p_BenchDataForFunction;
	if (p_FuncBench)
	{
		p_FuncBench -> StartTime = TIM_ul_GetLowPartTimerInternalCounter();
	}
}
void AIBnch_AfterRunFunc(AI_tdst_Function	*p_Func)
// Called from Ai runfunction end
{
	AIBnch_Function	*p_FuncBench;
	p_FuncBench = (AIBnch_Function	*)p_Func->p_BenchDataForFunction;
	if (p_FuncBench)
	{
		if (p_FuncBench->StartTime)
		{
			p_FuncBench->AccTime = TIM_ul_GetLowPartTimerInternalCounter() - p_FuncBench -> StartTime;
			p_FuncBench->StartTime = 0;
		}
		p_FuncBench->ulNumberOfCall++;
	}
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif


#endif //BENCH_IA_TEST