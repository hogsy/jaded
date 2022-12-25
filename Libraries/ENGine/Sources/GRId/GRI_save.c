/*$T GRI_save.c GC! 1.092 09/04/00 10:46:30 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#include "Precomp.h"
#include "MATHs/MATH.h"
#include "ENGine/Sources/GRId/GRI_vars.h"
#include "ENGine/Sources/GRId/GRI_compute.h"
#include "ENGine/Sources/WORld/WORstruct.h"
#include "BIGfiles/SAVing/SAVdefs.h"
#include "BIGfiles/BIGfat.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/BIGkey.h"
#include "BIGfiles/LOAding/LOAdefs.h"
#include "EDIpaths.h"
#include "BASe/CLIbrary/CLIstr.h"
#include "BASe/MEMory/MEM.h"
#include "BASe/ERRors/ERRasser.h"
#include "ENGine/Sources/GRId/GRI_struct.h"
#include "ENGine/Sources/GRId/GRI_save.h"
#include "LINks/LINKstruct.h"
#include "LINks/LINKstruct_reg.h"

#if defined(PSX2_TARGET) && defined(__cplusplus)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
 ***********************************************************************************************************************
 */

#ifdef ACTIVE_EDITORS

BOOL GRI_gb_Modified = FALSE;

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_CreateGrid(WOR_tdst_World *_pst_World, int _i_Num)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRID_tdst_World *pst_Grid;
	BIG_INDEX		ul_Index;
	char			asz_Name[BIG_C_MaxLenName];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) _pst_World);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	L_strcpy(asz_Name, BIG_NameFile(ul_Index));
	*L_strrchr(asz_Name, '.') = 0;
	if(_i_Num == 2) L_strcat(asz_Name, "1");
	pst_Grid = GRI_CreateGridGrid(ul_Index, asz_Name);

	/* Associate to world */
	if(_i_Num == 1)
		_pst_World->pst_Grid = pst_Grid;
	else
		_pst_World->pst_Grid1 = pst_Grid;

	/* Save the grid */
	GRI_SaveGrid(pst_Grid);

	/* Uncompress current editor array */
	GRI_UnCompressReal(pst_Grid);
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
GRID_tdst_World *GRI_CreateGridGrid(BIG_INDEX ul_Index, char *_psz_Name)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	GRID_tdst_World *pst_Grid;
	ULONG			ul_Size;
	BIG_INDEX		ul_File;
	char			asz_Path[BIG_C_MaxLenPath];
	char			asz_Name[BIG_C_MaxLenPath];
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	pst_Grid = (GRID_tdst_World*)MEM_p_Alloc(sizeof(GRID_tdst_World));
	L_memset(pst_Grid, 0, sizeof(GRID_tdst_World));

	/* Default values */
	pst_Grid->f_MinXTotal = 0.0f;
	pst_Grid->f_MinYTotal = 0.0f;
	pst_Grid->f_MinXReal = 0.0f;
	pst_Grid->f_MinYReal = 0.0f;
	pst_Grid->w_NumGroupsX = 10;
	pst_Grid->w_NumGroupsY = 10;
	pst_Grid->w_NumRealGroupsX = GRID_REAL_X;
	pst_Grid->w_NumRealGroupsY = GRID_REAL_Y;
	pst_Grid->c_SizeGroup = 25;

	/* Compress buffer */
	pst_Grid->p_Groups = (void **) MEM_p_Alloc(pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY * sizeof(void *));
	L_memset(pst_Grid->p_Groups, 0, pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY * sizeof(void *));
	pst_Grid->p_RealGroups = (void **) MEM_p_Alloc(pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY * sizeof(void *));
	L_memset(pst_Grid->p_RealGroups, 0, pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY * sizeof(void *));

	/* Total buf */
	ul_Size = pst_Grid->w_NumGroupsX * pst_Grid->c_SizeGroup;
	ul_Size *= pst_Grid->w_NumGroupsY * pst_Grid->c_SizeGroup;
	ul_Size *= sizeof(GRID_tdst_Elem);
	pst_Grid->p_CompressBuf = MEM_p_Alloc(2 * ul_Size);

	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, EDI_Csz_Path_Grid);
	L_strcpy(asz_Name, _psz_Name);
	L_strcat(asz_Name, EDI_Csz_ExtGridCompressed);
	SAV_Begin(asz_Path, asz_Name);
	SAV_Buffer(asz_Path, 1);
	ul_File = SAV_ul_End();
	LOA_AddAddress(ul_File, pst_Grid->p_CompressBuf);

	/* Real time array */
	ul_Size = pst_Grid->c_SizeGroup * pst_Grid->w_NumRealGroupsX;
	ul_Size *= pst_Grid->c_SizeGroup * pst_Grid->w_NumRealGroupsY;
	ul_Size *= sizeof(GRID_tdst_Elem);
	pst_Grid->pst_RealArray = (GRID_tdst_Elem *) MEM_p_Alloc(ul_Size);
	L_memset(pst_Grid->pst_RealArray, 0, ul_Size);

	/* Eval array */
	ul_Size = pst_Grid->c_SizeGroup * pst_Grid->w_NumRealGroupsX;
	ul_Size *= pst_Grid->c_SizeGroup * pst_Grid->w_NumRealGroupsY;
	ul_Size *= sizeof(GRID_tdst_Eval);
	pst_Grid->pst_EvalArray = (GRID_tdst_Eval *) MEM_p_Alloc(ul_Size);

	/* Editor array */
	ul_Size = pst_Grid->c_SizeGroup * pst_Grid->w_NumGroupsX;
	ul_Size *= pst_Grid->c_SizeGroup * pst_Grid->w_NumGroupsY;
	ul_Size *= sizeof(GRID_tdst_Elem);
	pst_Grid->pst_EditArray = (GRID_tdst_Elem *) MEM_p_Alloc(ul_Size);
	L_memset(pst_Grid->pst_EditArray, 0, ul_Size);

	/* Save definition of grid */
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);
	L_strcat(asz_Path, "/");
	L_strcat(asz_Path, EDI_Csz_Path_Grid);
	L_strcpy(asz_Name, _psz_Name);
	L_strcat(asz_Name, EDI_Csz_ExtGridDef);
	SAV_Begin(asz_Path, asz_Name);
	SAV_Buffer(asz_Path, 1);
	ul_File = SAV_ul_End();
	LOA_AddAddress(ul_File, pst_Grid);
	LINK_RegisterPointer(pst_Grid, LINK_C_ENG_Grid, BIG_NameFile(ul_File), asz_Path);

	return pst_Grid;
}

