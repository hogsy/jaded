/*$T SNDinsert.c GC 1.138 11/10/04 11:25:37 */

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#include "Precomp.h"

#include "../../../Main/WinEditors/Sources/EDIpaths.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGread.h"

#include "BASe/ERRors/ERRasser.h"
#include "BASe/MEMory/MEM.h"

#include "LINks/LINKtoed.h"

#include "SNDstruct.h"
#include "SND.h"
#include "SNDconst.h"
#include "SNDinsert.h"
#include "SNDmacros.h"

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#define SND_Cte_InsertFileId		0x696e7374
#define SND_Cte_InsertVersionFile	0x00000001

#define SND_Cte_InsertMaxVar		32

typedef struct	SND_tdtst_InsertVarDesc_
{
	float	f_Value;
	short	s_Instance;
	short	s_VarIndex;
} SND_tdtst_InsertVarDesc;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

static int	SND_i_InsertGetFree(void);

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

static SND_tdtst_InsertVarDesc	*SND_gax_InsertVar;
static int						SND_gi_InsertVarSize;

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_InsertInitModule(void)
{
	/*~~*/
	int i;
	/*~~*/

	SND_gi_InsertVarSize = SND_Cte_MinAllocInst;
	SND_gax_InsertVar = (SND_tdtst_InsertVarDesc*)MEM_p_Alloc(SND_Cte_MinAllocInst * sizeof(SND_tdtst_InsertVarDesc));
	L_memset(SND_gax_InsertVar, 0, SND_Cte_MinAllocInst * sizeof(SND_tdtst_InsertVarDesc));

	for(i = 0; i < SND_gi_InsertVarSize; i++)
	{
		SND_gax_InsertVar[i].s_Instance = -1;	/* this line is free */
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_InsertCloseModule(void)
{
	if(SND_gax_InsertVar) MEM_Free(SND_gax_InsertVar);
	SND_gax_InsertVar = NULL;
	SND_gi_InsertVarSize = 0;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_InsertVarGet(int i_Instance, int i_Idx)
{
	/*~~*/
	int i;
	/*~~*/

    if(i_Instance <0) return 0.0f;

	for(i = 0; i < SND_gi_InsertVarSize; i++)
	{
		if((int) SND_gax_InsertVar[i].s_Instance != i_Instance) continue;
		if((int) SND_gax_InsertVar[i].s_VarIndex != i_Idx) continue;

		return SND_gax_InsertVar[i].f_Value;
	}

	return 0.0f;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_InsertVarSet(int i_Instance, int i_Idx, float f_Value)
{
	/*~~~~~~~~~~~~~~~*/
	int i, i_Free = -1;
	/*~~~~~~~~~~~~~~~*/

    if(!SND_gst_Params.l_Available) return;
    if(i_Instance <0) return;

	/* saturation */
	if(f_Value < 0.000001f) f_Value = 0.0f;
	if(f_Value > 1.0f) f_Value = 1.0f;

	/* validity */
	if(i_Idx < 0) return;
	if(i_Idx >= SND_Cte_InsertMaxVar) return;

	/* find the value */
	for(i = 0; i < SND_gi_InsertVarSize; i++)
	{
		if(i_Free == -1)
		{
			if(SND_gax_InsertVar[i].s_Instance == -1) i_Free = i;
		}

		if((int) SND_gax_InsertVar[i].s_Instance != i_Instance) continue;
		if((int) SND_gax_InsertVar[i].s_VarIndex != i_Idx) continue;

		/* found => assignation */
		SND_gax_InsertVar[i].f_Value = f_Value;
		return;
	}

	/* we have to allocate a new space */
	if(i_Free == -1)
	{
		i_Free = SND_gi_InsertVarSize;
		SND_gi_InsertVarSize += SND_Cte_MinAllocInst;

		SND_gax_InsertVar = (SND_tdtst_InsertVarDesc*)MEM_p_Realloc(SND_gax_InsertVar, SND_gi_InsertVarSize * sizeof(SND_tdtst_InsertVarDesc));
		L_memset(&SND_gax_InsertVar[i_Free], 0, SND_Cte_MinAllocInst * sizeof(SND_tdtst_InsertVarDesc));
		for(i = i_Free; i < SND_gi_InsertVarSize; i++)
		{
			SND_gax_InsertVar[i].s_Instance = -1;	/* this line is free */
		}
	}

	/* alloc the new space */
	SND_gax_InsertVar[i_Free].s_Instance = (short) i_Instance;
	SND_gax_InsertVar[i_Free].s_VarIndex = (short) i_Idx;
	SND_gax_InsertVar[i_Free].f_Value = f_Value;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_InsertDestroyInstanceCallback(int i_Instance)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < SND_gi_InsertVarSize; i++)
	{
		if((int) SND_gax_InsertVar[i].s_Instance != i_Instance) continue;

		SND_gax_InsertVar[i].s_Instance = -1;
		SND_gax_InsertVar[i].s_VarIndex = 0;
		SND_gax_InsertVar[i].f_Value = 0.0f;
	}
}

void SND_InsertVarFreeAll(void)
{
	/*~~*/
	int i;
	/*~~*/

	for(i = 0; i < SND_gi_InsertVarSize; i++)
	{
		SND_gax_InsertVar[i].s_Instance = -1;
		SND_gax_InsertVar[i].s_VarIndex = 0;
		SND_gax_InsertVar[i].f_Value = 0.0f;
	}
}
/*
 =======================================================================================================================
 =======================================================================================================================
 */
int SND_i_InsertGetIndex(ULONG _ul_WavKey)
{
	/*~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Insert *pInsert;
	int				i;
	/*~~~~~~~~~~~~~~~~~~~~~*/

	for(i = 0; i < SND_gst_Params.l_InsertNumber; i++)
	{
		pInsert = SND_gst_Params.dst_Insert + i;

		if((pInsert->ul_Flags & SND_Cul_DSF_Used) == 0) continue;
		if(pInsert->ul_FileKey == _ul_WavKey) return i;
	}

	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG SND_ul_CallbackInsertLoad(ULONG _ul_FilePos)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	ULONG						ul_Size;
	char						*pc_Buff;
	SND_tdst_InsertFileHeader	st_FileHeader;
	SND_tdst_InsertChunkHeader	st_ChunkHeader;
	unsigned int				uiIdx;
	SND_tdst_Insert				*pst_Insert;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/*$1- file access ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Size = LOA_ul_SearchAddress(_ul_FilePos);
	if(ul_Size != (ULONG) - 1)
	{
		((SND_tdst_Insert *) ul_Size)->ul_UserCount++;
		return ul_Size;
	}

	pc_Buff = BIG_pc_ReadFileTmp(_ul_FilePos, &ul_Size);
	ERR_X_Assert(ul_Size > sizeof(SND_tdst_InsertFileHeader));

	/*$1- file check ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	st_FileHeader.ui_FileId = LOA_ReadULong(&pc_Buff);
	st_FileHeader.ui_Version = LOA_ReadULong(&pc_Buff);
	st_FileHeader.ui_DataSize = LOA_ReadULong(&pc_Buff);
	ul_Size -= sizeof(SND_tdst_InsertFileHeader);

	ERR_X_Assert(st_FileHeader.ui_FileId == SND_Cte_InsertFileId);
	ERR_X_Assert(st_FileHeader.ui_DataSize == ul_Size);

	if(st_FileHeader.ui_FileId != SND_Cte_InsertFileId) return 0;

	/* struct alloc + init */
	uiIdx = SND_i_InsertGetFree();
	pst_Insert = SND_gst_Params.dst_Insert + uiIdx;
	pst_Insert->ul_UserCount = 1;
	pst_Insert->ul_Flags = LOA_ReadLong(&pc_Buff);
	ul_Size -= sizeof(ULONG);
	pst_Insert->ul_FileKey = LOA_ul_GetCurrentKey();

	/*$1- file interpretation ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	while(ul_Size)
	{
		st_ChunkHeader.e_ChunkId = (SND_tden_InsRefType) LOA_ReadULong(&pc_Buff);
		st_ChunkHeader.ui_ChunkSize = LOA_ReadULong(&pc_Buff);

		switch(st_ChunkHeader.e_ChunkId)
		{
		case en_InsXaxisDistance:
			pst_Insert->Xaxis.pst_DistDef = (SND_tdst_InsXDistanceDef*)MEM_p_Alloc(sizeof(SND_tdst_InsXDistanceDef));
			pst_Insert->Xaxis.pst_DistDef->st_Header.e_ChunkId = st_ChunkHeader.e_ChunkId;
			pst_Insert->Xaxis.pst_DistDef->st_Header.ui_ChunkSize = st_ChunkHeader.ui_ChunkSize;
			pst_Insert->Xaxis.pst_DistDef->ui_GaoKey = LOA_ReadULong(&pc_Buff);
			pst_Insert->Xaxis.pst_DistDef->ui_Axis = LOA_ReadULong(&pc_Buff);
			pst_Insert->Xaxis.pst_DistDef->f_Min = LOA_ReadFloat(&pc_Buff);
			pst_Insert->Xaxis.pst_DistDef->f_Max = LOA_ReadFloat(&pc_Buff);
			ERR_X_Assert(pst_Insert->Xaxis.pst_DistDef->st_Header.ui_ChunkSize == sizeof(SND_tdst_InsXDistanceDef));
			ul_Size -= st_ChunkHeader.ui_ChunkSize;
			break;

		case en_InsXaxisTime:
			pst_Insert->Xaxis.pst_TimeDef = (SND_tdst_InsXTimeDef*)MEM_p_Alloc(sizeof(SND_tdst_InsXTimeDef));
			pst_Insert->Xaxis.pst_TimeDef->st_Header.e_ChunkId = st_ChunkHeader.e_ChunkId;
			pst_Insert->Xaxis.pst_TimeDef->st_Header.ui_ChunkSize = st_ChunkHeader.ui_ChunkSize;
			pst_Insert->Xaxis.pst_TimeDef->f_Min = LOA_ReadFloat(&pc_Buff);
			pst_Insert->Xaxis.pst_TimeDef->f_Max = LOA_ReadFloat(&pc_Buff);
			ERR_X_Assert(pst_Insert->Xaxis.pst_TimeDef->st_Header.ui_ChunkSize == sizeof(SND_tdst_InsXTimeDef));
			ul_Size -= st_ChunkHeader.ui_ChunkSize;
			break;

		case en_InsXaxisVar:
			pst_Insert->Xaxis.pst_VarDef = (SND_tdst_InsXVarDef*)MEM_p_Alloc(sizeof(SND_tdst_InsXVarDef));
			pst_Insert->Xaxis.pst_VarDef->st_Header.e_ChunkId = st_ChunkHeader.e_ChunkId;
			pst_Insert->Xaxis.pst_VarDef->st_Header.ui_ChunkSize = st_ChunkHeader.ui_ChunkSize;
			pst_Insert->Xaxis.pst_VarDef->ui_VarId = LOA_ReadULong(&pc_Buff);

			if(pst_Insert->Xaxis.pst_VarDef->ui_VarId >= SND_Cte_InsertMaxVar)
				pst_Insert->Xaxis.pst_VarDef->ui_VarId = SND_Cte_InsertMaxVar;

			ERR_X_Assert(pst_Insert->Xaxis.pst_VarDef->st_Header.ui_ChunkSize == sizeof(SND_tdst_InsXVarDef));
			ul_Size -= st_ChunkHeader.ui_ChunkSize;
			break;

		case en_InsYaxisVol:
			pst_Insert->Yaxis.pst_VolDef = (SND_tdst_InsYVolDef*)MEM_p_Alloc(sizeof(SND_tdst_InsYVolDef));
			pst_Insert->Yaxis.pst_VolDef->st_Header.e_ChunkId = st_ChunkHeader.e_ChunkId;
			pst_Insert->Yaxis.pst_VolDef->st_Header.ui_ChunkSize = st_ChunkHeader.ui_ChunkSize;
			pst_Insert->Yaxis.pst_VolDef->f_Min = LOA_ReadFloat(&pc_Buff);
			pst_Insert->Yaxis.pst_VolDef->f_Max = LOA_ReadFloat(&pc_Buff);
			pst_Insert->Yaxis.pst_VolDef->b_Wet = LOA_ReadULong(&pc_Buff);
			ERR_X_Assert(pst_Insert->Yaxis.pst_VolDef->st_Header.ui_ChunkSize == sizeof(SND_tdst_InsYVolDef));
			ul_Size -= st_ChunkHeader.ui_ChunkSize;
			break;

		case en_InsYaxisPan:
			pst_Insert->Yaxis.pst_PanDef = (SND_tdst_InsYPanDef*)MEM_p_Alloc(sizeof(SND_tdst_InsYPanDef));
			pst_Insert->Yaxis.pst_PanDef->st_Header.e_ChunkId = st_ChunkHeader.e_ChunkId;
			pst_Insert->Yaxis.pst_PanDef->st_Header.ui_ChunkSize = st_ChunkHeader.ui_ChunkSize;
			pst_Insert->Yaxis.pst_PanDef->f_Min = LOA_ReadFloat(&pc_Buff);
			pst_Insert->Yaxis.pst_PanDef->f_Max = LOA_ReadFloat(&pc_Buff);
			pst_Insert->Yaxis.pst_PanDef->b_Surround = LOA_ReadULong(&pc_Buff);
			ERR_X_Assert(pst_Insert->Yaxis.pst_PanDef->st_Header.ui_ChunkSize == sizeof(SND_tdst_InsYPanDef));
			ul_Size -= st_ChunkHeader.ui_ChunkSize;
			break;

		case en_InsYaxisFreq:
			pst_Insert->Yaxis.pst_FreqDef = (SND_tdst_InsYFreqDef*)MEM_p_Alloc(sizeof(SND_tdst_InsYFreqDef));
			pst_Insert->Yaxis.pst_FreqDef->st_Header.e_ChunkId = st_ChunkHeader.e_ChunkId;
			pst_Insert->Yaxis.pst_FreqDef->st_Header.ui_ChunkSize = st_ChunkHeader.ui_ChunkSize;
			pst_Insert->Yaxis.pst_FreqDef->f_Min = LOA_ReadFloat(&pc_Buff);
			pst_Insert->Yaxis.pst_FreqDef->f_Max = LOA_ReadFloat(&pc_Buff);
			ERR_X_Assert(pst_Insert->Yaxis.pst_FreqDef->st_Header.ui_ChunkSize == sizeof(SND_tdst_InsYFreqDef));
			ul_Size -= st_ChunkHeader.ui_ChunkSize;
			break;

		case en_InsKeyArray:
			pst_Insert->pst_KeyArray = (SND_tdst_InsKeyArray*)MEM_p_Alloc(sizeof(SND_tdst_InsKeyArray));
			pst_Insert->pst_KeyArray->st_Header.e_ChunkId = st_ChunkHeader.e_ChunkId;
			pst_Insert->pst_KeyArray->st_Header.ui_ChunkSize = st_ChunkHeader.ui_ChunkSize;
			pst_Insert->pst_KeyArray->ui_Size = LOA_ReadULong(&pc_Buff);
			pst_Insert->pst_KeyArray->dst_Key = (SND_tdst_InsKey*)MEM_p_Alloc(pst_Insert->pst_KeyArray->ui_Size * sizeof(SND_tdst_InsKey));
			for(uiIdx = 0; uiIdx < pst_Insert->pst_KeyArray->ui_Size; uiIdx++)
			{
				pst_Insert->pst_KeyArray->dst_Key[uiIdx].tx = LOA_ReadFloat(&pc_Buff);
				pst_Insert->pst_KeyArray->dst_Key[uiIdx].ty = LOA_ReadFloat(&pc_Buff);
			}

			ul_Size -= st_ChunkHeader.ui_ChunkSize;
			ERR_X_Assert(!ul_Size);
			break;

		default:
			ERR_X_Assert(0);
			return 0;
		}
	}

	return(ULONG) pst_Insert;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
static int SND_i_InsertGetFree(void)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	int				l_Index;
	SND_tdst_Insert *pst_Insert;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	for(l_Index = 0; l_Index < SND_gst_Params.l_InsertNumber; l_Index++)
	{
		pst_Insert = SND_gst_Params.dst_Insert + l_Index;
		if(!(pst_Insert->ul_Flags & SND_Cul_DSF_Used))
		{
			pst_Insert->ul_Flags |= SND_Cul_DSF_Used;
			return l_Index;
		}
	}

	SND_EdiRedWarningMsg("SND_i_InsertGetFree", "max of insert is reached");
	return -1;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_InsertUnload(int l_Index)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Insert *pst_Insert;
	/*~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(l_Index < 0) return;
	if(l_Index >= SND_gst_Params.l_InsertNumber) return;
	pst_Insert = SND_gst_Params.dst_Insert + l_Index;
	ERR_X_Assert(pst_Insert->ul_Flags & SND_Cul_DSF_Used);
	if(--pst_Insert->ul_UserCount) return;

	LOA_DeleteAddress(pst_Insert);
	MEM_Free(pst_Insert->Xaxis.pst_InsVoid);
	MEM_Free(pst_Insert->Yaxis.pst_InsVoid);
	MEM_Free(pst_Insert->pst_KeyArray->dst_Key);
	MEM_Free(pst_Insert->pst_KeyArray);

	L_memset(pst_Insert, 0, sizeof(SND_tdst_Insert));
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_InsertChangeType(SND_tdst_Insert *pInsert, SND_tden_InsRefType eType)
{
	ERR_X_Assert(pInsert->ul_Flags & SND_Cul_DSF_Used);

	switch(eType)
	{
	case en_InsXaxisDistance:
		MEM_Free(pInsert->Xaxis.pst_InsVoid);
		pInsert->Xaxis.pst_DistDef = (SND_tdst_InsXDistanceDef*)MEM_p_Alloc(sizeof(SND_tdst_InsXDistanceDef));
		L_memset(pInsert->Xaxis.pst_DistDef, 0, sizeof(SND_tdst_InsXDistanceDef));
		pInsert->Xaxis.pst_DistDef->st_Header.e_ChunkId = eType;
		pInsert->Xaxis.pst_DistDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsXDistanceDef);
		pInsert->Xaxis.pst_DistDef->ui_Axis = SND_Cte_InsAxisX;
		pInsert->Xaxis.pst_DistDef->ui_GaoKey = BIG_C_InvalidKey;
		pInsert->Xaxis.pst_DistDef->f_Min = 0.0f;
		pInsert->Xaxis.pst_DistDef->f_Max = 100.0f;
		break;

	case en_InsXaxisTime:
		MEM_Free(pInsert->Xaxis.pst_InsVoid);
		pInsert->Xaxis.pst_TimeDef = (SND_tdst_InsXTimeDef*)MEM_p_Alloc(sizeof(SND_tdst_InsXTimeDef));
		L_memset(pInsert->Xaxis.pst_TimeDef, 0, sizeof(SND_tdst_InsXTimeDef));
		pInsert->Xaxis.pst_TimeDef->st_Header.e_ChunkId = eType;
		pInsert->Xaxis.pst_TimeDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsXTimeDef);
		pInsert->Xaxis.pst_TimeDef->f_Max = 1.0f;
		pInsert->Xaxis.pst_TimeDef->f_Min = 0.0f;
		break;

	case en_InsXaxisVar:
		MEM_Free(pInsert->Xaxis.pst_InsVoid);
		pInsert->Xaxis.pst_VarDef = (SND_tdst_InsXVarDef*)MEM_p_Alloc(sizeof(SND_tdst_InsXVarDef));
		L_memset(pInsert->Xaxis.pst_VarDef, 0, sizeof(SND_tdst_InsXVarDef));
		pInsert->Xaxis.pst_VarDef->st_Header.e_ChunkId = eType;
		pInsert->Xaxis.pst_VarDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsXVarDef);
		pInsert->Xaxis.pst_VarDef->ui_VarId = 0;
		break;

	case en_InsYaxisVol:
		MEM_Free(pInsert->Yaxis.pst_InsVoid);
		pInsert->Yaxis.pst_VolDef = (SND_tdst_InsYVolDef*)MEM_p_Alloc(sizeof(SND_tdst_InsYVolDef));
		L_memset(pInsert->Yaxis.pst_VolDef, 0, sizeof(SND_tdst_InsYVolDef));
		pInsert->Yaxis.pst_VolDef->st_Header.e_ChunkId = eType;
		pInsert->Yaxis.pst_VolDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsYVolDef);
		pInsert->Yaxis.pst_VolDef->b_Wet = FALSE;
		pInsert->Yaxis.pst_VolDef->f_Min = 0.0f;
		pInsert->Yaxis.pst_VolDef->f_Max = 100.0f;
		break;

	case en_InsYaxisPan:
		MEM_Free(pInsert->Yaxis.pst_InsVoid);
		pInsert->Yaxis.pst_PanDef = (SND_tdst_InsYPanDef*)MEM_p_Alloc(sizeof(SND_tdst_InsYPanDef));
		L_memset(pInsert->Yaxis.pst_PanDef, 0, sizeof(SND_tdst_InsYPanDef));
		pInsert->Yaxis.pst_PanDef->st_Header.e_ChunkId = eType;
		pInsert->Yaxis.pst_PanDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsYPanDef);
		pInsert->Yaxis.pst_PanDef->b_Surround = FALSE;
		pInsert->Yaxis.pst_PanDef->f_Min = 0.0f;
		pInsert->Yaxis.pst_VolDef->f_Max = 0.0f;
		break;

	case en_InsYaxisFreq:
		MEM_Free(pInsert->Yaxis.pst_InsVoid);
		pInsert->Yaxis.pst_FreqDef = (SND_tdst_InsYFreqDef*)MEM_p_Alloc(sizeof(SND_tdst_InsYFreqDef));
		L_memset(pInsert->Yaxis.pst_FreqDef, 0, sizeof(SND_tdst_InsYFreqDef));
		pInsert->Yaxis.pst_FreqDef->st_Header.e_ChunkId = eType;
		pInsert->Yaxis.pst_FreqDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsYFreqDef);
		pInsert->Yaxis.pst_FreqDef->f_Min = 0.0f;
		pInsert->Yaxis.pst_FreqDef->f_Max = 100.0f;
		break;

	default:
		ERR_X_Assert(0);
		break;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_InsertGetXmin(SND_tdst_Insert *pInsert)
{
	switch(pInsert->Xaxis.pst_InsVoid->e_ChunkId)
	{
	case en_InsXaxisDistance:	return pInsert->Xaxis.pst_DistDef->f_Min;
	case en_InsXaxisTime:		return pInsert->Xaxis.pst_TimeDef->f_Min;
	case en_InsXaxisVar:		return 0.0f;
	default:					return 0.0f;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_InsertGetXmax(SND_tdst_Insert *pInsert)
{
	switch(pInsert->Xaxis.pst_InsVoid->e_ChunkId)
	{
	case en_InsXaxisDistance:	return pInsert->Xaxis.pst_DistDef->f_Max;
	case en_InsXaxisTime:		return pInsert->Xaxis.pst_TimeDef->f_Max;
	case en_InsXaxisVar:		return 100.0f;
	default:					return 100.0f;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_InsertGetYmin(SND_tdst_Insert *pInsert)
{
	switch(pInsert->Yaxis.pst_InsVoid->e_ChunkId)
	{
	default:				return 0.0f;
	case en_InsYaxisVol:	return 0.01f * pInsert->Yaxis.pst_VolDef->f_Min;
	case en_InsYaxisPan:	return 0.01f * pInsert->Yaxis.pst_PanDef->f_Min;
	case en_InsYaxisFreq:	return 0.01f * pInsert->Yaxis.pst_FreqDef->f_Min;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_InsertGetYmax(SND_tdst_Insert *pInsert)
{
	switch(pInsert->Yaxis.pst_InsVoid->e_ChunkId)
	{
	default:				return 1.0f;
	case en_InsYaxisVol:	return 0.01f * pInsert->Yaxis.pst_VolDef->f_Max;
	case en_InsYaxisPan:	return 0.01f * pInsert->Yaxis.pst_PanDef->f_Max;
	case en_InsYaxisFreq:	return 0.01f * pInsert->Yaxis.pst_FreqDef->f_Max;
	}
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_InsertGetYvsX(SND_tdst_Insert *pInsert, float fX, int iInstance)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float			fY, tx, ty, prev_tx, prev_ty;
	unsigned int	i;
	SND_tdst_InsKey *pKey;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(pInsert->Xaxis.pst_InsVoid->e_ChunkId)
	{
	case en_InsXaxisDistance:
		if(pInsert->ul_Flags & SND_Cte_InsCycling) fX = SND_f_FloatModulo(fX, pInsert->Xaxis.pst_DistDef->f_Max);
		if(fX <= pInsert->Xaxis.pst_DistDef->f_Min) return SND_f_InsertGetYmin(pInsert);
		if(fX >= pInsert->Xaxis.pst_DistDef->f_Max) return SND_f_InsertGetYmax(pInsert);
		fX = (fX - pInsert->Xaxis.pst_DistDef->f_Min) / (pInsert->Xaxis.pst_DistDef->f_Max - pInsert->Xaxis.pst_DistDef->f_Min);
		break;

	case en_InsXaxisTime:
		if(pInsert->ul_Flags & SND_Cte_InsCycling) fX = SND_f_FloatModulo(fX, pInsert->Xaxis.pst_TimeDef->f_Max);
		if(fX <= pInsert->Xaxis.pst_TimeDef->f_Min) return SND_f_InsertGetYmin(pInsert);
		if(fX >= pInsert->Xaxis.pst_TimeDef->f_Max) return SND_f_InsertGetYmax(pInsert);
		fX = (fX - pInsert->Xaxis.pst_TimeDef->f_Min) / (pInsert->Xaxis.pst_TimeDef->f_Max - pInsert->Xaxis.pst_TimeDef->f_Min);
		break;

	case en_InsXaxisVar:
		if(pInsert->ul_Flags & SND_Cte_InsCycling) fX = SND_f_FloatModulo(fX, 100.0f);
		if(fX <= 0.0f) return SND_f_InsertGetYmin(pInsert);
		if(fX >= 100.0f) return SND_f_InsertGetYmax(pInsert);
		fX = SND_f_InsertVarGet(iInstance, pInsert->Xaxis.pst_VarDef->ui_VarId);
		break;

	default:
		break;
	}

	/* if(pInsert->pst_KeyArray->ui_Size>2) */
	{
		pKey = pInsert->pst_KeyArray->dst_Key;
		tx = prev_tx = 0.0f;
		ty = prev_ty = pKey->tx;
		pKey++;

		for(i = 1; i < pInsert->pst_KeyArray->ui_Size - 1; i++, pKey++)
		{
			tx = pKey->tx;
			ty = prev_ty + (tx - prev_tx) * pKey[-1].ty;

			if((prev_tx <= fX) && (fX <= tx)) break;

			prev_tx = tx;
			prev_ty = ty;
		}
	}

	/* get last point */
	if(!((prev_tx <= fX) && (fX <= tx)))
	{
		tx = 1.0f;
		ty = pKey->tx;
		if(!((prev_tx <= fX) && (fX <= tx))) return SND_f_InsertGetYmax(pInsert);
	}

	fY = MATH_f_FloatBlend(prev_ty, ty, (fX - prev_tx) / (tx - prev_tx));

	switch(pInsert->Yaxis.pst_InsVoid->e_ChunkId)
	{
	default:
		break;

	case en_InsYaxisVol:
		fY = MATH_f_FloatBlend(pInsert->Yaxis.pst_VolDef->f_Min, pInsert->Yaxis.pst_VolDef->f_Max, fY);
		break;

	case en_InsYaxisPan:
		fY = MATH_f_FloatBlend(pInsert->Yaxis.pst_PanDef->f_Min, pInsert->Yaxis.pst_PanDef->f_Max, fY);
		break;

	case en_InsYaxisFreq:
		fY = MATH_f_FloatBlend(pInsert->Yaxis.pst_FreqDef->f_Min, pInsert->Yaxis.pst_FreqDef->f_Max, fY);
		break;
	}

	return 0.01f * fY;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
float SND_f_InsertGetXvsY(SND_tdst_Insert *pInsert, float fY)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	float			fX, tx, ty, prev_tx, prev_ty;
	unsigned int	i;
	SND_tdst_InsKey *pKey;
	BOOL			b_break;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	fY *= 100.0f;
	switch(pInsert->Yaxis.pst_InsVoid->e_ChunkId)
	{
	default:
		break;

	case en_InsYaxisVol:
		if(fY <= pInsert->Yaxis.pst_VolDef->f_Min) return SND_f_InsertGetXmin(pInsert);
		if(fY >= pInsert->Yaxis.pst_VolDef->f_Max) return SND_f_InsertGetXmax(pInsert);
		fY = fY - pInsert->Yaxis.pst_VolDef->f_Min;
		fY = fY / (pInsert->Yaxis.pst_VolDef->f_Max - pInsert->Yaxis.pst_VolDef->f_Min);
		break;

	case en_InsYaxisPan:
		if(fY <= pInsert->Yaxis.pst_PanDef->f_Min) return SND_f_InsertGetXmin(pInsert);
		if(fY >= pInsert->Yaxis.pst_PanDef->f_Max) return SND_f_InsertGetXmax(pInsert);
		fY = fY - pInsert->Yaxis.pst_PanDef->f_Min;
		fY = fY / (pInsert->Yaxis.pst_PanDef->f_Max - pInsert->Yaxis.pst_PanDef->f_Min);
		break;

	case en_InsYaxisFreq:
		if(fY <= pInsert->Yaxis.pst_FreqDef->f_Min) return SND_f_InsertGetXmin(pInsert);
		if(fY >= pInsert->Yaxis.pst_FreqDef->f_Max) return SND_f_InsertGetXmax(pInsert);
		fY = fY - pInsert->Yaxis.pst_FreqDef->f_Min;
		fY = fY / (pInsert->Yaxis.pst_FreqDef->f_Max - pInsert->Yaxis.pst_FreqDef->f_Min);
		break;
	}

	pKey = pInsert->pst_KeyArray->dst_Key;
	tx = prev_tx = 0.0f;
	ty = prev_ty = pKey->tx;
	pKey++;

	b_break = FALSE;
	for(i = 1; i < pInsert->pst_KeyArray->ui_Size - 1; i++, pKey++)
	{
		tx = pKey->tx;
		ty = prev_ty + (tx - prev_tx) * pKey[-1].ty;

		if(prev_ty <= ty)
		{
			if((prev_ty <= fY) && (fY <= ty)) b_break = TRUE;
		}
		else
		{
			if((ty <= fY) && (fY <= prev_ty)) b_break = TRUE;
		}

		if(b_break) break;

		prev_tx = tx;
		prev_ty = ty;
	}

	/* get last point */
	if(!b_break)
	{
		tx = 1.0f;
		ty = pKey->tx;
		if(prev_ty <= ty)
		{
			if((prev_ty <= fY) && (fY <= ty)) b_break = TRUE;
		}
		else
		{
			if((ty <= fY) && (fY <= prev_ty)) b_break = TRUE;
		}

		if(!b_break) return SND_f_InsertGetXmax(pInsert);
	}

	fX = MATH_f_FloatBlend(prev_tx, tx, (fY - prev_ty) / (ty - prev_ty));

	switch(pInsert->Xaxis.pst_InsVoid->e_ChunkId)
	{
	case en_InsXaxisDistance:
		fX = MATH_f_FloatBlend(pInsert->Xaxis.pst_DistDef->f_Min, pInsert->Xaxis.pst_DistDef->f_Max, fY);
		break;

	case en_InsXaxisTime:
		fX = MATH_f_FloatBlend(pInsert->Xaxis.pst_TimeDef->f_Min, pInsert->Xaxis.pst_TimeDef->f_Max, fY);
		break;

	case en_InsXaxisVar:
		fX = MATH_f_FloatBlend(0.0f, 100.0f, fY);
		break;

	default:
		break;
	}

	return fX;
}

/*$4
 ***********************************************************************************************************************
    ACTIVE_EDITORS
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void SND_InsertSave(SND_tdst_Insert *pst_Insert, BIG_KEY _ul_Key)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_InsertFileHeader	st_FileHeader;
	ULONG						ul_Index;
	char						asz_Path[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ERR_X_Assert(pst_Insert->ul_Flags & SND_Cul_DSF_Used);

	/*$1- file access ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(_ul_Key == BIG_C_InvalidIndex) _ul_Key = LOA_ul_SearchKeyWithAddress((ULONG) pst_Insert);

	ul_Index = BIG_ul_SearchKeyToFat(_ul_Key);
	if(ul_Index == BIG_C_InvalidIndex)
	{
		ERR_X_Warning(0, "[SND] Cannot Find Insert file", NULL);
		return;
	}

	if(BIG_b_IsFileExtension(ul_Index, EDI_Csz_ExtSoundFade))
		pst_Insert->ul_Flags |= SND_Cte_InsFade;
	else
		pst_Insert->ul_Flags &= ~SND_Cte_InsFade;

	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	SAV_Begin(asz_Path, BIG_NameFile(ul_Index));

	/*$1- file header ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	/* update chunsize */
	pst_Insert->pst_KeyArray->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsertChunkHeader) +
		sizeof(unsigned int) +
		pst_Insert->pst_KeyArray->ui_Size *
		sizeof(SND_tdst_InsKey);

	st_FileHeader.ui_FileId = SND_Cte_InsertFileId;
	st_FileHeader.ui_Version = SND_Cte_InsertVersionFile;
	st_FileHeader.ui_DataSize = sizeof(ULONG);
	st_FileHeader.ui_DataSize += pst_Insert->Xaxis.pst_InsVoid->ui_ChunkSize;
	st_FileHeader.ui_DataSize += pst_Insert->Yaxis.pst_InsVoid->ui_ChunkSize;
	st_FileHeader.ui_DataSize += pst_Insert->pst_KeyArray->st_Header.ui_ChunkSize;
	SAV_Buffer(&st_FileHeader, sizeof(SND_tdst_InsertFileHeader));

	/*$1- file data ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SAV_Buffer(&pst_Insert->ul_Flags, sizeof(ULONG));
	SAV_Buffer(pst_Insert->Xaxis.pst_InsVoid, pst_Insert->Xaxis.pst_InsVoid->ui_ChunkSize);
	SAV_Buffer(pst_Insert->Yaxis.pst_InsVoid, pst_Insert->Yaxis.pst_InsVoid->ui_ChunkSize);
	SAV_Buffer(&pst_Insert->pst_KeyArray->st_Header, sizeof(SND_tdst_InsertChunkHeader));
	SAV_Buffer(&pst_Insert->pst_KeyArray->ui_Size, sizeof(unsigned int));
	SAV_Buffer(pst_Insert->pst_KeyArray->dst_Key, pst_Insert->pst_KeyArray->ui_Size * sizeof(SND_tdst_InsKey));
	SAV_ul_End();
	LINK_FatHasChanged();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
SND_tdst_Insert *SND_p_InsertCreate
(
	ULONG				ul_FileKey,
	SND_tden_InsRefType eY,
	SND_tden_InsRefType eX,
	unsigned int		uiKeyNb
)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	SND_tdst_Insert *pInsert;
	int				idx = SND_i_InsertGetFree();
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	SND_M_GetInsertOrReturn(idx, pInsert, NULL);
	pInsert->ul_UserCount = 1;
	pInsert->ul_Flags = SND_Cul_DSF_Used;
	pInsert->ul_FileKey = ul_FileKey;

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(eX)
	{
	case en_InsXaxisDistance:
		pInsert->Xaxis.pst_DistDef = (SND_tdst_InsXDistanceDef*)MEM_p_Alloc(sizeof(SND_tdst_InsXDistanceDef));
		L_memset(pInsert->Xaxis.pst_DistDef, 0, sizeof(SND_tdst_InsXDistanceDef));
		pInsert->Xaxis.pst_DistDef->st_Header.e_ChunkId = eX;
		pInsert->Xaxis.pst_DistDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsXDistanceDef);
		pInsert->Xaxis.pst_DistDef->ui_Axis = SND_Cte_InsAxisX;
		pInsert->Xaxis.pst_DistDef->ui_GaoKey = BIG_C_InvalidKey;
		pInsert->Xaxis.pst_DistDef->f_Min = 0.0f;
		pInsert->Xaxis.pst_DistDef->f_Max = 100.0f;
		break;

	case en_InsXaxisTime:
		pInsert->Xaxis.pst_TimeDef = (SND_tdst_InsXTimeDef*)MEM_p_Alloc(sizeof(SND_tdst_InsXTimeDef));
		L_memset(pInsert->Xaxis.pst_TimeDef, 0, sizeof(SND_tdst_InsXTimeDef));
		pInsert->Xaxis.pst_TimeDef->st_Header.e_ChunkId = eX;
		pInsert->Xaxis.pst_TimeDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsXTimeDef);
		pInsert->Xaxis.pst_TimeDef->f_Max = 1.0f;
		pInsert->Xaxis.pst_TimeDef->f_Min = 0.0f;
		break;

	case en_InsXaxisVar:
		pInsert->Xaxis.pst_VarDef = (SND_tdst_InsXVarDef*)MEM_p_Alloc(sizeof(SND_tdst_InsXVarDef));
		L_memset(pInsert->Xaxis.pst_VarDef, 0, sizeof(SND_tdst_InsXVarDef));
		pInsert->Xaxis.pst_VarDef->st_Header.e_ChunkId = eX;
		pInsert->Xaxis.pst_VarDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsXVarDef);
		pInsert->Xaxis.pst_VarDef->ui_VarId = 0;
		break;

	default:
		ERR_X_Assert(0);
		return NULL;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	switch(eY)
	{
	case en_InsYaxisVol:
		pInsert->Yaxis.pst_VolDef = (SND_tdst_InsYVolDef*)MEM_p_Alloc(sizeof(SND_tdst_InsYVolDef));
		L_memset(pInsert->Yaxis.pst_VolDef, 0, sizeof(SND_tdst_InsYVolDef));
		pInsert->Yaxis.pst_VolDef->st_Header.e_ChunkId = eY;
		pInsert->Yaxis.pst_VolDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsYVolDef);
		pInsert->Yaxis.pst_VolDef->b_Wet = FALSE;
		pInsert->Yaxis.pst_VolDef->f_Min = 0.0f;
		pInsert->Yaxis.pst_VolDef->f_Max = 100.0f;
		break;

	case en_InsYaxisPan:
		pInsert->Yaxis.pst_PanDef = (SND_tdst_InsYPanDef*)MEM_p_Alloc(sizeof(SND_tdst_InsYPanDef));
		L_memset(pInsert->Yaxis.pst_PanDef, 0, sizeof(SND_tdst_InsYPanDef));
		pInsert->Yaxis.pst_PanDef->st_Header.e_ChunkId = eY;
		pInsert->Yaxis.pst_PanDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsYPanDef);
		pInsert->Yaxis.pst_PanDef->b_Surround = FALSE;
		pInsert->Yaxis.pst_PanDef->f_Min = 0.0f;
		pInsert->Yaxis.pst_VolDef->f_Max = 100.0f;
		break;

	case en_InsYaxisFreq:
		pInsert->Yaxis.pst_FreqDef = (SND_tdst_InsYFreqDef*)MEM_p_Alloc(sizeof(SND_tdst_InsYFreqDef));
		L_memset(pInsert->Yaxis.pst_FreqDef, 0, sizeof(SND_tdst_InsYFreqDef));
		pInsert->Yaxis.pst_FreqDef->st_Header.e_ChunkId = eY;
		pInsert->Yaxis.pst_FreqDef->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsYFreqDef);
		pInsert->Yaxis.pst_FreqDef->f_Min = 0.0f;
		pInsert->Yaxis.pst_FreqDef->f_Max = 100.0f;
		break;

	default:
		ERR_X_Assert(0);
		return NULL;
	}

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pInsert->pst_KeyArray = (SND_tdst_InsKeyArray*)MEM_p_Alloc(sizeof(SND_tdst_InsKeyArray));
	L_memset(pInsert->pst_KeyArray, 0, sizeof(SND_tdst_InsKeyArray));
	pInsert->pst_KeyArray->st_Header.e_ChunkId = en_InsKeyArray;
	pInsert->pst_KeyArray->st_Header.ui_ChunkSize = sizeof(SND_tdst_InsertChunkHeader) +
		sizeof(unsigned int) +
		uiKeyNb *
		sizeof(SND_tdst_InsKey);
	pInsert->pst_KeyArray->ui_Size = uiKeyNb;
	pInsert->pst_KeyArray->dst_Key = (SND_tdst_InsKey*)MEM_p_Alloc(uiKeyNb * sizeof(SND_tdst_InsKey));
	L_memset(pInsert->pst_KeyArray->dst_Key, 0, uiKeyNb * sizeof(SND_tdst_InsKey));

	/*$1-~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	return pInsert;
}
#endif

/*$4
 ***********************************************************************************************************************
    EOF
 ***********************************************************************************************************************
 */
