/*$T GRI_load.c GC! 1.081 05/31/00 14:40:11 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "BASe/BAStypes.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "MATHs/MATHfloat.h"
#include "BIGfiles/BIGread.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "BIGfiles/LOAding/LOAread.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGread.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_vars.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

extern int GRID_gi_Current;
extern ULONG		LOA_ul_FileTypeSize[40];

/*
 =======================================================================================================================
 =======================================================================================================================
 */

ULONG GRID_ul_CallbackCompressedGrid(ULONG _ul_PosFile)
{
	CHAR	*pc_Buf;
	ULONG	ul_Size;
	CHAR * p_CompGrid, * p_Ret;
	ULONG	ul_GroupSize;
	CHAR	*pc_EndGroup;
	CHAR	*pc_EndGrid;
	CHAR	c_Mask;

	pc_Buf = (char *) BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);

	if(!ul_Size)
	{
		p_CompGrid = p_Ret = (CHAR *)MEM_p_Alloc(4);
		*(LONG*)p_CompGrid = 0;
	}
	else
	{
		p_CompGrid = p_Ret = (CHAR *)MEM_p_Alloc(ul_Size);
		LOA_ul_FileTypeSize[39] += ul_Size;
		pc_EndGrid = pc_Buf + ul_Size;
		// The goal here is not to decompress the grid, but to detect which entries are char and which are short
		while(pc_Buf < pc_EndGrid)
		{
			
			ul_GroupSize = _LOA_ReadLong(&pc_Buf, (LONG*)p_CompGrid, LOA_eBinEngineData);
			p_CompGrid += sizeof(LONG); // advance the pointer
			ul_GroupSize -= sizeof(LONG); // Stored size includes the 4 bytes for the size field.
			pc_EndGroup = pc_Buf + ul_GroupSize;
			while(pc_Buf < pc_EndGroup)
			{
				*p_CompGrid++ = c_Mask = LOA_ReadChar(&pc_Buf);
				// 0xC0 is ~COMPRESS_MASK, COMPRESS_MASK is 0x3F
				if((c_Mask & 0xC0) == (CHAR) 0xC0)
				{ // Read one more char: 1st one was the count, 2nd is the value
					*p_CompGrid++ = LOA_ReadChar(&pc_Buf);
				}
				else if(c_Mask == (CHAR) 0x80)
				{ // Read two more char: 1st one was a mark, 2nd is the count, 3rd is the value
					*p_CompGrid++ = LOA_ReadChar(&pc_Buf); 
					*p_CompGrid++ = LOA_ReadChar(&pc_Buf);
				}
				else if(c_Mask == (CHAR) 0x40)
				{ // Read a short and a char: 1st one was a mark, 2nd(short) is the count, 3rd is the value
					_LOA_ReadShort(&pc_Buf, (SHORT*)p_CompGrid, LOA_eBinEngineData); // Read one short
					p_CompGrid += sizeof(SHORT);
					*p_CompGrid++ = LOA_ReadChar(&pc_Buf); // Read one more char
				}
			}
		}
	}
	return (ULONG) p_Ret;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
