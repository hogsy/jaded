/*$T OBJgrp.c GC! 1.100 06/12/01 12:41:32 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


/* Aim: Handle the groups of objects */
#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/BIGgroup.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "ENGine/Sources/OBJects/OBJaccess.h"
#include "ENGine/Sources/OBJects/OBJload.h"
#include "ENGine/Sources/OBJects/OBJinit.h"

#ifdef PSX2_TARGET
#include "MainPsx2/Sources/PSX2debug.h"
#define _ReadLong(_a)	ReadLong((char *) _a)
#else
#define _ReadLong(_a)	*(ULONG *) _a
#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG OBJ_ul_World_LoadGolCallback(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFtable	*pst_AllObjects;
	ULONG				ul_Length;
	TAB_tdst_PFelem		*pst_Elem;
	ULONG				*pul_Buffer;
	CHAR				*puc_BufferInit;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_AllObjects = (TAB_tdst_PFtable *) MEM_p_Alloc(sizeof(TAB_tdst_PFtable));
	L_memset(pst_AllObjects, 0, sizeof(TAB_tdst_PFtable));

	puc_BufferInit = BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Length);
	pul_Buffer = (ULONG *) puc_BufferInit;

	TAB_PFtable_Init(pst_AllObjects, ul_Length, 0.1f);
	while(pul_Buffer < (ULONG*)(puc_BufferInit + ul_Length))
	{
		TAB_PFtable_AddElemWithData(pst_AllObjects, 0, 0);
		pst_Elem = TAB_pst_PFtable_GetLastElem(pst_AllObjects);
		LOA_MakeFileRef
		(
			LOA_ReadLong((CHAR**)&pul_Buffer),
			(ULONG *) &pst_Elem->p_Pointer,
			OBJ_ul_GameObjectCallback,
			LOA_C_MustExists
		);
		LOA_ReadLong_Ed((CHAR**)&pul_Buffer, NULL); // Type, not used
	}

	return (ULONG) pst_AllObjects;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_FreeGroupGrl(TAB_tdst_PFtable *_pst_AllObjects)
{
	if(!_pst_AllObjects) return;
	TAB_PFtable_Close(_pst_AllObjects);
	LOA_DeleteAddress(_pst_AllObjects);
	MEM_Free(_pst_AllObjects);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_FreeGroup(OBJ_tdst_Group *_pst_Group)
{
	OBJ_FreeGroupGrl(_pst_Group->pst_AllObjects);
#ifdef ACTIVE_EDITORS
	if(_pst_Group->sz_Name) MEM_Free(_pst_Group->sz_Name);
#endif
	MEM_Free(_pst_Group);
	LOA_DeleteAddress(_pst_Group);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_AddInGroup(OBJ_tdst_Group *_pst_Group, void *_pst_GO)
{
	TAB_PFtable_AddElemWithDataAndResize(_pst_Group->pst_AllObjects, _pst_GO, 0);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_DelInGroup(OBJ_tdst_Group *_pst_Group, void *_pst_GO)
{
	TAB_PFtable_RemoveElemWithPointer(_pst_Group->pst_AllObjects, _pst_GO);
}

#ifdef ACTIVE_EDITORS

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_SaveGolGroup(OBJ_tdst_Group *_pst_Group)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX			ul_Index;
	char				asz_Path[BIG_C_MaxLenPath];
	BIG_tdst_GroupElem	st_Buf;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	OBJ_tdst_GameObject *pst_GO;
	char				*psz_Temp;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_Group->pst_AllObjects);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	SAV_Begin(asz_Path, BIG_NameFile(ul_Index));

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Group->pst_AllObjects);
	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		if(!TAB_b_IsAHole(pst_GO))
		{
			ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_GO);
			if(ul_Index != BIG_C_InvalidIndex)
			{
				psz_Temp = L_strrchr(BIG_NameFile(ul_Index), '.');
				st_Buf.ul_Key = BIG_FileKey(ul_Index);
				st_Buf.ul_Type = *((LONG *) psz_Temp);
				SAV_Buffer(&st_Buf, sizeof(BIG_tdst_GroupElem));
			}
		}
	}

	SAV_ul_End();
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_OrderGroupByHie(OBJ_tdst_Group *_pst_Group)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	OBJ_tdst_GameObject *pst_GO;
	OBJ_tdst_GameObject *pst_GO1;
	TAB_tdst_PFelem		*pst_CurrentElem;
	TAB_tdst_PFelem		*pst_EndElem;
	TAB_tdst_PFelem		*pst_CurrentElem1;
	TAB_tdst_PFelem		st_Elem;
	BOOL				b_Swap;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Group->pst_AllObjects);
	do
	{
		b_Swap = FALSE;
		for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
		{
			pst_GO = (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
			if(TAB_b_IsAHole(pst_GO)) continue;
			if
			(
				(pst_GO->ul_IdentityFlags & OBJ_C_IdentityFlag_Hierarchy)
			&&	(pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit)
			)
			{
				pst_CurrentElem1 = TAB_pst_PFtable_GetFirstElem(_pst_Group->pst_AllObjects);
				for(pst_CurrentElem1; pst_CurrentElem1 <= pst_EndElem; pst_CurrentElem1++)
				{
					pst_GO1 = (OBJ_tdst_GameObject *) pst_CurrentElem1->p_Pointer;
					if(TAB_b_IsAHole(pst_GO1)) continue;
					if(pst_GO1 == pst_GO->pst_Base->pst_Hierarchy->pst_FatherInit)
					{
						if(pst_CurrentElem1 > pst_CurrentElem)
						{
							L_memcpy(&st_Elem, pst_CurrentElem1, sizeof(TAB_tdst_PFelem));
							L_memcpy(pst_CurrentElem1, pst_CurrentElem, sizeof(TAB_tdst_PFelem));
							L_memcpy(pst_CurrentElem, &st_Elem, sizeof(TAB_tdst_PFelem));
							b_Swap = TRUE;
						}
						break;
					}
				}
			}
		}
	} while(b_Swap);
}

#endif /* ACTIVE_EDITORS */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void OBJ_GroupSetAsNotRTL(void)
{
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
OBJ_tdst_GameObject *OBJ_pst_GroupGetByRank(OBJ_tdst_Group *_pst_Group, int rank)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	TAB_tdst_PFelem *pst_CurrentElem;
	TAB_tdst_PFelem *pst_EndElem;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	if(!_pst_Group) return NULL;
	pst_CurrentElem = TAB_pst_PFtable_GetFirstElem(_pst_Group->pst_AllObjects);
	pst_EndElem = TAB_pst_PFtable_GetLastElem(_pst_Group->pst_AllObjects);
	for(pst_CurrentElem; pst_CurrentElem <= pst_EndElem; pst_CurrentElem++)
	{
		if(!rank) return (OBJ_tdst_GameObject *) pst_CurrentElem->p_Pointer;
		rank--;
	}

	return NULL;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