/*
 =======================================================================================================================
 =======================================================================================================================
 */
void GRI_SaveGrid(GRID_tdst_World *pst_Grid)
{
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	BIG_INDEX	ul_Index, ul_Index1;
	char		asz_Path[BIG_C_MaxLenPath];
	ULONG		ul_TotalSize;
	void		*p_Buf;
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	GRI_gb_Modified = FALSE;
	if(!pst_Grid) return;
	ul_Index = LOA_ul_SearchIndexWithAddress((ULONG) pst_Grid);
	ERR_X_Assert(ul_Index != BIG_C_InvalidIndex);
	BIG_ComputeFullName(BIG_ParentFile(ul_Index), asz_Path);

	/* Update compress buffer */
	ul_TotalSize = GRI_UpdateCompress(pst_Grid);

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Grid compressed
	 -------------------------------------------------------------------------------------------------------------------
	 */

	ul_Index1 = (ULONG) pst_Grid->p_CompressBuf;
	ul_Index1 = LOA_ul_SearchIndexWithAddress(ul_Index1);
	ERR_X_Assert(ul_Index1 != BIG_C_InvalidIndex);
	SAV_Begin(asz_Path, BIG_NameFile(ul_Index1));
	SAV_Buffer((char *) pst_Grid->p_CompressBuf, ul_TotalSize);
	SAV_ul_End();

	/*$2
	 -------------------------------------------------------------------------------------------------------------------
	    Grid def
	 -------------------------------------------------------------------------------------------------------------------
	 */

	SAV_Begin(asz_Path, BIG_NameFile(ul_Index));

	/* Grid struct */
	p_Buf = pst_Grid->p_CompressBuf;
	pst_Grid->p_CompressBuf = (void *) BIG_FileKey(ul_Index1);
	SAV_Buffer(&pst_Grid->p_Groups, 4);
	SAV_Buffer(&pst_Grid->p_RealGroups, 4);
	SAV_Buffer(&pst_Grid->p_CompressBuf, 4);
	SAV_Buffer(&pst_Grid->pst_RealArray, 4);
	SAV_Buffer(&pst_Grid->pst_EvalArray, 4);
	SAV_Buffer(&pst_Grid->pst_EditArray, 4);
	SAV_Buffer(&pst_Grid->f_MinXTotal, 4);
	SAV_Buffer(&pst_Grid->f_MinYTotal, 4);
	SAV_Buffer(&pst_Grid->f_MinXReal, 4);
	SAV_Buffer(&pst_Grid->f_MinYReal, 4);
	SAV_Buffer(&pst_Grid->w_NumGroupsX, 2);
	SAV_Buffer(&pst_Grid->w_NumGroupsY, 2);
	SAV_Buffer(&pst_Grid->w_NumRealGroupsX, 2);
	SAV_Buffer(&pst_Grid->w_NumRealGroupsY, 2);
	SAV_Buffer(&pst_Grid->w_XRealGroup, 2);
	SAV_Buffer(&pst_Grid->w_YRealGroup, 2);
	SAV_Buffer(&pst_Grid->c_SizeGroup, 1);
	SAV_Buffer(&pst_Grid->p_GameObject, 4);
	SAV_Buffer(&pst_Grid->c_SizeGroup, 1);
	SAV_Buffer(&pst_Grid->c_SizeGroup, 1);
	SAV_Buffer(&pst_Grid->c_SizeGroup, 1);
	pst_Grid->p_CompressBuf = p_Buf;

	/* All references to compress buffers (index) */
	SAV_Buffer
	(
		pst_Grid->p_Groups,
		pst_Grid->w_NumGroupsX * pst_Grid->w_NumGroupsY * 4
	);

	SAV_ul_End();
}



#endif
#if defined(PSX2_TARGET) && defined(__cplusplus)
}
#endif
