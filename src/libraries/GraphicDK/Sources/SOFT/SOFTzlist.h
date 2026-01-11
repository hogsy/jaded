/*$T SOFTzlist.h GC! 1.081 01/29/02 11:15:42 */


/*$6
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 */


#ifndef PSX2_TARGET
#pragma once
#endif
#ifndef __SOFTZLIST_H__
#define __SOFTZLIST_H__
#include "BASe/BAStypes.h"

#include "GDInterface/GDInterface.h"
#include "SOFT/SOFTstruct.h"
#include "MATerial/MATstruct.h"

#if defined (__cplusplus) && !defined(JADEFUSION)
extern "C"
{
#endif

/*$4
 ***********************************************************************************************************************
    constants
 ***********************************************************************************************************************
 */

#define NumberOfZListes 8
#define ZListesBase		4

#if defined(PSX2_TARGET)
#define SOFT_l_MaxEntry		(LONG) 12
#define SOFT_l_MaxNode		(LONG) 10
#define SOFT_l_MaxVertex	(LONG) 12
#define SOFT_l_MaxOC		(LONG) 8
#elif defined(_GAMECUBE)
/*
#define SOFT_l_MaxEntry		(LONG) 8
#define SOFT_l_MaxNode		(LONG) 8
#define SOFT_l_MaxVertex	(LONG) 14
#define SOFT_l_MaxOC		(LONG) 8
*/
#define SOFT_l_MaxEntry		(LONG) 12
#define SOFT_l_MaxNode		(LONG) 11
#define SOFT_l_MaxVertex	(LONG) 12
#define SOFT_l_MaxOC		(LONG) 8

#else
#define SOFT_l_MaxEntry		(LONG) 14
#define SOFT_l_MaxNode		(LONG) 10
#define SOFT_l_MaxVertex	(LONG) 14
#define SOFT_l_MaxOC		(LONG) 8
#endif

/* Following flags are used in SOFT_pst_ZList_AddNode */
#define SOFT_l_TON_Vertices			0
#define SOFT_l_TON_ObjectContext	1


#define size_SOFT_gst_ZList			((NumberOfZListes * sizeof(SOFT_tdst_ZList)) + 64 & ~63)
#define size_SOFT_gst_ZList_CP		(sizeof(SOFT_tdst_ZList_CommonParrams) + 64 & ~63)
#define size_SOFT_Compute			(sizeof(SOFT_tdst_ComputingBuffers) + 64 & ~63)
#define size_AfterFxBuffer			(1146880 + 64 & ~63) // GXGetTexBufferSize( FRAME_BUFFER_WIDTH, (gul_FRAME_BUFFER_HEIGHT), GX_TF_RGBA8, FALSE, 0xFF);
#define size_AfterFxBuffer_ZSR		(286720 + 64 & ~63)  // GXGetTexBufferSize((FRAME_BUFFER_WIDTH )>>1, gul_FRAME_BUFFER_HEIGHT>>1, GX_TF_RGBA8, FALSE, 0xFF);

#ifdef _GAMECUBE
#define SIZE_BIG_BUFFER				(size_SOFT_gst_ZList + size_SOFT_gst_ZList_CP + size_SOFT_Compute + size_AfterFxBuffer + size_AfterFxBuffer_ZSR)
//#define SIZE_BIG_BUFFER				(size_SOFT_gst_ZList + size_SOFT_gst_ZList_CP + GXI_MaxExternalFrameBufferSize + GXI_MaxExternalFrameBufferSize + GXI_DefaultFIFOSize)
#else
#define SIZE_BIG_BUFFER				(size_SOFT_gst_ZList + size_SOFT_gst_ZList_CP + size_SOFT_Compute)
#endif


/*$4
 ***********************************************************************************************************************
    struct
 ***********************************************************************************************************************
 */

/*
 -----------------------------------------------------------------------------------------------------------------------
    this stuct is used for store the display context of an object (Matrix, ...)
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SOFT_tdst_ZListObjectContext_
{
	OBJ_tdst_GameObject *pst_GO;
	MATH_tdst_Matrix	st_Matrix;
	MATH_tdst_Matrix	st_MatrixGO;

#ifdef ACTIVE_EDITORS
	ULONG				ulForcedColorSaved;
#endif
} SOFT_tdst_ZListObjectContext;

/*
 -----------------------------------------------------------------------------------------------------------------------
    a node (sorted object or sorted faces)
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SOFT_tdst_ZList_Node_
{
	struct SOFT_tdst_ZList_Node_	*pst_NextNode;

	union
	{
		SOFT_tdst_Vertex				*pst_Vertices;
		SOFT_tdst_ZListObjectContext	*pst_OC;
	};
	ULONG					l_DrawMask;
	MAT_tdst_MultiTexture	*pst_Material;
} SOFT_tdst_ZList_Node;

/*
 -----------------------------------------------------------------------------------------------------------------------
    all data
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SOFT_tdst_ZList_CommonParrams_
{
	SOFT_tdst_ZList_Node			ast_Node[1 << SOFT_l_MaxNode];
	SOFT_tdst_ZList_Node			*pst_CurrentNode;
	SOFT_tdst_Vertex				*pst_CurrentVertex;
	SOFT_tdst_ZListObjectContext	*pst_CurrentObjectContext;

	/* BEGIN DO NOT CHANGE THIS ORDER */
	SOFT_tdst_Vertex				ast_Vertex[(LONG) 1 << SOFT_l_MaxVertex];
	SOFT_tdst_ZListObjectContext	ast_OC[(LONG) 1 << SOFT_l_MaxOC];
	ULONG							ulLastField;

	/* END DO NOT CHANGE THIS ORDER */
} SOFT_tdst_ZList_CommonParrams;

/*
 -----------------------------------------------------------------------------------------------------------------------
    list data
 -----------------------------------------------------------------------------------------------------------------------
 */
typedef struct	SOFT_tdst_ZList_
{
	LONG					l_Min;
	LONG					l_Max;
	SOFT_tdst_ZList_Node	*apst_SortedNode[1 << SOFT_l_MaxEntry];
} SOFT_tdst_ZList;

/*$4
 ***********************************************************************************************************************
    globals
 ***********************************************************************************************************************
 */

extern SOFT_tdst_ZList	*SOFT_gst_ZList;
extern SOFT_tdst_ZList	*p_Current_SOFT_gst_ZList;

/*$4
 ***********************************************************************************************************************
    functions
 ***********************************************************************************************************************
 */

/*
 =======================================================================================================================
 =======================================================================================================================
 */
_inline_ void SOFT_ZList_SelectZListe(LONG Zlist_To_Select)
{
#ifdef _XENON_RENDER
	GDI_gpst_CurDD->g_cZListCurrentDisplayOrder = (CHAR)Zlist_To_Select;
#endif
	Zlist_To_Select = lMin(Zlist_To_Select, NumberOfZListes - 1);
	p_Current_SOFT_gst_ZList = &SOFT_gst_ZList[Zlist_To_Select];
}

SOFT_tdst_ZList_Node	*SOFT_pst_ZList_AddNode(float, LONG, LONG);
void					SOFT_ZList_Init(void);
void					SOFT_ZList_Clear(void);
void					SOFT_ZList_Send(void);
void					SOFT_ZList_Lock(void);
void					SOFT_ZList_Unlock(void);

void					SOFT_AddCurrentObjectInZList(GDI_tdst_DisplayData *, OBJ_tdst_GameObject *);

#if defined (__cplusplus) && !defined(JADEFUSION)
}
#endif
#endif /* __SOFTZLIST_H */