ULONG GRID_ul_CallbackWorldGrid(ULONG _ul_PosFile)
{
	/*~~~~~~~~~~~~~~~~~~~~~~*/
	GRID_tdst_World *pst_Grid;
	char			*pc_Buf;
	ULONG			ul_Size;
	/*~~~~~~~~~~~~~~~~~~~~~~*/

	GRID_gi_Current = 0;
	pst_Grid = (GRID_tdst_World *) MEM_p_Alloc(sizeof(GRID_tdst_World));
	LOA_ul_FileTypeSize[39] += sizeof(GRID_tdst_World);
	L_memset(pst_Grid, 0, sizeof(GRID_tdst_World));

	/* Read grid def */
	pc_Buf = (char *) BIG_pc_ReadFileTmp(_ul_PosFile, &ul_Size);


	// Read GRID_tdst_World Structure
	LOA_ReadULong_Ed(&pc_Buf, (ULONG*)&pst_Grid->p_Groups); // Skip Pointer, allocated below
	LOA_ReadULong_Ed(&pc_Buf, (ULONG*)&pst_Grid->p_RealGroups); // Skip Pointer, allocated below
	*(ULONG*)&pst_Grid->p_CompressBuf = LOA_ReadULong(&pc_Buf); // Contains the Key of the compressed grid to load
	LOA_ReadULong_Ed(&pc_Buf, (ULONG*)&pst_Grid->pst_RealArray); // Skip Pointer, allocated below
	LOA_ReadULong_Ed(&pc_Buf, (ULONG*)&pst_Grid->pst_EvalArray); // Skip Pointer, allocated below
	LOA_ReadULong_Ed(&pc_Buf, (ULONG*)&pst_Grid->pst_EditArray); // Skip Pointer, allocated below
	pst_Grid->f_MinXTotal = LOA_ReadFloat(&pc_Buf);
	pst_Grid->f_MinYTotal = LOA_ReadFloat(&pc_Buf);
	pst_Grid->f_MinXReal = LOA_ReadFloat(&pc_Buf);
	pst_Grid->f_MinYReal = LOA_ReadFloat(&pc_Buf);
	pst_Grid->w_NumGroupsX = LOA_ReadShort(&pc_Buf);
	pst_Grid->w_NumGroupsY = LOA_ReadShort(&pc_Buf);
	LOA_ReadShort_Ed(&pc_Buf, &pst_Grid->w_NumRealGroupsX); // ignore the saved value, set to GRID_REAL_X below
	LOA_ReadShort_Ed(&pc_Buf, &pst_Grid->w_NumRealGroupsY); // ignore the saved value, set to GRID_REAL_Y below
	pst_Grid->w_XRealGroup = LOA_ReadShort(&pc_Buf);
	pst_Grid->w_YRealGroup = LOA_ReadShort(&pc_Buf);
	pst_Grid->c_SizeGroup = LOA_ReadChar(&pc_Buf);
	LOA_ReadULong_Ed(&pc_Buf, (ULONG*)&pst_Grid->p_GameObject); // Skip Pointer, set to null below
	// Skip 3 bytes of alignment
	LOA_ReadChar_Ed(&pc_Buf, NULL);
	LOA_ReadChar_Ed(&pc_Buf, NULL);
	LOA_ReadChar_Ed(&pc_Buf, NULL);

	pst_Grid->w_NumRealGroupsX = GRID_REAL_X;
	pst_Grid->w_NumRealGroupsY = GRID_REAL_Y;

	ERR_X_Warning
	(
		MATH_f_FloatModulo(pst_Grid->f_MinXTotal, GRID_CASE) == pst_Grid->f_MinXTotal,
		"Real time grid is not align on 1.0 m in X",
		NULL
	);
	ERR_X_Warning
	(
		MATH_f_FloatModulo(pst_Grid->f_MinYTotal, GRID_CASE) == pst_Grid->f_MinYTotal,
		"Real time grid is not align on 1.0 m in Y",
		NULL
	);
	pst_Grid->p_GameObject = NULL;

	/* Array of pointers on groups */
	pst_Grid->p_Groups = (void **) MEM_p_Alloc(pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY * sizeof(void *));
	LOA_ul_FileTypeSize[39] += pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY * sizeof(void *);
	pst_Grid->p_RealGroups = (void **) MEM_p_Alloc(pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY * sizeof(void *));
	LOA_ul_FileTypeSize[39] += pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY * sizeof(void *);
	L_memset(pst_Grid->p_RealGroups, 0, pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY * sizeof(void *));

	/* Read all offsets on the grid buffer */
	{
		LONG i;
		ULONG * pulGroups;
		for(i = 0, pulGroups = (ULONG*)pst_Grid->p_Groups; i < pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY; ++i, ++pulGroups)
		{
			*pulGroups = LOA_ReadULong(&pc_Buf);
			if(*pulGroups == 0x00FFFFFF) *pulGroups = -1;
			if(*pulGroups == 0xFFFFFF00) *pulGroups = -1;
		}

	}


	/* The compressed buffers */
	LOA_MakeFileRef
	(
		(ULONG) pst_Grid->p_CompressBuf,
		(ULONG *) &pst_Grid->p_CompressBuf,
		GRID_ul_CallbackCompressedGrid,
		LOA_C_MustExists
	);

	/* Real time array */
	ul_Size = pst_Grid->w_NumRealGroupsX * pst_Grid->c_SizeGroup;
	ul_Size *= pst_Grid->w_NumRealGroupsY * pst_Grid->c_SizeGroup;
	ul_Size *= sizeof(GRID_tdst_Elem);
	pst_Grid->pst_RealArray = (GRID_tdst_Elem *) MEM_p_Alloc(ul_Size);
	LOA_ul_FileTypeSize[39] += ul_Size;

	/* Eval array */
	pst_Grid->pst_EvalArray = NULL;

	/* Editor array */
#ifdef ACTIVE_EDITORS
	ul_Size = pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup;
	ul_Size *= pst_Grid->w_NumGroupsY * pst_Grid->c_SizeGroup;
	ul_Size *= sizeof(GRID_tdst_Elem);
	pst_Grid->pst_EditArray = (GRID_tdst_Elem *) MEM_p_Alloc(ul_Size);
#endif
#ifdef ACTIVE_EDITORS
	{
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
		BIG_INDEX	ul_Fat;
		char		asz_Path[BIG_C_MaxLenPath];
		/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

		ul_Fat = BIG_ul_SearchKeyToFat(LOA_ul_GetCurrentKey());
		BIG_ComputeFullName(BIG_ParentFile(ul_Fat), asz_Path);
		LINK_RegisterPointer(pst_Grid, LINK_C_ENG_Grid, BIG_NameFile(ul_Fat), asz_Path);
	}

#endif
	return (ULONG) pst_Grid;
}

#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
